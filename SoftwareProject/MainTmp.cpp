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
#define OPEN_DEFAULT_CONFIG_ERRROR "The default configuration file spcbir.config couldn�t be open"
#define OPEN_CONFIG_EROOR "The configuration file %s couldn�t be open"
#define OPEN_LOGGER_ERROR "Cannot initialize logger"

struct image_rate_t{
	int imgIndex;
	int rate;
};

typedef struct image_rate_t* imageRate;


imageRate imageRateCreate(int imgIndex, int rate){

	imageRate imgR = (imageRate)malloc(sizeof(struct image_rate_t)); ////TODO FREE- Make sure we free this space
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
	int totalFeatures = 0;//TODO ??? do we still need this?
	SPKDArray kdArray = NULL;
	KDTreeNode kdTree = NULL;
	SPBPQueue bpq = NULL;
	imageRate* imagesRates = NULL;
	int QueryNumOfFeats = 0;
	SPPoint* queryFeatures = NULL;
	SPListElement currElement = NULL;
	char queryPath[1024] = {0};

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

	imageProc = new sp::ImageProc(config);

	if (imageProc == NULL) {
		printf("null imageproc\n");
	}

	if (config->spExtractionMode == true) {

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
	}
	//TODO if no feature files exist - print error, terminate

	for (i=0; i<spConfigGetNumOfImages(config, &configMsg); i++) {
		configMsg =  spConfigCreateFeatsPath(featsPath,config, i);
		allFeatures = getFeaturesFromFile(allFeatures,&totalFeatures, featsPath, i);
	}
	//TODO check all features - empty, NULL

	kdArray = init(allFeatures, totalFeatures);
	kdTree = initTree(kdArray, config->spKDTreeSplitMethod, 0);
	if (kdTree == NULL) {
		printf("kdArray is null\n");
	}
	printf("Please enter an image path:\n");
	fflush(stdout);
	scanf("%s", queryPath);
	fflush(stdin);
	queryFeatures = imageProc->getImageFeatures((const char*)queryPath, config->spNumOfImages, &QueryNumOfFeats);


	bpq = spBPQueueCreate(config->spKNN);

	imagesRates = (imageRate*) calloc(config->spNumOfImages,sizeof(imageRate));

	//initializing an array of numOfImages images rates which will hold the results
	for(i=0; i<config->spNumOfImages; i++) {
		imagesRates[i] = imageRateCreate(i, 0);
	}

	printf("query num of features is %d\n",QueryNumOfFeats);

	for (i=0; i<QueryNumOfFeats; i++) {
		kNearestNeighbors(kdTree, bpq,queryFeatures[i]);
		for (j = 0; j<config->spKNN; j++){
			currElement = spBPQueuePeek(bpq);
			imagesRates[spListElementGetIndex(currElement)]->rate+=1;
			spBPQueueDequeue(bpq);
		}
	}

	qsort(imagesRates, config->spNumOfImages, sizeof(int),rateCompare);

	for (i =0; i<config->spNumOfSimilarImages; i++) {
		printf("the %d closet image is %d\n",i,  imagesRates[i]->imgIndex);
	}

	//free image rates
	for (i =0; i<config->spNumOfSimilarImages; i++) {
		free(imagesRates[i]);
	}
	free(imagesRates);

	spLoggerDestroy();
	delete imageProc;
	printf("done\n");

	return 0;

}
