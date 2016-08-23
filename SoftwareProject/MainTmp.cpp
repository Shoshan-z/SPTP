#include <cstdio>
#include <cstdlib>
extern "C" {
#include "SPConfig.h"
}


int main(int argc, char** argv) {
	char* configFName = "spcbir.config";
	SPConfig config = NULL;
	SP_CONFIG_MSG msg;

	if (argc == 0) {
		config = spConfigCreate(configFName, &msg);
	}

	if (msg == SP_CONFIG_CANNOT_OPEN_FILE) {
		printf("“The default configuration file spcbir.config couldn’t be open\n");
	}


	return 0;

}


