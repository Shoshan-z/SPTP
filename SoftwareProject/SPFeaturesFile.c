#include "SPFeaturesFile.h"
#include<stdio.h>
#include <stdlib.h>

void storeFeatures(SPPoint* features, int numOfFeatures, char* imgPrefix, int imgIndex, char*  spImagesDirectory) {
	FILE* fp = NULL;
	char fileName[1024] = {0};
	int dim = 0;
	int i = 0;
	int j =0;
	double curr = 0;

	sprintf(fileName, "%s/%s%d.feats",spImagesDirectory, imgPrefix, imgIndex);

	//TODO if features is NULL, handle error

	dim = spPointGetDimension(features[0]);

	fp = fopen(fileName, "wb");
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
SPPoint* getFeaturesFromFile(int* numOfFeatures, char* imgPrefix, int imgIndex, char* spImagesDirectory) {
	SPPoint* features = NULL;
	SPPoint tmpPoint = NULL;
	FILE* fp = NULL;
	char fileName[1024] = {0};
	int indexFromFile = 0;
	int dim = 0;
	double* coor = NULL;
	int i = 0;
	int j =0;

	sprintf(fileName, "%s/%s%d.feats",spImagesDirectory, imgPrefix, imgIndex);

	fp = fopen(fileName, "rb");

	fread(&indexFromFile, sizeof(int), 1, fp);
	//assert(indexFromFile == imgIndex);

	fread(numOfFeatures, sizeof(int), 1, fp);

	features = (SPPoint*) malloc (sizeof(SPPoint)*(*numOfFeatures));

	fread(&dim, sizeof(int), 1, fp);

	coor = (double*) malloc (sizeof(double)*(dim));

	for (i = 0; i<*numOfFeatures; i++){
		for(j=0; j<dim; j++) {
			fread(coor+j, sizeof(double), 1, fp);
		}
		tmpPoint = spPointCreate(coor, dim, imgIndex);
		features[i] = tmpPoint;
	}


	fclose(fp);

	return features;

}

