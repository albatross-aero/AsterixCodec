#ifndef CONFIG_FACADECOMMONVARIABLES_H_
#define CONFIG_FACADECOMMONVARIABLES_H_

#include <iostream>

#include "thread_pool/ThreadPool.hpp"

using namespace std;

/**
 * This class contains common variables shared between Encoder and Decoder interfaces (e.g. values provided by the user)
 */
class FacadeCommonVariables {
public:

    static FacadeCommonVariables *getInstance();

    static void deleteInstance();

    //GETTERS AND SETTERS
    bool isRefDecodingEnabled();

    void setRefDecodingEnabled(bool refDecodingEnabled);

    bool isSpfDecodingEnabled();

    void setSpfDecodingEnabled(bool spfDecodingEnabled);

    bool isRefEncodingEnabled();

    void setRefEncodingEnabled(bool refEncodingEnabled);

    bool isSpfEncodingEnabled();

    void setSpfEncodingEnabled(bool spfEncodingEnabled);

    bool isFilterDecodingEnabled();

    void setFilterDecodingEnabled(bool filterDecodingEnabled);

    bool isFilterEncodingEnabled();

    void setFilterEncodingEnabled(bool filterEncodingEnabled);

    unsigned short getTotalNumberOfThread();

    void setTotalNumberOfThread(unsigned short _numberOfThreads);

    unsigned short getNumberOfProducerThreads();

    void setNumberOfProducerThreads(unsigned short _numberOfThreads);

    unsigned short getNumberOfDecodingThreads();

    void setNumberOfConsumerThreads(unsigned short _numberOfThreads);

    unsigned short getNumberOfEncoderThreads();

    void setNumberOfEncoderThreads(unsigned short _numberOfThreads);

    unsigned int getNumberOfElementsInPreallocatedParsedRecord();

    void setNumberOfElementsInPreallocatedParsedRecord(unsigned int _numberOfElementsInPreallocatedParsedRecord);

    unsigned int getNumberOfPreallocatedParsedRecord();

    void setNumberOfPreallocatedParsedRecord(unsigned int _numberOfPreallocatedParsedRecord);

    unsigned int getMaximumNumberOfDatablock();

    void setMaximumNumberOfDatablock(unsigned int _maximumNumberOfDatablock);

    unsigned int getAverageNumberOfRecordPerDatablock();

    void setAverageNumberOfRecordPerDatablock(unsigned int _averageNumberOfRecordPerDatablock);

    ThreadPool *getConsumerThreadPool();

    void setConsumerThreadPool(ThreadPool *consumer);

    ThreadPool *getParserThreadPool();

    void setProducerThreadPool(ThreadPool *producer);

    bool isJsonOutputEnableForDecoder();

    void setJsonOutputEnableForDecoder(bool _jsonOutputEnableForDecoder);

    bool isDumpOnFileEnableForDecoder();

    bool isAstOutputEnableForEncoder();

    void setAstOutputEnableForEncoder(bool _astOutputEnableForEncoder);

    void setDumpOnFileEnableForDecoder(bool _dumpOnFileEnableForDecoder);


    // OTHER METHODS
    void printCommonParameters();

private:
    FacadeCommonVariables();

    // Copy constructor
    FacadeCommonVariables(const FacadeCommonVariables &source) = delete;

    // Copy assignment operator
    FacadeCommonVariables &operator=(const FacadeCommonVariables &source) = delete;

    ~FacadeCommonVariables();

    // Instance of the Singleton
    static FacadeCommonVariables *commonSingleton;

    // Command line parameters
    bool filter_decodingEnabled, filter_encodingEnabled;
    bool REF_decodingEnabled, SPF_decodingEnabled;
    bool REF_encodingEnabled, SPF_encodingEnabled;

    bool jsonOutputEnableForDecoder, dumpOnFileEnableForDecoder, astOutputEnableForEncoder;

    unsigned int numberOfElementsInPreallocatedParsedRecord;       // Number of elements in one record
    unsigned int numberOfPreallocatedParsedRecord;

    unsigned int maximumNumberOfDatablockToDecode;    // Maximum number of datablocks to parse/decode
    unsigned int averageNumberOfRecordPerDatablock;

    // Other variables
    unsigned short int totalNumberOfThreads;
    unsigned short int numberOfProducerThreads;        // Number of threads in the Thread Pool for the producer
    unsigned short int numberOfConsumerThreads;        // Number of threads in the Thread Pool for the consumer
    unsigned short int numberOfEncoderThreads;

    /**
     * Threading variable to keep in memory the pool of threads for producer.
     */
    ThreadPool *producer;

    /**
     * Threading variable to keep in memory the pool of threads for consumer.
     */
    ThreadPool *consumer;
};

#endif /* CONFIG_FACADECOMMONVARIABLES_H_ */
