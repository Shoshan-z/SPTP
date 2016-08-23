#ifndef SPFEATURESFILE_H_
#define SPFEATURESFILE_H_
#include "SPPoint.h"
#include "assert.h"

void storeFeatures(SPPoint* features, int numOfFeatures, char* imgPrefix, int imgIndex, char*  spImagesDirectory);

SPPoint* getFeaturesFromFile(int* numOfFeatures, char* imgPrefix, int imgIndex, char* spImagesDirectory);

#endif /* SPFEATURESFILE_H_ */
