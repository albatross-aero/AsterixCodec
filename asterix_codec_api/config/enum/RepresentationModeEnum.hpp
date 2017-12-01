#ifndef DATA_MODEL_REPRESENTATIONMODEENUM_H_
#define DATA_MODEL_REPRESENTATIONMODEENUM_H_

enum class RepresentationModeEnum {
	InvalidValue = -1,
	IntegerValue = 0,
	FloatValue = 1,
	DoubleValue = 2,
	HexadecimalValue = 3,
	OctalValue = 4,
	StringValue = 5,
	Signed8bitIntegerValue = 6,
	Signed16bitIntegerValue = 7,
	Signed32bitIntegerValue = 8,
	Signed64bitIntegerValue = 9,
	Unsigned8bitIntegerValue = 10,
	Unsigned16bitIntegerValue = 11,
	Unsigned32bitIntegerValue = 12,
	Unsigned64bitIntegerValue = 13
};

#endif /* DATA_MODEL_REPRESENTATIONMODEENUM_H_ */
