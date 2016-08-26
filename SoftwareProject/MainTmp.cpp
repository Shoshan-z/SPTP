#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "SPImageProc.h"

extern "C" {
#include "SPConfig.h"
#include "SPLogger.h"
#include "SPFeaturesFile.h"
}


int main(int argc, char** argv) {
	char configFName[1024] = "spcbir.config";
	SPConfig config = NULL;
	SP_CONFIG_MSG configMsg;
	SP_LOGGER_MSG loggerMsg;
	int i = 0;
	sp::ImageProc* imageProc = NULL;
	char imgPath[1024] = {0};
	SPPoint* imgFeatures = NULL;
	SPPoint* allFeatures = NULL;
	int featuresPerImage = 0;
	int totalFeatures = 0;

	if (argc == 1) {
		config = spConfigCreate(configFName, &configMsg);
		if (configMsg == SP_CONFIG_CANNOT_OPEN_FILE) {
			printf("The default configuration file spcbir.config couldn’t be open\n");
			return 0;
		}
	}

	if (argc == 2 || argc>3) {
		printf("Invalid command line : use -c <config_filename>\n");
		return 0;
	}

	if (strcmp(argv[1], "-c") != 0) {
		printf("Invalid command line : use -c <config_filename>\n");
		return 0;
	}

	strcpy(configFName, argv[2]);
	config = spConfigCreate(configFName, &configMsg);
	if (configMsg == SP_CONFIG_CANNOT_OPEN_FILE) {
		printf("The configuration file %s couldn’t be open\n",configFName);
		return 0;
	}


	loggerMsg = spLoggerCreate(config->spLoggerFilename, config->spLoggerLevel);

	if (config->spExtractionMode == true) {
		imageProc = new sp::ImageProc(config);

		for (i =0; i<config->spNumOfImages; i++) {
			sprintf(imgPath, "%s/%s%d%s",config->spImagesDirectory, config->spImagesPrefix, i, config->spImagesSuffix);
			imgFeatures = imageProc->getImageFeatures(imgPath, i, &featuresPerImage);
			storeFeatures(imgFeatures, featuresPerImage, config->spImagesPrefix, i, config->spImagesDirectory);
			totalFeatures += featuresPerImage;
		}
	delete imageProc;

	}

	//TODO problem -while going over the pictures I don't know the total num of Features
	allFeatures = (SPPoint*) malloc(sizeof(SPPoint)*totalFeatures);


	printf("Please enter an image path:\n");



	return 0;

}
