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


SPPoint* getFeaturesFromFile(SPPoint* allFeatures,int* numOfFeatures, char* featsPath, int imgIndex) {
	SPPoint tmpPoint = NULL;
	SPPoint* tmpFeatures = NULL;
	FILE* fp = NULL;
	int indexFromFile = 0;
	int dim = 0;
	double* coor = NULL;
	int i = 0;
	int j =0;
	int currNumOfFeatures =0;
	int fileSize = 0;
	int readBytes = 0; //will hold the bytes which were currently read
	char errorString[1024] = {0};
	bool success = false;

	if (featsPath == NULL){
		goto cleanup;
	}

	fp = fopen(featsPath, "rb");
	if (fp == NULL){ //couldn't open the file
		goto cleanup;
	}

	//check file size
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	//each file should contain at least the imgageIndex, number of features and dimension
	if ((unsigned int) fileSize <3*sizeof(int)) {
		goto cleanup;
	}

	//read the image index
	readBytes = fread(&indexFromFile, sizeof(int), 1, fp);
	if (readBytes != 1) {
		goto cleanup;
	}

	//read number of features
	readBytes = fread(&currNumOfFeatures, sizeof(int), 1, fp);
	if (readBytes != 1) {
			goto cleanup;
		}

	//store a pointer to the new all features array
	tmpFeatures = (SPPoint*) calloc(sizeof(SPPoint), (*numOfFeatures+currNumOfFeatures));
		if (tmpFeatures == NULL) {
			goto cleanup;
	}

	memcpy(tmpFeatures, allFeatures,*numOfFeatures*(sizeof(SPPoint)));


	readBytes = fread(&dim, sizeof(int), 1, fp);
	if (readBytes != 1) {
		goto cleanup;
		}

	//create the coordinates array for the current point
	coor = (double*) malloc (sizeof(double)*(dim));
	if (coor == NULL) {
		goto cleanup;
	}

	for (i=*numOfFeatures; i<*numOfFeatures+ currNumOfFeatures; i++){
		for(j=0; j<dim; j++) {
			readBytes = fread(coor+j, sizeof(double), 1, fp);
			if (readBytes != 1) {
				goto cleanup;
			}
		}
		tmpPoint = spPointCreate(coor, dim, imgIndex);
		if (tmpPoint == NULL) {
			goto cleanup;
		}
		tmpFeatures[i] = tmpPoint;
	}

	//update the total number of features
	*numOfFeatures += currNumOfFeatures;

	//if the function reached this line, no error has occurred
	success = true;

cleanup:
	if (!success) {
		sprintf(errorString, FEATS_FILE_ERROR, imgIndex);
		spLoggerPrintWarning(errorString,__FILE__, __func__, __LINE__);

		if (tmpFeatures != NULL) {
			for (i=*numOfFeatures; tmpFeatures[i] != NULL; i++) {
				spPointDestroy(tmpFeatures[i]);
			}
			free(tmpFeatures);
		}
		tmpFeatures = allFeatures;
		}
	else //if success
		{
		free(allFeatures);
		}

	if (fp != NULL) {
		fclose(fp);
	}
	return tmpFeatures;
}


