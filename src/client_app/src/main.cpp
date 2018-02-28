#include <chrono>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "asterix_codec/asterix_codec.h"
#include "boost/program_options.hpp"
//#include "data_model/decoder_objects/ElementValue.hpp"		// unused import

using namespace std;
namespace po = boost::program_options;

CodecInterface *codecInterface = CodecInterface::getInstance();
DecoderInterface *decoderInterface = nullptr;
EncoderInterface *encoderInterface = nullptr;

/**
 * Read parameters passed at command line.
 * @param argc Number of argument of argv
 * @param argv Command line arguments
 * @param fileName Passed by reference and filled with file name if it is expressed
 * @param ASTERIX_categoryRepositoryPath Passed by reference and filled with the path to a JSON repository or an archive (.dat)
 * @param justSerialiseAndQuit Passed as reference and filled with the path where the user want to save the serialized archive
 * @return Error code, -1 if something wrong, otherwise 0
 */
int readCommandLineArgs(int argc, char *argv[], std::string &fileName, string &ASTERIX_categoryRepositoryPath,
                        string &justSerialiseAndQuit,
                        bool &encodingFunctionOn) {
    // Example format: asterix_decoder -f fileName -d -re
    po::options_description option_description("Command line options");
    po::variables_map command_arguments_map;    // contains command line arguments value - key: name of option - value: value of argument

    encodingFunctionOn = true;

    try {
        unsigned short int threadsConsumer = 0, threadsProducer = 0, threadsEncoder = 0, totalThreads = 0;
        unsigned int elementsNumber = 0, poolSize = 0, datablockNumber = 0, recordNumber = 0;

        // Define options for the argument of command line
        option_description.add_options()("help", "produce help message")("file_path,f", po::value<string>(),
                                                                         "To specify ASTERIX byte stream file")(
                "datablock", po::value<unsigned int>(),
                "To set the number of maximum number of datablock to parse/decode")(
                "record", po::value<unsigned int>(), "To set the average number of record per datablock")(
                "thread", po::value<unsigned short int>(), "To set the total number of threads to be used")(
                "consumer", po::value<unsigned short int>(),
                "To set the number of threads to be used from the consumer")(
                "producer", po::value<unsigned short int>(),
                "To set the number of threads to be used from the producer")(
                "encoder", po::value<unsigned short int>(), "To set the number of threads to be used from the encoder")(
                "encoder_off", "To disable the encoding function. [ENABLED by default]")(
                "encode", "To run the Encoding Client")(
                "debug,d", "To enable the output printing on the console for the DECODER. [DISABLED by default]")(
                "debug_encoder,c",
                "To enable the output printing on the console for the ENCODER. [DISABLED by default]")(
                "re,r", "To enable RE item parsing. [RE JUMPED by default]")(
                "spf", "To enable SPF item parsing. [SPF JUMPED by default]")(
                "filter", "To enable FILTERING. [FILTER disable by default]")(
                "pool", po::value<unsigned int>(), "To set the number of object pool to be used")(
                "elements_number,e", po::value<unsigned int>(), "To set the number of elements for one record")(
                "json_output,o", "To enable of the decoded records the output on JSON files. [DISABLED by default]")(
                "dump", "To enable the dump of the decoded records on TXT files. [DISABLED by default]")(
                "ast,a", "To enable the dump of the encoded records on AST file. [DISABLED by default]")(
                "repository,p",
                po::value<string>(),
                "To specify a path for Category repository (default: ./asterix/asterixConfiguration.json ) or the ASTERIX Binary configuration file (example: ./asterix/CategoryDescription.dat).")(
                "serialize,s", po::value<string>(), "To specify a path for archiving ASTERIX Category description");

        // store command line arguments in 'coomand_arguments_map'
        po::store(po::parse_command_line(argc, argv, option_description), command_arguments_map);

        if (command_arguments_map.count("help") || argc == 1) {
            cout << option_description << "\n";
            return 0;
        }

        // Notify if a mandatory option is missing
        po::notify(command_arguments_map);

        // get value from map and store in a common variables
        if (command_arguments_map.count("file_path")) {
            fileName = command_arguments_map["file_path"].as<string>();
        } else if (command_arguments_map.count("f")) {
            fileName = command_arguments_map["f"].as<string>();
        }

        if (command_arguments_map.count("encoder_off")) {
            encodingFunctionOn = false;
        }

        if (command_arguments_map.count("debug") || command_arguments_map.count("d")) {
            codecInterface->enableDebugForDecoder(true);
        }

        if (command_arguments_map.count("debug_encoder") || command_arguments_map.count("c")) {
            codecInterface->enableDebugForEncoder(true);
        }

        if (command_arguments_map.count("re") || (command_arguments_map.count("r"))) {
            codecInterface->enableReservedExpansionForDecoding(true);
            codecInterface->enableReservedExpansionForEncoding(true);
        }

        if (command_arguments_map.count("spf")) {
            codecInterface->enableSpecialPurposeFieldForDecoding(true);
            codecInterface->enableSpecialPurposeFieldForEncoding(true);
        }

        if (command_arguments_map.count("filter")) {
            codecInterface->enableFilteringForDecoding(true);
            codecInterface->enableFilteringForEncoding(true);
        }

        if (command_arguments_map.count("datablock")) {
            datablockNumber = command_arguments_map["datablock"].as<unsigned int>();
        }

        if (command_arguments_map.count("record")) {
            recordNumber = command_arguments_map["record"].as<unsigned int>();
        }

        if (command_arguments_map.count("thread")) {
            totalThreads = command_arguments_map["thread"].as<unsigned short int>();
        }

        if (command_arguments_map.count("consumer")) {
            threadsConsumer = command_arguments_map["consumer"].as<unsigned short int>();
        }

        if (command_arguments_map.count("encoder")) {
            threadsEncoder = command_arguments_map["encoder"].as<unsigned short int>();
        }

        if (command_arguments_map.count("consumer")) {
            threadsConsumer = command_arguments_map["consumer"].as<unsigned short int>();
        }

        if (command_arguments_map.count("producer")) {
            threadsProducer = command_arguments_map["producer"].as<unsigned short int>();
        }

        if (command_arguments_map.count("pool")) {
            poolSize = command_arguments_map["pool"].as<unsigned int>();
        }

        if (command_arguments_map.count("elements_number")) {
            elementsNumber = command_arguments_map["elements_number"].as<unsigned int>();

        } else if (command_arguments_map.count("e")) {
            elementsNumber = command_arguments_map["e"].as<unsigned int>();
        }

        if (command_arguments_map.count("json_output") || command_arguments_map.count("o")) {
            codecInterface->enableJsonOutputCreationForDecoder(true);
        }

        if (command_arguments_map.count("dump")) {
            codecInterface->enableDumpOnFileForDecoder(true);
        }

        if (command_arguments_map.count("ast") || command_arguments_map.count("a")) {
            codecInterface->enableAstOutputCreationForEncoder(true);
        }

        if (command_arguments_map.count("repository")) {
            ASTERIX_categoryRepositoryPath = command_arguments_map["repository"].as<string>();
        } else if (command_arguments_map.count("p")) {
            ASTERIX_categoryRepositoryPath = command_arguments_map["p"].as<string>();
        }

        if (command_arguments_map.count("serialize")) {
            justSerialiseAndQuit = command_arguments_map["serialize"].as<string>();
        } else if (command_arguments_map.count("s")) {
            justSerialiseAndQuit = command_arguments_map["s"].as<string>();
        }

        if (command_arguments_map.count("encode")) {
            return 10;
        }

        // Set parameters into the library
        if (totalThreads != 0)
            codecInterface->setTotalNumberOfThreads(totalThreads);

        if (threadsProducer != 0)
            codecInterface->setNumberOfProducerThreads(threadsProducer);

        if (threadsConsumer != 0)
            codecInterface->setNumberOfConsumerThreads(threadsConsumer);

        if (threadsEncoder != 0)
            codecInterface->setNumberOfEcoderThreads(threadsEncoder);

        if (elementsNumber != 0)
            codecInterface->setObjectPoolSize(elementsNumber);

        if (poolSize != 0)
            codecInterface->setObjectPoolNumber(poolSize);

        if (datablockNumber != 0) {
            codecInterface->setMaximumNumberOfDatablock(datablockNumber);
        }

        if (recordNumber != 0) {
            codecInterface->setAverageNumberOfRecordPerDatablock(recordNumber);
        }

    } catch (exception &e) {
        cerr << "error: " << e.what() << "\n";
        cout << option_description << "\n";
        return -1;
    } catch (...) {
        cerr << "Exception of unknown type!\n";
        cout << option_description << "\n";
        return -1;
    }
    return 0;
}

unsigned char *loadSourceFile(std::string filePath, unsigned long &ASTinput_fileSize, ReturnStatus &status) {
    ifstream ASTERIX_inputFile(filePath, ios::in | ios::binary | ios::ate);    // ASTERIX data source
    status.clear();

    if (!(ASTERIX_inputFile) || !(ASTERIX_inputFile.is_open())) {
        string message("ASTERIX input file not found.\n");
        status.setCode(ReturnCodes::AST_LOADER_ERROR);
        status.setMessage(message);
        return nullptr;
    }

    // Set a cursor at the end of file and ask for the file size
    ASTinput_fileSize = ASTERIX_inputFile.seekg(0, ios_base::end).tellg();
    //Cursor back on top of the file
    ASTERIX_inputFile.clear();
    ASTERIX_inputFile.seekg(0, ios_base::beg);

    // Save input file into a global variable 'buffer'
    unsigned char *ASTERIX_datastream = new unsigned char[ASTinput_fileSize];
    ASTERIX_inputFile.read((char *) ASTERIX_datastream, ASTinput_fileSize);

    ASTERIX_inputFile.close();
    return ASTERIX_datastream;
}

// Printing the timing results.
void printPerformanceInformation(unsigned long long timing, bool isDecoderTiming, unsigned long long recordNumber,
                                 unsigned long ASTinput_fileSize) {
    double parsingRate = 0.0;

    if (isDecoderTiming) {
        cout << endl << std::dec << "** DECODER ** " << endl << "\tElapsed time: " << timing << " ms || "
             << timing / 1000 << " s .::. "
             << recordNumber
             << " records decoded" << endl;
    } else {
        cout << endl << std::dec << "** ENCODER ** " << endl << "\tElapsed time: " << timing << " ms || "
             << timing / 1000 << " s .::. "
             << recordNumber
             << " records encoded" << endl;
    }

    if (timing != 0) {
        cout << endl << "\tPERFORMANCE: " << endl;
        if (timing < 1000) {
            cout << endl << "\t\tToo short to give reliable results in records per second." << endl;
        } else {
            cout << endl << "\t\t" << (recordNumber / timing) * 1000 << " rec/s" << endl;
        }

        if (ASTinput_fileSize != 0) {
            parsingRate = (((double) ASTinput_fileSize) / (1024 * 1024)) / (((double) timing) / 1000);
            cout << endl << "\t\t" << std::fixed << std::setprecision(2) << parsingRate << " MB/s" << endl;
        }
    } else {
        cerr << "Error while calculating the parsing rate, expressed in records per second or in megabyte per second."
             << endl;
    }
}

void decodeAndEncodeAsterixFile(string ASTERIX_inputFilePath, bool encodingFunctionOn, ReturnStatus &status) {
    /************************
     * VARIABLES DEFINITION *
     ************************/
    RecordCollection *decodedRecords = nullptr;

    chrono::time_point<chrono::system_clock> startDecoder, startEncoder;
    chrono::time_point<chrono::system_clock> endDecoder, endEncoder;

    unsigned char *ASTERIX_datastream = nullptr;
    unsigned long ASTinput_fileSize = 0;    // Size in byte of the input ASTERIX file
    unsigned long long encoderTimingAccumulator = 0;
    unsigned long long totalTiming = 0;
    unsigned long long decodedRecordNumber = 0;
    unsigned long long encodedRecordNumber = 0;
    unsigned int currentEncodedRecordNumber = 0;
    unsigned char *outputEncodedBytes = nullptr;

    // Load and set datastream to decode in the library
    ASTERIX_datastream = loadSourceFile(ASTERIX_inputFilePath, ASTinput_fileSize, status);
    if (status.getCode() != ReturnCodes::SUCCESS)
        return;

    decoderInterface->setSourceDatastream(ASTERIX_datastream, ASTinput_fileSize);

    //** START TIMING **
    startDecoder = chrono::system_clock::now();

    // 'decodeFromFile()' decodes blocks of N datablocks of ASTERIX_bytestream - the index in the ASTERIX_bytestream is managed by the library
    // If 'decodeFromFile()' return nullptr -> no more datablocks to decode
    while ((decodedRecords = decoderInterface->decodeFromFile()) != nullptr) {

        decodedRecordNumber += decodedRecords->getTotalNumberOfRecords();

        if (encodingFunctionOn) {
            // ** ENCODE Records **
            startEncoder = chrono::system_clock::now();

            outputEncodedBytes = encoderInterface->encodeRecords(decodedRecords, &currentEncodedRecordNumber);

            encodedRecordNumber += currentEncodedRecordNumber;
            // Clear memory used by the encoded bytes
            delete[] outputEncodedBytes;

            endEncoder = chrono::system_clock::now();
            encoderTimingAccumulator += chrono::duration_cast<chrono::milliseconds>(endEncoder - startEncoder).count();
        }

        // I am done with decodedRecords -> release
        decoderInterface->releaseAllDecodedRecords(decodedRecords);

        // Move on to the next datablocks...
    }

    //** STOP TIMING **
    endDecoder = chrono::system_clock::now();
    totalTiming = chrono::duration_cast<chrono::milliseconds>(endDecoder - startDecoder).count();

    printPerformanceInformation(totalTiming - encoderTimingAccumulator, true, decodedRecordNumber, ASTinput_fileSize);

    if (encoderTimingAccumulator != 0)
        printPerformanceInformation(encoderTimingAccumulator, false, encodedRecordNumber, ASTinput_fileSize);

    if (ASTERIX_datastream != nullptr) {
        delete[] ASTERIX_datastream;
        ASTERIX_datastream = nullptr;
    }

    return;
}

void decodeAndEncodeFromOtherSource(bool encodingFunctionOn) {
    RecordCollection *decodedRecords = nullptr;

    unsigned int datablockLength = 311;
    unsigned int currentEncodedRecordNumber = 0;

    unsigned char ASTERIX_datablock[] = {0x15, 0x00, 0xaf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x32, 0x50, 0x71, 0x49,
                                         0x2c, 0x09, 0xa5, 0x00, 0x6d,
                                         0xec,
                                         0x2c, 0x22, 0x37, 0xeb, 0x0b, 0x5c, 0x0c, 0x22, 0x37, 0xeb, 0x5a, 0x0b, 0x5c,
                                         0x0c, 0x37, 0x6d, 0xec, 0x2c,
                                         0x96, 0x69, 0x96, 0x69, 0x44, 0x7c,
                                         0x10, 0x6d, 0xec, 0x2c, 0x40, 0x6d, 0xec, 0x2c, 0x6d, 0xec, 0x2c, 0x40, 0x6d,
                                         0xec, 0x2c, 0x72, 0x50, 0x15,
                                         0x57, 0x2a, 0x0a, 0x0c, 0x15, 0x04,
                                         0x8e, 0x7d, 0x23, 0x9e, 0x21, 0x00, 0x23, 0x01, 0x21, 0xe3, 0x21, 0xe3, 0x45,
                                         0xc8, 0x03, 0x34, 0x6d, 0xec,
                                         0x57, 0xc3, 0x0c, 0x30, 0xc3, 0x0c,
                                         0x30, 0x12, 0x80, 0x6a, 0xc3, 0xa5, 0x76, 0x5a, 0xc9, 0xc0, 0x80, 0x02, 0x6d,
                                         0xec, 0x2c, 0x6d, 0xec, 0x2c,
                                         0x40, 0x6d, 0xec, 0x2c, 0x72, 0x50,
                                         0x15, 0x57, 0x2a, 0x00, 0x6d, 0xec, 0x2c, 0x22, 0x37, 0xeb, 0x0b, 0x5c, 0x0c,
                                         0x22, 0x37, 0xeb, 0x5a, 0x0b,
                                         0x04, 0xbb, 0x25, 0x04, 0x7c, 0x01,
                                         0xab, 0x12, 0x5f, 0x83, 0xe5, 0x6a, 0x23, 0x6f, 0x52, 0x8a, 0x1e, 0x63, 0x25,
                                         0xf3, 0x5e, 0x5f, 0xff, 0x7f,
                                         0x4f, 0xc0, 0x41, 0x11, 0xc4, 0xda,
                                         0x1e, 0x64, 0x5a, 0x65, 0xa1, 0xde, 0x54, 0x6e, 0x87, 0xa2, 0x67, 0xc5, 0x12,
                                         0x68, 0xff};

    decodedRecords = decoderInterface->startDecodingDatablocks(ASTERIX_datablock, &datablockLength);

    //Example: how to use the DecoderResult object
    int recordsNumber = decodedRecords->getTotalNumberOfRecords();
    int elementsNameSize = 0;
    ElementValue *elementValue = nullptr;

    string elementNameToSearch;

    for (int i = 0; i < recordsNumber; i++) {
        cout << "Record #" << i << endl;
        cout << "\t Category number: " << std::dec << decodedRecords->getCategoryNumberOfARecord(i) << endl;
        cout << "\t\t Raw record: " << std::hex << decodedRecords->getRawRecordInString(i) << endl;

        vector<string *> *elementsName = decodedRecords->getElementNamesOfARecord(i);
        elementsNameSize = elementsName->size();

        for (int j = 0; j < elementsNameSize; j++) {
            elementNameToSearch = elementsName->at(j)->c_str();

            elementValue = decodedRecords->getElementValueOfRecord(i, elementNameToSearch);
            if (elementValue != nullptr) {
                cout << "\t\t " << elementNameToSearch << " - " << elementValue->getDecodedDataInString() << endl;
            }
        }
        cout << endl << endl;
    }

    // **ENCODE records
    if (encodingFunctionOn) {
        unsigned char *outputEncodedBytes = nullptr;    // Where raw bytestream is saved
        outputEncodedBytes = encoderInterface->encodeRecords(decodedRecords, &currentEncodedRecordNumber);

        // ... Print on file or process the raw datastream, and then free memory used...
        delete[] outputEncodedBytes;
    }

    // I am done
    decoderInterface->releaseAllDecodedRecords(decodedRecords);
}

void encodeSingleRecord(ReturnStatus &status) {
    Record *record;

    // VARIABLE DEFINITIONS
    unsigned int categoryNumber = 0;
    unsigned char *datablockWithSingleRecord = nullptr;
    unsigned int currentEncodedRecordNumber = 0;
    vector<pair<string, string>> elements;

    // TRIAL
    categoryNumber = 21;
    elements.push_back(pair<string, string>("SAC", "50"));
    //elements.push_back(pair<string, string>("SIC", "80"));
    //	elements.push_back(pair<string, string>("Latitude", "48.119662"));		// 0x2237eb
    //	elements.push_back(pair<string, string>("Longitude", "15.9743771"));	// 0x0b5c0c
    elements.push_back(pair<string, string>("Mode3A", "06025"));                // 0x0c15  			3093d = 06025

    // Variable LSB | 0x8320
    //	elements.push_back(pair<string, string>("AirSpeed", "0.8"));
    //	elements.push_back(pair<string, string>("IM", "1"));
    //	elements.push_back(pair<string, string>("AirSpeed", "1137.743"));
    //	elements.push_back(pair<string, string>("IM", "0"));

    // Repetitive
    elements.push_back(pair<string, string>("MB DATA", "ab125f83e56a23"));
    elements.push_back(pair<string, string>("BDS1", "6"));
    elements.push_back(pair<string, string>("BDS2", "f"));

    //	//
    //elements.push_back(pair<string, string>("MB DATA_1", "32a65e38f521ba"));
    //elements.push_back(pair<string, string>("BDS1_1", "a"));
    //elements.push_back(pair<string, string>("BDS2_1", "4"));

    // Item021_160	-> 0x21 e3 45 c8
    elements.push_back(pair<string, string>("RE-GSP", "0"));
    elements.push_back(pair<string, string>("Ground Speed", "0.529479"));
    elements.push_back(pair<string, string>("Track Angle", "98.252"));

    // Compound	| FSPEC = 0xc0
    //elements.push_back(pair<string, string>("WS", "85"));	// 0x0055
    //elements.push_back(pair<string, string>("WD", "330"));  // 0x014a

    //I021_110
    elements.push_back(pair<string, string>("NAV", "1"));
    elements.push_back(pair<string, string>("NVB", "0"));

    // Extended | 0x092110
    elements.push_back(pair<string, string>("ATP", "0"));
    elements.push_back(pair<string, string>("ARC", "1"));
    elements.push_back(pair<string, string>("RC", "0"));
    elements.push_back(pair<string, string>("RAB", "0"));
    elements.push_back(pair<string, string>("DCR", "0"));
    elements.push_back(pair<string, string>("GBS", "1"));
    elements.push_back(pair<string, string>("SIM", "1"));
    elements.push_back(pair<string, string>("TST", "1"));
    elements.push_back(pair<string, string>("SAA", "0"));
    elements.push_back(pair<string, string>("CL", "0"));

    //elements.push_back(pair<string, string>("CPR", "1"));
    // REF | 0x0dd5
    elements.push_back(pair<string, string>("HDR", "1"));
    elements.push_back(pair<string, string>("Stat", "1"));
    elements.push_back(pair<string, string>("SelH", "330"));

    //	elements.push_back(pair<string, string>("", ""));

    record = new Record(categoryNumber, elements);

    // // //

    //	record->setCategoryNumber(21);
    //
    //	record->getElements()->insert(
    //			pair<string, ElementValue*>("SAC", new UnsignedInteger8bitValue(nullptr, (uint8_t) stoi("50"), RepresentationModeEnum::IntegerValue)));
    //	record->getElements()->insert(
    //			pair<string, ElementValue*>("SIC", new UnsignedInteger8bitValue(nullptr, (uint8_t) stoi("80"), RepresentationModeEnum::IntegerValue)));
    //	record->getElements()->insert(pair<string, ElementValue*>("Latitude", new FloatValue(nullptr, stoi("48.119662"))));
    //	record->getElements()->insert(pair<string, ElementValue*>("Longitude", new FloatValue(nullptr, stoi("15.9743771"))));
    //	record->getElements()->insert(
    //			pair<string, ElementValue*>("Mode3A", new UnsignedInteger16bitValue(nullptr, (uint16_t) stoi("3093"), RepresentationModeEnum::IntegerValue)));

    if (record->getElements()->size() > 0) {
        datablockWithSingleRecord = encoderInterface->encodeSingleRecord(record, &currentEncodedRecordNumber);
    }

    if (datablockWithSingleRecord != nullptr) {
        delete[] datablockWithSingleRecord;
        datablockWithSingleRecord = nullptr;
    }
}

int main(int argc, char *argv[]) {

    /************************
     * VARIABLES DEFINITION *
     ************************/
    int cmdLineArgsParsing_rc = -1;                    // Result of the command line arguments parsing
    string justSerialiseAndQuit = "";    // string to hold the path to serialised Asterix description and  to flag if user just want to serialise their configuration for further usage and then quit

    string ASTERIX_inputFilePath;                    // Path to the ASTERIX binary input file
    string ASTERIX_categoryRepositoryPath = "asterix/asterixConfiguration.json";                    // Path to the ASTERIX binary input file

    chrono::time_point<chrono::system_clock> startDecoder, startEncoder;
    chrono::time_point<chrono::system_clock> endDecoder, endEncoder;

    ReturnStatus status = ReturnStatus();

    bool encodingFunctionOn = true;

    /**********************************
     * Command Line Arguments parsing *
     **********************************/
    cmdLineArgsParsing_rc = readCommandLineArgs(argc, argv, ASTERIX_inputFilePath, ASTERIX_categoryRepositoryPath,
                                                justSerialiseAndQuit,
                                                encodingFunctionOn);

    //FIXME: not sure if readCommandLineArgs(..) is able to return a negative value
    if (cmdLineArgsParsing_rc < 0) {
        return -1;
    }

    /********************************************
     * ASTX JSON Categories description loading *
     ********************************************/
    codecInterface->loadAsterixConfiguration(ASTERIX_categoryRepositoryPath, status);
    if (status.getCode() != ReturnCodes::SUCCESS) {
        cout << status.getMessage() << endl;
        if (status.getCode() != ReturnCodes::WARNING)
            return status.getCode();
    }

    /********************************************************
     * Serialization of the ASTERIX Description, if enabled *
     ********************************************************/
    if (!justSerialiseAndQuit.empty()) {
        codecInterface->serializeAsterixConfiguration(justSerialiseAndQuit, status);

        if (status.getCode() != ReturnCodes::SUCCESS) {
            cout << status.getMessage() << endl;
            return status.getCode();
        }

        cout << "ASTERIX Description files have been encoded and stored here: " << justSerialiseAndQuit << endl;
        return status.getCode();
    }

    // ** SET UP **
    codecInterface->setUpEnvironment(status);

    if (status.getCode() == ReturnCodes::SPEED_ISSUE) {
        cout << status.getMessage() << endl;
    } else if (status.getCode() != ReturnCodes::SUCCESS) {
        cout << status.getMessage() << endl;
        return status.getCode();
    }

    decoderInterface = codecInterface->getDecoderInterface();
    encoderInterface = codecInterface->getEncoderInterface();

    //** DECODE / ENCODE from file **
    if (ASTERIX_inputFilePath != "") {

        decodeAndEncodeAsterixFile(ASTERIX_inputFilePath, encodingFunctionOn, status);

        if (status.getCode() != ReturnCodes::SUCCESS) {
            cout << status.getMessage() << endl;
            return status.getCode();
        }

    } else if (cmdLineArgsParsing_rc == 10) {

        encodeSingleRecord(status);

        if (status.getCode() != ReturnCodes::SUCCESS) {
            cout << status.getMessage() << endl;
            return status.getCode();
        }
    } else {    //** DECODE / ENCODE from somewhere else **

        decodeAndEncodeFromOtherSource(encodingFunctionOn);
    }

    // Failure report management
    cout << "Saving failure report in a file..." << endl;

    FailureReport *report = codecInterface->getFinalReport();
    if (report != nullptr) {
        if (report->getFailuresNumber() > 0) {
            report->printOnFile("failure_report.log");
        } else {
            remove("failure_report.log");
        }
        report->clearFailures();
    }

    cout << "Done! - Thank you for your patience!" << endl;

    codecInterface->releaseMemory();

    return 0;
}
