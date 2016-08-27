#ifndef SPFEATURESFILE_H_
#define SPFEATURESFILE_H_
#include "SPPoint.h"
#include "assert.h"
#include "SPLogger.h"

#define FEATS_FILE_ERROR "Invalid features file for image %d"

void storeFeatures(SPPoint* features, int numOfFeatures, char* featsPath, int imgIndex, int dim);

SPPoint* getFeaturesFromFile(SPPoint* allFeatures,int* numOfFeatures, char* featsPath, int imgIndex);

void printFileReadError(int imgIndex);

#endif /* SPFEATURESFILE_H_ */
