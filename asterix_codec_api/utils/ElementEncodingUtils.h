#ifndef UTILS_ELEMENTENCODINGUTILS_H_
#define UTILS_ELEMENTENCODINGUTILS_H_

#include <config/CommonVariables.h>
#include <cmath>

#include "data_model/decoder_objects/DoubleValue.hpp"
#include "data_model/decoder_objects/ElementValue.hpp"
#include "data_model/decoder_objects/FloatValue.hpp"
#include "data_model/decoder_objects/IntegerValue.hpp"
#include "data_model/decoder_objects/StringValue.hpp"
#include "data_model/decoder_objects/SignedInteger16bitValue.hpp"
#include "data_model/decoder_objects/SignedInteger32bitValue.hpp"
#include "data_model/decoder_objects/SignedInteger8bitValue.hpp"
#include "data_model/decoder_objects/SignedInteger64bitValue.hpp"
#include "data_model/decoder_objects/UnsignedInteger16bitValue.hpp"
#include "data_model/decoder_objects/UnsignedInteger32bitValue.hpp"
#include "data_model/decoder_objects/UnsignedInteger8bitValue.hpp"
#include "data_model/decoder_objects/UnsignedInteger64bitValue.hpp"
#include "utils/BitUtils.h"
#include "facade/reports/FailureReport.h"

class ElementEncodingUtils {

	public:

		static unsigned char* encodeDecimal(ElementValue* element, Range *validityRange, int lengthInBit, double multFactor, string *elementName,
				unsigned int categoryNumber);

		static unsigned char* encodeDecimalToCA2(ElementValue* element, Range *validityRange, int lengthInBit, double multFactor, string *elementName,
				unsigned int categoryNumber);

		static unsigned char* encodeAscii(ElementValue* element, int lengthInBit);

		static unsigned char* encodeTargetId(ElementValue* element, int lengthInBit);

		static unsigned char* encodeMBData(ElementValue* element, int lengthInBit);

	private:
		ElementEncodingUtils();
		virtual ~ElementEncodingUtils();

		/**
		 * It checks if the current Element value is included in the validity range coming from the Item description.
		 * If not, a failure is logged in the FailureReport.
		 * @param elementRange Reference range with lower bound and upper bound of the Element
		 * @param elementValue Value to be checked
		 * @param elementName Name of the current Element
		 * @param categoryNumber Category number of the Record that contains the current Element
		 */
		static void checkValidityRange(string *elementName, double elementValue, Range* elementRange, unsigned int categoryNumber);

		static unsigned char* getRawElement(long long integerToEncode, int lengthInBit);

		static CommonVariables* commonVariables;

		static FailureReport *failureReport;
};

#endif /* UTILS_ELEMENTENCODINGUTILS_H_ */
