#include "utils/PrintingUtils.h"

using namespace std;

void PrintingUtils::printElement(string elementName, int elementLength, unsigned char *elementValue) {
    cout << "\t" << elementName << " - ";
    cout << preprareElement(elementLength, elementValue, true) << endl;
}

string PrintingUtils::preprareElement(int elementLength, unsigned char *elementValue, bool addSpace) {
    stringstream ss;

    int nBytes = elementLength / 8;
    if (elementLength % 8 != 0)
        nBytes++;

    for (int j = 0; j < nBytes; j++) {
        if (elementValue) {
            if (((j == nBytes - 1) || (j == 0)) && (elementLength % 8 != 0))
                ss << std::hex << std::setw(1) << std::setfill('0') << static_cast<unsigned int>(elementValue[j]) << "";
            else
                ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(elementValue[j]) << "";
        }
        if (addSpace)
            ss << " ";
    }
    return ss.str();
}

void PrintingUtils::printItems(std::vector<Item> itemsVector) {
    Item itemTmp;

    for (unsigned int i = 0; i < itemsVector.size(); i++) {
        itemTmp = itemsVector[i];
        std::cout << "\nFRN: " << i << " - " << itemTmp.getName() << std::endl;

        for (unsigned int j = 0; j < itemTmp.getAllElementNames()->size(); j++) {
            if (((*itemTmp.getAllElementNames())[j]) != kSpare)
                std::cout << "\t\t " << (*itemTmp.getAllElementNames())[j] << " - " << (*itemTmp.getAllElementDescription())[j] << " bits"
                          << std::endl;
        }
    }
}
