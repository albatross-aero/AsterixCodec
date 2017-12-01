#include "DecoderInterface.h"
#include "utils/decoderForParsedRecord.hpp"
// Here, to avoid MULTIPLE instantiations

// Initialization of the static field 'singletonDecoder'
DecoderInterface* DecoderInterface::singletonDecoder = nullptr;

DecoderInterface::DecoderInterface() {
	common = CommonVariables::getInstance();
	facadeCommon = FacadeCommonVariables::getInstance();
	bqManager = nullptr;
	ASTERIX_datastream = nullptr;
	ASTinput_fileSize = 0;
}

DecoderInterface::~DecoderInterface() {
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
	if (!singletonDecoder) {
		singletonDecoder->deleteInstance();
		singletonDecoder = nullptr;
	}

	unsigned int sizeOfVector = decodedValueOPManagerVector.size();
	DecodedValueObjectPoolManager* tmp = nullptr;

	for (unsigned int i = 0; i < sizeOfVector; i++) {
		tmp = decodedValueOPManagerVector.at(i);
		if (tmp != nullptr) {
			delete tmp;
			tmp = nullptr;
		}
	}
}

DecoderInterface* DecoderInterface::getInstance() {
	if (!singletonDecoder) {
		singletonDecoder = new DecoderInterface();
	}
	return singletonDecoder;
}

void DecoderInterface::deleteInstance() {
	if (singletonDecoder != nullptr)
		delete singletonDecoder;

	singletonDecoder = nullptr;
}

//** OTHER FUNCTIONS **
void DecoderInterface::startDecoderThreads() {
	if (decodedValueOPManagerVector.empty()) {
		// For EACH CONSUMER, creating an OP to store the decoded values
		for (unsigned int i = 0; i < facadeCommon->getNumberOfDecodingThreads(); i++) {
			decodedValueOPManagerVector.push_back(new DecodedValueObjectPoolManager());
		}
	}

	ThreadPool* decoderTP = facadeCommon->getConsumerThreadPool();

	for (unsigned int i = 0; i < facadeCommon->getNumberOfDecodingThreads(); i++) {

		// For EACH CONSUMER, getting the OP to store the decoded values
		decodingResults.push_back(decoderTP->enqueue(decodeParsedRecords, i, decodedValueOPManagerVector.at(i)));
	}
}

RecordCollection* DecoderInterface::waitForConsumer() {
	RecordCollection* tmpResult = nullptr;
	RecordCollection* decodedRecords = new RecordCollection();

	for (auto && result : decodingResults) {
		tmpResult = result.get();

		if (tmpResult != nullptr) {
			decodedRecords->addRecordsInCollection(tmpResult);
		}
	}

	// All futures in the vector have been used, now we can clear the vector
	decodingResults.clear();

	return decodedRecords;
}

void DecoderInterface::releaseAllDecodedRecords(RecordCollection* decoderResult) {
	for (vector<DecodedValueObjectPoolManager*>::iterator it = decodedValueOPManagerVector.begin(); it != decodedValueOPManagerVector.end(); it++) {
		(*it)->releaseAllElementValueToOP();
	}

	delete decoderResult;
	decoderResult = nullptr;
}

void DecoderInterface::setSourceDatastream(unsigned char* _ASTERIX_datastream, unsigned long _ASTinput_fileSize) {
	ASTERIX_datastream = _ASTERIX_datastream;
	ASTinput_fileSize = _ASTinput_fileSize;
}

RecordCollection* DecoderInterface::startDecodingDatablocks(unsigned char* ASTERIX_datastream, unsigned long* endOfBufferDecodingIndex) {

	/************************
	 * VARIABLES DEFINITION *
	 ************************/
	ThreadPool* parserTP = nullptr;
	RecordCollection* decodedRecords = nullptr;
	unsigned long long recordNumber = 0;		// Number of record that have been parsed in the current datablock TODO: modify to be atomic variable

	unsigned int datablockCategory = -1;    				// Datablock CAT field value
	unsigned int datablockLength = -1;    				// Datablock LEN field value
	unsigned long bufferIndex = 0;				// Main index to move around the 'ASTERIX_datastream' buffer

	//Do not move this line!
	if (bqManager == nullptr)
		bqManager = BlockingQueueManager::getInstance();

	unordered_map<int, CategoryDescription*>* categoriesDescription = common->getCategoriesDescription();

	bqManager->openAllBlockingQueues();
	startDecoderThreads();

	parserTP = facadeCommon->getParserThreadPool();

	//TODO: Check current number of datablocks if is less or equal to common->getMaximumDatablock

	// There are bytes to read while 'bufferIndex' is less than 'ASTERIX_inputFileSize'
	while (bufferIndex < (*endOfBufferDecodingIndex)) {

		// Reading the datablock Category number (1 Byte)
		datablockCategory = (unsigned int) ASTERIX_datastream[bufferIndex];
		// Reading the datablock Length value (2 Bytes)
		datablockLength = (unsigned int) (ASTERIX_datastream[bufferIndex + 2] + ((ASTERIX_datastream[bufferIndex + 1] << 8) & 0xFF00));

		if((bufferIndex + datablockLength) <= (*endOfBufferDecodingIndex) ) {
			parsingResults.push_back(
					parserTP->enqueue(ParsingUtils::parseDatablock, ASTERIX_datastream, datablockCategory, datablockLength, categoriesDescription, bufferIndex));
		} else {
			// Last Datablock is not complete, return to the caller a message error in the FailureReport
			string message = "The Datablock provided starting at index " + to_string(bufferIndex) + " in the data stream is not complete. The declared size is "
					+ to_string(datablockLength) + " but the data stream ended at " + to_string(bufferIndex + datablockLength) + " position.";
			FailureReport::getInstance()->addFailure(datablockCategory, bufferIndex, "", nullptr, message, ReturnCodes::PARSING_ERROR);
		}

		bufferIndex += datablockLength;
	}

	// Waits for the completion of all parsing threads
	for (auto && result : parsingResults) {
		recordNumber += result.get();
	}

	// Close the blocking queue to notify the consumer to finish data and return
	bqManager->closeAllBlockingQueues();

	parsingResults.clear();

	decodedRecords = waitForConsumer();

	if (facadeCommon->isJsonOutputEnableForDecoder())
		printAllDecodedRecordsOnJson(decodedRecords);

	return decodedRecords;
}

RecordCollection* DecoderInterface::decodingDatablocksFromFile() {
	/************************
	 * VARIABLES DEFINITION *
	 ************************/
	int numberOfDatablocksToParse = facadeCommon->getMaximumNumberOfDatablock();
	int currentNumbeOfDatablocks = 0;

	unsigned long startIndexOfDatablocks = indexInASTERIX_datastream;
	int datablockLength = -1;    		// Datablock LEN field value
	unsigned long lengthOfBytestreamToParse = 0;

	// There are bytes to read until 'bufferIndex' is less than 'ASTinput_fileSize'
	//if currentNumbeOfDatablocks == numberOfDatablocksToParse -> we have collect enough datablocks -> we can decode
	while ((numberOfDatablocksToParse != currentNumbeOfDatablocks) && (indexInASTERIX_datastream < ASTinput_fileSize)) {

		// Reading the datablock Length value (2 Bytes)
		datablockLength = (unsigned int) (ASTERIX_datastream[indexInASTERIX_datastream + 2]
															 + ((ASTERIX_datastream[indexInASTERIX_datastream + 1] << 8) & 0xFF00));    //1111 1111 0000 0000b

		if ((indexInASTERIX_datastream + datablockLength) <= ASTinput_fileSize) {
			currentNumbeOfDatablocks++;
			indexInASTERIX_datastream += datablockLength;
		} else {
			// Last Datablock is not complete
			indexInASTERIX_datastream = ASTinput_fileSize;    // Set the current index at the end of the file to Stop the outer loop, end of file reached
		}
	}

	if (currentNumbeOfDatablocks == 0) {
		return nullptr;
	}

	lengthOfBytestreamToParse = indexInASTERIX_datastream - startIndexOfDatablocks;
	return startDecodingDatablocks(ASTERIX_datastream + startIndexOfDatablocks, &lengthOfBytestreamToParse);
}

void DecoderInterface::printAllDecodedRecordsOnJson(RecordCollection* recordsToPrint) {
	unsigned int recordsToPrintSize = 0;
	unsigned int maximumNumberOfRecordInJson = 0;
	unsigned int sizeOfElementsToPrint = 0;

	ElementValue* elementToPrint = nullptr;
	string* elementName = nullptr;
	unsigned int totalNumRecordsToPrint = 0;

	unsigned int actualNumberOfFiles = 0;

	vector<string*>* elementsNameToPrint = nullptr;
	vector<ElementValue*>* elementsValueToPrint = nullptr;

	recordsToPrintSize = recordsToPrint->getTotalNumberOfRecords();
	maximumNumberOfRecordInJson = common->getMaximumNumberOfRecordInJSON();

	// DOM document of a JSON file
	rapidjson::Document document;
	document.SetArray();    // Set the root of the document as an array and not an object
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();    // Must pass an allocator when the object may need to allocate memory
	rapidjson::Value jsonElementName, jsonElementValue;

	autojsoncxx::ParsingResult result;
	std::string originalFileName, fileName;

	originalFileName = common->getOutputJsonPathForDecoder() + "/out_decoding.json";

	for (unsigned int i = 0; i < recordsToPrintSize; i++) {

		elementsNameToPrint = recordsToPrint->getElementNamesOfARecord(i);
		elementsValueToPrint = recordsToPrint->getElementValuesOfARecord(i);

		sizeOfElementsToPrint = elementsValueToPrint->size();

		rapidjson::Value jsonRecord(rapidjson::kObjectType);

		// put in the DOM all elements
		for (unsigned int j = 0; j < sizeOfElementsToPrint; j++) {
			elementToPrint = elementsValueToPrint->at(j);
			elementName = elementsNameToPrint->at(j);

			jsonElementName.SetString(elementName->c_str(), elementName->size(), allocator);

			jsonElementValue.SetString(elementToPrint->getDecodedDataInString().c_str(), elementToPrint->getDecodedDataInString().length(), allocator);

			jsonRecord.AddMember(jsonElementName, jsonElementValue, allocator);
		}

		totalNumRecordsToPrint++;
		document.PushBack(jsonRecord, allocator);

		if (totalNumRecordsToPrint > maximumNumberOfRecordInJson) {

			fileName = originalFileName;
			actualNumberOfFiles = common->getActualNumberOfOutFiles();
			fileName += "." + std::to_string(actualNumberOfFiles);

			std::remove(fileName.c_str());

			// document is written on a file
			ofstream ofs(fileName);
			rapidjson::OStreamWrapper osw(ofs);
			rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
			document.Accept(writer);

			totalNumRecordsToPrint = 0;    // Reset all counters
			document.Empty(); 			// Empty the document object
			document.SetArray();
		}
	}

	// Check if document has something else to print on file
	if (document.Size() > 0) {
		fileName = originalFileName;
		actualNumberOfFiles = common->getActualNumberOfOutFiles();
		fileName += "." + std::to_string(actualNumberOfFiles);

		std::remove(fileName.c_str());

		// document is written on a file
		ofstream ofs(fileName);
		rapidjson::OStreamWrapper osw(ofs);
		rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
		document.Accept(writer);

		totalNumRecordsToPrint = 0; 			// Reset all counters
		document.Empty(); 			// Empty the document object
		document.SetArray();
	}
}
