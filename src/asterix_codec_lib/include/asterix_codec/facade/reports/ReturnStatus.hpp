#ifndef FACADE_REPORTS_RETURNSTATUS_HPP_
#define FACADE_REPORTS_RETURNSTATUS_HPP_

#include <string>

#include "return_codes.h"

/**
 * Representation for the return code.
 */
typedef returnCodesEnum ReturnCodes;

class ReturnStatus {
public:

    /**
     * Default constructor.<br>
     * Set the code private field to SUCCESS as default value.
     */
    ReturnStatus() {
        message = "Done.";
        code = ReturnCodes::SUCCESS;
    }

    /**
     * Gets the string message.
     * @return Message
     */
    std::string getMessage() {
        return message;
    }

    /**
     * Sets the string message.
     * @param _message Message to be set
     */
    void setMessage(std::string _message) {
        message = _message;
    }

    /**
     * Gets the actual value of the code field.
     * @return Actual value of code
     */
    ReturnCodes getCode() {
        return code;
    }

    /**
     * Sets the current code.
     * @param _code Enumeration value for the code
     */
    void setCode(ReturnCodes _code) {
        code = _code;
    }

    /**
     * Sets the code to the default one (SUCCESS by default).
     */
    void clear() {
        message = "Done.";
        code = ReturnCodes::SUCCESS;
    }

private:
    /**
     * Textual message
     */
    std::string message;

    /**
     * Enumerator code
     */
    ReturnCodes code;

};

#endif /* FACADE_REPORTS_RETURNSTATUS_HPP_ */
