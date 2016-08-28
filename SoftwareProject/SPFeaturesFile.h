#ifndef SPFEATURESFILE_H_
#define SPFEATURESFILE_H_
#include "SPPoint.h"
#include "assert.h"
#include "SPLogger.h"

#define FEATS_FILE_ERROR "Invalid features file for image %d"
#define WRITE_FAIL "Could not create file for image %d"


/*
 * Saves the features of each image to the relevant .feats file
 *
 * @param features - the features to be written to the file
 * @param numOfFeatures - the number of features to be written
 * @param featsPath - the path to the file that should be written
 * @param imgindex - the index of the relevant image
 * @param dim - the dimension of all the features of the image
 *
 * A warning is printed if:
 * 	-features is NULL
 * 	-numOfFeatires is zero
 * 	-featsPath is NULL
 * 	-an error occurred when writing to the file
 */
void storeFeatures(SPPoint* features, int numOfFeatures, char* featsPath, int imgIndex, int dim);


/*
 * Retrieves the features (an SPPoint array) of the image given in imgIndex,
 * adds them to the allFeatures array (which hold the features of from all files read
 * so far) and returns allFeatures
 *
 * @param allFeatures - the current allFeatures array
 * @param numOfFeatures - a pointer to an int which now holds size of the current allFeatures array,
 * 						  and in which the new size will be stores
 * @param featsPath - the path to the file to be read
 * @param imgIndex - the index of the image which corresponds to the current file
 *
 * @return SPPoint array which includes the current image features, the former SPPoint array
 * 					in case of error
 *
 *A warning is printed if:
 * 	-featsPath is NULL
 * 	-the file size is less than 3*sizeof(int)
 * 	-an error occurred when reading the file
 */
SPPoint* getFeaturesFromFile(SPPoint* allFeatures,int* numOfFeatures, char* featsPath, int imgIndex);


#endif /* SPFEATURESFILE_H_ */
