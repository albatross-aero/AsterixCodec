#include "CodecTests.hpp"

/**
 * It checks that the ASTERIX bytestream has been loaded in memory correctly.
 */
TEST_F(CodecTest, LoadsAsterixBytestream) {
    ASSERT_TRUE(ASTERIX_inputFile.is_open()) << "Error opening the ASTERIX bytestream input file.";

    ASSERT_EQ(ASTERIX_inputFileSize, 563);

    ASSERT_TRUE(loadASTXbytestream_rc);

    ASSERT_EQ(ASTERIX_datastream[0], 0x15)
                                << "Error while checking the datablock CAT number!";                        // Checks CAT No
    ASSERT_EQ(ASTERIX_datastream[1], 0x01)
                                << "Error while checking the datablock LEN most significant byte!";// Checks LEN MSByte
    ASSERT_EQ(ASTERIX_datastream[2], 0x4f)
                                << "Error while checking the datablock LEN least significant byte!";// Checks LEN LSByte
    //
    ASSERT_EQ(ASTERIX_datastream[ASTERIX_inputFileSize - 2], 0xb0)
                                << "Error while checking the second to last byte of the bytestream!";// Checks second to last Byte
    ASSERT_EQ(ASTERIX_datastream[ASTERIX_inputFileSize - 1], 0x38)
                                << "Error while checking the very last byte of the bytestream";// Checks the very last Byte
}

/**
 * It checks that the ASTERIX Category JSON description files has been loaded in memory correctly.
 */
TEST_F(CodecTest, LoadsCategoryJSONDescription) {
    JsonCategoriesDescriptionLoader::loadAsterixDescription(ASTERIX_categoryRepositoryPath, &categoriesDescription,
                                                            &status);
    ASSERT_EQ(status.getCode(), ReturnCodes::SUCCESS);
    EXPECT_GT(categoriesDescription.size(), 0);
    ASSERT_EQ(categoriesDescription.size(), 2);
}

/*
 * It collects all the known ASTERIX Category numbers and merges them in a single vector.
 * After checking its size, the test verifies that the Category number read from the bytestream belongs to the previous loaded vector.
 * Doing so, it checks that the Category number read on the bytestream is valid.
 */
TEST_F(CodecTest, DatablockCategoryIdentification) {
    vector<unsigned char> ASTERIX_StdCivilMilitaryAppCategories = {0x00, 0x01, 0x02, 0x03, 0x04, 0x07, 0x08, 0x09, 0x0a,
                                                                   0x0b, 0x0c, 0x0d, 0x0e, 0x11,
                                                                   0x12,
                                                                   0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1f, 0x20,
                                                                   0x21, 0x22, 0x30, 0x3d, 0x3e,
                                                                   0x3f, 0x41};
    vector<unsigned char> ASTERIX_SpecialAndMilitaryAppCategories = {0x80, 0x96, 0x97, 0x98, 0x99, 0xcc, 0xdd, 0xef,
                                                                     0xf0};
    vector<unsigned char> ASTERIX_NATO_ACCSAppCategories = {0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xb4, 0xc9, 0xcb};
    vector<unsigned char> ASTERIX_NoStdCivilMilitaryAppCategories = {0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
                                                                     0xf9, 0xfa, 0xfb, 0xfc, 0xfd,
                                                                     0xfe, 0xff};

    vector<unsigned char> ASTERIX_Categories;
    ASTERIX_Categories.reserve(
            ASTERIX_StdCivilMilitaryAppCategories.size() + ASTERIX_SpecialAndMilitaryAppCategories.size() +
            ASTERIX_NATO_ACCSAppCategories.size()
            + ASTERIX_NoStdCivilMilitaryAppCategories.size());

//	EXPECT_EQ(ASTERIX_StdCivilMilitaryAppCategories.size(), 31);
//	EXPECT_EQ(ASTERIX_SpecialAndMilitaryAppCategories.size(), 9);
//	EXPECT_EQ(ASTERIX_NATO_ACCSAppCategories.size(), 9);
//	EXPECT_EQ(ASTERIX_NoStdCivilMilitaryAppCategories.size(), 15);

    ASTERIX_Categories.insert(ASTERIX_Categories.end(), ASTERIX_StdCivilMilitaryAppCategories.begin(),
                              ASTERIX_StdCivilMilitaryAppCategories.end());
    ASTERIX_Categories.insert(ASTERIX_Categories.end(), ASTERIX_SpecialAndMilitaryAppCategories.begin(),
                              ASTERIX_SpecialAndMilitaryAppCategories.end());
    ASTERIX_Categories.insert(ASTERIX_Categories.end(), ASTERIX_NATO_ACCSAppCategories.begin(),
                              ASTERIX_NATO_ACCSAppCategories.end());
    ASTERIX_Categories.insert(ASTERIX_Categories.end(), ASTERIX_NoStdCivilMilitaryAppCategories.begin(),
                              ASTERIX_NoStdCivilMilitaryAppCategories.end());

    EXPECT_EQ(ASTERIX_Categories.size(), 64);

    ASSERT_NE(std::find(ASTERIX_Categories.begin(), ASTERIX_Categories.end(), ASTERIX_datastream[0]),
              ASTERIX_Categories.end())
                                << "An unknown datablock Category value has been read from the bytestream!";

    ASSERT_EQ(ASTERIX_datastream[0], 0x15);
}

/**
 * It checks that the Datablock LEN value, read from the bytestream, is below the maximum value allowed.
 */
TEST_F(CodecTest, DatablockLengthIdentification) {
    ASSERT_LE(ASTERIX_datastream[1], 0xff);        // Checks LEN MSByte
    ASSERT_LE(ASTERIX_datastream[2], 0xff);        // Checks LEN LSByte

    // A bit redundant.. this assert has already been done in the LoadsAsterixBytestream test
    //TODO: consider to remove this redundant test part
    ASSERT_EQ(ASTERIX_datastream[1], 0x01);        // Checks LEN MSByte
    ASSERT_EQ(ASTERIX_datastream[2], 0x4f);        // Checks LEN LSByte
}

/**
 * It checks the value returned by the FSPEC parsing function and then it verifies that the right number of Item has been loaded in the collection
 * that is going to be accessed during the parsing of the records.
 */
TEST_F(CodecTest, ReadDatablockFSPEC) {
    unsigned short int bytesRead = 3;
    vector<int> FRN_list = vector<int>();

    bytesRead += ParsingUtils::readFspec(ASTERIX_datastream, bytesRead, &FRN_list);
    ASSERT_EQ(bytesRead, 10);

    EXPECT_GE(FRN_list.size(), 1) << "Failed to read the FSPEC: at least one element should be marked as present!";
    ASSERT_EQ(FRN_list.size(), 43) << "Wrong number of Item has been read from the datablock FSPEC!";
}

TEST_F(CodecTest, LoadsAsterixCategoriesDescription) {
    unsigned short int bufferIndex = 0;
    unsigned short int bytesRead = 0;
    unsigned short int datablockCategory;
    int categorySacSicInt = 0;
    CategoryDescription *categoryDescription = nullptr;
    unordered_map<int, CategoryDescription *>::iterator categoryDescription_it;
    vector<Item> *itemCollection = nullptr;

    stringstream sacValue;
    stringstream sicValue;
    string categorySacSicValue;

    datablockCategory = (short int) ASTERIX_datastream[bufferIndex];
    bytesRead += 1;
    datablockLength = (short int) (ASTERIX_datastream[bufferIndex + 2] +
                                   ((ASTERIX_datastream[bufferIndex + 1] << 8) & 0xFF00));
    bytesRead += 2;

    bytesRead += 7;    // Substitutes the FSPEC reading

    // Get values of SAC and SIC from the bytestream
    sacValue << std::hex << static_cast<unsigned int>(ASTERIX_datastream[bytesRead]);
    ASSERT_EQ(sacValue.str(), "32");
    sicValue << std::hex << static_cast<unsigned int>(ASTERIX_datastream[bytesRead + 1]);
    ASSERT_EQ(sicValue.str(), "50");

    // Create key to use for lookup on categoriesDescription
    categorySacSicValue = to_string(datablockCategory) + sacValue.str() + sicValue.str();
    categorySacSicInt = stoi(categorySacSicValue, nullptr, 10);
    ASSERT_EQ(categorySacSicInt, 213250);

    JsonCategoriesDescriptionLoader::loadAsterixDescription(ASTERIX_categoryRepositoryPath, &categoriesDescription,
                                                            &status);
    ASSERT_EQ(status.getCode(), ReturnCodes::SUCCESS);

    // Search for 'categorySacSicValue' inside 'categoriesDescription' to get the description of a Category
    // This search should fail, because SAC=32 SIC=50 is not listed in AsterixConfiguration.json.
    // This means that 'categoriesDescription_it' should be EQUAL TO 'categoriesDescription.end()'.
    categoryDescription_it = categoriesDescription.find(categorySacSicInt);
    ASSERT_EQ(categoryDescription_it, categoriesDescription.end())
                                << "SAC=32 SIC=50 has been found on \"asterixConfiguration.json!\", even if it shouldn't be there.";

    // Search for 'datablockCategory' (fallback on default case) inside 'categoriesDescription' to get the description of a Category
    categoryDescription_it = categoriesDescription.find(datablockCategory);
    ASSERT_NE(categoryDescription_it, categoriesDescription.end())
                                << "The fallback action on the default Category description has failed.";

    // Get the description of the given Category, thanks to 'categoryDescription_it'
    categoryDescription = categoryDescription_it->second;
    ASSERT_NE(categoryDescription, nullptr);

    // Get the description of all the Items of the given Category
    itemCollection = categoryDescription->getItemCollection();
    EXPECT_GE(itemCollection->size(), 1);
    ASSERT_GE(itemCollection->size(), 48);
}

/**
 * Taking from the byteStream the first Fixed Length Item, it checks that parsing is correct.
 */
TEST_F(CodecTest, ParsesFixedLengthItem) {
    unsigned short int bufferIndex = 0, bytesRead = 3;
    vector<int> FRN_list = vector<int>();
    short int FRN = -1;

    CategoryDescription *categoryDescription = nullptr;
    vector<Item> *itemsDescription = nullptr;
    Item *itemToParse = nullptr;
    categoriesDescriptionLoadingAndFSPECreation(&bufferIndex, &bytesRead, (unsigned short int) 21, &categoryDescription,
                                                &itemsDescription,
                                                &FRN_list);

    FRN = FRN_list[0];
    EXPECT_GT(FRN, 0);
    ASSERT_LE(FRN, itemsDescription->size()) << "FRN #" << FRN << " not found." << endl;

    itemToParse = &(itemsDescription->at(FRN - 1));
    ASSERT_NE(itemToParse->getItemType(), ItemTypeEnum::InvalidValue);
    ASSERT_EQ(itemToParse->getName(), "I021_010");
    ASSERT_EQ(itemToParse->getItemType(), ItemTypeEnum::FixedLength);
    ASSERT_EQ(itemToParse->getLengthInByte(), 2);

    bytesRead += ParsingUtils::parseFixedLengthItem(ASTERIX_datastream, itemToParse, bufferIndex + bytesRead, true,
                                                    parsedElementsOP);

    ASSERT_EQ(bytesRead, 12);

    // Check the content of the filled item in memory
    ASSERT_EQ(parsedElementsOP->getObjectPoolCurrentSize(), 2);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getName(), "SAC");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getStringValue(), "32");

    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(1)->getName(), "SIC");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(1)->getStringValue(), "50");
}

/**
 * Taking from the bytestream the first Extended Length Item, it checks that parsing is correct.
 */
TEST_F(CodecTest, ParsesExtendedLengthItem) {
    unsigned short int bufferIndex = 0, bytesRead = 3;
    vector<int> FRN_list = vector<int>();
    short int FRN = -1;
    CategoryDescription *categoryDescription = nullptr;
    vector<Item> *itemsDescription = nullptr;
    Item *itemToParse = nullptr;
    categoriesDescriptionLoadingAndFSPECreation(&bufferIndex, &bytesRead, (unsigned short int) 21, &categoryDescription,
                                                &itemsDescription,
                                                &FRN_list);

    // Let's skip whatever comes before the 1st ExtendedLength Item..
    bytesRead += 2;        // skipping FRN=1 (I021_010)

    FRN = FRN_list[1];
    EXPECT_GT(FRN, 0);
    ASSERT_LE(FRN, itemsDescription->size()) << "FRN #" << FRN << " not found." << endl;

    itemToParse = &(itemsDescription->at(FRN - 1));
    ASSERT_NE(itemToParse->getItemType(), ItemTypeEnum::InvalidValue);
    ASSERT_EQ(itemToParse->getName(), "I021_040");
    ASSERT_EQ(itemToParse->getItemType(), ItemTypeEnum::ExtendedLength);
    ASSERT_EQ(itemToParse->getLengthInByte(), 1);

    bytesRead += ParsingUtils::parseExtendedLengthItem(ASTERIX_datastream, itemToParse, bufferIndex + bytesRead, true,
                                                       parsedElementsOP);
    ASSERT_EQ(bytesRead, 15);

    // Checking the amount of Elements in the filled Item
    ASSERT_EQ(parsedElementsOP->getObjectPoolCurrentSize(), 16);

    // Checking the Element names..
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getName(), "ATP");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(1)->getName(), "ARC");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(2)->getName(), "RC");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(3)->getName(), "RAB");
    // FX
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(4)->getName(), "DCR");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(5)->getName(), "GBS");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(6)->getName(), "SIM");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(7)->getName(), "TST");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(8)->getName(), "SAA");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(9)->getName(), "CL");
    // FX
    // Spare
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(10)->getName(), "LLC");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(11)->getName(), "IPC");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(12)->getName(), "NOGO");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(13)->getName(), "CPR");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(14)->getName(), "LDPJ");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(15)->getName(), "RFC");
    // FX

    // Checking the Element (undecoded) values..
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getParsedValue()[0], 0x3);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(1)->getParsedValue()[0], 0x2);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(2)->getParsedValue()[0], 0x0);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(3)->getParsedValue()[0], 0x0);
    // FX
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(4)->getParsedValue()[0], 0x0);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(5)->getParsedValue()[0], 0x1);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(6)->getParsedValue()[0], 0x0);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(7)->getParsedValue()[0], 0x0);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(8)->getParsedValue()[0], 0x1);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(9)->getParsedValue()[0], 0x0);
    // FX
    // Spare
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(10)->getParsedValue()[0], 0x0);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(11)->getParsedValue()[0], 0x1);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(12)->getParsedValue()[0], 0x0);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(13)->getParsedValue()[0], 0x1);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(14)->getParsedValue()[0], 0x1);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(15)->getParsedValue()[0], 0x0);
    // FX
}

/**
 * Taking from the bytestream the first Repetitive Length Item, it checks that parsing is correct.
 */
TEST_F(CodecTest, ParsesRepetitiveLengthItem) {
    unsigned short int bufferIndex = 0, bytesRead = 3;
    vector<int> FRN_list = vector<int>();
    short int FRN = -1;
    CategoryDescription *categoryDescription = nullptr;
    vector<Item> *itemsDescription = nullptr;

    unordered_map<string, CategoryDescription *>::iterator categoryDescription_it;
    Item *itemToParse = nullptr;

    categoriesDescriptionLoadingAndFSPECreation(&bufferIndex, &bytesRead, (unsigned short int) 21, &categoryDescription,
                                                &itemsDescription,
                                                &FRN_list);

    // Let's skip whatever comes before the 1st RepetitiveLength Item..
    bytesRead += 129;    // From FRN=1 to FRN=38 included

    FRN = FRN_list[38];
    EXPECT_GT(FRN, 0);
    ASSERT_LE(FRN, itemsDescription->size()) << "FRN #" << FRN << " not found." << endl;

    itemToParse = &(itemsDescription->at(FRN - 1));
    ASSERT_NE(itemToParse->getItemType(), ItemTypeEnum::InvalidValue);
    ASSERT_EQ(itemToParse->getName(), "I021_250");
    ASSERT_EQ(itemToParse->getItemType(), ItemTypeEnum::RepetitiveLength);
    ASSERT_EQ(itemToParse->getLengthInByte(), 8);

    bytesRead += ParsingUtils::parseRepetitiveLengthItem(ASTERIX_datastream, itemToParse, bufferIndex + bytesRead, true,
                                                         parsedElementsOP);
    ASSERT_EQ(bytesRead, 148);

    // Checking the content of the filled Item
    ASSERT_EQ(parsedElementsOP->getObjectPoolCurrentSize(), 3);
    // 1st Element
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getName(), "MB DATA");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getParsedValue()[0], 0xab);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getParsedValue()[1], 0x12);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getParsedValue()[2], 0x5f);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getParsedValue()[3], 0x83);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getParsedValue()[4], 0xe5);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getParsedValue()[5], 0x6a);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getParsedValue()[6], 0x23);
    // 2nd Element
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(1)->getName(), "BDS1");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(1)->getParsedValue()[0], 0x6);
    // 3rd Element
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(2)->getName(), "BDS2");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(2)->getParsedValue()[0], 0xf);
}

/**
 * Taking from the bytestream the first Compound Length Item, it checks that parsing is correct.
 */
TEST_F(CodecTest, ParsesCompoundItem) {
    unsigned short int bufferIndex = 0, bytesRead = 3;
    vector<int> FRN_list = vector<int>();
    short int FRN = -1;
    CategoryDescription *categoryDescription = nullptr;
    unordered_map<string, CategoryDescription *>::iterator categoryDescription_it;
    Item *itemToParse = nullptr;
    vector<Item> *itemsDescription = nullptr;
    vector<Item> *compoundSubItemsDescription = nullptr;

    categoriesDescriptionLoadingAndFSPECreation(&bufferIndex, &bytesRead, (unsigned short int) 21, &categoryDescription,
                                                &itemsDescription,
                                                &FRN_list);

    compoundSubItemsDescription = categoryDescription->getCompoundSubItemsDescription();

    // Let's skip whatever comes before the 1st CompoundLength Item..
    bytesRead += 84;    // From FRN=1 to FRN=30 included

    FRN = FRN_list[30];
    EXPECT_GT(FRN, 0);
    ASSERT_LE(FRN, itemsDescription->size()) << "FRN #" << FRN << " not found." << endl;

    itemToParse = &(itemsDescription->at(FRN - 1));
    ASSERT_NE(itemToParse->getItemType(), ItemTypeEnum::InvalidValue);
    ASSERT_EQ(itemToParse->getName(), "I021_220");
    ASSERT_EQ(itemToParse->getItemType(), ItemTypeEnum::CompoundLength);
    ASSERT_EQ(itemToParse->getLengthInByte(), 1);

    bytesRead += ParsingUtils::parseCompoundLengthItem(ASTERIX_datastream, itemToParse, bufferIndex + bytesRead, true,
                                                       compoundSubItemsDescription,
                                                       (unsigned int) 21, parsedElementsOP);
    ASSERT_EQ(bytesRead, 97);

    // Check the content of the filled Item
    ASSERT_EQ(parsedElementsOP->getObjectPoolCurrentSize(), 1);                // Subitem: WS
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getName(), "WS");
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getParsedValue()[0], 0x6a);
    ASSERT_EQ(parsedElementsOP->getAllParsedElementsPtr()->at(0)->getParsedValue()[1], 0xc3);
}

/**
 * Taking from the bytestream the Reserved Expansion Field, it checks that parsing is correct.
 */
TEST_F(CodecTest, ParsesRefItem) {
    unsigned short int bufferIndex = 0, bytesRead = 3;
    vector<int> FRN_list = vector<int>();
    short int FRN = -1;
    CategoryDescription *categoryDescription = nullptr;
    Item *itemToParse = nullptr;
    vector<Item> *itemsDescription = nullptr;
    vector<Item> *compoundSubItemsDescription = nullptr;
    vector<Item> *refItemDescription = nullptr;
    vector<Item> *spfItemDescription = nullptr;

    categoriesDescriptionLoadingAndFSPECreation(&bufferIndex, &bytesRead, (unsigned short int) 21, &categoryDescription,
                                                &itemsDescription,
                                                &FRN_list);

    compoundSubItemsDescription = categoryDescription->getCompoundSubItemsDescription();
    refItemDescription = categoryDescription->getRefItemDescription();
    spfItemDescription = categoryDescription->getSpfItemDescription();

    // Let's skip whatever comes before the 1st RE Item..
    bytesRead += 169;    // From FRN=1 to FRN=42 included

    FRN = FRN_list[42];
    EXPECT_GT(FRN, 0);
    ASSERT_LE(FRN, itemsDescription->size()) << "FRN #" << FRN << " not found." << endl;

    itemToParse = &(itemsDescription->at(FRN - 1));

    ASSERT_NE(itemToParse->getItemType(), ItemTypeEnum::InvalidValue);
    ASSERT_EQ(itemToParse->getName(), kREF);
    ASSERT_EQ(itemToParse->getItemType(), ItemTypeEnum::ExplicitLength);

    ASSERT_EQ(itemToParse->getLengthInByte(), 1);

    bytesRead += ParsingUtils::parseExplicitLengthItem(ASTERIX_datastream, itemToParse, bufferIndex + bytesRead, true,
                                                       refItemDescription,
                                                       compoundSubItemsDescription, spfItemDescription,
                                                       (unsigned int) 21, parsedElementsOP);

    ASSERT_EQ(bytesRead, 202);
}

/**
 *
 */
TEST_F(CodecTest, ParsesSpfItem) {
    unsigned short int bytesRead = 3;        // Let's skip whatever comes before the CAT062 datablock.. | +3 skips CAT and LEN fields..
    unsigned short int bufferIndex = 335;
    vector<int> FRN_list = vector<int>();
    short int FRN = -1;
    CategoryDescription *categoryDescription = nullptr;
    Item *itemToParse = nullptr;
    vector<Item> *itemsDescription = nullptr;
    vector<Item> *compoundSubItemsDescription = nullptr;
    vector<Item> *refItemDescription = nullptr;
    vector<Item> *spfItemDescription = nullptr;

    categoriesDescriptionLoadingAndFSPECreation(&bufferIndex, &bytesRead, (unsigned short int) 62, &categoryDescription,
                                                &itemsDescription,
                                                &FRN_list);

    compoundSubItemsDescription = categoryDescription->getCompoundSubItemsDescription();
    refItemDescription = categoryDescription->getRefItemDescription();
    spfItemDescription = categoryDescription->getSpfItemDescription();

    // Let's skip whatever comes before the SPF..
    bytesRead += 166;

    FRN = FRN_list[22];
    EXPECT_GT(FRN, 0);
    ASSERT_LE(FRN, itemsDescription->size()) << "FRN #" << FRN << " not found." << endl;

    itemToParse = &(itemsDescription->at(FRN - 1));

    ASSERT_NE(itemToParse->getItemType(), ItemTypeEnum::InvalidValue);
    ASSERT_EQ(itemToParse->getName(), kSPF);
    ASSERT_EQ(itemToParse->getItemType(), ItemTypeEnum::ExplicitLength);
    ASSERT_EQ(itemToParse->getLengthInByte(), 1);

    bytesRead += ParsingUtils::parseExplicitLengthItem(ASTERIX_datastream, itemToParse, bufferIndex + bytesRead, true,
                                                       refItemDescription,
                                                       compoundSubItemsDescription, spfItemDescription,
                                                       (unsigned int) 62, parsedElementsOP);
    ASSERT_EQ(bytesRead, 228);
}
