#ifndef UTILS_BITUTILS_H_
#define UTILS_BITUTILS_H_

class BitUtils {

	public:
		/**
		 * It shifts to the right the bits of 'arrayToShift' by the number of bits specified in shiftInBit.
		 * @param arrayToShift Pointer to the array to be shifted
		 * @param arraySize Size of the specified array
		 * @param shiftInBit Number of bits for the right shift
		 */
		static void shiftRight(unsigned char *arrayToShift, int arraySize, int shiftInBit);

		/**
		 * It shifts to the left the bits of 'arrayToShift' by the number of bits specified in shiftInBit.
		 * @param arrayToShift Pointer to the array to be shifted
		 * @param arraySize Size of the specified array
		 * @param shiftInBit Number of bits for the left shift
		 */
		static void shiftLeft(unsigned char *arrayToShift, int arraySize, int shiftInBit);

		/**
		 * It builds a mask, given the mask width and it set to '1' a given number of bits, starting from the LSB.
		 * @param bytesNum Total number of bytes of the mask
		 * @param bitOneNum Number of '1' to be set inside the mask
		 * @return Pointer to the created mask
		 */
		static char *buildMask(int bytesNum, int bitOneNum);

	private:
		BitUtils();
		virtual ~BitUtils();
};

#endif /* UTILS_BITUTILS_H_ */
