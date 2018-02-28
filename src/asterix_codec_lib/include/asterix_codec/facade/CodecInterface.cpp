#include "asterix_codec/CodecInterface.h"

// Initialization of the static field 'singletonDecoder'
CodecInterface *CodecInterface::codecInterface = nullptr;

/**
 * Remove all files in 'directoryPath'.
 * @param directoryPath Path of the folder to be cleaned
 */
void cleanDirectory(const char *directoryPath);

CodecInterface::CodecInterface() {
    common = CommonVariables::getInstance();
    facadeCommon = FacadeCommonVariables::getInstance();
    bqManager = nullptr;
    ASTERIX_datastream = nullptr;
    ASTinput_fileSize = 0;
    isTotalNumberSetted = false;
    isSingleNumberSetted = false;

    decoderInterface = DecoderInterface::getInstance();
    encoderInterface = EncoderInterface::getInstance();
}

CodecInterface::~CodecInterface() {
    if (!common) {
        common->deleteInstance();
        common = nullptr;
    }
    if (!facadeCommon) {
        facadeCommon->deleteInstance();
        facadeCommon = nullptr;
    }
    if (!bqManager) {
        bqManager->deleteInstance();
        bqManager = nullptr;
    }

    if (!codecInterface) {
        codecInterface->deleteInstance();
        codecInterface = nullptr;
    }

    unsigned int sizeOfVector = decodedValueOPManagerVector.size();
    DecodedValueObjectPoolManager *tmp = nullptr;

    for (unsigned int i = 0; i < sizeOfVector; i++) {
        tmp = decodedValueOPManagerVector.at(i);
        if (tmp != nullptr) {
            delete tmp;
            tmp = nullptr;
        }
    }
}

CodecInterface *CodecInterface::getInstance() {
    if (!codecInterface) {
        codecInterface = new CodecInterface();
    }
    return codecInterface;
}

void CodecInterface::deleteInstance() {
    if (codecInterface != nullptr)
        delete codecInterface;

    codecInterface = nullptr;
}

//** GETTER AND SETTERS **
DecoderInterface *CodecInterface::getDecoderInterface() {
    return decoderInterface;
}

EncoderInterface *CodecInterface::getEncoderInterface() {
    return encoderInterface;
}

void CodecInterface::setObjectPoolSize(unsigned int poolSize) {
    facadeCommon->setNumberOfElementsInPreallocatedParsedRecord(poolSize);
}

void CodecInterface::setObjectPoolNumber(unsigned int poolNumber) {
    facadeCommon->setNumberOfPreallocatedParsedRecord(poolNumber);
}

void CodecInterface::setMaximumNumberOfDatablock(unsigned int numberOfDatablock) {
    facadeCommon->setMaximumNumberOfDatablock(numberOfDatablock);
}

void CodecInterface::setAverageNumberOfRecordPerDatablock(unsigned int averageNumberOfRecordPerDatablock) {
    facadeCommon->setAverageNumberOfRecordPerDatablock(averageNumberOfRecordPerDatablock);
}

void CodecInterface::setNumberOfProducerThreads(unsigned short nThreads) {
    facadeCommon->setNumberOfProducerThreads(nThreads);
    isSingleNumberSetted = true;
}

void CodecInterface::setNumberOfConsumerThreads(unsigned short nThreads) {
    facadeCommon->setNumberOfConsumerThreads(nThreads);
    facadeCommon->setNumberOfEncoderThreads(nThreads);
    isSingleNumberSetted = true;
}

void CodecInterface::setTotalNumberOfThreads(unsigned short nThreads) {
    facadeCommon->setTotalNumberOfThread(nThreads);
    isTotalNumberSetted = true;
}

void CodecInterface::setNumberOfEcoderThreads(unsigned short encoderThreads) {
    facadeCommon->setNumberOfEncoderThreads(encoderThreads);
    facadeCommon->setNumberOfConsumerThreads(encoderThreads);
    isSingleNumberSetted = true;
}

void CodecInterface::enableReservedExpansionForDecoding(bool active) {
    facadeCommon->setRefDecodingEnabled(active);
}

void CodecInterface::enableSpecialPurposeFieldForDecoding(bool active) {
    facadeCommon->setSpfDecodingEnabled(active);
}

bool CodecInterface::isReservedExpansionForDecodingEnabled() {
    return facadeCommon->isRefDecodingEnabled();
}

bool CodecInterface::isSpecialPurposeFieldForDecodingEnabled() {
    return facadeCommon->isSpfDecodingEnabled();
}

void CodecInterface::enableReservedExpansionForEncoding(bool active) {
    facadeCommon->setRefEncodingEnabled(active);
}

void CodecInterface::enableSpecialPurposeFieldForEncoding(bool active) {
    facadeCommon->setSpfEncodingEnabled(active);
}

bool CodecInterface::isReservedExpansionForEncodingEnabled() {
    return facadeCommon->isRefEncodingEnabled();
}

bool CodecInterface::isSpecialPurposeFieldForEncodingEnabled() {
    return facadeCommon->isSpfEncodingEnabled();
}

void CodecInterface::enableFilteringForDecoding(bool active) {
    facadeCommon->setFilterDecodingEnabled(active);
}

bool CodecInterface::isFilteringForDecodingEnabled() {
    return facadeCommon->isFilterDecodingEnabled();
}

void CodecInterface::enableFilteringForEncoding(bool active) {
    facadeCommon->setFilterEncodingEnabled(active);
}

bool CodecInterface::isFilteringForEncodingEnabled() {
    return facadeCommon->isFilterEncodingEnabled();
}

void CodecInterface::enableJsonOutputCreationForDecoder(bool active) {
    facadeCommon->setJsonOutputEnableForDecoder(active);
}

bool CodecInterface::isJsonOutputCreationForDecoderEnabled() {
    return facadeCommon->isJsonOutputEnableForDecoder();
}

void CodecInterface::enableDumpOnFileForDecoder(bool active) {
    facadeCommon->setDumpOnFileEnableForDecoder(active);
}

bool CodecInterface::isDumpOnFileForDecoderEnable() {
    return facadeCommon->isDumpOnFileEnableForDecoder();
}

void CodecInterface::enableAstOutputCreationForEncoder(bool active) {
    facadeCommon->setAstOutputEnableForEncoder(active);
}

bool CodecInterface::isAstOutputCreationForEncoderEnabled() {
    return facadeCommon->isAstOutputEnableForEncoder();
}

void CodecInterface::enableDebugForDecoder(bool active) {
    common->setDebugEnabledForDecoder(active);
}

void CodecInterface::enableDebugForEncoder(bool active) {
    common->setEncodingDebugEnabled(active);
}

//** OTHER FUNCTIONS **
void CodecInterface::loadAsterixConfiguration(std::string pathToAsterixConfigurationFile, ReturnStatus &status) {
    status.clear();

    unordered_map<int, CategoryDescription *> *categoriesDescription = common->getCategoriesDescription();

    if (pathToAsterixConfigurationFile.find(".dat") != std::string::npos) {
        JsonCategoriesDescriptionLoader::getCategoriesDescriptionMapFromSerialization(pathToAsterixConfigurationFile,
                                                                                      categoriesDescription, &status);
    } else {
        JsonCategoriesDescriptionLoader::loadAsterixDescription(pathToAsterixConfigurationFile, categoriesDescription,
                                                                &status);
    }
}

void CodecInterface::serializeAsterixConfiguration(std::string pathToArchiveFile, ReturnStatus &status) {
    status.clear();
    JsonCategoriesDescriptionLoader::serializeCategoriesDescriptionMap(pathToArchiveFile,
                                                                       common->getCategoriesDescription(), &status);
}

void CodecInterface::setUpEnvironment(ReturnStatus &status) {
    unsigned int sizeDecodedValueOP = 0;
    status.clear();    // Set the current status to SUCCESS

    if (isTotalNumberSetted && isSingleNumberSetted) {
        string message =
                "Total number of threads is a mutually exclusive parameter over the number of threads for Producer, Consumer and threadsEncoder parameters."
                        " Total number of threads value is the only one considered";
        status.setCode(ReturnCodes::SPEED_ISSUE);
        status.setMessage(message);
        isSingleNumberSetted = false;
    }

    // If isTotalNumberSetted is setted -> divide number between producer and consumer
    if (isTotalNumberSetted) {
        // Divide numberOfThread in numberOfProducer and numberOfConsumer - best rate is 1:3
        unsigned short numberOfProducer = 0;
        unsigned short numberOfConsumer = 0;
        unsigned short numberOfThread = facadeCommon->getTotalNumberOfThread();

        if (numberOfThread < 2) {
            stringstream message;
            message << "Attention: the Total number of threads SHOULD be grater than 2." << endl;
            status.setCode(ReturnCodes::CRITICAL_PARAMETERS);
            status.setMessage(message.str());
            return;    // Impossible to continue, wrong input parameters
        }

        double division = numberOfThread / 4.0;    //TODO: 4 -> common variable

        numberOfProducer = division;
        numberOfConsumer = division * 3;

        double consumerDivision = (division * 3.0) - numberOfConsumer;

        if (consumerDivision > (division - numberOfProducer))
            numberOfConsumer++;
        else if (consumerDivision != 0)
            numberOfProducer++;

        if (numberOfConsumer == 0)
            numberOfConsumer += 1;

        if (numberOfProducer == 0)
            numberOfProducer += 1;

        facadeCommon->setNumberOfProducerThreads(numberOfProducer);
        facadeCommon->setNumberOfConsumerThreads(numberOfConsumer);
        facadeCommon->setNumberOfEncoderThreads(numberOfConsumer);

        // Crosscheck between number of consumer, producer and pool number:
        if ((facadeCommon->getNumberOfDecodingThreads() % facadeCommon->getNumberOfProducerThreads()) != 0) {
            string message = "To boost performance, please set the Total number of threads divisible number for 4.";
            status.setCode(ReturnCodes::SPEED_ISSUE);
            status.setMessage(message);
        }

        if (facadeCommon->getNumberOfPreallocatedParsedRecord() < facadeCommon->getNumberOfProducerThreads()) {
            string message = "Attention: the Object Pool number should be greater than or equal to the total number of thread/4";
            status.setCode(ReturnCodes::CRITICAL_PARAMETERS);
            status.setMessage(message);
            return;    // Impossible to continue, wrong input parameters
        }

    } else {
        if (facadeCommon->getNumberOfProducerThreads() == 0) {
            facadeCommon->setNumberOfProducerThreads(1);
        }

        if (facadeCommon->getNumberOfDecodingThreads() == 0) {
            facadeCommon->setNumberOfConsumerThreads(1);
        }

        if (facadeCommon->getNumberOfEncoderThreads() == 0) {
            facadeCommon->setNumberOfEncoderThreads(1);
        }

        // Crosscheck between number of consumer, producer and pool number:
        // 1. The Producer thread number should be greater than or equal to the Consumer thread number;
        // 	  Hint: The best configuration has the Consumer number equals to the Producer number multiplied by a factor 3;
        // 2. The Pool number should not be lower than the number of Producer threads.
        if (facadeCommon->getNumberOfDecodingThreads() < facadeCommon->getNumberOfProducerThreads()) {
            stringstream message;
            message << "Attention: the Consumer thread number SHOULD be grater than the Producer thread number." << endl
                    << "To boost performance, please set the Consumer thread number equal to the Producer thread number multiplied by a factor 3.";
            status.setCode(ReturnCodes::CRITICAL_PARAMETERS);
            status.setMessage(message.str());
            return;    // Impossible to continue, wrong input parameters
        }

        if ((facadeCommon->getNumberOfDecodingThreads() % facadeCommon->getNumberOfProducerThreads()) != 0) {
            string message = "To boost performance, please set the Consumer thread number equal to the Producer thread number multiplied by a factor 3.";
            status.setCode(ReturnCodes::SPEED_ISSUE);
            status.setMessage(message);
        }

        if (facadeCommon->getNumberOfPreallocatedParsedRecord() < facadeCommon->getNumberOfProducerThreads()) {
            string message = "Attention: the Producer thread number should be greater than or equal to the Object Pool size";
            status.setCode(ReturnCodes::CRITICAL_PARAMETERS);
            status.setMessage(message);
            return;    // Impossible to continue, wrong input parameters
        }

    }

    bqManager = BlockingQueueManager::getInstance();    // Reference to the blocking queue where the parsing is going to store the results

    // It calculates an average number of records that each consumer has to consume. That number is used to allocate the object pool for the decode values.
    sizeDecodedValueOP =
            (facadeCommon->getMaximumNumberOfDatablock() * facadeCommon->getAverageNumberOfRecordPerDatablock())
            / facadeCommon->getNumberOfDecodingThreads();
    if ((facadeCommon->getMaximumNumberOfDatablock() * facadeCommon->getAverageNumberOfRecordPerDatablock()) %
        facadeCommon->getNumberOfDecodingThreads() != 0)
        sizeDecodedValueOP++;

    common->setSizeDecodedValueOP(sizeDecodedValueOP);

    // Producer ThreadPool creation
    facadeCommon->setProducerThreadPool(new ThreadPool(facadeCommon->getNumberOfProducerThreads()));

    // Consumer ThreadPool creation
    facadeCommon->setConsumerThreadPool(new ThreadPool(facadeCommon->getNumberOfDecodingThreads()));

    if (decodedValueOPManagerVector.empty()) {
        DecodedValueObjectPoolManager *decodedValueObjectPoolManager = nullptr;

        // Create a OP to store the decoded values for each consumer
        for (unsigned int i = 0; i < facadeCommon->getNumberOfDecodingThreads(); i++) {
            decodedValueObjectPoolManager = new DecodedValueObjectPoolManager();
            decodedValueOPManagerVector.push_back(decodedValueObjectPoolManager);
        }
    }

    std::cout << endl << "** Common parameters **" << endl;
    common->printCommonParameters();
    facadeCommon->printCommonParameters();

    struct stat sb;

    // Output directory creation, if output is enabled from command line
    if (facadeCommon->isJsonOutputEnableForDecoder()) {
        const char *jsonOutputPath = common->getOutputJsonPathForDecoder().c_str();

        if (stat(jsonOutputPath, &sb) == 0 && S_ISDIR(sb.st_mode)) {
            cleanDirectory(jsonOutputPath);

        } else {
            mkdir(jsonOutputPath, S_IRUSR | S_IWUSR | S_IXUSR);
        }

    } else if (facadeCommon->isDumpOnFileEnableForDecoder()) {
        const char *txtOutputPath = common->getOutputTxtPathForDecoder().c_str();

        if (stat(txtOutputPath, &sb) == 0 && S_ISDIR(sb.st_mode)) {
            cleanDirectory(txtOutputPath);

        } else {
            mkdir(txtOutputPath, S_IRUSR | S_IWUSR | S_IXUSR);
        }
    }

    if (facadeCommon->isAstOutputEnableForEncoder()) {
        remove(common->getOutputAstPathForEncoder().c_str());
    }
}

void cleanDirectory(const char *directoryPath) {

    DIR *directory;                // Representing the directory stream
    struct dirent *entry;        // Represent next directory entry in the directory stream, pointed by directory
    char filePathToDelete[PATH_MAX];

    //Open directory and read all entry, if a entry is different from '.' and '..', it is a file to remove
    directory = opendir(directoryPath);
    if (directory == NULL) {
        return;
    }

    while ((entry = readdir(directory)) != NULL) {

        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            snprintf(filePathToDelete, (size_t) PATH_MAX, "%s/%s", directoryPath,
                     entry->d_name);    // create path of file to remove
            remove(filePathToDelete);
        }
    }

    closedir(directory);
}

void CodecInterface::releaseMemory() {
    unordered_map<int, CategoryDescription *> *categoriesDescription = common->getCategoriesDescription();

    // Release memory pointed by categories description map
    for (auto it = categoriesDescription->begin(); it != categoriesDescription->end(); it++) {
        delete it->second;
    }

    // Release memory used by the blocking queues
    bqManager->deleteInstance();
    common->deleteInstance();

    decoderInterface->deleteInstance();
    encoderInterface->deleteInstance();

    codecInterface->deleteInstance();

    ASTERIX_datastream = nullptr;
}

FailureReport *CodecInterface::getFinalReport() {
    return FailureReport::getInstance();
}
