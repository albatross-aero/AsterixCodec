#include "EncoderInterface.h"
#include "utils/encoderForDecodedRecord.hpp"  // Here, to avoid MULTIPLE instantiations

// Initialization of the static field 'singletonDecoder'
EncoderInterface* EncoderInterface::singletonDecoder = nullptr;

EncoderInterface::EncoderInterface() {
	common = CommonVariables::getInstance();
	facadeCommon = FacadeCommonVariables::getInstance();
}

EncoderInterface::~EncoderInterface() {
	if (!common) {
		common->deleteInstance();
		common = nullptr;
	}
	if (!facadeCommon) {
		facadeCommon->deleteInstance();
		facadeCommon = nullptr;
	}
	if (!singletonDecoder) {
		singletonDecoder->deleteInstance();
		singletonDecoder = nullptr;
	}
}

EncoderInterface* EncoderInterface::getInstance() {
	if (!singletonDecoder) {
		singletonDecoder = new EncoderInterface();
	}
	return singletonDecoder;
}

void EncoderInterface::deleteInstance() {
	if (singletonDecoder != nullptr)
		delete singletonDecoder;

	singletonDecoder = nullptr;
}

unsigned char* EncoderInterface::encodeRecords(RecordCollection* recordsToEncode, unsigned int* recordNumberEncoded) {
	unsigned int datablockCategoryEncodedRecord, datablockLengthEncodeRecord, recordNumberToBeEncoded = 0;
	unsigned long long totalBytestreamSize = 0;
	unsigned char *encodedDatablock = nullptr, *outputRawData = nullptr;
	std::vector<unsigned char*> encodedDatablocksCollection;    // Used to collect results from different thread

	bool isAstOutputEnable = facadeCommon->isAstOutputEnableForEncoder();
	bool isEncodingDebugEnabled = common->isEncodingDebugEnabled();

	recordNumberToBeEncoded = recordsToEncode->getTotalNumberOfRecords();

	ThreadPool* consumer = facadeCommon->getConsumerThreadPool();

	encoderResults = std::vector<future<unsigned char*>>();

	// Start a task for each decoded record
	for (unsigned int i = 0; i < recordNumberToBeEncoded; i++) {
		encoderResults.push_back(consumer->enqueue(encodeForDecodedRecord, recordsToEncode, i));
	}

	(*recordNumberEncoded) = 0;

	if (isAstOutputEnable) {
		outFile.open(common->getOutputAstPathForEncoder(), ios::out | ios::binary | ios::app);    // "ios::app" to append at the end of the file
	}

	// Waits for the completion of all encoding threads
	for (auto && result : encoderResults) {
		encodedDatablock = result.get();

		if (encodedDatablock == nullptr) {
			continue;
		}

		(*recordNumberEncoded)++;

		// Get length in bytes of the current Datablock
		datablockLengthEncodeRecord = (unsigned int) (encodedDatablock[2] + ((encodedDatablock[1] << 8) & 0xFF00));
		totalBytestreamSize += datablockLengthEncodeRecord;

		if (isEncodingDebugEnabled) {
			// Get CAT number just for debug purposes
			datablockCategoryEncodedRecord = (unsigned int) (encodedDatablock[0]);

			cout << endl << "*** DATABLOCK ***" << endl;
			cout << std::dec << "CAT: " << static_cast<int>(datablockCategoryEncodedRecord) << " - LEN: " << static_cast<int>(datablockLengthEncodeRecord)
							<< endl;
			cout << endl << endl;
		}

		if (isAstOutputEnable) {
			// Save on file and free the memory used
			printASTOnFile(encodedDatablock);
			delete[] encodedDatablock;
			encodedDatablock = nullptr;
		} else {
			// All decoded records are collected to build the final bytestream returned to the client
			encodedDatablocksCollection.push_back(encodedDatablock);
		}
	}

	// Clear the collection of futures
	encoderResults.clear();

	if (isAstOutputEnable) {
		outFile.close();
	} else {
		// Build a complete bytestream returned to the client
		outputRawData = new unsigned char[totalBytestreamSize]();
		int offset = 0;

		// Each raw data collected is copied in the output bytestream
		for (unsigned int i = 0; i < encodedDatablocksCollection.size(); i++) {
			datablockLengthEncodeRecord = (unsigned int) (encodedDatablocksCollection.at(i)[2] + ((encodedDatablocksCollection.at(i)[1] << 8) & 0xFF00));
			memcpy((outputRawData + offset), encodedDatablocksCollection.at(i), datablockLengthEncodeRecord);
			offset += datablockLengthEncodeRecord;

			// Clear memory collected in the current position of the vector
			delete[] encodedDatablocksCollection.at(i);
		}
	}

	return outputRawData;
}

unsigned char* EncoderInterface::encodeSingleRecord(Record* recordToEncode, unsigned int* recordNumberEncoded) {
	ThreadPool* encoderTP = facadeCommon->getConsumerThreadPool();
	encoderResults = vector<future<unsigned char*>>();
	unsigned char* encodedDatablock = nullptr;
	unsigned char* outputRawData = nullptr;
	vector<unsigned char*> encodedDatablocksCollection;    // Used to collect results from different thread
	// //
	unsigned int encodedCategoryNumber = 0;
	unsigned int encodedDatablockLength = 0;
	unsigned long long totalBytestreamSize = 0;

	encoderResults.push_back(encoderTP->enqueue(encodeForSingleRecord, recordToEncode));

	if (facadeCommon->isAstOutputEnableForEncoder()) {
		outFile.open(common->getOutputAstPathForEncoder(), ios::out | ios::binary | ios::app);    // "ios::app" to append at the end of the file
	}

	(*recordNumberEncoded)=0;

	// Waits for the completion of all encoding threads
	for (auto && result : encoderResults) {
		encodedDatablock = result.get();
		if (encodedDatablock == nullptr) {
			continue;
		}
		(*recordNumberEncoded)++;

		// Get length in bytes of the current Datablock
		encodedDatablockLength = (unsigned int) (encodedDatablock[2] + ((encodedDatablock[1] << 8) & 0xFF00));
		totalBytestreamSize += encodedDatablockLength;

		if (common->isEncodingDebugEnabled()) {
			// Get CAT number just for debug purposes
			encodedCategoryNumber = (unsigned int) (encodedDatablock[0]);

			cout << endl << "*** DATABLOCK ***" << endl;
			cout << std::dec << "CAT: " << static_cast<int>(encodedCategoryNumber) << " - LEN: " << static_cast<int>(encodedDatablockLength) << endl;
			cout << endl << endl;
		}

		if (facadeCommon->isAstOutputEnableForEncoder()) {
			// Save on file and free the memory used
			printASTOnFile(encodedDatablock);
			delete[] encodedDatablock;
			encodedDatablock = nullptr;

			outFile.close();
		} else {
			// All decoded records are collected to build the final bytestream returned to the client
			encodedDatablocksCollection.push_back(encodedDatablock);
		}
	}

	// Clear the collection of futures
	encoderResults.clear();

	// Build a complete bytestream returned to the client
	outputRawData = new unsigned char[totalBytestreamSize]();
	int offset = 0;

	// Each raw data collected is copied in the output bytestream
	for (unsigned int i = 0; i < encodedDatablocksCollection.size(); i++) {
		encodedDatablockLength = (unsigned int) (encodedDatablocksCollection.at(i)[2] + ((encodedDatablocksCollection.at(i)[1] << 8) & 0xFF00));
		memcpy((outputRawData + offset), encodedDatablocksCollection.at(i), encodedDatablockLength);
		offset += encodedDatablockLength;

		// Clear memory collected in the current position of the vector
		delete[] encodedDatablocksCollection.at(i);
	}

	return outputRawData;
}

void EncoderInterface::printASTOnFile(unsigned char* ASTERIX_bytestream) {
	int datablockLength = (unsigned int) (ASTERIX_bytestream[2] + ((ASTERIX_bytestream[1] << 8) & 0xFF00));
	outFile.write((char*) ASTERIX_bytestream, datablockLength);
}
