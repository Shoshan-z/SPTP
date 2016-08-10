#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SPConfig.h"

bool isComment(char* str) {
	if (str == NULL) {
		//TODO error of some sort
		return true;
	}
	while ((*str == ' ' || *str == '\t') && *str != '\0') { //find the first char which isn't a space or a tab
		str++;
	}
	//if the first char which isn't a space is # return true, else return false
	return (*str == '#');
}

/*
 * return true if the line contains an equal sign
 */
bool containsEqual(char* str) {
	while (*str != '\0') {
		if (*str == '=') {
			return true;
		}
		str++;
	}
	return false;
}

/*
 * split lines by '=' sign
 *
 * if one of the tokens doesn't exist, the pointers will contain "NULL"
 */
void splitLine(char* line, char* name, char* val) {
	char lineCopy[1025];
	char* tmpName = NULL;
	char* tmpVal = NULL;
	int len = 0; //will contain the line length

	len = strlen(line);
	if (line[len-1] == '=') { //the line ends with "="
		line[len-1] = '\0';
		strcpy(name, line);
		val = NULL;
	}
	else {
		strcpy(lineCopy, line); //since strcpy changes it's input string
		tmpName = strtok(lineCopy, "=");
		strcpy(name, tmpName);
		tmpVal = strtok(NULL, "\n");
		strcpy(val, tmpVal);
	}
	return;
}
/*
 * returns a pointer to the string with no spaces in the start and end
 */
char* trimSpaces(char* str) {
	int len = 0;
	char* strEnd = NULL;

	if (str == NULL || str[0] == '\0') {
		return NULL;
	}

	len = strlen(str);
	strEnd = str + len-1; //a pointer to the last char in the string

	//count spaces in the beginning
	while (*str == ' ' || *str == '\t') {
		str++;
	}
	//count spcaces in the end of the va
	while (*strEnd == ' ' || *str == '\t') {
		strEnd--;
		}

	//determine the new end of the string
	*(strEnd+1) = '\0';

	return str;
}

/*
 * returns true if the string contains no spaces
 */
bool checkStrConstraint(char* str) {

	while (*str != '\0') {
		if (*str == ' ' || *str == '\t') {
			return false;
		}
		str++;
	}
	return true;
}

/*
 * returns "false" if the string doesn't represent a positive integer
 */
bool isInt(char* str){
	if (*str == '+') {
		str++;
	}

	while (*str != '\0') {
		if (*str<'0' || *str>'9') {
			return false;
		}
		str++;
	}
	return true;
}

/*
 * returns "false" if the string is not "true" or "false"
 * the boolen value itself will be in *b
 */
bool isBool(char* str, bool* b){
	char copy[6] = {0};
	int i =0;

	while (*str != '\0') {
		copy[i] = tolower(*str);
		str++;
		i++;
	}

	if (strcmp(copy, "true") == 0) {
		*b = true;
		return true;
	}
	if (strcmp(copy, "false") == 0) {
		*b = false;
		return true;
	}
	return false;

}

/**
 * changes a string to be all upper case
 */
void strToUpperCase(char* str) {
	while (*str != '\0') {
		*str = toupper(*str);
		str++;
	}
}


/*
 * the functions takes each line and fill in the correspondent value in the configuration struct
 * return: SP_CONFIG_SUCCESS in case of success,
 * 		   SP_CONFIG_INVALID_STRING
 * 		   SP_CONFIG_INVALID_INTEGER
 * 		   SP_CONFIG_MISSING_SUFFIX
 */
SP_CONFIG_MSG matchValue(SPConfig config, char* name, char* value){
	int tmpNum = 0;
	bool tmpBool = false;

	if (!checkStrConstraint(name) || !checkStrConstraint(value)){
		return SP_CONFIG_INVALID_STRING;
	}

	if (strcmp(name, "spImagesDirectory") == 0) {
		if (value == NULL) {
			return SP_CONFIG_MISSING_DIR;
		}
		strcpy(config->spImagesDirectory, value);
	}

	if (strcmp(name, "spImagesPrefix") == 0) {
		strcpy(config->spImagesPrefix, value);
	}

	if (strcmp(name, "spImagesSuffix") == 0) {
		if (strcmp(value, ".jpg") != 0 && strcmp(value, ".png") !=0 && strcmp(value, ".bmp") != 0  && strcmp(value, ".gif") != 0 ) {
			return SP_CONFIG_MISSING_SUFFIX;
		}
		strcpy(config->spImagesSuffix, value);
	}

	if (strcmp(name, "spNumOfImages") == 0) {
		if (!isInt(value)) {
			return SP_CONFIG_INVALID_INTEGER;
		}
		tmpNum = atoi(value);
		config->spNumOfImages = tmpNum;
		}

	if (strcmp(name, "spPCADimension") == 0) {
			if (!isInt(value)) {
				return SP_CONFIG_INVALID_INTEGER;
			}
			tmpNum = atoi(value);
			if (tmpNum<10 || tmpNum>128) {
				return SP_CONFIG_INVALID_INTEGER;
			}
		config->spPCADimension = tmpNum;
		}

	if (strcmp(name, "spPCAFilename") == 0){
		strcpy(config->spPCAFilename, value);
		}
	if (strcmp(name, "spNumOfFeatures") == 0) {
		if (!isInt(value)) {
			return SP_CONFIG_INVALID_INTEGER;
			}
			tmpNum = atoi(value);
			if (tmpNum==0) {
				return SP_CONFIG_INVALID_INTEGER;
				}
			config->spNumOfFeatures = tmpNum;
		}
	if (strcmp(name, "spExtractionMode") == 0) {
		if (!isBool(value, &tmpBool)) {
			return SP_CONFIG_INVALID_STRING;
		}
		config->spExtractionMode = tmpBool;
		}
	if (strcmp(name, "spNumOfSimilarImages") == 0) {
			if (!isInt(value)) {
				return SP_CONFIG_INVALID_INTEGER;
			}
			tmpNum = atoi(value);
			if (tmpNum==0) {
				return SP_CONFIG_INVALID_INTEGER;
			}
		config->spNumOfSimilarImages = tmpNum;
			}

	if (strcmp(name, "spKDTreeSplitMethod") == 0) {
		strToUpperCase(value);
		if(strcmp(value, "RANDOM") == 0) {
			config->spKDTreeSplitMethod = RANDOM;
			}
		else if(strcmp(value, "MAX_SPREAD") == 0) {
			config->spKDTreeSplitMethod = MAX_SPREAD;
			}
		else if(strcmp(value, "INCREMENTAL") == 0) {
			config->spKDTreeSplitMethod = INCREMENTAL;
			}
		else
			{
			return SP_CONFIG_INVALID_STRING;
				}
			}
		if (strcmp(name, "spKNN") == 0) {
				if (!isInt(value)) {
					return SP_CONFIG_INVALID_INTEGER;
				}
				tmpNum = atoi(value);
				if (tmpNum==0) {
					return SP_CONFIG_INVALID_INTEGER;
				}
			config->spKNN = tmpNum;
				}
		if (strcmp(name, "spMinimalGUI") == 0) {
				if (!isBool(value, &tmpBool)) {
					return SP_CONFIG_INVALID_STRING;
				}
			config->spMinimalGUI = tmpBool;
			}

		if (strcmp(name, "spLoggerLevel") == 0) {
				if (!isInt(value)) {
					return SP_CONFIG_INVALID_INTEGER;
				}
				tmpNum = atoi(value);
				if (tmpNum<1 || tmpNum>4) {
					return SP_CONFIG_INVALID_INTEGER;
				}
			config->spLoggerLevel = tmpNum;
			}

		if (strcmp(name, "spLoggerFilename") == 0) {
			strcpy(config->spLoggerFilename, value);
			}


		return SP_CONFIG_SUCCESS;
}

/*
 * the function returns a pointer to a configuration struct with empty fields
 */
SPConfig createEmptyConfig(){
	SPConfig config = NULL;
	config = (SPConfig) calloc (sizeof(struct sp_config_t), 1);
		if (config == NULL) {
			return NULL;
		}
	 config->spNumOfImages = -1;
	 config->spPCADimension = -1;
	 config->spNumOfFeatures = -1;
	 config->spExtractionMode = false;
	 config->spNumOfSimilarImages = -1;
	 config->spKDTreeSplitMethod = NONE;
	 config->spKNN = -1;
	 config->spMinimalGUI = false;
	 config->spLoggerLevel = -1;

	return config;
}

/*
 *
 *
 */
void printError(const char* filename, int line, SP_CONFIG_MSG msg){
	char* message = NULL;
	char* param = NULL;
	if (msg == SP_CONFIG_INVALID_LINE) {
		message = "Invalid configuration line";
	}
	if (msg == SP_CONFIG_INVALID_STRING || msg == SP_CONFIG_INVALID_INTEGER || msg == SP_CONFIG_MISSING_SUFFIX) {
		message = "Invalid value - constraint not met";
	}
	else {
		if (msg == SP_CONFIG_MISSING_DIR) {
		param = "spImagesDirectory";
		}
		if (msg == SP_CONFIG_MISSING_PREFIX) {
		param = "spImagesPrefix";
		}
		if (msg == SP_CONFIG_MISSING_SUFFIX) {
		param = "spImagesSuffix";
		}
		if (msg == SP_CONFIG_MISSING_NUM_IMAGES) {
		param = "spNumOfImages";
		}
		sprintf(message, "Parameter %s is not set", param);
	}

	printf("File: %s\n",filename);
	printf("Line: %d\n", line);
	printf("Message: %s\n", message);
}

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg){
	FILE* fp = NULL;
	char line[1024] = {0}; //1025? the zero terminating the string
	char name[1024] = {0};
	char val[1024] = {0};
	char* namePtr = NULL;
	char* valPtr = NULL;
	SPConfig config = NULL;
	int lineNum = 0;

	if (filename == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		goto cleanup;
	}

	fp = fopen("config.txt", "r");
	if (fp == NULL) {
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		goto cleanup;
	}

	config = createEmptyConfig();
	if (config == NULL) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	while(fgets(line, sizeof(line), fp) != NULL){
		lineNum++;
		if (isComment(line)) {
				continue;
			}
		if (!(containsEqual(line))) {
			*msg = SP_CONFIG_INVALID_LINE;
			goto cleanup;
			}

		splitLine(line, name, val);
		namePtr = trimSpaces(name);
		valPtr = trimSpaces(val);
		*msg =  matchValue(config, namePtr, valPtr);
		if (*msg != SP_CONFIG_SUCCESS) {
			goto cleanup;
			}
		}


cleanup:
	if (*msg != SP_CONFIG_SUCCESS) {
		printError(filename, lineNum, *msg);
		if (config != NULL) {
			free(config);
		}
	}
	if (fp != NULL) {
		fclose(fp);
	}
	return config;
}

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg) {

	return false;
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg){

	return false;
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg){
	return 0;
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg){

	return 0;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg){

	return 0;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config, int index){

	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config){

	return SP_CONFIG_SUCCESS;
}

void spConfigDestroy(SPConfig config){

}

