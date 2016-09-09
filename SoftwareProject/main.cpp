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

/**
 * the function saves all images features to .feats file
 *
 * @param config - the configuration struct
 * @param imageProc - the image processing object
 *
 * @return the numbers of files created when successful, zero otherwise
 */
int saveFeaturesToFiles(SPConfig config, sp::ImageProc* imageProc) {
	SP_CONFIG_MSG configMsg;
	int i =0;
	int featuresPerImage = 0;
	int filesStored = 0;
	char imgPath[1024] = {0}, featsPath[1024] = {0};
	SPPoint* imgFeatures = NULL;
	int dim = 0;

	//store the dimension of all points
	dim = config->spPCADimension;

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
			spLoggerPrintWarning(FEATURES_WARNING,__FILE__, __func__, __LINE__);
			continue;
		}
		if (storeFeatures(imgFeatures, featuresPerImage, featsPath, i, dim)== true) {
			filesStored++;
		}
		if (imgFeatures != NULL) {
			freePointsArray(imgFeatures, featuresPerImage);
		}
	}
	return filesStored;
}

int main(int argc, char** argv) {
	SPConfig config = NULL;
	SP_CONFIG_MSG configMsg;
	SP_LOGGER_MSG loggerMsg;
	int i = 0, j=0;
	sp::ImageProc* imageProc = NULL;
	char imgPath[1024] = {0}, featsPath[1024] = {0}, queryPath[1024] = {0};
	SPPoint* allFeatures = NULL;
	int totalFeatures = 0, queryNumOfFeats = 0;
	SPKDArray kdArray = NULL;
	KDTreeNode kdTree = NULL;
	SPBPQueue bpq = NULL;
	imageRate* imagesRates = NULL;
	SPPoint* queryFeatures = NULL;
	SPListElement currElement = NULL;
	int filesStored = 0; //will hold the number of features file successfully stored
	FILE* fp = NULL;
	bool extractMode = false, minGui = false, success = false;

	config = checkArgs(argc, argv);
	if (config == NULL) {
		return -1;
	}

	loggerMsg = spLoggerCreate(config->spLoggerFilename, config->spLoggerLevel);
	if (loggerMsg != SP_LOGGER_SUCCESS) {
		printf(OPEN_LOGGER_ERROR"\n");
		goto cleanup;
	}

	imageProc = new sp::ImageProc(config);

	if (imageProc == NULL) {
		spLoggerPrintError(CREATE_IMGPROC_ERROR, __FILE__, __func__, __LINE__);
		goto cleanup;
	}
	extractMode = spConfigIsExtractionMode(config, &configMsg);

	if (extractMode) {
		spLoggerPrintInfo("Extracting features...");
		filesStored = saveFeaturesToFiles(config, imageProc);
		if (filesStored == 0) {
			spLoggerPrintError(FEATURES_ERROR,__FILE__, __func__, __LINE__);
			goto cleanup;
		}
	}

	for (i=0; i<spConfigGetNumOfImages(config, &configMsg); i++) { 	//create an array which stores the features
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

	spLoggerPrintInfo("Creating database");
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

		spLoggerPrintInfo("Searching for similar images");
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
			if (bpq==NULL){
				spLoggerPrintError(BPQ_ERROR, __FILE__, __func__, __LINE__);
				goto cleanup;
			}
			for (j = 0; j<config->spKNN; j++){
				currElement = spBPQueuePeek(bpq);
				imagesRates[spListElementGetIndex(currElement)]->rate+=1;
				spBPQueueDequeue(bpq);
				spListElementDestroy(currElement);
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

		spLoggerPrintInfo("Displaying results");
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

	printf("Exiting...\n");
	success = true;

cleanup:
	if (allFeatures != NULL){
		freePointsArray(allFeatures, totalFeatures);
	}
	if (kdArray != NULL) {
		SPKDArrayDestroy(kdArray);
	}
	if (queryFeatures != NULL){
		freePointsArray(queryFeatures, queryNumOfFeats);
	}
	if (bpq != NULL){
		spBPQueueDestroy(bpq);
		}
	if (imagesRates != NULL) {
		for (i =0; i<config->spNumOfImages; i++) {
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

	if (config!= NULL) {
		spConfigDestroy(config);
	}
	if (!success) {
		printf("An error occurred, see log file for details\n");
		return -1;
	}
	return 0;
}
