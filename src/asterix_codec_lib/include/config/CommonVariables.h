#ifndef INCLUDE_COMMON_HPP_
#define INCLUDE_COMMON_HPP_

#include <atomic>
#include <string>
#include <unordered_map>
#include <vector>

#include "data_model/asterix_description/CategoryDescription.h"

using namespace std;

/**
 * This class contains common variables used during the decoding and encoding phase.
 */
class CommonVariables {

public:

    static CommonVariables *getInstance();

    static void deleteInstance();

    //GETTERS AND SETTERS
    unordered_map<int, CategoryDescription *> *getCategoriesDescription();

    void setCategoriesDescription(unordered_map<int, CategoryDescription *> *categoriesDescription);

    unsigned int getIntegerValuePoolSize();

    unsigned int getInteger8bitValuePoolSize();

    unsigned int getInteger16bitValuePoolSize();

    unsigned int getInteger32bitValuePoolSize();

    unsigned int getInteger64bitValuePoolSize();

    unsigned int getFloatValuePoolSize();

    unsigned int getDoubleValuePoolSize();

    unsigned int getStringValuePoolSize();

    unsigned int getOctalValuePoolSize();

    unsigned int getHexadecimalValuePoolSize();

    unsigned int getSizeDecodedValueOP();

    void setSizeDecodedValueOP(unsigned int _sizeDecodedValueOP);

    unsigned int getMaximumNumberOfRecordInJSON();

    unsigned int getNumberOfElementsRepetitionInItemDescription();

    unsigned char *getMasks();

    string getOutputJsonPathForDecoder();

    string getOutputTxtPathForDecoder();

    string getOutputAstPathForEncoder();

    vector<char> *getIcaoAlphabet();

    unsigned int getActualNumberOfOutFiles();

    Item *getSacSicItem();

    bool isDebugEnabledForDecoder();

    void setDebugEnabledForDecoder(bool debugEnabled);

    bool isEncodingDebugEnabled();

    void setEncodingDebugEnabled(bool _encodingDebugEnabled);

    bool isDebugConsumerObjectPoolEnabled();

    void setDebugConsumerObjectPoolEnabled(bool _debugObjectPoolEnabledConsumer);

    bool isDebugProducerObjectPoolEnabled();

    void setDebugProducerObjectPoolEnabled(bool _debugObjectPoolEnabledProducer);

    bool isDebugDecodedValueEnabled();

    void setDebugDecodedValueEnabled(bool _debugObjectPoolEnabledDecodedValue);

    vector<string> *getSupportedFunctionNameForDecoding();

    vector<string> *getRepresentationModeForDecodingList();

    // OTHER METHODS
    void printCommonParameters();

private:

    CommonVariables();

    // Copy constructor
    CommonVariables(const CommonVariables &source) = delete;

    // Copy assignment operator
    CommonVariables &operator=(const CommonVariables &source) = delete;

    ~CommonVariables();

    // Instance of the Singleton
    static CommonVariables *commonSingleton;

    /**
     * Map containing the ASTERIX Description accessible by a key.
     * The key is made of the Category number (to be found at the beginning of a Datablock)
     * or the concatenation of Category-SAC-SIC to customize the decoding for a specific Datasource.
     */
    unordered_map<int, CategoryDescription *> *categoriesDescription;

    unsigned int integerValuePoolSize;
    unsigned int integer8bitValuePoolSize;
    unsigned int integer16bitValuePoolSize;
    unsigned int integer32bitValuePoolSize;
    unsigned int integer64bitValuePoolSize;
    unsigned int floatValuePoolSize;
    unsigned int doubleValuePoolSize;
    unsigned int stringValuePoolSize;
    unsigned int octalValuePoolSize;
    unsigned int hexadecimalValuePoolSize;

    unsigned int sizeDecodedValueOP;

    unsigned short int maximumNumberOfRecordInJSON;

    unsigned short int numberOfElementsRepetitionInItemDescription;    // used for repetitive item -
    // Store number of repetitions created for the base element - each elements that compose a RepetitiveItem is duplicate numberOfElementsRepetitionInItemDescription times

    unsigned char *masks;

    string outputJsonPathForDecoder;
    string outputTxtPathForDecoder;
    string outputAstPathForDecoder;

    vector<char> icaoAlphabet;

    vector<string> supportedFunctionNameForDecoding;

    vector<string> supportedRepresentationModeForDecoding;

    atomic<int> actualNumberOfOutFiles;    // Needed for consumer threads to print on different output file

    Item *sacSicItem;                            // used for store the value of Item that contains SAC and SIC

    bool debugEnabledForDecoder, debugEnabledForEncoder;

    bool debugConsumerObjectPoolEnabled;        // enabled by the class constructor
    bool debugProducerObjectPoolEnabled;        // enabled by the class constructor
    bool debugDecodedValuesEnabled;    // enabled by the class constructor
};

#endif /* INCLUDE_COMMON_HPP_ */
