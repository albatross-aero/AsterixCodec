#ifndef FACADE_CODECINTERFACE_H_
#define FACADE_CODECINTERFACE_H_

#include <future>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <dirent.h>

#include "DecoderInterface.h"
#include "EncoderInterface.h"
#include "RecordCollection.h"
#include "data_model/blocking_queue/BlockingQueueManager.h"
#include "data_model/decoder_object_pool/DecodedValueObjectPoolManager.h"
#include "asterix_codec/facade/reports/return_codes.h"
#include "asterix_codec/facade/reports/ReturnStatus.hpp"
#include "asterix_codec/facade/reports/FailureReport.h"
#include "utils/asterix_json_description/JsonCategoriesDescriptionLoader.h"

using namespace std;

/*! \mainpage This is the description of the ASTERIX CODEC API - EIRS
 *
 *CodecInterface class provide access to the ASTERIX all function for the set up.<br>
 *EncoderInterface class provide access to the ASTERIX all encoding function.<br>
 *DecoderInterface class provide access to the ASTERIX all decoding function.<br>
 *DecoderResult class provide access to the decoded records, the output of the library.<br>
 *ReturnStatus class provide access to a return message and a code to better understand what happened while using API.<br>
 *return_codes.h provides a description of return codes contained into the ReturnStatus class.<br>
 *
 *These classes encapsulate access to point and return message to the API.<br>
 *
 *The API has 3 types of service:
 *	- Setup methods of the library: Setup services are flagged with SETUP
 *
 *	- Decode ASTERIX: Decode services are flagged with DECODE
 *	- Encode ASTERIX: Encode services are flagged with ENCODE
 *<br><br>
 *
 *More information:<br>
 *
 *Setup methods of the library:
 *
 *- Loading ASTERIX Configuration <br>
 *
 *  	- ASTERIX Configuration File<br>
 *  	The library needs to load ASTERIX Descriptions from an ASTERIX Configuration file.
 *  	This file can either be a JSON file as described <a href="http://mana-new/srs-codec/codec-wiki/index.php/Asterix_Configuration_File">here</a>
 *  	or it can be loaded from a serialized ASTERIX Configuration (a file with the extension .dat).<br>
 *  	The first operation for user of the library is to load an ASTERIX Configuration.
 * 		ASTERIX Configuration file contains the path of ASTERIX Category description, Reserved Expansion Field description and Special Purpose Field description.
 * 		These are called ASTERIX description files.
 *		<br><br>
 *
 *  	- ASTERIX Description Files<br>
 *  	ASTERIX Description file are the translation into a dedicated JSON style of ASTERIX Specifications (available <a href="http://www.eurocontrol.int/asterix-specifications-library">here</a>).<br>
 *  	The grammar to be used is described <a href="http://mana-new/srs-codec/codec-wiki/index.php/JSON_input_files">here</a>.
 *  	<br><br>
 *
 *  	- Internal Configuration of the library<br>
 *     	The library embarks for speed purpose a Thread Pool and an Object Pool. Depending on the computing capacities and the amount of it to be given
 *     	to the CODEC, these methods can be used.<br>
 *		TODO: Document more the Object Pool and Thread Pool configuration from an API user point of view <br><br>
 *
 *Decode ASTERIX:
 *- Decoding functions<br>
 *  Decoding function will turn the ASTERIX binary into more DecoderResult objects usable by external software.
 *
 *Encode ASTERIX:
 *- Encoding functions<br>
 *  Encoding function will turn the DecoderResult objects a ASTERIX binary.
 *
 *Success/Error Return:
 *- Link to ReturnStatus and its return codes return_codes.h for details on return messages.
 *
 */

class CodecInterface {
public:

    /**
     * Provides the instance of the interface to the API. The API is a singleton.
     */
    static CodecInterface *getInstance();

    /**
     * DECODE - It provides the instance of the interface of the library to access to the Decoding features.
     */
    DecoderInterface *getDecoderInterface();

    /**
     * ENCODE - It provides the instance of the interface of the library to access to the Decoding features.
     */
    EncoderInterface *getEncoderInterface();

    /**
     * SETUP - Loads ASTERIX Configurations from the given path and check if it is an Archive (Serialization) or a JSON Asterix configuration file.
     * @param pathToAsterixConfigurationFile Path where to find the repository
     * @param status Object filled with a return message and a code indicating what happens while using that function (Default: 0 - SUCCESS)
     */
    void loadAsterixConfiguration(std::string pathToAsterixConfigurationFile, ReturnStatus &status);

    /**
     * SETUP - Serializes the ASTERIX JSON Descriptions
     * @param pathToArchiveFile Path where to find the JSON repository
     * @param status Object filled with a return message and a code indicating what happens while using that function (Default: 0 - SUCCESS)
     */
    void serializeAsterixConfiguration(std::string pathToArchiveFile, ReturnStatus &status);

    /**
     * SETUP - Sets the number of elements available inside a single Object Pool (Default: 150).
     * @param poolSize Number of elements
     */
    void setObjectPoolSize(unsigned int poolSize);

    /**
     * SETUP - Sets the total number of Object Pool available for the system during the parsing phase (Default: 1).
     * @param poolNumber Number of Object Pool
     */
    void setObjectPoolNumber(unsigned int poolNumber);

    /**
     * SETUP - Sets the number of datablocks to be decoded and returned together in an object to the client (Default: 1000).
     * @param numberOfDatablock Number of datablock to be decoded
     */
    void setMaximumNumberOfDatablock(unsigned int numberOfDatablock);

    /**
     * SETUP - Sets an average number of records per datablock. Number needed for static allocation at startup (Default: 5).
     * @param averageNumberOfRecordPerDatablock Records per datablock
     */
    void setAverageNumberOfRecordPerDatablock(unsigned int averageNumberOfRecordPerDatablock);

    /**
     * SETUP - Sets the number of threads used by the producer.
     * @param nThreads Number of threads
     */
    void setNumberOfProducerThreads(unsigned short nThreads);

    /**
     * SETUP - Sets the number of threads used by the consumer.
     * @param nThreads Number of threads
     */
    void setNumberOfConsumerThreads(unsigned short nThreads);

    /**
     * SETUP - Sets the total number of threads used by the consumer and the producer.
     * @param nThreads Number of threads
     */
    void setTotalNumberOfThreads(unsigned short nThreads);

    /**
     * SETUP - Sets the number of threads used by the encoder.
     * @param encoderThreads Number of threads
     */
    void setNumberOfEcoderThreads(unsigned short encoderThreads);

    /**
     * DECODE - Toggles the decoding of the RE item (Default: disabled).
     * @param active Boolean value to activate/deactivate
     */
    void enableReservedExpansionForDecoding(bool active);    // SETTER

    /**
     * DECODE - Checks if the REF item decoding is activated.
     * @return True/false based on the flag to activate/deactivate REF decoding
     */
    bool isReservedExpansionForDecodingEnabled();    // GETTER

    /**
     * DECODE - Toggles the SPF decoding (Default: disabled).
     * @param active Boolean value to activate/deactivate
     */
    void enableSpecialPurposeFieldForDecoding(bool active);    // SETTER

    /**
     * DECODE - Checks if the SP item decoding is activated.
     * @return True/false based on the flag to activate/deactivate SPF decoding
     */
    bool isSpecialPurposeFieldForDecodingEnabled();    // GETTER

    /**
     * ENCODE - Toggles the encoding of the RE item (Default: disabled).
     * @param active Boolean value to activate/deactivate
     */
    void enableReservedExpansionForEncoding(bool active);    // SETTER

    /**
     * ENCODE - Checks if the RE item encoding is activated.
     * @return True/false based on the flag to activate/deactivate REF decoding
     */
    bool isReservedExpansionForEncodingEnabled();    // GETTER

    /**
     * ENCODE - Toggles the SPF encoding (Default: disabled).
     * @param active Boolean value to activate/deactivate
     */
    void enableSpecialPurposeFieldForEncoding(bool active);    // SETTER

    /**
     * ENCODE - Checks if the SP item encoding is activated.
     * @return True/false based on the flag to activate/deactivate SPF decoding
     */
    bool isSpecialPurposeFieldForEncodingEnabled();    // GETTER

    /**
     * DECODE - Toggles the FILTERING for the decoding (Default: disabled).
     * @param active Boolean value to activate/deactivate
     */
    void enableFilteringForDecoding(bool active);    // SETTER

    /**
     * DECODE - Checks if the filtering for the decoding is activated.
     * @return True/false based on the flag to activate/deactivate
     */
    bool isFilteringForDecodingEnabled();    // GETTER

    /**
     * ENCODE - Toggles the FILTERING for the encoding (Default: disabled).
     * @param active Boolean value to activate/deactivate
     */
    void enableFilteringForEncoding(bool active);    // SETTER

    /**
     * ENCODE - Checks if the filtering for the encoding is activated.
     * @return True/false based on the flag to activate/deactivate
     */
    bool isFilteringForEncodingEnabled();    // GETTER

    /**
     * DECODE - Toggles the output on JSON files for the decoder(Default: disabled).
     * @param active Boolean value to activate/deactivate
     */
    void enableJsonOutputCreationForDecoder(bool active);    // SETTER

    /**
     * DECODE - Checks if JSON output is activated for the decoder.
     * * @return True/false based on the flag to activate/deactivate output to JSON files
     */
    bool isJsonOutputCreationForDecoderEnabled();    // GETTER

    /**
     * DECODE - Toggles the output on TXT files for the decoder (Default: disabled).
     * @param active Boolean value to activate/deactivate
     */
    void enableDumpOnFileForDecoder(bool active);

    /**
     * DECODE - Checks if TXT output is activated for the decoder.
     * * @return True/false based on the flag to activate/deactivate output to JSON files
     */
    bool isDumpOnFileForDecoderEnable();

    /**
     * ENCODE - Toggles the output on AST files for the encoder (Default: disabled).
     * @param active Boolean value to activate/deactivate
     */
    void enableAstOutputCreationForEncoder(bool active);

    /**
     * ENCODE - Checks if TXT output is activated for the encoder.
     * * @return True/false based on the flag to activate/deactivate output to JSON files
     */
    bool isAstOutputCreationForEncoderEnabled();

    /**
     * DECODE - Toggles Debug traces.
     * @param active Boolean value to activate/deactivate
     */
    void enableDebugForDecoder(bool active);

    /**
     * ENCODE - Toggles Debug traces.
     * @param active Boolean value to activate/deactivate
     */
    void enableDebugForEncoder(bool active);

    /**
     * SETUP - Checks if all parameters already set by the client are compliant with the minimum performances rules, initialize the blocking
     * queue for both producer and consumer, start thread pool for consumer waiting for the producer results and create the output
     * JSON directory if needed.
     * @param status Object filled with a return message and a code indicating what happens while using that function (Default: 0 - SUCCESS)
     */
    void setUpEnvironment(ReturnStatus &status);

    /**
     * SETUP - Releases the dynamically allocated memory, reserved during the Codec life cycle.
     */
    void releaseMemory();

    /**
     * SETUP - Return a pointer to the FailureReport object build during the decoding and encoding phase.
     */
    FailureReport *getFinalReport();

private:

    /**
     * Constructor.<br>
     * The API interface is a singleton therefore this method is private.
     */
    CodecInterface();

    /**
     * Copy constructor.<br>
     * The API interface is a singleton therefore this method is private.
     */
    CodecInterface(const CodecInterface &source) = delete;

    /**
     *  Copy assignment operator.
     *  The API interface is a singleton therefore this method is private.
     */
    CodecInterface &operator=(const CodecInterface &source) = delete;

    /**
     * Destructor.<br>
     * The API interface is a singleton therefore this method is private
     */
    ~CodecInterface();

    /**
     * Remove the instance of the Singleton.
     */
    static void deleteInstance();

    /**
     * Singleton private instance
     */
    static CodecInterface *codecInterface;

    DecoderInterface *decoderInterface;
    EncoderInterface *encoderInterface;

    unsigned char *ASTERIX_datastream;
    unsigned long ASTinput_fileSize;

    unsigned long indexInASTERIX_datastream = 0;            // Main index to move around the 'ASTERIX_bytestream' buffer

    /**
     * Pointer to the singleton that manage all common variables, including the parameters set by the user.
     */
    CommonVariables *common;                // Pointer to the Singleton

    FacadeCommonVariables *facadeCommon;

    /**
     * Singleton that manage access to the blocking queue for both producer side and consumer side. It is thread safe.
     */
    BlockingQueueManager *bqManager;        // Pointer to the Singleton

    /**
     * Variable to collect all results of the producer threads (i.e. number of parsed records).
     */
    vector<future<unsigned long long>> producerResults;

    /**
     * Vector of object pool used only during the decoding phase. Is a collector for results before building a DecoderResult
     * object and return it to the client.
     */
    vector<DecodedValueObjectPoolManager *> decodedValueOPManagerVector;

    /**
     * Variable to collect all results of the consumer threads (i.e. an object with the decoded records inside).
     */
    vector<future<RecordCollection *>> consumerResults;

    bool isTotalNumberSetted, isSingleNumberSetted;
};

#endif /* FACADE_CODECINTERFACE_H_ */
