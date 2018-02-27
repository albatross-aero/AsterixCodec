#ifndef INCLUDE_CODEC_TESTS_HPP_
#define INCLUDE_CODEC_TESTS_HPP_

#include <gtest/gtest.h>

#include <stddef.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <unordered_map>

#include "asterix_codec/CodecInterface.h"
#include "asterix_codec/DecoderInterface.h"

#include "config/CommonVariables.h"
#include "config/enum/RepresentationModeEnum.h"
#include "config/enum/ItemTypeEnum.h"
#include "config/FacadeCommonVariables.h"
#include "data_model/asterix_description/CategoryDescription.h"
#include "data_model/decoder_object_pool/ParsedRecord.h"
#include "data_model/blocking_queue/BlockingQueueManager.h"
#include "data_model/asterix_description/Item.h"
#include "utils/asterix_json_description/JsonCategoriesDescriptionLoader.h"
#include "utils/ParsingUtils.h"
#include "utils/DecodingUtils.h"
#include "utils/asterix_json_description/JsonCategoriesDescriptionLoader.h"
#include "utils/ElementEncodingUtils.h"

// Variables DEFINITION
unsigned char *ASTERIX_datastream = 0;
bool debugEnabled = false;
bool RE_itemParsingEnabled = false;
int k_numberOfDatablocks = 50;
int k_numberOfRecords = 5;
int k_numberOfItems = 45;
int k_numberOfElements = 50;

using namespace std;

class CodecTest : public ::testing::Test {
protected:
    CodecTest() {
        ASTERIX_inputFilePath = "asterix/input_pure_ast/forTest_cat21ed24_cat62ed117withSPF-edited.ast";
        ASTERIX_categoryRepositoryPath = "asterix/asterixConfiguration.json";
        common = nullptr;
        facadeCommon = nullptr;
        decoderInterface = nullptr;
    }

    virtual void SetUp() {
        // Singleton to be instantiated to use the features of the library correctly
        decoderInterface = (CodecInterface::getInstance())->getDecoderInterface();
        common = CommonVariables::getInstance();
        facadeCommon = FacadeCommonVariables::getInstance();

        facadeCommon->setRefDecodingEnabled(true);
        facadeCommon->setSpfDecodingEnabled(true);
        facadeCommon->setRefEncodingEnabled(true);
        facadeCommon->setSpfEncodingEnabled(true);

        // Code here will be called immediately after the constructor (right before each test).
        ASTERIX_inputFile.open(ASTERIX_inputFilePath.c_str(), ios::in | ios::binary | ios::ate);
        ASTERIX_inputFileSize = getFileSize(ASTERIX_inputFile);
        loadASTXbytestream_rc = loadAsterixBytestreamBuffer();
        datablockLength = readDatablockLength();

        parsedElementsOP = new ParsedRecord(40);    //40: arbitrary size of OP
        decodedElementsOP = new DecodedValueObjectPoolManager();

        status = ReturnStatus();
    }

    virtual void TearDown() {
        // Release the Object Pool used by the parsing function
        parsedElementsOP->setFreeAllElements();
        decodedElementsOP->releaseAllElementValueToOP();
        delete decodedElementsOP;
    }

    virtual ~CodecTest() {
    }

    /***********************
     * UTILITIES functions *
     ***********************/

    bool loadAsterixBytestreamBuffer() {
        bool success = false;
        ASTERIX_datastream = new unsigned char[ASTERIX_inputFileSize];
        if (ASTERIX_inputFile.read((char *) ASTERIX_datastream, ASTERIX_inputFileSize).gcount()) {
            success = true;
        }
        return success;
    }

    int getFileSize(ifstream &inputFile) {
        // Set a cursor at the end of file and ask for the file size
        int fileSize = inputFile.seekg(0, ios_base::end).tellg();
        //Cursor back on top of the file
        inputFile.clear();
        inputFile.seekg(0, ios_base::beg);

        return fileSize;
    }

    unsigned short int readDatablockLength() {
        return (unsigned short int) (ASTERIX_datastream[2] + ((ASTERIX_datastream[1] << 8) & 0xFF00));
    }

    void categoriesDescriptionLoadingAndFSPECreation(unsigned short int *bufferIndex, unsigned short int *bytesRead,
                                                     unsigned short int datablockCategoryNumber, CategoryDescription **categoryDescription,
                                                     vector<Item> **itemCollection, vector<int> *FRN_list) {
        unordered_map<int, CategoryDescription *>::iterator categoryDescription_it;

        JsonCategoriesDescriptionLoader::loadAsterixDescription(ASTERIX_categoryRepositoryPath, &categoriesDescription,
                                                                &status);

        categoryDescription_it = categoriesDescription.find(datablockCategoryNumber);
        ASSERT_NE(categoryDescription_it, categoriesDescription.end()) << "The fallback action on the default Category description has failed.";

        // Get the description of the given Category, thanks to 'categoryDescription_it'
        *categoryDescription = categoryDescription_it->second;
        // Get the description of all the Items of the given Category
        *itemCollection = (*categoryDescription)->getItemCollection();

        EXPECT_GE((*itemCollection)->size(), 1);
        if (datablockCategoryNumber == 21) {
            ASSERT_EQ((*itemCollection)->size(), 49);
        } else if (datablockCategoryNumber == 62) {
            ASSERT_EQ((*itemCollection)->size(), 35);
        }

        *bytesRead += ParsingUtils::readFspec(ASTERIX_datastream, (*bufferIndex) + (*bytesRead), FRN_list);

        if (datablockCategoryNumber == 21) {
            ASSERT_EQ(*bytesRead, 10);
        } else if (datablockCategoryNumber == 62) {
            ASSERT_EQ(*bytesRead, 8);
        }

        EXPECT_GE((*FRN_list).size(), 1);

        if (datablockCategoryNumber == 21) {
            ASSERT_EQ((*FRN_list).size(), 43);
        } else if (datablockCategoryNumber == 62) {
            ASSERT_EQ((*FRN_list).size(), 23);
        }
    }

    void skipAllParsing(unsigned short int bufferIndex, unsigned short int datablockCategoryNumber, int FRN, int alreadyParsedBytes) {
        unsigned short int bytesRead = 3;

        CategoryDescription *categoryDescription = nullptr;
        std::vector<Item> *itemsDescription = nullptr;
        std::vector<int> FRN_list = std::vector<int>();
        std::vector<Item> *compoundSubItemsDescription = nullptr;
        std::vector<Item> *refItemDescription = nullptr;
        std::vector<Item> *spfItemDescription = nullptr;

        Item *itemToParse = nullptr;

        categoriesDescriptionLoadingAndFSPECreation((unsigned short int *) (&bufferIndex), &bytesRead, datablockCategoryNumber, &categoryDescription,
                                                    &itemsDescription, &FRN_list);

        compoundSubItemsDescription = categoryDescription->getCompoundSubItemsDescription();
        refItemDescription = categoryDescription->getRefItemDescription();
        spfItemDescription = categoryDescription->getSpfItemDescription();

        itemToParse = &(itemsDescription->at(FRN - 1));
        bytesRead += alreadyParsedBytes;

        switch (itemToParse->getItemType()) {
            case ItemTypeEnum::FixedLength:
                ParsingUtils::parseFixedLengthItem(ASTERIX_datastream, itemToParse, bufferIndex + bytesRead, true, parsedElementsOP);
                break;
            case ItemTypeEnum::ExtendedLength:
                ParsingUtils::parseExtendedLengthItem(ASTERIX_datastream, itemToParse, bufferIndex + bytesRead, true, parsedElementsOP);
                break;
            case ItemTypeEnum::RepetitiveLength:
                ParsingUtils::parseRepetitiveLengthItem(ASTERIX_datastream, itemToParse, bufferIndex + bytesRead, true, parsedElementsOP);
                break;
            case ItemTypeEnum::CompoundLength:
                ParsingUtils::parseCompoundLengthItem(ASTERIX_datastream, itemToParse, bufferIndex + bytesRead, true, compoundSubItemsDescription,
                                                      datablockCategoryNumber, parsedElementsOP);
                break;
            case ItemTypeEnum::ExplicitLength:
                ParsingUtils::parseExplicitLengthItem(ASTERIX_datastream, itemToParse, bufferIndex + bytesRead, true, refItemDescription,
                                                      compoundSubItemsDescription, spfItemDescription, datablockCategoryNumber, parsedElementsOP);
                break;
            default:
                break;
        }
    }

    /**************
     * ATTRIBUTES *
     **************/

    CommonVariables *common;
    FacadeCommonVariables *facadeCommon;
    DecoderInterface *decoderInterface;

    ifstream ASTERIX_inputFile;
    string ASTERIX_inputFilePath;
    string ASTERIX_categoryRepositoryPath;
    int ASTERIX_inputFileSize = -1;

    unsigned short int datablockLength = -1;
    unordered_map<int, CategoryDescription *> categoriesDescription;        // key: Category number - value: obj with description of all items
    bool loadASTXbytestream_rc = false;

    ParsedRecord *parsedElementsOP = nullptr;
    DecodedValueObjectPoolManager *decodedElementsOP = nullptr;

    ReturnStatus status;
};

#endif /* INCLUDE_CODEC_TESTS_HPP_ */
