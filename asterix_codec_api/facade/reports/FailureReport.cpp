#include "facade/reports/FailureReport.h"

FailureReport* FailureReport::failureReportSingleton = nullptr;
std::mutex FailureReport::instantiationMutex;

FailureReport::FailureReport() {
}

FailureReport::~FailureReport() {
}

FailureReport *FailureReport::getInstance() {
	if (failureReportSingleton == nullptr) {
		std::unique_lock<std::mutex> ul(instantiationMutex);    // Get the lock to instantiate for the first time the Report
		failureReportSingleton = new FailureReport();
	}
	return failureReportSingleton;
}

void FailureReport::addFailure(unsigned int categoryNumber, unsigned int ASTERIX_bytestreamOffset, std::string itemName, unsigned char* rawDatablock,
		std::string message, ReturnCodes code) {

	std::unique_lock<std::mutex> lock(mutex);    // Get the lock to insert a failure report

	categoryNumbers.push_back(categoryNumber);
	ASTERIX_bytestreamOffsets.push_back(ASTERIX_bytestreamOffset);
	itemConcerned.push_back(itemName);
	rawDatablocks.push_back(rawDatablock);

	ReturnStatus status;
	status.setCode(code);
	status.setMessage(message);

	returnStatuses.push_back(status);
}

void FailureReport::clearFailures() {
	std::unique_lock<std::mutex> lock(mutex);    // Get the lock to clear all failures

	categoryNumbers.clear();
	ASTERIX_bytestreamOffsets.clear();
	itemConcerned.clear();
	returnStatuses.clear();
}

unsigned int FailureReport::getFailuresNumber() {
	return returnStatuses.size();
}

Failure FailureReport::get(unsigned int index) {
	Failure failure;
	if (categoryNumbers.size() > index) {
		failure.categoryNumber = categoryNumbers[index];
		failure.bytestreamOffset = ASTERIX_bytestreamOffsets[index];
		failure.itemName = itemConcerned[index];
		failure.rawDatablock = rawDatablocks[index];
		failure.status = returnStatuses[index];
	}

	return failure;
}

/**
 * Override of the stream operator in order to print the name of the Error Code instead of an integer
 * when flushing to streams (i.e. file).
 * It is used from the FailureReport::printOnFile function.
 */
std::ostream& operator<<(std::ostream& out, const ReturnCodes value) {
	static std::map<ReturnCodes, std::string> strings;
	if (strings.size() == 0) {
#define INSERT_ELEMENT(p) strings[p] = #p
		INSERT_ELEMENT(ReturnCodes::SUCCESS);
		INSERT_ELEMENT(ReturnCodes::SERIALIZATION_FAILED);
		INSERT_ELEMENT(ReturnCodes::AST_LOADER_ERROR);
		INSERT_ELEMENT(ReturnCodes::AST_CONFIG_LOADER_ERROR);
		INSERT_ELEMENT(ReturnCodes::AST_MALFORMED_DESCRIPTION);
		INSERT_ELEMENT(ReturnCodes::SPEED_ISSUE);
		INSERT_ELEMENT(ReturnCodes::CRITICAL_PARAMETERS);
		INSERT_ELEMENT(ReturnCodes::PARSING_ERROR);
		INSERT_ELEMENT(ReturnCodes::UNKNOWN_CATEGORY);
		INSERT_ELEMENT(ReturnCodes::UNKNOWN_FRN);
		INSERT_ELEMENT(ReturnCodes::DECODING_ERROR);
		INSERT_ELEMENT(ReturnCodes::UNKNOWN_DECODING_FUNCTION);
		INSERT_ELEMENT(ReturnCodes::UNKNOWN_REPRESENTATION_MODE);
		INSERT_ELEMENT(ReturnCodes::ENCODING_ERROR);
		INSERT_ELEMENT(ReturnCodes::OUT_OF_RANGE_DECODED_ELEMENT);
		INSERT_ELEMENT(ReturnCodes::OUT_OF_RANGE_ENCODED_ELEMENT);
		INSERT_ELEMENT(ReturnCodes::MISSING_MANDATORY_ITEM);
		INSERT_ELEMENT(ReturnCodes::WARNING);
#undef INSERT_ELEMENT
	}

	return out << strings[value];
}

void FailureReport::printOnFile(string fileName) {
	ofstream outFile;
	outFile.open(fileName);

	for (unsigned int i = 0; i < returnStatuses.size(); i++) {
		ReturnStatus currentStatus = returnStatuses[i];
		// Print error code, CAT number and error message
		outFile << "===== " << currentStatus.getCode() << " ===== CAT: " << std::dec<< categoryNumbers[i];
		if (itemConcerned[i] != "") {
			outFile << " ===== Item: " << std::dec << itemConcerned[i];
		}

		outFile << endl << currentStatus.getMessage() << endl;

		// Print the rawDatablock not parsed or decoded if it exists.
		if (rawDatablocks[i] != nullptr) {
			outFile <<  "<<<< Raw Datablock >>>>" << endl;
			unsigned int datablockLength = (unsigned int) (rawDatablocks[i][2] + ((rawDatablocks[i][1] << 8) & 0xFF00));

			for (unsigned int j = 0; j < datablockLength; j++) {
				if (rawDatablocks[i][j]) {
					outFile << std::hex << std::setw(1) << std::setfill('0') << static_cast<unsigned int>(rawDatablocks[i][j]) << "";
				}
			}
			outFile << endl << endl;
		}
		outFile << endl;
	}

	outFile.close();
}
