#ifndef THREADING_DECODER_H_
#define THREADING_DECODER_H_

#include <iostream>
#include <string>
#include <pthread.h>
#include <vector>

#include "asterix_codec/RecordCollection.h"
#include "data_model/decoder_objects/ElementValue.hpp"
#include "data_model/asterix_description/Item.h"
#include "data_model/blocking_queue/BlockingQueueManager.h"
#include "data_model/decoder_objects/ParsedElement.h"
#include "data_model/decoder_object_pool/ParsedRecord.h"
#include "asterix_codec/facade/reports/FailureReport.h"
#include "utils/DecodingUtils.h"
#include "config/CommonVariables.h"
#include "config/enum/FunctionNameEnum.h"
#include "config/enum/RepresentationModeEnum.h"
#include "config/FacadeCommonVariables.h"

using namespace std;

extern CommonVariables *common;
extern FacadeCommonVariables *facadeCommon;
extern FailureReport *decoderFailureReport;

void decodeAllElementsOfRecord(vector<ParsedElement *> *allParsedElements, unsigned int parsedElementsSize, unsigned int categoryNumber,
                               DecodedValueObjectPoolManager *decodedValueOPManager, vector<string *> *decodedElementNames,
                               vector<ElementValue *> *decodedElementValues,
                               ofstream *outputFile);

RecordCollection *decodeParsedRecords(unsigned int numberOfConsumer, DecodedValueObjectPoolManager *decodedValueOPManager);

#endif /* THREADING_DECODER_H_ */
