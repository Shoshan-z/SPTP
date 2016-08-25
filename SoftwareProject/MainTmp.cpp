#include <cstdio>
#include <cstdlib>
#include <cstring>
extern "C" {
#include "SPConfig.h"
}


int main(int argc, char** argv) {
	char configFName[1024] = {0};
	SPConfig config = NULL;
	SP_CONFIG_MSG msg;

	if (argc == 1) {
		strcpy(configFName, "spcbir.config");
		config = spConfigCreate(configFName, &msg);
		if (msg == SP_CONFIG_CANNOT_OPEN_FILE) {
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

	config = spConfigCreate(configFName, &msg);
	if (msg == SP_CONFIG_CANNOT_OPEN_FILE) {
		printf("The configuration file %s couldn’t be open",configFName);
		return 0;
	}





	return 0;

}


