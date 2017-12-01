#ifndef UTILS_CONVERSIONFROMSTRINGTOENUM_HPP_
#define UTILS_CONVERSIONFROMSTRINGTOENUM_HPP_

#include <config/enum/FunctionNameEnum.hpp>
#include <config/enum/ItemTypeEnum.hpp>
#include <config/enum/RepresentationModeEnum.hpp>
#include <string>

using namespace std;

class ConversionFromStringToEnum {

	public:

		static RepresentationModeEnum getRepresentationModeEnumFromString(string representationModeEnumInString) {
			if (representationModeEnumInString == "UnsignedInteger8bitValue") {
				return RepresentationModeEnum::Unsigned8bitIntegerValue;

			} else if (representationModeEnumInString == "UnsignedInteger16bitValue") {
				return RepresentationModeEnum::Unsigned16bitIntegerValue;

			} else if (representationModeEnumInString == "UnsignedInteger32bitValue") {
				return RepresentationModeEnum::Unsigned32bitIntegerValue;

			} else if (representationModeEnumInString == "UnsignedInteger64bitValue") {
				return RepresentationModeEnum::Unsigned64bitIntegerValue;

			} else if (representationModeEnumInString == "SignedInteger8bitValue") {
				return RepresentationModeEnum::Signed8bitIntegerValue;

			} else if (representationModeEnumInString == "SignedInteger16bitValue") {
				return RepresentationModeEnum::Signed16bitIntegerValue;

			} else if (representationModeEnumInString == "SignedInteger32bitValue") {
				return RepresentationModeEnum::Signed32bitIntegerValue;

			} else if (representationModeEnumInString == "SignedInteger64bitValue") {
				return RepresentationModeEnum::Signed64bitIntegerValue;

			} else if (representationModeEnumInString == "FloatValue") {
				return RepresentationModeEnum::FloatValue;

			} else if (representationModeEnumInString == "DoubleValue") {
				return RepresentationModeEnum::DoubleValue;

			} else if (representationModeEnumInString == "StringValue") {
				return RepresentationModeEnum::StringValue;

			} else
				return RepresentationModeEnum::InvalidValue;
		}

		static FunctionNameEnum getFunctionNameEnumFromString(string functionNameEnumInString) {

			if (functionNameEnumInString == "toDecimal") {
				return FunctionNameEnum::toDecimal;

			} else if (functionNameEnumInString == "toHexadecimal") {
				return FunctionNameEnum::toHexadecimal;

			} else if (functionNameEnumInString == "CA2toDecimal") {
				return FunctionNameEnum::CA2toDecimal;

			} else if (functionNameEnumInString == "CA2toHexadecimal") {
				return FunctionNameEnum::CA2toHexadecimal;

			} else if (functionNameEnumInString == "toOctal") {
				return FunctionNameEnum::toOctal;

			} else if (functionNameEnumInString == "toTargetId") {
				return FunctionNameEnum::toTargetId;

			} else if (functionNameEnumInString == "toASCII") {
				return FunctionNameEnum::toASCII;

			} else if (functionNameEnumInString == "toMBData") {
				return FunctionNameEnum::toMBData;

			} else
				return FunctionNameEnum::InvalidValue;
		}
};

#endif /* UTILS_CONVERSIONFROMSTRINGTOENUM_HPP_ */
