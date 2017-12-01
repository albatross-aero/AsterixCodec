#ifndef FACADE_REPORTS_RETURN_CODES_H_
#define FACADE_REPORTS_RETURN_CODES_H_

/**
 * List of all available message and error codes set in the facade/reports/ReturnStatus class.
 *
 * When a new Enumeration is added, the overloaded operator <i>std::ostream& operator<< </i> in FailureReport.cpp shall be updated with the definition of
 * the new enum type.
 */
enum returnCodesEnum {

	/**
	 * SUCCESS value - Returned if everything went fine.
	 */
	SUCCESS = 0,

	/**
	 * A problem occurred with the serialization (serialization not completed).
	 */
	SERIALIZATION_FAILED = 1,

	/**
	 * An error occurred during the loading of the ASTERIX input file.
	 */
	AST_LOADER_ERROR = 2,

	/**
	 * An error occurred during the loading of the ASTERIX configuration Archive or JSON files not found.
	 */
	AST_CONFIG_LOADER_ERROR = 3,

	/**
	 * Impossible to load the ASTERIX Configuration into the library because it is not well-formed.
	 */
	AST_MALFORMED_DESCRIPTION = 4,

	/**
	 * Speed issue warning (i.e. number of producer threads not proportional to the
	 * number of consumer threads, pool size too small for the total threads accessing to it ...).
	 */
	SPEED_ISSUE = 5,

	/**
	 * Critical input parameters, impossible to continue with the decoding in order to have good performance.
	 */
	CRITICAL_PARAMETERS = 6,

	/**
	 * An error occurred during the parsing phase (i.e. empty Datablock).
	 */
	PARSING_ERROR = 7,

	/**
	 * An error occurred: the specified Category is unknown.
	 */
	UNKNOWN_CATEGORY = 8,

	/**
	 * An error occurred: the Field Reference Number does not exist.
	 */
	UNKNOWN_FRN = 9,

	/**
	 * An error occurred during the decoding phase.
	 */
	DECODING_ERROR = 10,

	/**
	 * An error occurred during the decoding/encoding phase because the decoding function for the specified Element is unknown.
	 */
	UNKNOWN_DECODING_FUNCTION = 11,

	/**
	 * An error occurred during the decoding/encoding phase because the representation mode specified for the Element is unknown.
	 */
	UNKNOWN_REPRESENTATION_MODE = 12,

	/**
	 * An error occurred during the encoding phase.
	 */
	ENCODING_ERROR = 13,

	/**
	 * Validation error. The Element value is out of its valid range during the decoding phase.
	 */
	OUT_OF_RANGE_DECODED_ELEMENT = 14,

	/**
	 * Validation error. The Element value is out of its valid range during the decoding phase.
	 */
	OUT_OF_RANGE_ENCODED_ELEMENT = 15,

	/**
	 * Validation error. A Mandatory Item of the Category the parsing Item belong to was missing.
	 */
	MISSING_MANDATORY_ITEM = 16,

	/**
	 * Generic WARNING. It is not a blocking error.
	 */
	WARNING = 17
};

#endif /* FACADE_REPORTS_RETURN_CODES_H_ */
