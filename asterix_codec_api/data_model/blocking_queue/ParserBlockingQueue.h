#ifndef DATA_MODEL_BLOCKING_QUEUE_PARSERBLOCKINGQUEUE_H_
#define DATA_MODEL_BLOCKING_QUEUE_PARSERBLOCKINGQUEUE_H_

#include <config/CommonVariables.h>
#include <data_model/decoder_object_pool/ParsedRecord.h>
#include <condition_variable>
#include <mutex>
#include <queue>


/**
 * Contains queue of ElementsObjectPool for the producer.
 */
class ParserBlockingQueue {
	public:

		ParserBlockingQueue(unsigned int _sizeBlockingQueue, unsigned int elementPoolSize, unsigned int membershipQueue);

		~ParserBlockingQueue();

		/**
		 * Gets a object pool from the blocking queue.
		 * If the queue is empty and the queue is open, the method waits.
		 * If the queue is empty and the queue is closed, the method return nullptr.
		 * @return a object pool from the blocking queue. Return nullptr if the queue is empty and closed.
		 */
		ParsedRecord* getObjectPoolFromQueue();

		/**
		 * Puts a object pool in the queue and release all data saved in ElementsObjectPool object.
		 * If the queue is full, the method waits.
		 * If the queue is close, the method put the object pool in the queue for release reason.
		 * @param threadId Id of the thread that are calling that function
		 * @param objectPool object pool to put in the queue
		 */
		void setObjectPoolInQueue(unsigned int threadId, ParsedRecord* objectPool);

		/**
		 * Closes the blocking queue and wakes up all the waiting thread
		 */
		void closeBlockingQueueProducer();

		/**
		 * Opens the blocking queue
		 */
		void openBlockingQueueProducer();

		void setMembershipQueue(int index);

		int getMembershipQueue();

	private:

		std::queue<ParsedRecord*> elementObjectPoolQueue;

		std::mutex queueMutex;
		std::condition_variable queueCv;bool blockingQueueIsOpen;

		CommonVariables* commonVariables;    // Pointer to the Singleton - this class is not responsible for the deallocation of the pointer
};

#endif /* DATA_MODEL_BLOCKING_QUEUE_PARSERBLOCKINGQUEUE_H_ */
