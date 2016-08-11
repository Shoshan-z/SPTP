#include <cstdio>
extern "C" {
	#include "SPConfig.h"
}


int main() {
	SPConfig config = NULL;
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;


	config = spConfigCreate("config.txt", &msg);

	printf("%d\n", msg);

	if (msg == SP_CONFIG_SUCCESS ) {
		printf("%s\n", config->spImagesDirectory);
		printf("%s\n",config->spImagesPrefix);
		printf("%s\n",config->spImagesSuffix);
		printf("%d\n",config->spNumOfImages);
		printf("%d\n", config->spPCADimension);
		printf("%s\n",config->spPCAFilename);
		printf("%d\n", config->spNumOfFeatures);
		//config->spExtractionMode = false;
		printf("%d\n", config->spNumOfSimilarImages);
		// config->spKDTreeSplitMethod = NONE;
		printf("%d\n", config->spKNN);
		//config->spMinimalGUI = false;
		printf("%d\n",config->spLoggerLevel);
		printf("%s\n", config->spLoggerFilename);

	}

		char path[1024] = {0};
		spConfigGetImagePath(path, config, 3 );

		printf(path);
	//printf("AAAAAAAA\n");



	return 0;
}
