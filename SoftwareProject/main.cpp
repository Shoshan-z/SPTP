#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "SPImageProc.h"

extern "C" {
#include "SPConfig.h"
#include "SPLogger.h"
#include "SPFeaturesFile.h"
#include "KDTree.h"
#include "KNNSearch.h"
#include "SPListElement.h"
#include "Utilities.h"
}

#define USAGE_ERROR "Invalid command line : use -c <config_filename>"
#define OPEN_DEFAULT_CONFIG_ERRROR "The default configuration file spcbir.config couldn’t be open"
#define OPEN_CONFIG_EROOR "The configuration file %s couldn’t be open"
#define OPEN_LOGGER_ERROR "Cannot initialize logger"
#define CREATE_IMGPROC_ERROR "Could not create the image processing object"
#define IMAGE_PATH_WARNING "Image path couldn't be resolved"
#define FEATS_PATH_WARNING "Features file path couldn't be resolved"
#define FEATURES_WARNING "Image features could not be extracted"
#define FEATURES_ERROR "No features files created"
#define NO_FEATURES_EXTRACTED_ERROR "No features were extracted"
#define ALLOC_ERROR_MSG "Allocation error"
#define MINIMAL_GUI_NOT_SET_WARNING "Cannot display images in non-Minimal-GUI mode"
#define KD_ARRAY_ERROR "Could not create KDArray"
#define KD_TREE_ERROR "Could not create KDTree"
#define QUERY_PATH_ERROR "Could not open query image"
#define BPQ_ERROR "Could not create BPQueue"

//TODO add info printing!!!

int main(int argc, char** argv) {
	char configFName[1024] = "spcbir.config";
	SPConfig config = NULL;
	SP_CONFIG_MSG configMsg;
	SP_LOGGER_MSG loggerMsg;
	int i = 0;
	int j = 0;
	sp::ImageProc* imageProc = NULL;
	char imgPath[1024] = {0};
	char featsPath[1024] = {0};
	SPPoint* imgFeatures = NULL;
	SPPoint* allFeatures = NULL;
	int dim = 0;
	int featuresPerImage = 0;
	int totalFeatures = 0;
	SPKDArray kdArray = NULL;
	KDTreeNode kdTree = NULL;
	SPBPQueue bpq = NULL;
	imageRate* imagesRates = NULL;
	int queryNumOfFeats = 0;
	SPPoint* queryFeatures = NULL;
	SPListElement currElement = NULL;
	char queryPath[1024] = {0};
	int filesStored = 0; //will hold the number of features file successfully stored
	FILE* fp = NULL;
	bool extractMode = false;
	bool minGui = false;
	bool success = false;


	if (argc == 1) {
		config = spConfigCreate(configFName, &configMsg);
		if (configMsg == SP_CONFIG_CANNOT_OPEN_FILE) {
			printf(OPEN_DEFAULT_CONFIG_ERRROR"\n");
			return -1;
		}
	}

	if (argc == 2 || argc>3) {
		printf(USAGE_ERROR"\n");
		return -1;
	}

	if (strcmp(argv[1], "-c") != 0) {
		printf(USAGE_ERROR"\n");
		return -1;
	}

	strcpy(configFName, argv[2]);
	config = spConfigCreate(configFName, &configMsg);
	if (configMsg == SP_CONFIG_CANNOT_OPEN_FILE) {
		printf(OPEN_CONFIG_EROOR"\n",configFName);
		return -1;
	}
	else if (configMsg != SP_CONFIG_SUCCESS) {
		return -1;
	}


	loggerMsg = spLoggerCreate(config->spLoggerFilename, config->spLoggerLevel);
	if (loggerMsg != SP_LOGGER_SUCCESS) {
		printf(OPEN_LOGGER_ERROR"\n");
		goto cleanup;
	}

	//store the dimension of all points
	dim = config->spPCADimension;

	imageProc = new sp::ImageProc(config);

	if (imageProc == NULL) {
		spLoggerPrintError(CREATE_IMGPROC_ERROR, __FILE__, __func__, __LINE__);
		goto cleanup;
	}
	extractMode = spConfigIsExtractionMode(config, &configMsg);

	if (extractMode) {
		for (i =0; i<spConfigGetNumOfImages(config, &configMsg); i++) {
			configMsg =  spConfigGetImagePath(imgPath,config,i);
			if (configMsg != SP_CONFIG_SUCCESS) {
				spLoggerPrintWarning(IMAGE_PATH_WARNING, __FILE__, __func__, __LINE__);
				continue;
			}
			configMsg =  spConfigCreateFeatsPath(featsPath,config, i);
			if (configMsg != SP_CONFIG_SUCCESS) {
				spLoggerPrintWarning(FEATS_PATH_WARNING, __FILE__, __func__, __LINE__);
				continue;
				}
			imgFeatures = imageProc->getImageFeatures(imgPath, i, &featuresPerImage);
			if (imgFeatures == NULL) {
				loggerMsg = spLoggerPrintWarning(FEATURES_WARNING,__FILE__, __func__, __LINE__);
				continue;
				}
			if (storeFeatures(imgFeatures, featuresPerImage, featsPath, i, dim)== true) {
				filesStored++;
			}
			if (imgFeatures != NULL) {
				freePointsArray(imgFeatures, featuresPerImage);
			}

		}
		if (filesStored == 0) {
			spLoggerPrintError(FEATURES_ERROR,__FILE__, __func__, __LINE__);
			goto cleanup;
		}
	}

	//create an array which stores the features
	for (i=0; i<spConfigGetNumOfImages(config, &configMsg); i++) {
		configMsg =  spConfigCreateFeatsPath(featsPath,config, i);
		if (configMsg != SP_CONFIG_SUCCESS) {
			spLoggerPrintWarning(FEATS_PATH_WARNING, __FILE__, __func__, __LINE__);
			continue;
		}
		allFeatures = getFeaturesFromFile(allFeatures,&totalFeatures, featsPath, i);
	}

	if (allFeatures == NULL || totalFeatures == 0) {
		spLoggerPrintError(NO_FEATURES_EXTRACTED_ERROR, __FILE__, __func__, __LINE__);
		goto cleanup;
	}

	kdArray = init(allFeatures, totalFeatures);
	if (kdArray == NULL){
		spLoggerPrintError(KD_ARRAY_ERROR, __FILE__, __func__, __LINE__);
		goto cleanup;
	}

	kdTree = initTree(kdArray, config->spKDTreeSplitMethod, 0);
	if (kdTree == NULL || treeSuccess == false) {
		spLoggerPrintError(KD_TREE_ERROR, __FILE__, __func__, __LINE__);
		goto cleanup;
	}

	while (strcmp(queryPath, "<>")!=0 ) {
		bpq = NULL;
		queryFeatures = NULL;

		printf("Please enter an image path:\n");
		fflush(stdout);
		scanf("%s", queryPath);
		fflush(stdin);

		if (strcmp(queryPath, "<>")==0) {
			break;
		}
		fp = fopen (queryPath, "r");
		if (fp == NULL){
			printf("Invalid query path\n\n");
			spLoggerPrintWarning(IMAGE_PATH_WARNING, __FILE__, __func__, __LINE__);
			fflush(stdout);
			continue;
		}
		fclose(fp);

		queryFeatures = imageProc->getImageFeatures((const char*)queryPath, config->spNumOfImages, &queryNumOfFeats);
		if (queryFeatures == NULL){
			spLoggerPrintError(QUERY_PATH_ERROR, __FILE__, __func__, __LINE__);
			goto cleanup;
		}

		bpq = spBPQueueCreate(config->spKNN);
		if (bpq == NULL) {
			spLoggerPrintError(BPQ_ERROR, __FILE__, __func__, __LINE__);
			goto cleanup;
		}
		imagesRates = (imageRate*) calloc(config->spNumOfImages,sizeof(imageRate));
		if (imagesRates == NULL) {
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			goto cleanup;
		}
		//initializing an array of numOfImages images rates which will hold the results
		for(i=0; i<config->spNumOfImages; i++) {
			imagesRates[i] = imageRateCreate(i, 0);
		}

		//counting the number of "hits" per image
		for (i=0; i<queryNumOfFeats; i++) {
			kNearestNeighbors(kdTree, bpq,queryFeatures[i]);
			for (j = 0; j<config->spKNN; j++){
				currElement = spBPQueuePeek(bpq);
				imagesRates[spListElementGetIndex(currElement)]->rate+=1;
				spBPQueueDequeue(bpq);
			}
		}

		//free the queue and the query features
		if (bpq != NULL){
			spBPQueueDestroy(bpq);
		}
		if (queryFeatures != NULL) {
			freePointsArray(queryFeatures, queryNumOfFeats);
		}

		qsort(imagesRates, config->spNumOfImages, sizeof(imagesRates[0]),rateCompare);

		//display the results
		minGui = spConfigMinimalGui(config, &configMsg);
		if (configMsg != SP_CONFIG_SUCCESS) {
			loggerMsg = spLoggerPrintWarning(MINIMAL_GUI_NOT_SET_WARNING,__FILE__, __func__, __LINE__);
		}

		for (i=0; i<config->spNumOfSimilarImages; i++) {
			configMsg =  spConfigGetImagePath(imgPath,config,imagesRates[i]->imgIndex);
			if (configMsg != SP_CONFIG_SUCCESS) {
				spLoggerPrintWarning(IMAGE_PATH_WARNING, __FILE__, __func__, __LINE__);
				continue;
			}
			if (minGui) {
				imageProc->showImage(imgPath);
			} else {
				if (i==0) {
					printf("Best candidates for - %s - are:\n", queryPath);
				}
				printf("%s\n", imgPath);
			}
		}
	}

	printf("Exiting…\n");
	success = true;

cleanup:
	if (config!= NULL) {
		spConfigDestroy(config);
	}
	if (allFeatures != NULL){
		freePointsArray(allFeatures, totalFeatures);
	}

	if (kdArray != NULL) {
		SPKDArrayDestroy(kdArray);
	}

	if (queryFeatures != NULL){
		freePointsArray(queryFeatures, queryNumOfFeats);
	}

	if (bpq != NULL){ //TODO check if need to also be freed here
		spBPQueueDestroy(bpq);
		}
	//free image rates
	if (imagesRates != NULL) {
		for (i =0; i<config->spNumOfSimilarImages; i++) {
			free(imagesRates[i]);
		}
		free(imagesRates);
	}
	if (imageProc != NULL) {
		delete imageProc;
	}

	if (kdTree != NULL){
		KDTreeDestroy(kdTree);
	}
	spLoggerDestroy();

	if (!success) {
		printf("An error occurred, see log file for details\n");
		return -1;
	}
	return 0;
}
