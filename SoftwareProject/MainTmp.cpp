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

struct image_rate_t{
	int imgIndex;
	int rate;
};

typedef struct image_rate_t* imageRate;


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
				}
			storeFeatures(imgFeatures, featuresPerImage, featsPath, i, dim);
			filesStored++;
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

	//TODO errors and looger are set up to this point in main
	kdArray = init(allFeatures, totalFeatures);
	kdTree = initTree(kdArray, config->spKDTreeSplitMethod, 0);
	if (kdTree == NULL) {
		printf("kdAtree is null\n");
	}

	printf("Please enter an image path:\n");
	fflush(stdout);
	scanf("%s", queryPath);
	fflush(stdin);

	while (strcmp(queryPath, "<>")!=0 ) {
		queryFeatures = imageProc->getImageFeatures((const char*)queryPath, config->spNumOfImages, &queryNumOfFeats);
		if (queryFeatures == NULL){
			//handle error
		}

		bpq = spBPQueueCreate(config->spKNN);
		if (bpq == NULL) {
			//handle error
		}
		imagesRates = (imageRate*) calloc(config->spNumOfImages,sizeof(imageRate));
		if (imagesRates == NULL) {
			//handle error
		}
		//initializing an array of numOfImages images rates which will hold the results
		for(i=0; i<config->spNumOfImages; i++) {
			imagesRates[i] = imageRateCreate(i, 0);
		}

		for (i=0; i<queryNumOfFeats; i++) {
			kNearestNeighbors(kdTree, bpq,queryFeatures[i]);
			for (j = 0; j<config->spKNN; j++){
				currElement = spBPQueuePeek(bpq);
				imagesRates[spListElementGetIndex(currElement)]->rate+=1;
				spBPQueueDequeue(bpq);
			}
		}

		qsort(imagesRates, config->spNumOfImages, sizeof(int),rateCompare);

		minGui = spConfigMinimalGui(config, &configMsg);
		if (configMsg != SP_CONFIG_SUCCESS) {
			loggerMsg = spLoggerPrintWarning(MINIMAL_GUI_NOT_SET_WARNING,__FILE__, __func__, __LINE__);
		}

		//display the results
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
		printf("Please enter an image path:\n");
		fflush(stdout);
		scanf("%s", queryPath);
		fflush(stdin);
	}

	printf("Exiting…\n");
	success = true;

cleanup:
	if (allFeatures != NULL){
		for (i=0; i<totalFeatures; i++){
			spPointDestroy(allFeatures[i]);
		}
		free(allFeatures);
	}
	if (queryFeatures != NULL) {
		for (i=0; i<queryNumOfFeats; i++) {
			spPointDestroy(queryFeatures[i]);
		}
		free(queryFeatures);
	}
	if (kdArray != NULL) {
		SPKDArrayDestroy(kdArray);
	}
	//free image rates
	if (imagesRates != NULL) {
		for (i =0; i<config->spNumOfSimilarImages; i++) {
			free(imagesRates[i]);
		}
		free(imagesRates);
	}
	if (config!= NULL) {
		spConfigDestroy(config);
	}
	if (imageProc != NULL) {
		delete imageProc;
	}
	if (bpq != NULL){
		spBPQueueDestroy(bpq);
	}
	spLoggerDestroy();


	if (!success) {
		return -1;
	}
	return 0;
}
