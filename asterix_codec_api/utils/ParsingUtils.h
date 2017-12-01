#ifndef UTILS_PARSINGUTILS_H_
#define UTILS_PARSINGUTILS_H_

#include <config/CommonVariables.h>
#include <config/Constants.h>
#include <config/FacadeCommonVariables.h>
#include <vector>

#include "data_model/asterix_description/CategoryDescription.h"
#include "data_model/asterix_description/Item.h"
#include "data_model/blocking_queue/BlockingQueueManager.h"
#include "data_model/decoder_object_pool/ParsedRecord.h"
#include "facade/reports/FailureReport.h"
#include "utils/PrintingUtils.h"
#include "utils/BitUtils.h"

class ParsingUtils {
	public:

		/**
		 * It parses an entire Datablock of ASTERIX.
		 * @param ASTERIX_datablock Pointer to the ASTERIX bytestream
		 * @param categoriesDescription Pointer to the description of ASTERIX already loaded in memory
		 * @param datablockCategory Category number of the datablock to be parsed
		 * @param datablockLength Length in byte of the datablock to be parsed
		 * @param bufferIndex Position of the datablock into the ASTERIX bytestream
		 * @return Number of records read
		 */
		static unsigned long long parseDatablock(unsigned char *ASTERIX_datablock, unsigned short int datablockCategory, int datablockLength,
				unordered_map<int, CategoryDescription*>* categoriesDescription, int bufferIndex);

		/**
		 * It parses all records into a Datablock of ASTERIX.
		 * @param ASTERIX_datablock Pointer to the ASTERIX bytestream
		 * @param bufferIndex Position of the datablock into the ASTERIX bytestream
		 * @param datablockCategory Category number of the datablock to be parsed
		 * @param datablockLength Length in byte of the datablock to be parsed
		 * @param categoriesDescription Pointer to the description of ASTERIX already loaded in memory
		 * @param threadId Thread id where the funciton is running
		 * @return Number of records read
		 */
		static int parseAllRecords(unsigned char *ASTERIX_datablock, int bufferIndex, int datablockCategory, int datablockLength,
				unordered_map<int, CategoryDescription*>* categoriesDescription, unsigned long int threadId);

		/**
		 * It parses an element, the smallest part of an ASTERIX message saving the result in the 'octets' parameter.
		 * @param lengthInBits Length in bits of the element
		 * @param offset Offset of the element inside its item (i.e. I021/010 - offset of SIC = 8, offset of SAC = 0)
		 * @param octetsSize Size in bytes of 'octets' parameter
		 * @param octets Pointer where the element value is located and where the element value is parsed and saved (this parameter acts as input and output)
		 */
		static void parseElement(int lengthInBits, int offset, int octetsSize, unsigned char *octets);

		/**
		 * It parses the FSPEC from the data stream and it fills a list of FRN numbers
		 * of the items to be parsed.
		 * @param ASTERIX_datablock Pointer to the input ASTERIX bytestream to be parsed
		 * @param bufferIndex Start index inside the data stream, where the FSPEC to be parsed is located
		 * @param FRN_list Reference to a list filled with item to be parsed
		 * @return Number of bytes read (i.e. FSPEC length)
		 */
		static int readFspec(unsigned char *ASTERIX_datablock, int bufferIndex, std::vector<int>* FRN_list);

		/**
		 * It parses SAC/SIC to build up the key to lookup in the Category description already loaded in memory.
		 * @param ASTERIX_datablock Pointer to the input ASTERIX bytestream to be parsed
		 * @param index Start index inside the data stream, where the SAC/SIC bytes to be parsed are located
		 * @return Integer representation of SAC/SIC in the format \<SAC in Dec\>\<SIC in Dec\>.
		 */
		static int parseFirstTwoBytes(unsigned char *ASTERIX_datablock, int index);

		/*
		 * It parses a fixed length Item, given the description of a Item and a index that indicate where the item is located inside the data stream.
		 * @param ASTERIX_datablock Pointer to the input ASTERIX bytestream to be parsed
		 * @param itemToParse Pointer to the item description loaded in memory used to parse the current item
		 * @param index Start index inside the data stream, where the item to be parsed is located
		 * @param isItemToBeDecoded Boolean flag set via the filtering JSON file, it indicates if the item is to filter out or not
		 * @param objectPool Pointer to the object pool where the parsed item is saved
		 * @return Length in bytes of the parsed Item
		 */
		static int parseFixedLengthItem(unsigned char *ASTERIX_datablock, Item* itemToParse, int index, bool isItemToBeDecoded, ParsedRecord* objectPool);

		/**
		 * It parses an Explicit Length Item, given the description of a Item and an index that indicate where the item is located inside the data stream.
		 * @param ASTERIX_datablock Pointer to the beginning of the current ASTERIX datablock
		 * @param itemToParse Pointer to the description of the item to be parsed
		 * @param index Index inside the data stream, where the item to parse is located
		 * @param isItemToBeDecoded Boolean flag set via the filtering JSON file, it indicates if the item is to filter out or not
		 * @param objectPool Object pool where the parsed data are saved
		 * @return Length in bytes of the parsed item
		 */
		static int parseExtendedLengthItem(unsigned char *ASTERIX_datablock, Item* itemToParse, int index, bool isItemToBeDecoded, ParsedRecord* objectPool);

		/**
		 * Parses a repetitive Item, given the description of a Item and an index that indicate where the item is located inside the data stream.
		 * @param ASTERIX_datablock Pointer to the input ASTERIX bytestream to be parsed
		 * @param itemToParse Pointer to the item description loaded in memory used to parse the current item
		 * @param index Start index inside the data stream, where the item to be parsed is located
		 * @param isItemToBeDecoded Boolean flag set via the filtering JSON file, it indicates if the item is to filter out or not
		 * @param objectPool Pointer to the object pool where the parsed item is saved
		 * @return Length in bytes of the parsed Item
		 */
		static int parseRepetitiveLengthItem(unsigned char *ASTERIX_datablock, Item* itemToParse, int index, bool isItemToBeDecoded, ParsedRecord* objectPool);

		/**
		 * Parses a compound Item, given the description of a Item and an index that indicate where the item is located inside the data stream.
		 * @param ASTERIX_datablock Pointer to the input ASTERIX bytestream to be parsed
		 * @param itemToParse Pointer to the item description loaded in memory used to parse the current item
		 * @param index Start index inside the data stream, where the item to be parsed is located
		 * @param isItemToBeDecoded Boolean flag set via the filtering JSON file, it indicates if the item is to filter out or not
		 * @param compoundSubItemsDescription Pointer to a vector that contains all compound sub-items of the current Category
		 * @param categoryNumber Category number that the currentItem belongs to
		 * @param objectPool Pointer to the object pool where the parsed item is saved
		 * @return Length in bytes of the parsed Item
		 */
		static int parseCompoundLengthItem(unsigned char *ASTERIX_datablock, Item* itemToParse, int index, bool isItemToBeDecoded,
				std::vector<Item> *compoundSubItemsDescription, unsigned int categoryNumber, ParsedRecord* objectPool);

		/**
		 * Parses an Explicit Length Item, given the description of a Item and an index that indicate where the item is located inside the data stream.
		 * @param ASTERIX_datablock Pointer to the input ASTERIX bytestream to be parsed
		 * @param itemToParse Pointer to the item description loaded in memory used to parse the current item
		 * @param index Start index inside the data stream, where the item to be parsed is located
		 * @param isItemToBeDecoded Boolean flag set via the filtering JSON file, it indicates if the item is to filter out or not
		 * @param refItemDescription Pointer to a vector that contains all REF sub-items
		 * @param subItemsDescription Pointer to a vector that contains all compound sub-items of the current Category
		 * @param spfItemDescription Pointer to a vector that contains all SPF sub-items
		 * @param categoryNumber Category number that the currentItem belongs to
		 * @param objectPool Pointer to the object pool where the parsed item is saved
		 * @return Length in bytes of the parsed Item
		 */
		static int parseExplicitLengthItem(unsigned char *ASTERIX_datablock, Item* itemToParse, int index, bool isItemToBeDecoded,
				std::vector<Item>* refItemDescription, std::vector<Item>* subItemsDescription, std::vector<Item>* spfItemDescription,
				unsigned int categoryNumber, ParsedRecord* objectPool);

		/**
		 * Calculate the size of an extended length Item given the and an index that indicate where the item is located inside the data stream.
		 * @param ASTERIX_datablock Pointer to the input ASTERIX bytestream to be parsed
		 * @param index Start index inside the data stream, where the item to be parsed is located
		 * @param primarySubfieldLength Size in bytes of the primary subfield
		 * @return Length in bytes of the extended Item
		 */
		static int calculateExtendedItemLength(unsigned char *ASTERIX_datablock, int index, int primarySubfieldLength);

	private:

		ParsingUtils();

		virtual ~ParsingUtils() {

		}

		/**
		 * Static fields are shared amongst all the instance of the class
		 */
		static CommonVariables* commonVariables;
		static FacadeCommonVariables* facadeCommonVariables;
		static BlockingQueueManager* bqManager;
		static FailureReport *failureReport;
};

#endif /* UTILS_PARSINGUTILS_H_ */
