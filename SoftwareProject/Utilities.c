#include <stdio.h>
#include <stdlib.h>
#include "Utilities.h"
#include <string.h>

#define USAGE_ERROR "Invalid command line : use -c <config_filename>"
#define OPEN_DEFAULT_CONFIG_ERRROR "The default configuration file spcbir.config couldn't be open"
#define OPEN_CONFIG_EROOR "The configuration file %s couldn't be open"
#define MAX_SIZE 1024

imageRate imageRateCreate(int imgIndex, int rate){

	imageRate imgR = (imageRate)malloc(sizeof(struct image_rate_t));
	if (imgR == NULL) {
		return NULL;
	}
	imgR->imgIndex = imgIndex;
	imgR->rate = rate;
	return imgR;
}

int rateCompare(const void* a, const void* b) {
	imageRate img1 = *(imageRate* )a;
	imageRate img2 = *(imageRate* )b;

	if ((img2->rate - img1->rate)>0) {
		return 1;
	}
	else if (img2->rate - img1->rate <0){
		return -1;
	}
	else { //if the rate is equal, the image with the smaller index should be returned
		return (img1->imgIndex - img2->imgIndex);
	}
}

void freePointsArray(SPPoint* points, int size) {
	int i = 0;
	for (i=0; i<size; i++) {
		if (points[i] != NULL) {
			spPointDestroy(points[i]);
		}
	}
	free(points);
}


SPConfig checkArgs(int argc, char** argv){
	SPConfig config = NULL;
	char configFName[MAX_SIZE] = "spcbir.config";
	SP_CONFIG_MSG configMsg;

	if (argc == 1) {
		config = spConfigCreate(configFName, &configMsg);
		if (configMsg == SP_CONFIG_CANNOT_OPEN_FILE) {
			printf(OPEN_DEFAULT_CONFIG_ERRROR"\n");
			return NULL;
		}
		return config;
	}
	if (argc == 2 || argc>3) {
		printf(USAGE_ERROR"\n");
		return NULL;
	}
	if (strcmp(argv[1], "-c") != 0) {
		printf(USAGE_ERROR"\n");
		return NULL;
	}
	strcpy(configFName, argv[2]);
	config = spConfigCreate(configFName, &configMsg);
	if (configMsg == SP_CONFIG_CANNOT_OPEN_FILE) {
		printf(OPEN_CONFIG_EROOR"\n",configFName);
		return NULL;
	}
	else if (configMsg != SP_CONFIG_SUCCESS) {
		return NULL;
	}

	return config;
}
