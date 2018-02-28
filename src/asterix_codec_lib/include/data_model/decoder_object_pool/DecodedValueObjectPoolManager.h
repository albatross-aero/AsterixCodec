#ifndef DATA_MODEL_DECODER_OBJECT_POOL_DECODEDVALUEOBJECTPOOLMANAGER_H_
#define DATA_MODEL_DECODER_OBJECT_POOL_DECODEDVALUEOBJECTPOOLMANAGER_H_

#include "data_model/decoder_object_pool/ObjectPool.hpp"
#include "data_model/decoder_objects/DoubleValue.h"
#include "data_model/decoder_objects/FloatValue.h"
#include "data_model/decoder_objects/IntegerValue.h"
#include "data_model/decoder_objects/SignedInteger16bitValue.h"
#include "data_model/decoder_objects/SignedInteger32bitValue.h"
#include "data_model/decoder_objects/SignedInteger8bitValue.h"
#include "data_model/decoder_objects/SignedInteger64bitValue.h"
#include "data_model/decoder_objects/StringValue.h"
#include "data_model/decoder_objects/UnsignedInteger64bitValue.h"
#include "data_model/decoder_objects/UnsignedInteger16bitValue.h"
#include "data_model/decoder_objects/UnsignedInteger32bitValue.h"
#include "data_model/decoder_objects/UnsignedInteger8bitValue.h"

/**
 * Manager for all object pool where the decoded values are going to be stored
 */
class DecodedValueObjectPoolManager {
public:

    DecodedValueObjectPoolManager();

    ~DecodedValueObjectPoolManager();

    //SIGNED INTEGER 8 bit VALUE
    /**
     * Get a pointer to a free SignedInteger8bitValue from the object pool.
     * This function is called during the decoding phase.
     * @return SignedInteger8bitValue pointer to use
     */
    SignedInteger8bitValue *getSignedInteger8bitValueFromOP();

    //SIGNED INTEGER 16 bit VALUE
    /**
     * Get a pointer to a free SignedInteger16bitValue from the object pool.
     * This function is called during the decoding phase.
     * @return SignedInteger16bitValue pointer to use
     */
    SignedInteger16bitValue *getSignedInteger16bitValueFromOP();

    //SIGNED INTEGER 32 bit VALUE
    /**
     * Get a pointer to a free SignedInteger32bitValue from the object pool.
     * This function is called during the decoding phase.
     * @return SignedInteger32bitValue pointer to use
     */
    SignedInteger32bitValue *getSignedInteger32bitValueFromOP();

    //SIGNED INTEGER 64 bit VALUE
    /**
     * Get a pointer to a free SignedInteger64bitValue from the object pool.
     * This function is called during the decoding phase.
     * @return IntegerValue object to use
     */
    SignedInteger64bitValue *getSignedInteger64bitValueFromOP();

    //UNSIGNED INTEGER 8 bit VALUE
    /**
     * Get a pointer to a free UnsignedInteger8bitValue from the object pool.
     * This function is called during the decoding phase.
     * @return UnsignedInteger8bitValue pointer to use
     */
    UnsignedInteger8bitValue *getUnsignedInteger8bitValueFromOP();

    //UNSIGNED INTEGER 16 bit VALUE
    /**
     * Get a pointer to a free UnsignedInteger16bitValue from the object pool.
     * This function is called during the decoding phase.
     * @return UnsignedInteger16bitValue pointer to use
     */
    UnsignedInteger16bitValue *getUnsignedInteger16bitValueFromOP();

    //UNSIGNED INTEGER 32 bit VALUE
    /**
     * Get a pointer to a free UnsignedInteger32bitValue from the object pool.
     * This function is called during the decoding phase.
     * @return UnsignedInteger32bitValue pointer to use
     */
    UnsignedInteger32bitValue *getUnsignedInteger32bitValueFromOP();

    //UNSIGNED INTEGER 64 bit VALUE
    /**
     * Get a pointer to a free UnsignedInteger64bitValue from the object pool.
     * This function is called during the decoding phase.
     * @return IntegerValue object to use
     */
    UnsignedInteger64bitValue *getUnsignedInteger64bitValueFromOP();

    //FLOAT VALUE
    /**
     * Get a pointer to a free FloatValue from the object pool.
     * This function is called during the decoding phase.
     * @return FloatValue object to use
     */
    FloatValue *getFloatValueFromOP();

    //DOUBLE VALUE
    /**
     * Get a pointer to a free DoubleValue from the object pool.
     * This function is called during the decoding phase.
     * @return DoubleValue object to use
     */
    DoubleValue *getDoubleValueFromOP();

    //STRING VALUE
    /**
     * Get a pointer to a free StringValue from the object pool.
     * This function is called during the decoding phase.
     * @return StringValue object to use
     */
    StringValue *getStringValueFromOP();

    void releaseAllElementValueToOP();

private:

    //ObjectPool<IntegerValue>* integerValueObjectPool;
    ObjectPool<SignedInteger8bitValue> *signedInteger8bitValueObjectPool;
    ObjectPool<SignedInteger16bitValue> *signedInteger16bitValueObjectPool;
    ObjectPool<SignedInteger32bitValue> *signedInteger32bitValueObjectPool;
    ObjectPool<SignedInteger64bitValue> *signedInteger64bitValueObjectPool;
    ObjectPool<UnsignedInteger8bitValue> *unsignedInteger8bitValueObjectPool;
    ObjectPool<UnsignedInteger16bitValue> *unsignedInteger16bitValueObjectPool;
    ObjectPool<UnsignedInteger32bitValue> *unsignedInteger32bitValueObjectPool;
    ObjectPool<UnsignedInteger64bitValue> *unsignedInteger64bitValueObjectPool;
    ObjectPool<FloatValue> *floatValueObjectPool;
    ObjectPool<DoubleValue> *doubleValueObjectPool;
    ObjectPool<StringValue> *stringValueObjectPool;
};

#endif /* DATA_MODEL_DECODER_OBJECT_POOL_DECODEDVALUEOBJECTPOOLMANAGER_H_ */
