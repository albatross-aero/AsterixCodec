#ifndef FACADE_DECODERINTERFACE_H_
#define FACADE_DECODERINTERFACE_H_

#include <fstream>
#include <future>
#include <vector>

#include "RecordCollection.h"
#include "asterix_codec/facade/reports/ReturnStatus.hpp"
#include "config/FacadeCommonVariables.h"
#include "data_model/blocking_queue/BlockingQueueManager.h"
#include "data_model/decoder_object_pool/DecodedValueObjectPoolManager.h"
#include "utils/decoderForParsedRecord.h"
#include "utils/ParsingUtils.h"

using namespace std;

/**
 * Provide access to the encoding functions.
 */
class DecoderInterface {
    friend class CodecInterface;
    // A friend class is a class whose members have access to the private or protected members of another class
    // -> it is a way to prevent the instantiation or the release of instance from other classes other than CodecInterface class

public:

    /**
     * SETUP - Store in the DecoderInterface the input for the 'decodingDatablocksFromFile' methods.
     * @param _ASTERIX_datastream Pointer to the ASTERIX bytestream opened from the client
     * @param _ASTinput_fileSize Size in byte of the input ASTERIX file
     */
    void setSourceDatastream(unsigned char *_ASTERIX_datastream, unsigned long _ASTinput_fileSize);

    /**
     * DECODE - Prints the decoded records onto a set of JSON files.
     * @param recordsToPrint Parsed record to be printed
     */
    void printAllDecodedRecordsOnJson(RecordCollection *recordsToPrint);

    /**
     * TODO
     * DECODE - Parses the ASTERIX bytestream already loaded into memory.
     * @param ASTERIX_datastream Pointer to the ASTERIX bytestream into memory
     * @param lengthOfBytestreamToParse Size in byte of the input file stream
     * @return Collection of decoded records
     */
    RecordCollection *
    startDecodingDatablocks(unsigned char *ASTERIX_datastream, unsigned int *lengthOfBytestreamToParse);


    unsigned int setupDecodingFromFile();

    /**
     * DECODE - Parses and decodes a given maximum number of datablocks in the input file.
     *
     * The method is responsible for:
     * <ul>
     *  <li> the reading of the LEN datablock field (bytes 2 and 3, right after the CAT field)
     *  <li> the calculation of the exact number of datablock that can be entirely parsed, in the input file provided
     * </ul>
     * @return a collection of decoded Records
     */
    RecordCollection *decodeFromFile();

    /*
     * DECODE - Releases all decoded records to be filled with other records. Function to be used after the client has consumed
     * the records inside ConsumerResult object.
     * @param consumerResult Result to be released
     */
    void releaseAllDecodedRecords(RecordCollection *consumerResult);

private:

    /**
     * Provides the instance of the interface to the API. The API is a singleton.
     * DecoderInterface is instantiable only from the CodecInterface class, thanks to the friendship property
     */
    static DecoderInterface *getInstance();

    /**
     * SETUP - Remove the instance of the Singleton, it is possible only from the CodecInterface class, thanks to the friendship property
     */
    static void deleteInstance();

    /**
     * Constructor.<br>
     * The API interface is a singleton therefore this method is private.
     */
    DecoderInterface();

    /**
     * Copy constructor.
     */
    DecoderInterface(const DecoderInterface &source) = delete;

    /**
     *  Copy assignment operator.
     */
    DecoderInterface &operator=(const DecoderInterface &source) = delete;

    /**
     * Destructor.<br>
     * The API interface is a singleton therefore this method is private
     */
    ~DecoderInterface();

    /**
     * DECODE - Starts all consumer threads.
     */
    void startDecoderThreads();

    /**
     * DECODE - Waits for the completion of all consuming threads.
     * @return Collection of decoded records
     */
    RecordCollection *waitForConsumer();

    /**
     * Singleton private instance
     */
    static DecoderInterface *singletonDecoder;

    /**
     * Variable to keep in memory the ASTERIX bytestream
     */
    unsigned char *ASTERIX_bytestream;

    /**
     * Variable to keep in memory the size in byte of the 'ASTERIX_datastream'
     */
    unsigned int inputFile_sizeInByte;

    /**
     * Main index to move around the 'ASTERIX_datastream' buffer
     */
    unsigned int asterixBytestream_index = 0;


    /**
     * Variable to collect all results of the PARSER threads (i.e. number of parsed records).
     */
    vector<future<unsigned long long>> parsingResults;

    /**
     * Vector of object pool used only during the decoding phase. Is a collector for results before building a DecoderResult
     * object and return it to the client.
     */
    vector<DecodedValueObjectPoolManager *> decodedValueOPManagerVector;

    /**
     * Variable to collect all results of the DECODER threads (i.e. an object with the decoded records inside).
     */
    vector<future<RecordCollection *>> decodingResults;
};

#endif /* FACADE_DECODERINTERFACE_H_ */
