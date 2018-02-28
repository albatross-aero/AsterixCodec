#ifndef FACADE_REPORTS_FAILUREREPORT_H_
#define FACADE_REPORTS_FAILUREREPORT_H_

#include <string>
#include <vector>
#include <mutex>
#include <iostream>
#include <fstream>
#include <map>
#include <iomanip>

#include "ReturnStatus.hpp"

using namespace std;

/**
 * Output structure built at runtime, when the client call 'get(index)' on a FailureReport object.
 * @param categoryNumber
 * @param bytestreamOffset
 * @param itemName
 * @param rawDatablock
 * @param status
 */
struct Failure {
    unsigned int categoryNumber;
    unsigned int bytestreamOffset;
    std::string itemName;
    unsigned char *rawDatablock;
    ReturnStatus status;
};

/**
 * TODO FailureReport class documentation
 */
class FailureReport {

public:
    /**
     * Thread safe singleton to instantiate the local variables.
     */
    static FailureReport *getInstance();

    /**
     * It adds a failure in the common Failure Report object. For each report, a set of information (not mandatory) are associated.
     * @param categoryNumber Category number associated to the failure event
     * @param ASTERIX_bytestreamOffset Offset into the bytestream where the failure happened
     * @param itemName Name of the item when the failure event happened
     * @param rawDatablock Pointer to the raw Datablock where the failure happened (the client can parse/decode/encode it manually)
     * @param message String message releated to the failure
     * @param code Enumeration code related to the failure
     */
    void addFailure(unsigned int categoryNumber, unsigned int ASTERIX_bytestreamOffset, string itemName,
                    unsigned char *rawDatablock, string message,
                    ReturnCodes code);

    /**
     * It clears all vector related to the failure events. Only the client
     * should call this method in order to clear the Report.
     */
    void clearFailures();

    /**
     * It returns a single Failure event (struct) given a specific index zero-based.
     * @param index Index of the failure to get it back
     * @return Failure struct filled with the information about a single failure event, or an empty
     * structure if the given index is out of range
     */
    Failure get(unsigned int index);

    /**
     * It returns the number of failures actually logged in the current instance of FailureReport.
     * @return Number of failures
     */
    unsigned int getFailuresNumber();

    /**
     * It prints the current Report in an output file.
     * @param fileName File name where the reports is saved
     */
    void printOnFile(string fileName);

private:
    /**
     * Default constructor.<br>
     * This class is a singleton therefore this method is private.
     */
    FailureReport();

    /**
     * Destructor.<br>
     * It is a singleton therefore this method is private
     */
    ~FailureReport();

    /**
     * Singleton private instance
     */
    static FailureReport *failureReportSingleton;

    /**
     * Lockable object used to protect the instantiation of the singleton 'failureReportSingleton'
     */
    static std::mutex instantiationMutex;

    /**
     * Lockable object used to access to the reports vectors one thread per time
     */
    std::mutex mutex;

    vector<unsigned int> categoryNumbers;
    vector<unsigned int> ASTERIX_bytestreamOffsets;
    vector<unsigned char *> rawDatablocks;
    vector<string> itemConcerned;
    vector<ReturnStatus> returnStatuses;
};

#endif /* FACADE_REPORTS_FAILUREREPORT_H_ */
