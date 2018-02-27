#ifndef FACADE_ENCODERINTERFACE_H_
#define FACADE_ENCODERINTERFACE_H_

#include <future>
#include <vector>
#include <fstream>
#include <iostream>

#include "Record.h"
#include "RecordCollection.h"
#include "config/CommonVariables.h"
#include "config/FacadeCommonVariables.h"
#include "thread_pool/ThreadPool.hpp"

using namespace std;

/**
 * Provide access to the encoding functions.
 */
class EncoderInterface {
    friend class CodecInterface;
    // A friend class is a class whose members have access to the private or protected members of another class
    // -> it is a way to prevent the instantiation or the release of instance from other classes other than CodecInterface class

public:

    /**
     * ENCODE - Encode a collection of records giving back to the client a raw bytestream.
     * @param recordsToEncode Collection of records to encode
     * @param recordNumberEncoded Number of Record encoded
     * @return The raw bytestream with the encoded datablocks (one Record for each Datablock). If print on AST file is enabled,
     * it returns nullptr.
     */
    unsigned char *encodeRecords(RecordCollection *recordsToEncode, unsigned int *recordNumberEncoded);

    /**
     * ENCODE - Encode a single Record, giving back to the client a raw bytestream.
     * @param recordToEncode Record to encode
     * @param recordNumberEncoded Number of Record encoded
     * @return The raw bytestream with the Datablock hosting the single Record to encode
     */
    unsigned char *encodeSingleRecord(Record *recordToEncode, unsigned int *recordNumberEncoded);

    /**
     * ENCODE - Return a pointer to the FailureReport object build during the encoding phase.
     */
    FailureReport *getFinalReport();

private:

    /**
     * SETUP - Provides the instance of the interface to the API. The API is a singleton.
     * EncoderInterface is instantiable only from the CodecInterface class, thanks to the friendship property
     */
    static EncoderInterface *getInstance();

    /**
     * SETUP - Remove the instance of the Singleton, it is possible only from the CodecInterface class, thanks to the friendship property
     */
    static void deleteInstance();

    /**
     * ENCODE - Prints on a .ast file the encoded bytestream.
     * @param ASTERIX_bytestream Input ASTERIX bytestream to print on the file
     */
    void printASTOnFile(unsigned char *ASTERIX_bytestream);

    /**
     * Constructor.<br>
     * The API interface is a singleton therefore this method is private.
     */
    EncoderInterface();

    /**
     * Copy constructor.
     */
    EncoderInterface(const EncoderInterface &source) = delete;

    /**
     *  Copy assignment operator.
     */
    EncoderInterface &operator=(const EncoderInterface &source) = delete;

    /**
     * Destructor.<br>
     * The API interface is a singleton therefore this method is private
     */
    ~EncoderInterface();

    /**
     * Singleton private instance
     */
    static EncoderInterface *singletonDecoder;

    /**
     * Pointer to the singleton that manage common variables.
     */
    CommonVariables *common;

    /**
     * Pointer to the singleton that manage common variables shared with DecoderInterface.
     */
    FacadeCommonVariables *facadeCommon;

    /**
     * Variable to collect all results of the encoder.
     */
    vector<future<unsigned char *>> encoderResults;

    /**
     * File to write all results of the encoder.
     */
    ofstream outFile;
};

#endif /* FACADE_ENCODERINTERFACE_H_ */
