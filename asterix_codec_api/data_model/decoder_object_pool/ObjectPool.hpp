#ifndef DATA_MODEL_DECODER_OBJECT_POOL_OBJECTPOOL_HPP_
#define DATA_MODEL_DECODER_OBJECT_POOL_OBJECTPOOL_HPP_

#include <config/CommonVariables.h>
#include <iostream>
#include <vector>


class DecodedValueObjectPoolManager;
// It is a way to allow that two classes refer to each other

/**
 * Template for a object pool.
 * Contains a queue of T objects.
 * For simplicity reasons, we have only one unique file for declaration and implementation of methods.
 */
template<class T>
class ObjectPool {
	public:

		ObjectPool(DecodedValueObjectPoolManager* _ptrToManager, unsigned int _poolSize) {
			commonVariable = CommonVariables::getInstance();

			ptrToManager = _ptrToManager;
			objectPoolCurrentSize = 0;

			objectPool = vector<T*>(_poolSize);

			// Create <T> objects for initialize the object pool
			for (unsigned int i = 0; i < _poolSize; i++)
				objectPool[i] = (new T(_ptrToManager));
		}

		~ObjectPool() {
			int objectPoolSize = objectPool.size();

			//Release all objects in the object pool
			for (unsigned int i = 0; i < objectPoolSize; i++) {
				T* objectToDelete = objectPool[i];
				delete objectToDelete;
				objectPool[i] = nullptr;
			}
			objectPool.clear();

			objectPoolCurrentSize = 0;

			ptrToManager = nullptr;
			commonVariable = nullptr;
		}

		/**
		 * Get from the object pool a object to use for store a decode value.
		 * @return pointer to a specialized ElementValue to use for store a decoded value
		 */
		T* getObjectFromOP() {
			T* freeObject = nullptr;

			//If the queue is not empty, method returns a T object
			if (objectPoolCurrentSize < objectPool.size()) {
				freeObject = objectPool[objectPoolCurrentSize];
			} else {

				// If the queue is empty and the queue is open, I allocate a new T object
				if (commonVariable->isDebugDecodedValueEnabled())
					std::cout << "Not enough space for the decoded value" << std::endl;

				freeObject = new T(ptrToManager);

				objectPool.resize(objectPoolCurrentSize + 1);
				objectPool[objectPoolCurrentSize] = freeObject;
			}

			objectPoolCurrentSize++;

			return freeObject;
		}

		unsigned int sizeObjectPool() {
			return objectPoolCurrentSize;
		}

		void releaseObjects() {
			objectPoolCurrentSize = 0;
		}

	private:
		std::vector<T*> objectPool;
		unsigned int objectPoolCurrentSize;

		DecodedValueObjectPoolManager* ptrToManager;	// this class is not responsible for the deallocation of the pointer
		CommonVariables* commonVariable;				// Pointer to the Singleton - this class is not responsible for the deallocation of the pointer
};

#endif /* DATA_MODEL_DECODER_OBJECT_POOL_OBJECTPOOL_HPP_ */
