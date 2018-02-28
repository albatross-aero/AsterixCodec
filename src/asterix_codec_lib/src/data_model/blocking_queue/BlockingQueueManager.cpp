#include "data_model/blocking_queue/BlockingQueueManager.h"

// Initialization of static field 'blockingQueue' - It is mandatory
BlockingQueueManager *BlockingQueueManager::blockingQueueManager = nullptr;

BlockingQueueManager::BlockingQueueManager() {
    FacadeCommonVariables *facadeCommon = FacadeCommonVariables::getInstance();
    int producerThreadsNum = facadeCommon->getNumberOfProducerThreads();
    int elementIntoObjPool = facadeCommon->getNumberOfElementsInPreallocatedParsedRecord();
    int objPoolEachQueue = (facadeCommon->getNumberOfPreallocatedParsedRecord()) / producerThreadsNum;

    chrono::time_point<chrono::system_clock> start;
    chrono::time_point<chrono::system_clock> end;

    start = chrono::system_clock::now();

    matchMap_threadIdProducerManager = map<pthread_t, unsigned int>();
    matchMap_threadIdConsumerManager = map<pthread_t, unsigned int>();

    // One Blocking Queue for each producer thread
    for (int i = 0; i < producerThreadsNum; i++) {
        blockingQueuesForProducer.push_back(new ParserBlockingQueue(objPoolEachQueue, elementIntoObjPool, i));
    }

    // One Blocking Queue for each consumer thread (same number of the BQ of the producer)
    for (int i = 0; i < producerThreadsNum; i++) {
        blockingQueuesForConsumer.push_back(new DecoderBlockingQueue(objPoolEachQueue));
    }

    end = chrono::system_clock::now();
    cout << endl << std::dec << "Elapsed time for Object pool initialization: "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count()
         << " ms "
         << endl;
}

BlockingQueueManager::~BlockingQueueManager() {

    int vectorSize = blockingQueuesForProducer.size();
    for (int i = 0; i < vectorSize; i++) {
        delete blockingQueuesForProducer[i];
    }

    vectorSize = blockingQueuesForConsumer.size();
    for (int i = 0; i < vectorSize; i++) {
        delete blockingQueuesForConsumer[i];
    }
}

BlockingQueueManager *BlockingQueueManager::getInstance() {
    if (!blockingQueueManager) {
        blockingQueueManager = new BlockingQueueManager();
    }
    return blockingQueueManager;
}

void BlockingQueueManager::deleteInstance() {
    if (blockingQueueManager != nullptr)
        delete blockingQueueManager;

    blockingQueueManager = nullptr;
}

void BlockingQueueManager::closeAllBlockingQueues() {

    int vectorSize = blockingQueuesForProducer.size();
    for (int i = 0; i < vectorSize; i++) {
        blockingQueuesForProducer[i]->closeBlockingQueueProducer();
    }

    vectorSize = blockingQueuesForConsumer.size();
    for (int i = 0; i < vectorSize; i++) {
        blockingQueuesForConsumer[i]->closeBlockingQueueConsumer();
    }
}

void BlockingQueueManager::openAllBlockingQueues() {
    for (unsigned int i = 0; i < blockingQueuesForProducer.size(); i++) {
        blockingQueuesForProducer[i]->openBlockingQueueProducer();
    }
    for (unsigned int i = 0; i < blockingQueuesForConsumer.size(); i++) {
        blockingQueuesForConsumer[i]->openBlockingQueueConsumer();
    }
}

ParsedRecord *BlockingQueueManager::getObjectPoolForProducer(pthread_t threadId) {
    unsigned int objectPoolId = 0;
    auto poolIdIterator = matchMap_threadIdProducerManager.find(
            threadId);    // Check if the current thread already has an assigned queue already

    if (poolIdIterator == matchMap_threadIdProducerManager.end()) {        // 'poolId' HAS NOT BEEN found..
        unique_lock<std::mutex> uniqueLock(mutexProducerMatchMap);    // Takes the lock to write into the map
        unsigned int indexIntoMap = matchMap_threadIdProducerManager.size();
        matchMap_threadIdProducerManager.insert(std::pair<pthread_t, unsigned int>(threadId, indexIntoMap));
        objectPoolId = indexIntoMap;
    } else {        // 'poolId' HAS BEEN found..
        try {
            objectPoolId = matchMap_threadIdProducerManager.at(threadId);
        } catch (exception &e) {
            cout << "getObjectPoolForProducer - " << e.what() << " - threadId: " << threadId << endl;
        }
    }
    return blockingQueuesForProducer[objectPoolId]->getObjectPoolFromQueue();
}

void BlockingQueueManager::releaseObjectPoolForProducer(pthread_t threadId, ParsedRecord *elementObjectPool) {
    int poolId = elementObjectPool->getMembershipQueue();
    blockingQueuesForProducer[poolId]->setObjectPoolInQueue(threadId, elementObjectPool);
}

void BlockingQueueManager::setObjectPoolForConsumer(ParsedRecord *elementObjectPool) {
    int indexPool = elementObjectPool->getMembershipQueue();
    return blockingQueuesForConsumer[indexPool]->setObjectPoolInQueue(elementObjectPool);
}

ParsedRecord *BlockingQueueManager::getObjectPoolForConsumer(pthread_t threadId) {
    unsigned int poolId = 0;
    auto poolIdIterator = matchMap_threadIdConsumerManager.find(
            threadId);    // Check if the current thread already have a queue assigned to it

    if (poolIdIterator == matchMap_threadIdConsumerManager.end()) {
        unique_lock<std::mutex> uniqueLock(mutexConsumerMatchMap);    // It takes the lock to write into the map
        unsigned int indexIntoMap = (matchMap_threadIdConsumerManager.size()) %
                                    (blockingQueuesForConsumer.size());    // It calculates the index to access the vector of queue modulo the number of queue
        matchMap_threadIdConsumerManager.insert(std::pair<pthread_t, unsigned int>(threadId, indexIntoMap));
        poolId = indexIntoMap;
    } else {

        try {
            poolId = matchMap_threadIdConsumerManager.at(threadId);
        } catch (exception &e) {
            cout << "getObjectPoolForConsumer - " << e.what() << " - threadId: " << threadId << endl;
        }
    }
    return blockingQueuesForConsumer[poolId]->getObjectPoolFromQueue(threadId);
}
