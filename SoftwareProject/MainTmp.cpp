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

int intCompare(const void* a, const void* b) {
	return ( *(int*)a - *(int*)b );
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
	int totalFeatures = 0;//TODO ??? do we still need this?
	SPKDArray kdArray = NULL;
	KDTreeNode kdTree = NULL;
	SPBPQueue bpq = NULL;
	int* imagesRates = NULL;
	int QueryNumOfFeats = 0;
	SPPoint* queryFeatures = NULL;
	SPListElement currElement = NULL;

	if (argc == 1) {
		config = spConfigCreate(configFName, &configMsg);
		if (configMsg == SP_CONFIG_CANNOT_OPEN_FILE) {
			printf(OPEN_DEFAULT_CONFIG_ERRROR"\n");
			return 0;
		}
	}

	if (argc == 2 || argc>3) {
		printf(USAGE_ERROR"\n");
		return 0;
	}

	if (strcmp(argv[1], "-c") != 0) {
		printf(USAGE_ERROR"\n");
		return 0;
	}

	strcpy(configFName, argv[2]);
	config = spConfigCreate(configFName, &configMsg);
	if (configMsg == SP_CONFIG_CANNOT_OPEN_FILE) {
		printf(OPEN_CONFIG_EROOR"\n",configFName);
		return 0;
	}


	loggerMsg = spLoggerCreate(config->spLoggerFilename, config->spLoggerLevel);
	if (loggerMsg != SP_LOGGER_SUCCESS) {
		printf(OPEN_LOGGER_ERROR"\n");
		spConfigDestroy(config);
		return 0;
	}

	//store the dimension of all points
	dim = config->spPCADimension;

	if (config->spExtractionMode == true) {
		imageProc = new sp::ImageProc(config);

		for (i =0; i<spConfigGetNumOfImages(config, &configMsg); i++) {
			configMsg =  spConfigGetImagePath(imgPath,config,i);
			configMsg =  spConfigCreateFeatsPath(featsPath,config, i);

			//TODO check if message is some error
			imgFeatures = imageProc->getImageFeatures(imgPath, i, &featuresPerImage);
			if (imgFeatures == NULL) {//TODO come up with relevant string
				loggerMsg =  spLoggerPrintWarning("dfsfdsfs",__FILE__, __func__, __LINE__);
				}
			storeFeatures(imgFeatures, featuresPerImage, featsPath, i, dim);
		}
	delete imageProc;

	}
	//TODO if no feature files exist - print error, terminate

	for (i=0; i<spConfigGetNumOfImages(config, &configMsg); i++) {
		configMsg =  spConfigCreateFeatsPath(featsPath,config, i);
		allFeatures = getFeaturesFromFile(allFeatures,&totalFeatures, featsPath, i);
	}
	//TODO check all features - empty, NULL

	printf("Please enter an image path:\n");
	//scanf();

	kdArray = init(allFeatures, totalFeatures);
	kdTree = initTree(kdArray, config->spKDTreeSplitMethod, 0);
	bpq = spBPQueueCreate(config->spKNN);

	imagesRates = (int*) calloc(config->spNumOfImages,sizeof(int));

	for (i=0; i<QueryNumOfFeats; i++) {
		kNearestNeighbors(kdTree, bpq,queryFeatures[i]);
		for (j = 0; j<config->spKNN; j++);
			currElement = spBPQueuePeek(bpq);
			imagesRates[spListElementGetIndex(currElement)]+=1;
			spBPQueueDequeue(bpq);
	}

	qsort(imagesRates, config->spNumOfImages, sizeof(int),intCompare);

	for (i =0; i<config->spNumOfSimilarImages; i++) {
		printf("the %d closet image is %d\n",i,  imagesRates[i]);
	}


	spLoggerDestroy();
	free(imagesRates);
	printf("done\n");

	return 0;

}
