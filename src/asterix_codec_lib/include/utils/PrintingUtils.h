#ifndef UTILS_PRINTINGUTILS_H_
#define UTILS_PRINTINGUTILS_H_

#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

#include "config/Constants.h"
#include "data_model/asterix_description/Item.h"

class PrintingUtils {
public:
    /**
     * It prints the information related to an element, that are name and value.
     * @param elementName Name of the element
     * @param elementLength Length in bits of the element
     * @param elementValue Value of the element
     */
    static void printElement(std::string elementName, int elementLength, unsigned char *elementValue);

    /**
     * It prints the details of a vector of Item.
     * @param itemsVector Vector with all items to be printed
     */
    static void printItems(std::vector<Item> itemsVector);

private:
    /**
     * Constructor
     * The PrintingUtils is a static class so this method is private.
     */
    PrintingUtils();

    /**
     * Destructor
     * The PrintingUtils is a static class so this method is private.
     */
    virtual ~PrintingUtils();

    /**
     * It returns back the right String format of the value of an element.
     * @param elementLength Length in bits of the element
     * @param elementValue Value of the element
     * @param addSpace Boolean value to add a space at the end of the element (better readability)
     * @return String with the value of the element
     */
    static string preprareElement(int elementLength, unsigned char *elementValue, bool addSpace);
};

#endif /* UTILS_PRINTINGUTILS_H_ */
