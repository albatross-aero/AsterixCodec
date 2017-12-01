#include "DecodedValueObjectPoolManager.h"

DecodedValueObjectPoolManager::DecodedValueObjectPoolManager() {
	CommonVariables* common = CommonVariables::getInstance();

	//integerValueObjectPool = new ObjectPool<IntegerValue>(this, common->getIntegerValuePoolSize());

	signedInteger8bitValueObjectPool = new ObjectPool<SignedInteger8bitValue>(this, common->getInteger8bitValuePoolSize());

	signedInteger16bitValueObjectPool = new ObjectPool<SignedInteger16bitValue>(this, common->getInteger16bitValuePoolSize());

	signedInteger32bitValueObjectPool = new ObjectPool<SignedInteger32bitValue>(this, common->getInteger32bitValuePoolSize());

	signedInteger64bitValueObjectPool = new ObjectPool<SignedInteger64bitValue>(this, common->getInteger64bitValuePoolSize());

	unsignedInteger8bitValueObjectPool = new ObjectPool<UnsignedInteger8bitValue>(this, common->getInteger32bitValuePoolSize());

	unsignedInteger16bitValueObjectPool = new ObjectPool<UnsignedInteger16bitValue>(this, common->getInteger32bitValuePoolSize());

	unsignedInteger32bitValueObjectPool = new ObjectPool<UnsignedInteger32bitValue>(this, common->getInteger32bitValuePoolSize());

	unsignedInteger64bitValueObjectPool = new ObjectPool<UnsignedInteger64bitValue>(this, common->getInteger64bitValuePoolSize());

	floatValueObjectPool = new ObjectPool<FloatValue>(this, common->getFloatValuePoolSize());

	doubleValueObjectPool = new ObjectPool<DoubleValue>(this, common->getDoubleValuePoolSize());

	stringValueObjectPool = new ObjectPool<StringValue>(this, common->getStringValuePoolSize());
}

DecodedValueObjectPoolManager::~DecodedValueObjectPoolManager() {
	/*if (integerValueObjectPool != nullptr) {
	 delete integerValueObjectPool;
	 integerValueObjectPool = nullptr;
	 }*/

	if (signedInteger8bitValueObjectPool != nullptr) {
		delete signedInteger8bitValueObjectPool;
		signedInteger8bitValueObjectPool = nullptr;
	}

	if (signedInteger16bitValueObjectPool != nullptr) {
		delete signedInteger16bitValueObjectPool;
		signedInteger16bitValueObjectPool = nullptr;
	}

	if (signedInteger32bitValueObjectPool != nullptr) {
		delete signedInteger32bitValueObjectPool;
		signedInteger32bitValueObjectPool = nullptr;
	}

	if (floatValueObjectPool != nullptr) {
		delete floatValueObjectPool;
		floatValueObjectPool = nullptr;
	}

	if (doubleValueObjectPool != nullptr) {
		delete doubleValueObjectPool;
		doubleValueObjectPool = nullptr;
	}

	if (stringValueObjectPool != nullptr) {
		delete stringValueObjectPool;
		stringValueObjectPool = nullptr;
	}
}

/*IntegerValue* DecodedValueObjectPoolManager::getIntegerValueFromOP() {
 if (integerValueObjectPool != nullptr) {
 return integerValueObjectPool->getObjectFromOP();
 }
 return nullptr;
 }*/

SignedInteger8bitValue* DecodedValueObjectPoolManager::getSignedInteger8bitValueFromOP() {
	if (signedInteger8bitValueObjectPool != nullptr) {
		return signedInteger8bitValueObjectPool->getObjectFromOP();
	}
	return nullptr;
}

UnsignedInteger8bitValue* DecodedValueObjectPoolManager::getUnsignedInteger8bitValueFromOP() {
	if (unsignedInteger8bitValueObjectPool != nullptr) {
		return unsignedInteger8bitValueObjectPool->getObjectFromOP();
	}
	return nullptr;
}

SignedInteger16bitValue* DecodedValueObjectPoolManager::getSignedInteger16bitValueFromOP() {
	if (signedInteger16bitValueObjectPool != nullptr) {
		return signedInteger16bitValueObjectPool->getObjectFromOP();
	}
	return nullptr;
}

UnsignedInteger16bitValue* DecodedValueObjectPoolManager::getUnsignedInteger16bitValueFromOP() {
	if (unsignedInteger16bitValueObjectPool != nullptr) {
		return unsignedInteger16bitValueObjectPool->getObjectFromOP();
	}
	return nullptr;
}

SignedInteger32bitValue* DecodedValueObjectPoolManager::getSignedInteger32bitValueFromOP() {
	if (signedInteger32bitValueObjectPool != nullptr) {
		return signedInteger32bitValueObjectPool->getObjectFromOP();
	}
	return nullptr;
}

UnsignedInteger32bitValue* DecodedValueObjectPoolManager::getUnsignedInteger32bitValueFromOP() {
	if (unsignedInteger32bitValueObjectPool != nullptr) {
		return unsignedInteger32bitValueObjectPool->getObjectFromOP();
	}
	return nullptr;
}

SignedInteger64bitValue* DecodedValueObjectPoolManager::getSignedInteger64bitValueFromOP() {
	if (signedInteger64bitValueObjectPool != nullptr) {
		return signedInteger64bitValueObjectPool->getObjectFromOP();
	}
	return nullptr;
}

UnsignedInteger64bitValue* DecodedValueObjectPoolManager::getUnsignedInteger64bitValueFromOP() {
	if (unsignedInteger64bitValueObjectPool != nullptr) {
		return unsignedInteger64bitValueObjectPool->getObjectFromOP();
	}
	return nullptr;
}

FloatValue* DecodedValueObjectPoolManager::getFloatValueFromOP() {
	if (floatValueObjectPool != nullptr) {
		return floatValueObjectPool->getObjectFromOP();
	}
	return nullptr;
}

DoubleValue* DecodedValueObjectPoolManager::getDoubleValueFromOP() {
	if (doubleValueObjectPool != nullptr) {
		return doubleValueObjectPool->getObjectFromOP();
	}
	return nullptr;
}

StringValue* DecodedValueObjectPoolManager::getStringValueFromOP() {
	if (stringValueObjectPool != nullptr) {
		return stringValueObjectPool->getObjectFromOP();
	}
	return nullptr;
}

void DecodedValueObjectPoolManager::releaseAllElementValueToOP() {
	//integerValueObjectPool->releaseObjects();
	signedInteger8bitValueObjectPool->releaseObjects();
	signedInteger16bitValueObjectPool->releaseObjects();
	signedInteger32bitValueObjectPool->releaseObjects();
	floatValueObjectPool->releaseObjects();
	doubleValueObjectPool->releaseObjects();
	stringValueObjectPool->releaseObjects();
}
