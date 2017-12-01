#include "CodecTests.hpp"

/**
 * Running the parsing until the CAT021 Source Identifier Item, it checks that the toDecimal decoding is correct.
 */
TEST_F(CodecTest, DecodesToDecimal) {
	// I021_010	- SAC | SIC

	unsigned char *bytestream = new unsigned char[2];
	ElementValue *decodedSAC = nullptr, *decodedSIC = nullptr;
	int valueLengthInBit = 8;
	double multiplicationFactor = 1;
	RepresentationModeEnum representationModeForDecoding = RepresentationModeEnum::Unsigned8bitIntegerValue;
//	Range validRange = Range(0, 255);
	Range validRange;
	validRange.minValue = 0;
	validRange.maxValue = 255;

	memset(bytestream, 0, 2);
	bytestream[0] = 0x32;
	bytestream[1] = 0x50;
	string elementNames[2] = { "SAC", "SIC" };

	// SAC
	decodedSAC = DecodingUtils::toDecimal(&(bytestream[0]), &valueLengthInBit, &multiplicationFactor, &representationModeForDecoding, &validRange,
											(unsigned int) 21, &(elementNames[0]), decodedElementsOP);

	ASSERT_NE(decodedSAC, nullptr);
	ASSERT_EQ(decodedSAC->getDecodedDataInString(), "50");

	// SIC
	decodedSIC = DecodingUtils::toDecimal(&(bytestream[1]), &valueLengthInBit, &multiplicationFactor, &representationModeForDecoding, &validRange,
											(unsigned int) 21, &(elementNames[1]), decodedElementsOP);

	ASSERT_NE(decodedSIC, nullptr);
	ASSERT_EQ(decodedSIC->getDecodedDataInString(), "80");
}

/**
 *
 */
TEST_F(CodecTest, DecodesToHexadecimal) {
	// I021_080	- Target Address

	unsigned char *bytestream = new unsigned char[3];
	ElementValue *decodedTargetAddress = nullptr;
	int valueLengthInBit = 24;
	double multiplicationFactor = 1;
	RepresentationModeEnum representationModeForDecoding = RepresentationModeEnum::Unsigned32bitIntegerValue;
	Range validRange;

	memset(bytestream, 0, 3);
	bytestream[0] = 0x44;
	bytestream[1] = 0x7c;
	bytestream[2] = 0x10;
	string elementName = "Target Address";

	// Target Address
	decodedTargetAddress = DecodingUtils::toHexadecimal(bytestream, &valueLengthInBit, &multiplicationFactor, &representationModeForDecoding, &validRange,
														(unsigned int) 21, &elementName, decodedElementsOP);

	ASSERT_NE(decodedTargetAddress, nullptr);
	ASSERT_EQ(decodedTargetAddress->getDecodedDataInString(), "0x447c10");
}

/**
 *
 */
TEST_F(CodecTest, DecodesCA2ToDecimal) {
	// I021_130	- Latitude | Longitude

	unsigned char *bytestream = new unsigned char[6];
	ElementValue *decodedLatitude = nullptr, *decodedLongitude = nullptr;
	int valueLengthInBit = 24;
	double multiplicationFactor = 0.00002145767;
	RepresentationModeEnum representationModeForDecoding = RepresentationModeEnum::DoubleValue;
	Range validRange;

	memset(bytestream, 0, 6);
	bytestream[0] = 0x22;
	bytestream[1] = 0x37;
	bytestream[2] = 0xeb;
	bytestream[3] = 0x0b;
	bytestream[4] = 0x5c;
	bytestream[5] = 0x0c;
	string elementNames[2] = { "Latitude", "Longitude" };

	// Latitude
	decodedLatitude = DecodingUtils::toDecimal(&(bytestream[0]), &valueLengthInBit, &multiplicationFactor, &representationModeForDecoding, &validRange,
												(unsigned int) 21,
												&(elementNames[0]), decodedElementsOP);

	ASSERT_NE(decodedLatitude, nullptr);
	ASSERT_EQ(decodedLatitude->getDecodedDataInString(), "48.119662");

	// Longitude
	decodedLongitude = DecodingUtils::toDecimal(&(bytestream[3]), &valueLengthInBit, &multiplicationFactor, &representationModeForDecoding, &validRange,
												(unsigned int) 21,
												&(elementNames[1]), decodedElementsOP);

	ASSERT_NE(decodedLongitude, nullptr);
	ASSERT_EQ(decodedLongitude->getDecodedDataInString(), "15.974377");
}

TEST_F(CodecTest, DecodesCA2ToHexadecimal) {
	// Actually there is NO Element to be decoded with this method.
}

/**
 *
 */
TEST_F(CodecTest, DecodesToOctal) {
	// I021_070	- Mode3A

	unsigned char *bytestream = new unsigned char[2];
	ElementValue *decodedMode3A = nullptr;
	int valueLengthInBit = 12;
	double multiplicationFactor = 1;
	RepresentationModeEnum representationModeForDecoding = RepresentationModeEnum::Unsigned16bitIntegerValue;
	Range range;

	memset(bytestream, 0, 2);
	bytestream[0] = 0x0c;
	bytestream[1] = 0x15;
	string elementName = "Mode3A";

	// Mode3A
	decodedMode3A = DecodingUtils::toOctal(bytestream, &valueLengthInBit, &multiplicationFactor, &representationModeForDecoding, &range, (unsigned int) 21,
											&elementName, decodedElementsOP);

	ASSERT_NE(decodedMode3A, nullptr);
	ASSERT_EQ(decodedMode3A->getDecodedDataInString(), "3093");
}

/**
 *
 */
TEST_F(CodecTest, DecodesToTargetId) {
	// I021_170	- TargetID

	unsigned char *bytestream = new unsigned char[6];
	ElementValue *decodedTargetId = nullptr;
	int valueLengthInBit = 48;
	double multiplicationFactor = 1;
	RepresentationModeEnum representationModeForDecoding = RepresentationModeEnum::StringValue;

	memset(bytestream, 0, 6);
	bytestream[0] = 0x05;
	bytestream[1] = 0xa0;
	bytestream[2] = 0x71;
	bytestream[3] = 0xcb;
	bytestream[4] = 0x30;
	bytestream[5] = 0x42;
	string elementName = "Target Identification";

	// TargetID
	decodedTargetId = DecodingUtils::toTargetId(bytestream, &valueLengthInBit, &multiplicationFactor, &representationModeForDecoding, (unsigned int) 21,
												&elementName, decodedElementsOP);

	ASSERT_NE(decodedTargetId, nullptr);
	ASSERT_EQ(decodedTargetId->getDecodedDataInString(), "AZA123AB");
}

/**
 *
 */
TEST_F(CodecTest, DecodesToASCII) {
	// I062/390	- Flight Plan Related Data (CompoundItem) [CALLSIGN]

	unsigned char *bytestream = new unsigned char[7];
	ElementValue *decodedFPLCallsign = nullptr;
	int valueLengthInBit = 8;
	double multiplicationFactor = 1;
	RepresentationModeEnum representationModeForDecoding = RepresentationModeEnum::StringValue;

	memset(bytestream, 0, 8);
	bytestream[0] = 0x41;    // A (ASCII)
	bytestream[1] = 0x5a;    // Z (ASCII)
	bytestream[2] = 0x31;    // 1 (ASCII)
	bytestream[3] = 0x32;    // 2 (ASCII)
	bytestream[4] = 0x33;    // 3 (ASCII)
	bytestream[5] = 0x41;    // A (ASCII)
	bytestream[6] = 0x42;    // B (ASCII)
	string elementName = "Flight Plan Related Data";

	// FPL-Callsign
	decodedFPLCallsign = DecodingUtils::toAscii(bytestream, &valueLengthInBit, &multiplicationFactor, &representationModeForDecoding, (unsigned int) 62,
												&elementName, decodedElementsOP);

	ASSERT_NE(decodedFPLCallsign, nullptr);
	ASSERT_EQ(decodedFPLCallsign->getDecodedDataInString(), "AZ123AB");
}

/**
 *
 */
TEST_F(CodecTest, DecodesToMBData) {
	// I021_250	- MB Data

	unsigned char *bytestream = new unsigned char[7];
	ElementValue *decodedMBData = nullptr;
	int valueLengthInBit = 56;
	double multiplicationFactor = 1;
	RepresentationModeEnum representationModeForDecoding = RepresentationModeEnum::StringValue;

	memset(bytestream, 0, 7);
	bytestream[0] = 0xab;
	bytestream[1] = 0x12;
	bytestream[2] = 0x5f;
	bytestream[3] = 0x83;
	bytestream[4] = 0xe5;
	bytestream[5] = 0x6a;
	bytestream[6] = 0x23;
	string elementName = "MB Data";

	// MBData
	decodedMBData = DecodingUtils::toMBData(bytestream, &valueLengthInBit, &multiplicationFactor, &representationModeForDecoding, (unsigned int) 21,
											&elementName, decodedElementsOP);

	ASSERT_NE(decodedMBData, nullptr);
	ASSERT_EQ(decodedMBData->getDecodedDataInString(), "0xab125f83e56a23");
}
