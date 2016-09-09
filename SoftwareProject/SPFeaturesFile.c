#include "SPFeaturesFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALLOC_ERROR_MSG "Allocation error"
#define FEATS_FILE_ERROR "Invalid features file for image %d"
#define WRITE_FAIL "Could not create file for image %d"

bool storeFeatures(SPPoint* features, int numOfFeatures, char* featsPath, int imgIndex, int dim) {
	FILE* fp = NULL;
	int i = 0;
	int j =0;
	double curr = 0;
	int writtenElemnts = 0;
	bool success = false;
	char errorString[1024] = {0};

	if (features == NULL ||numOfFeatures ==0 ||featsPath == NULL ) { //LOGGER
		goto cleanup;
	}

	fp = fopen(featsPath, "wb");
	if (fp == NULL) {
		goto cleanup;
	}

	//write to the file the image index, number of features and dimension
	writtenElemnts = fwrite(&imgIndex, sizeof(imgIndex), 1, fp);
	writtenElemnts = fwrite(&numOfFeatures, sizeof(numOfFeatures), 1, fp);
	writtenElemnts= fwrite(&dim, sizeof(int), 1, fp);
	if (writtenElemnts != 1) {
		goto cleanup;
	}

	for (i=0; i<numOfFeatures; i++) {
		for (j=0; j<dim; j++) {
			curr = spPointGetAxisCoor(features[i], j);
			fwrite(&curr, sizeof(double),1, fp);
			if (writtenElemnts != 1) {
					goto cleanup;
				}
		}
	}
	success = true;

cleanup:
	if (fp!= NULL) {
		fclose(fp);
	}
	if (!success){
		sprintf(errorString, WRITE_FAIL, imgIndex);
		spLoggerPrintWarning(errorString,__FILE__, __func__, __LINE__);
	}

	return success;
}

/**
 * a private utility function
 * the function read from file the image index, number of features and dimension,
 * and checks that the file is in order
 *
 * @param fp - the features file pointer
 * @param imageIndex - the current image index in the loop
 * @param numOfFeatures - a pointer which will hold the number of features in the file
 * @param dim - the dimension according to the config file
 *
 * @return:
 * -true if the read was successful and all information was correct
 * -false otherwise
 */
static bool getMetadataFromFile(FILE* fp, int imageIndex, int* numOfFeatures, int* dim){
	int fileSize = 0, readBytes = 0;
	int imageIndexFromFile = 0;
	//check file size
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	//each file should contain at least the imgageIndex, number of features and dimension
	if ((unsigned int) fileSize <3*sizeof(int)) {
		return false;
	}

	readBytes = fread(&imageIndexFromFile, sizeof(int), 1, fp); 	//read the image index
	if (readBytes != 1) {
		return false;
	}

	if (imageIndexFromFile != imageIndex) {
		return false;
	}

	readBytes = fread(numOfFeatures, sizeof(int), 1, fp); 	//read number of features
	if (readBytes != 1) {
			return false;
		}

	readBytes = fread(dim, sizeof(int), 1, fp);
	if (readBytes != 1) {
		return false;
	}

	return true;
}

/**
 * a private utility function
 * the function create the next image feature(=point) according to the information
 * in the file
 *
 * @param fp - the features file pointer
 * @param imageIndex - the current image index in the loop
 * @param dim - the dimension according to the config file
 *
 * @return:
 * -the new SPPoint
 * -NULL if an allocation or read from file error occurred
 */
static SPPoint createPointFromFile(FILE* fp, int imgIndex, int dim){
	int j = 0, readBytes = 0;
	double* coor = NULL;
	SPPoint newPoint = NULL;

	coor = (double*) malloc (sizeof(double)*(dim)); //create the coordinates array for the current point
	if (coor == NULL) {
		return NULL;
	}
	for(j=0; j<dim; j++) {
		readBytes = fread(coor+j, sizeof(double), 1, fp);
		if (readBytes != 1) {
			free(coor);
			return NULL;
		}
	}

	newPoint = spPointCreate(coor, dim, imgIndex);
	free(coor);

	return newPoint;
}


/**
 * a private utility function
 * the function receives the current "allFeatures" array and add to it the features from the
 * current file being read.
 *
 * @param fp - the features file pointer
 * @param imageIndex - the current image index in the loop
 * @param dim - the dimension according to the config file
 * @param allFeatures - the current features array
 * @param oldNumOfFeatures - the current number of features in "all features"
 * @param numOfFeaturesFromFile - the number of features in the current file
 *
 * @return:
 * -the new allFeatures array if no error occurred
 * -in case of error, the old allFeatures array is returned (without the points from the current file)
 */
static SPPoint* mergePointsFromFile(FILE* fp, int imgIndex, int dim, SPPoint* allFeatures, int oldNumOfFeatures, int numOfFeaturesFromFile)
{
	int i = 0;
	SPPoint* tmpFeatures = NULL;

	tmpFeatures = (SPPoint*) calloc(sizeof(SPPoint), (oldNumOfFeatures + numOfFeaturesFromFile));	//store a pointer to the new all features array
		if (tmpFeatures == NULL) {
			goto error;
	}

	memcpy(tmpFeatures, allFeatures, oldNumOfFeatures * (sizeof(SPPoint)));

	for (i=oldNumOfFeatures; i<oldNumOfFeatures + numOfFeaturesFromFile; i++){
		tmpFeatures[i] = createPointFromFile(fp, imgIndex, dim);
		if (tmpFeatures[i] == NULL) {
			goto error;
		}
	}

	return tmpFeatures;
error:
	if (tmpFeatures != NULL) {
		for (i=oldNumOfFeatures; tmpFeatures[i] != NULL; i++) {
			spPointDestroy(tmpFeatures[i]);
		}
		free(tmpFeatures);
	}

	return NULL;
}

SPPoint* getFeaturesFromFile(SPPoint* allFeatures,int* numOfFeatures, char* featsPath, int imgIndex) {
	FILE* fp = NULL;
	SPPoint* tmpFeatures = NULL;
	int dim = 0, currNumOfFeatures = 0;
	char errorString[1024] = {0};
	bool success = false;

	if (featsPath == NULL){
		goto cleanup;
	}
	fp = fopen(featsPath, "rb");
	if (fp == NULL){ //couldn't open the file
		goto cleanup;
	}

	if (!getMetadataFromFile(fp, imgIndex, &currNumOfFeatures, &dim)) {
		goto cleanup;
	}

	tmpFeatures = mergePointsFromFile(fp, imgIndex, dim, allFeatures, *numOfFeatures, currNumOfFeatures);
	if (tmpFeatures == NULL)
		goto cleanup;

	//update the total number of features
	*numOfFeatures += currNumOfFeatures;

	//if the function reached this line, no error has occurred
	success = true;

cleanup:
	if (!success) {
		sprintf(errorString, FEATS_FILE_ERROR, imgIndex);
		spLoggerPrintWarning(errorString,__FILE__, __func__, __LINE__);

		tmpFeatures = allFeatures;
		}
	else { //if success
		free(allFeatures);
	}
	if (fp != NULL) {
		fclose(fp);
	}
	return tmpFeatures;
}
