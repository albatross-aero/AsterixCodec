#include "data_model/blocking_queue/DecoderBlockingQueue.h"

DecoderBlockingQueue::DecoderBlockingQueue(unsigned int _sizeBlockingQueue) {
    blockingQueueIsOpen = true;
    commonVariables = CommonVariables::getInstance();
    queuePreallocatedSize = _sizeBlockingQueue;
}

DecoderBlockingQueue::~DecoderBlockingQueue() {
    ParsedRecord *objectPool = nullptr;
    unsigned int queueSize = elementObjectPoolQueue.size();

    // Delete all ElementsObjectPool objects in the queue
    for (unsigned int i = 0; i < queueSize; i++) {
        objectPool = elementObjectPoolQueue.front();
        elementObjectPoolQueue.pop();

        delete objectPool;
    }

    commonVariables = nullptr;
}

ParsedRecord *DecoderBlockingQueue::getObjectPoolFromQueue(pthread_t consumerId) {
    ParsedRecord *freeObjectPool = nullptr;
    unique_lock<std::mutex> uniqueLock(queueMutex);

    //If the queue is not empty, I can return a OP and wake up thread waiting for a place in the OP
    if (!elementObjectPoolQueue.empty()) {
        freeObjectPool = elementObjectPoolQueue.front();
        elementObjectPoolQueue.pop();
        if (commonVariables->isDebugConsumerObjectPoolEnabled()) {
            cout << "[C" << consumerId << "] There is a free ElementObjectPool ready to be used" << endl;
        }
        return freeObjectPool;
    } else if (elementObjectPoolQueue.empty() && blockingQueueIsOpen) {
        //If the queue is empty and the queue is open, I wait for a OP
        if (commonVariables->isDebugConsumerObjectPoolEnabled()) {
            cout << "[C" << consumerId << "] Waiting - There is no ElementObjectPool ready to be used" << endl;
        }
        queueCv.wait(uniqueLock, [this]() {
            return (!elementObjectPoolQueue.empty() || !blockingQueueIsOpen);
        });

        if (!elementObjectPoolQueue.empty()) {
            if (commonVariables->isDebugConsumerObjectPoolEnabled()) {
                cout << "[C" << consumerId << "] Waking up - There is a free ElementObjectPool ready to be used" << endl;
            }
            freeObjectPool = elementObjectPoolQueue.front();
            elementObjectPoolQueue.pop();

            //queueCv.notify_one();
            return freeObjectPool;
        } else {
            return nullptr;
        }
    } else
        return nullptr;
}

void DecoderBlockingQueue::setObjectPoolInQueue(ParsedRecord *objectPool) {
    unique_lock<std::mutex> uniqueLock(queueMutex);
    if (commonVariables->isDebugConsumerObjectPoolEnabled()) {
        cout << "[P] Set one ElementObjectPool for consumer" << endl;
    }
    elementObjectPoolQueue.push(objectPool);
    queueCv.notify_one();
}

void DecoderBlockingQueue::closeBlockingQueueConsumer() {
    unique_lock<std::mutex> uniqueLock(queueMutex);

    if (commonVariables->isDebugConsumerObjectPoolEnabled()) {
        cout << "[C] Close buffer" << endl;
    }
    blockingQueueIsOpen = false;
    queueCv.notify_all();
}

void DecoderBlockingQueue::openBlockingQueueConsumer() {
    blockingQueueIsOpen = true;
}
