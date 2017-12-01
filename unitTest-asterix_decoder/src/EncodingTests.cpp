#include "CodecTests.hpp"

TEST_F(CodecTest, EncodesDecimal) {
	// I021_010	- SAC

	unsigned char *encodedSac = nullptr;
	UnsignedInteger8bitValue *sacToEncode = new UnsignedInteger8bitValue(decodedElementsOP, 50, RepresentationModeEnum::Unsigned8bitIntegerValue);
//	Range validRange = Range(0, 255);
	Range validRange;
	validRange.minValue = 0;
	validRange.maxValue = 255;
	string elementName = "SAC";

	encodedSac = ElementEncodingUtils::encodeDecimal(sacToEncode, &validRange, 8, 1, &elementName, (unsigned int) 21);

	ASSERT_NE(encodedSac, nullptr);
	ASSERT_EQ(*encodedSac, 0x32);
}

TEST_F(CodecTest, EncodesDecimalToCA2) {
	// I021_130	- Latitude

	unsigned char* encodedLatitude = nullptr;
	FloatValue* latitudeToEncode = new FloatValue(decodedElementsOP, 48.1196618241);
//	Range validRange = Range(-90, 90);
	Range validRange;
	validRange.minValue = -90;
	validRange.maxValue = 90;
	string elementName = "Latitude";

	encodedLatitude = ElementEncodingUtils::encodeDecimalToCA2(latitudeToEncode, &validRange, 24, 0.00002145767, &elementName, (unsigned int) 21);

	// Latitude
	ASSERT_NE(encodedLatitude, nullptr);
	ASSERT_EQ(encodedLatitude[0], 0x22);
	ASSERT_EQ(encodedLatitude[1], 0x37);
	ASSERT_EQ(encodedLatitude[2], 0xeb);
}

TEST_F(CodecTest, EncodesTargetId) {
	// I021_170	- TargetId

	unsigned char* encodedTargetId = nullptr;
	StringValue* targetIdToEncode = new StringValue(decodedElementsOP, "AZA123AB");

	encodedTargetId = ElementEncodingUtils::encodeTargetId(targetIdToEncode, 48);

	ASSERT_NE(encodedTargetId, nullptr);
	ASSERT_EQ(encodedTargetId[0], 0x05);
	ASSERT_EQ(encodedTargetId[1], 0xa0);
	ASSERT_EQ(encodedTargetId[2], 0x71);
	ASSERT_EQ(encodedTargetId[3], 0xcb);
	ASSERT_EQ(encodedTargetId[4], 0x30);
	ASSERT_EQ(encodedTargetId[5], 0x42);
}

TEST_F(CodecTest, EncodesAscii) {
	// I062_390 | Flight Plan Related Data (CompoundItem) [CALLSIGN]

	unsigned char* encodedCallsign = nullptr;
	StringValue* callsignToEncode = new StringValue(decodedElementsOP, "HDI181G");

	encodedCallsign = ElementEncodingUtils::encodeAscii(callsignToEncode, 56);

	ASSERT_NE(encodedCallsign, nullptr);
	ASSERT_EQ(encodedCallsign[0], 0x48);
	ASSERT_EQ(encodedCallsign[1], 0x44);
	ASSERT_EQ(encodedCallsign[2], 0x49);
	ASSERT_EQ(encodedCallsign[3], 0x31);
	ASSERT_EQ(encodedCallsign[4], 0x38);
	ASSERT_EQ(encodedCallsign[5], 0x31);
	ASSERT_EQ(encodedCallsign[6], 0x47);

}

TEST_F(CodecTest, EncodesMBData) {
	// I021_250	- MB Data

	unsigned char* encodedMBData = nullptr;
	StringValue* MBDataToEncode = new StringValue(decodedElementsOP, "ab125f83e56a23");

	encodedMBData = ElementEncodingUtils::encodeMBData(MBDataToEncode, 56);

	ASSERT_NE(encodedMBData, nullptr);
	ASSERT_EQ(encodedMBData[0], 0xab);
	ASSERT_EQ(encodedMBData[1], 0x12);
	ASSERT_EQ(encodedMBData[2], 0x5f);
	ASSERT_EQ(encodedMBData[3], 0x83);
	ASSERT_EQ(encodedMBData[4], 0xe5);
	ASSERT_EQ(encodedMBData[5], 0x6a);
	ASSERT_EQ(encodedMBData[6], 0x23);
}
