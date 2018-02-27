#ifndef DATA_MODEL_BLOCKING_QUEUE_BLOCKINGQUEUEMANAGER_H_
#define DATA_MODEL_BLOCKING_QUEUE_BLOCKINGQUEUEMANAGER_H_

#include <map>
#include <mutex>
#include <vector>
#include <chrono>
#include <exception>
#include <iostream>
#include <utility>

#include "config/FacadeCommonVariables.h"
#include "data_model/decoder_object_pool/ParsedRecord.h"
#include "data_model/blocking_queue/DecoderBlockingQueue.h"
#include "data_model/blocking_queue/ParserBlockingQueue.h"

using namespace std;

/**
 * Manager for all blocking queues, it works as a Singleton
 */
class BlockingQueueManager {

public:
    // Copy constructor
    BlockingQueueManager(const BlockingQueueManager &source) = delete;

    // Copy assignment operator
    BlockingQueueManager &operator=(const BlockingQueueManager &source) = delete;

    /**
     * Gets instance of the Singleton.
     * @return pointer to the BlockingQueueManager class
     */
    static BlockingQueueManager *getInstance();

    /**
     * Removes the instance of the Singleton
     */
    static void deleteInstance();

    /**
     * Closes the blocking queue of producer and of consumer
     */
    void closeAllBlockingQueues();

    /**
     * Opens the blocking queue of producer and of consumer
     */
    void openAllBlockingQueues();

    /**
     * Gets a free ElementsObjectPool from the Producer queue.
     * This function is called by a Producer.
     * @return ElementsObjectPool object to use
     */
    ParsedRecord *getObjectPoolForProducer(pthread_t threadId);

    /**
     * Releases the resources of a ElementsObjectPool object and put its pointer in the queue of the producer.
     * This function is called by a consumer.
     * @param threadId Id of the current thread calling this function
     * @param elementObjectPool ElementsObjectPool object to release
     */
    void releaseObjectPoolForProducer(pthread_t threadId, ParsedRecord *elementObjectPool);

    /**
     * Gets a full ElementsObjectPool from the Consumer queue.
     * This function is called by a Consumer.
     * @return ElementsObjectPool object to use
     */

    ParsedRecord *getObjectPoolForConsumer(pthread_t threadId);

    /**
     * Puts a ElementsObjectPool pointer in the Consumer queue.
     * This function is called by a Producer.
     * @param elementObjectPool Contains a parsed record
     */
    void setObjectPoolForConsumer(ParsedRecord *elementObjectPool);

private:
    BlockingQueueManager();        // is a private method, the class can be instantiate only one time by the 'getInstance' method

    ~BlockingQueueManager();

    // MEMBERS
    static BlockingQueueManager *blockingQueueManager;            // Pointer to the instance of the class

    // PRODUCER
    vector<ParserBlockingQueue *> blockingQueuesForProducer;    // Pointer to the Producer BQ
    map<unsigned long int, unsigned int> matchMap_threadIdProducerManager;
    std::mutex mutexProducerMatchMap;

    // CONSUMER
    vector<DecoderBlockingQueue *> blockingQueuesForConsumer;    // Pointer to the Consumer BQ
    map<pthread_t, unsigned int> matchMap_threadIdConsumerManager;
    std::mutex mutexConsumerMatchMap;
};

#endif /* DATA_MODEL_BLOCKING_QUEUE_BLOCKINGQUEUEMANAGER_H_ */
