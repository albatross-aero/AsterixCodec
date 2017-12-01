#ifndef DATA_MODEL_BLOCKING_QUEUE_DECODERBLOCKINGQUEUE_H_
#define DATA_MODEL_BLOCKING_QUEUE_DECODERBLOCKINGQUEUE_H_

#include <config/CommonVariables.h>
#include <data_model/decoder_object_pool/ParsedRecord.h>
#include <condition_variable>
#include <mutex>
#include <queue>


/**
 * Contains queue of ParsedRecord for the consumer.
 */
class DecoderBlockingQueue {
	public:

		DecoderBlockingQueue(unsigned int _sizeBlockingQueue);

		~DecoderBlockingQueue();

		/**
		 * Gets a object pool from the blocking queue.
		 * If the queue is empty and the queue is open, the method waits.
		 * If the queue is empty and the queue is closed, the method return nullptr.
		 * @return a object pool from the blocking queue. Return nullptr if the queue is empty and closed.
		 */
		ParsedRecord* getObjectPoolFromQueue(unsigned int threadId);

		/**
		 * Puts a object pool in the queue.
		 * If the queue is full, the method waits.
		 * @param objectPool object pool to put in the queue.
		 */
		void setObjectPoolInQueue(ParsedRecord* objectPool);

		/**
		 * Closes the blocking queue and wakes up all the waiting thread
		 */
		void closeBlockingQueueConsumer();

		/**
		 * Opens the blocking queue
		 */
		void openBlockingQueueConsumer();

	private:

		std::queue<ParsedRecord*> elementObjectPoolQueue;

		mutex queueMutex;
		condition_variable queueCv;
		bool blockingQueueIsOpen;

		CommonVariables* commonVariables;    // Pointer to the Singleton - this class is not responsible for the deallocation of the pointer
};

#endif /* DATA_MODEL_BLOCKING_QUEUE_DECODERBLOCKINGQUEUE_H_ */
