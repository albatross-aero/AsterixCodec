#include "data_model/blocking_queue/ParserBlockingQueue.h"

ParserBlockingQueue::ParserBlockingQueue(unsigned int _dimensionBlockingQueue, unsigned int elementObjectPoolSize,
                                         unsigned int membershipQueue) {

    commonVariables = CommonVariables::getInstance();
    blockingQueueIsOpen = true;

    // Create ElementsObjectPool objects and put in the queue
    for (unsigned int i = 0; i < _dimensionBlockingQueue; i++) {
        ParsedRecord *objectPool = new ParsedRecord(elementObjectPoolSize);
        objectPool->setMembershipQueue(membershipQueue);
        elementObjectPoolQueue.push(objectPool);
    }
}

ParserBlockingQueue::~ParserBlockingQueue() {
    unsigned int queueSize = elementObjectPoolQueue.size();

    // Delete all ElementsObjectPool objects in the queue
    for (unsigned int i = 0; i < queueSize; i++) {
        ParsedRecord *objectPool = elementObjectPoolQueue.front();
        elementObjectPoolQueue.pop();

        delete objectPool;
    }

    commonVariables = nullptr;
}

ParsedRecord *ParserBlockingQueue::getObjectPoolFromQueue() {
    ParsedRecord *freeObjectPool = nullptr;
    unique_lock<std::mutex> uniqueLock(queueMutex);

    //If the queue is not empty, I can return a OP and wake up thread waiting for a place in the OP
    if (!elementObjectPoolQueue.empty()) {
        if (commonVariables->isDebugProducerObjectPoolEnabled()) {
            cout << "[P] There is a free ElementObjectPool ready to be used" << endl;
        }
        freeObjectPool = elementObjectPoolQueue.front();
        elementObjectPoolQueue.pop();

        //queueCv.notify_one();
        return freeObjectPool;
    } else if (elementObjectPoolQueue.empty() && blockingQueueIsOpen) {
        //If the queue is empty and the queue is open, I wait for a OP
        if (commonVariables->isDebugProducerObjectPoolEnabled()) {
            cout << "[P] Waiting - There is no ElementObjectPool ready to be used" << endl;
        }
        queueCv.wait(uniqueLock, [this]() {
            return (!elementObjectPoolQueue.empty() || !blockingQueueIsOpen);
        });

        if (!elementObjectPoolQueue.empty()) {
            if (commonVariables->isDebugProducerObjectPoolEnabled()) {
                cout << "[P] Waking up - There is a free ElementObjectPool ready to be used" << endl;
            }
            freeObjectPool = elementObjectPoolQueue.front();
            elementObjectPoolQueue.pop();

            //queueCv.notify_one();
            return freeObjectPool;
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

void ParserBlockingQueue::setObjectPoolInQueue(pthread_t threadId, ParsedRecord *objectPool) {
    unique_lock<std::mutex> uniqueLock(queueMutex);

    if (commonVariables->isDebugProducerObjectPoolEnabled()) {
        cout << "[C " << threadId << "] Release one ElementObjectPool" << endl;
    }
    objectPool->setFreeAllElements();
    elementObjectPoolQueue.push(objectPool);

    queueCv.notify_one();
}

void ParserBlockingQueue::closeBlockingQueueProducer() {
    unique_lock<std::mutex> uniqueLock(queueMutex);
    blockingQueueIsOpen = false;
    queueCv.notify_all();
}

void ParserBlockingQueue::openBlockingQueueProducer() {
    blockingQueueIsOpen = true;
}
