#include "SPFeaturesFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void storeFeatures(SPPoint* features, int numOfFeatures, char* featsPath, int imgIndex) {
	FILE* fp = NULL;
	//char imgName[1024] = {0};
	int dim = 0;
	int i = 0;
	int j =0;
	double curr = 0;

	//TODO if features is NULL, handle error

	dim = spPointGetDimension(features[0]);

	fp = fopen(featsPath, "wb");
	fwrite(&imgIndex, sizeof(imgIndex), 1, fp);
	fwrite(&numOfFeatures, sizeof(numOfFeatures), 1, fp);
	fwrite(&dim, sizeof(int), 1, fp);

	for (i=0; i<numOfFeatures; i++) {
		for (j=0; j<dim; j++) {
			curr = spPointGetAxisCoor(features[i], j);
			fwrite(&curr, sizeof(double),1, fp);
		}
	}

	fclose(fp);
}


//numOfFeatures will hold the number of features extracted from the file
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

	fp = fopen(featsPath, "rb");
	if (fp == NULL){ //couldn't open the file
		goto cleanup;
	}

	//check file size
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	//each file should contain at least the imgageIndex, number of features and dimension
	if (fileSize <3*sizeof(int)) {
		goto cleanup;
	}

	//read the image index
	readBytes = fread(&indexFromFile, sizeof(int), 1, fp);
	if (readBytes != sizeof(int)) {
		goto cleanup;
	}

	//read number of features
	readBytes = fread(&currNumOfFeatures, sizeof(int), 1, fp);
	if (readBytes != sizeof(int)) {
			goto cleanup;
		}

	//store a pointer to the new all features array
	tmpFeatures = (SPPoint*) calloc(sizeof(SPPoint), (*numOfFeatures+currNumOfFeatures));
		if (tmpFeatures == NULL) {
			goto cleanup;
	}

	memcpy(tmpFeatures, allFeatures,*numOfFeatures*sizeof(SPPoint));


	//allFeatures is now the bigger array we've just re-allocated
	//allFeatures = tmpFeatures;

	readBytes = fread(&dim, sizeof(int), 1, fp);
	if (readBytes != sizeof(int)) {
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
			if (readBytes != sizeof(double)) {
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


