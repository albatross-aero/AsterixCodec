#include "BitUtils.h"

void BitUtils::shiftRight(unsigned char *arrayToShift, int arraySize, int shiftInBit) {
	int carry = 0;
	//   x x x 1 || A x x x x x x

	// 'carry' lets you to move the value '1' from the end of the first byte, to the MSB 'A' of the following byte
	while (shiftInBit--) {
		for (int i = 0; i <= arraySize - 1; ++i) {
			int nextCarry = 0;
			if (i != arraySize - 1)
				nextCarry = (arrayToShift[i] & 1) ? 0x80 : 0;
			arrayToShift[i] = carry | (arrayToShift[i] >> 1);

			carry = nextCarry;
		}
	}
}

void BitUtils::shiftLeft(unsigned char *arrayToShift, int arraySize, int shiftInBit) {
	int carry = 0;
	//   x x x A || 1 x x x x x x

	// 'carry' lets you to move the value '1' from the beginning of the first byte, to the LSB 'A' of the following byte

	while (shiftInBit--) {
		for (int i = arraySize - 1; i >= 0; --i) {
			int nextCarry = 0;
			if (i != 0) {
				if (arrayToShift[i] & 0x80) {    // byte like: 1000 0000
					nextCarry = 0x01;
				} else
					nextCarry = 0x0;
			}
			//nextCarry = (arrayToShift[i] & 1) ? 0x80 : 0;
			arrayToShift[i] = carry | (arrayToShift[i] << 1);

			carry = nextCarry;
		}
	}
	/*while (shiftInBit--) {
	 for (int i = 0; i <= arraySize - 1; ++i) {
			int nextCarry = 0;
			if (i != 0) {
				if (arrayToShift[i] & 0x80) {    // byte like: 1000 0000
					nextCarry = 0x01;
				} else
					nextCarry = 0x0;
			}
			//nextCarry = (arrayToShift[i] & 1) ? 0x80 : 0;
	 arrayToShift[i] = (carry | arrayToShift[i]) << 1;

			carry = nextCarry;
		}
	 }*/
}

char *BitUtils::buildMask(int bytesNum, int bitOneNum) {
	// Variables definition
	char *mask = new char[bytesNum];
	int bytesNeeded;

	for (int i = 0; i < bytesNum; i++)
		mask[i] = 0;

	bytesNeeded = bitOneNum / 8;
	if (bitOneNum % 8 > 0)
		bytesNeeded++;
	for (int i = bytesNeeded - 1; i >= 0; --i) {
		for (int y = 0; y < 8 && y < bitOneNum; ++y) {
			mask[bytesNum - i - 1] = mask[bytesNum - i - 1] << 1 | 1;
		}
	}
	return mask;
}
