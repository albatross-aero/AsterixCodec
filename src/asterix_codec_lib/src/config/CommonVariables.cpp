#include "config/CommonVariables.h"

// Initialization of static field 'commonSingleton'
CommonVariables *CommonVariables::commonSingleton = nullptr;

CommonVariables::CommonVariables() {
    debugEnabledForDecoder = false;
    debugEnabledForEncoder = false;

    categoriesDescription = new unordered_map<int, CategoryDescription *>();

    supportedFunctionNameForDecoding = vector<string> {
            "toDecimal",
            "toHexadecimal",
            "CA2toDecimal",
            "CA2toHexadecimal",
            "toOctal",
            "toTargetId",
            "toASCII",
            "toMBData"};

    supportedRepresentationModeForDecoding = vector<string> {
            "IntegerValue",
            "UnsignedInteger8bitValue",
            "UnsignedInteger16bitValue",
            "UnsignedInteger32bitValue",
            "UnsignedInteger64bitValue",
            "SignedInteger8bitValue",
            "SignedInteger16bitValue",
            "SignedInteger32bitValue",
            "SignedInteger64bitValue",
            "FloatValue",
            "DoubleValue",
            "StringValue"};

    numberOfElementsRepetitionInItemDescription = 5;

    debugProducerObjectPoolEnabled = false;
    debugConsumerObjectPoolEnabled = false;
    debugDecodedValuesEnabled = false;
    sizeDecodedValueOP = 1;

    masks = (unsigned char *) malloc(9);
    masks[0] = 0x00;
    masks[1] = 0x01;
    masks[2] = 0x03;
    masks[3] = 0x07;
    masks[4] = 0x0F;
    masks[5] = 0x1F;
    masks[6] = 0x3F;
    masks[7] = 0x7F;
    masks[8] = 0xFF;

    icaoAlphabet = vector<char> {
            '?', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
            'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S',
            'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '?', '?', '?',
            '?', '?', ' ', '?', '?', '?', '?', '?', '?', '?',
            '?', '?', '?', '?', '?', '?', '?', '?', '0', '1',
            '2', '3', '4', '5', '6', '7', '8', '9', '?', '?',
            '?', '?', '?', '?'
    };

    sacSicItem = new Item();
    sacSicItem->setName("Datasource");
    sacSicItem->setType(ItemTypeEnum::FixedLength);
    sacSicItem->setLengthInByte(2);
    sacSicItem->setAllElementDescription(vector<int> {8, 8});
    sacSicItem->setAllElementNames(vector<string> {"SAC", "SIC"});
    sacSicItem->setAllFunctionNamesForDecodingInString(vector<string> {"toDecimal", "toDecimal"});
    sacSicItem->setAllFunctionNamesInEnumForDecoding(
            vector<FunctionNameEnum> {FunctionNameEnum::toDecimal, FunctionNameEnum::toDecimal});

    vector<double> lsbValueOfElement{1};
    vector<vector<double>> lsbValues;
    lsbValues.push_back(lsbValueOfElement);
    lsbValues.push_back(lsbValueOfElement);

    sacSicItem->setAllLsbValues(lsbValues);

    sacSicItem->setAllRepresentationModesInStringForDecoding(vector<string> {"IntegerValue", "IntegerValue"});
    sacSicItem->setAllRepresentationModesInEnumForDecoding(
            vector<RepresentationModeEnum> {RepresentationModeEnum::IntegerValue,
                                            RepresentationModeEnum::IntegerValue});

    actualNumberOfOutFiles = 0;
    maximumNumberOfRecordInJSON = 5000;

    outputJsonPathForDecoder = "output";
    outputTxtPathForDecoder = "dump";
    outputAstPathForDecoder = "bytestream.ast";

    integerValuePoolSize = 100;
    integer8bitValuePoolSize = 50;
    integer16bitValuePoolSize = 10;
    integer32bitValuePoolSize = 10;
    integer64bitValuePoolSize = 5;
    floatValuePoolSize = 60;
    doubleValuePoolSize = 10;
    stringValuePoolSize = 10;
    octalValuePoolSize = 2;
    hexadecimalValuePoolSize = 10;
}

CommonVariables *CommonVariables::getInstance() {
    if (!commonSingleton) {
        commonSingleton = new CommonVariables();
    }
    return commonSingleton;
}

void CommonVariables::deleteInstance() {
    delete commonSingleton;
}

CommonVariables::~CommonVariables() {
    if (masks != nullptr) {
        free(masks);
        masks = nullptr;
    }

    if (sacSicItem != nullptr) {
        delete sacSicItem;
        sacSicItem = nullptr;
    }
}

//GETTERS AND SETTERS
unordered_map<int, CategoryDescription *> *CommonVariables::getCategoriesDescription() {
    return categoriesDescription;
}

void CommonVariables::setCategoriesDescription(unordered_map<int, CategoryDescription *> *_categoriesDescription) {
    categoriesDescription = _categoriesDescription;
}

unsigned int CommonVariables::getIntegerValuePoolSize() {
    return integerValuePoolSize * sizeDecodedValueOP;
}

unsigned int CommonVariables::getInteger8bitValuePoolSize() {
    return integer8bitValuePoolSize * sizeDecodedValueOP;
}

unsigned int CommonVariables::getInteger16bitValuePoolSize() {
    return integer16bitValuePoolSize * sizeDecodedValueOP;
}

unsigned int CommonVariables::getInteger32bitValuePoolSize() {
    return integer32bitValuePoolSize * sizeDecodedValueOP;
}

unsigned int CommonVariables::getInteger64bitValuePoolSize() {
    return integer64bitValuePoolSize * sizeDecodedValueOP;
}

unsigned int CommonVariables::getFloatValuePoolSize() {
    return floatValuePoolSize * sizeDecodedValueOP;
}

unsigned int CommonVariables::getDoubleValuePoolSize() {
    return doubleValuePoolSize * sizeDecodedValueOP;
}

unsigned int CommonVariables::getStringValuePoolSize() {
    return stringValuePoolSize * sizeDecodedValueOP;
}

unsigned int CommonVariables::getOctalValuePoolSize() {
    return octalValuePoolSize * sizeDecodedValueOP;
}

unsigned int CommonVariables::getHexadecimalValuePoolSize() {
    return hexadecimalValuePoolSize * sizeDecodedValueOP;
}

unsigned int CommonVariables::getSizeDecodedValueOP() {
    return sizeDecodedValueOP;
}

void CommonVariables::setSizeDecodedValueOP(unsigned int _sizeDecodedValueOP) {
    sizeDecodedValueOP = _sizeDecodedValueOP;
}

unsigned int CommonVariables::getMaximumNumberOfRecordInJSON() {
    return maximumNumberOfRecordInJSON;
}

unsigned int CommonVariables::getNumberOfElementsRepetitionInItemDescription() {
    return numberOfElementsRepetitionInItemDescription;
}

unsigned char *CommonVariables::getMasks() {
    return masks;
}

string CommonVariables::getOutputJsonPathForDecoder() {
    return outputJsonPathForDecoder;
}

string CommonVariables::getOutputTxtPathForDecoder() {
    return outputTxtPathForDecoder;
}

string CommonVariables::getOutputAstPathForEncoder() {
    return outputAstPathForDecoder;
}

vector<char> *CommonVariables::getIcaoAlphabet() {
    return &icaoAlphabet;
}

unsigned int CommonVariables::getActualNumberOfOutFiles() {
    actualNumberOfOutFiles++;
    return actualNumberOfOutFiles;
}

Item *CommonVariables::getSacSicItem() {
    return sacSicItem;
}

bool CommonVariables::isDebugEnabledForDecoder() {
    return debugEnabledForDecoder;
}

void CommonVariables::setDebugEnabledForDecoder(bool decodingDebugEnabled) {
    this->debugEnabledForDecoder = decodingDebugEnabled;
}

bool CommonVariables::isEncodingDebugEnabled() {
    return debugEnabledForEncoder;
}

void CommonVariables::setEncodingDebugEnabled(bool _encodingDebugEnabled) {
    debugEnabledForEncoder = _encodingDebugEnabled;
}

bool CommonVariables::isDebugConsumerObjectPoolEnabled() {
    return debugConsumerObjectPoolEnabled;
}

void CommonVariables::setDebugConsumerObjectPoolEnabled(bool _debugObjectPoolEnabledConsumer) {
    debugConsumerObjectPoolEnabled = _debugObjectPoolEnabledConsumer;
}

bool CommonVariables::isDebugProducerObjectPoolEnabled() {
    return debugProducerObjectPoolEnabled;
}

void CommonVariables::setDebugProducerObjectPoolEnabled(bool _debugObjectPoolEnabledProducer) {
    debugProducerObjectPoolEnabled = _debugObjectPoolEnabledProducer;
}

bool CommonVariables::isDebugDecodedValueEnabled() {
    return debugDecodedValuesEnabled;
}

void CommonVariables::setDebugDecodedValueEnabled(bool _debugObjectPoolEnabledDecodedValue) {
    debugDecodedValuesEnabled = _debugObjectPoolEnabledDecodedValue;
}

vector<string> *CommonVariables::getSupportedFunctionNameForDecoding() {
    return &supportedFunctionNameForDecoding;
}

vector<string> *CommonVariables::getRepresentationModeForDecodingList() {
    return &supportedRepresentationModeForDecoding;
}

// OTHER METHODS
void CommonVariables::printCommonParameters() {
    cout << "\t\t debugEnabled: " << std::boolalpha << debugEnabledForDecoder << endl << "\t\t encodingDebugEnabled: "
         << std::boolalpha
         << debugEnabledForEncoder << endl
         << "\t\t debugObjectPoolEnabledConsumer: " << std::boolalpha
         << debugConsumerObjectPoolEnabled << endl << "\t\t debugObjectPoolEnabledProducer: " << std::boolalpha
         << debugProducerObjectPoolEnabled
         << endl
         << "\t\t debugObjectPoolEnabledDecodedValue: " << std::boolalpha << debugDecodedValuesEnabled << endl;
}
