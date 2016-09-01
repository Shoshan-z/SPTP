#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SPConfig.h"
#include <assert.h>
#include "SPPoint.h"

bool isEmpty(char* str) {
	if (str == NULL) {
		return true;
	}
	if (str[0] == '\0') {
		return true;
	}

	if (str[0] == '\n') {
		return true;
	}

	return false;
}


bool isComment(char* str) {
	if (isEmpty(str)) {
		return false;
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
	if (isEmpty(str)) {
			return false;
		}
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
	if (line[len-2] == '=') { //the line ends with "="
		line[len-2] = '\0';
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
}
/*
 * returns a pointer to the string with no spaces in the start and end
 */
char* trimSpaces(char* str) {
	int len = 0;
	char* strEnd = NULL;

	if (isEmpty(str)) {
			return NULL;
		}

	len = strlen(str);
	strEnd = str + len-1; //a pointer to the last char in the string

	//count spaces in the beginning
	while (*str == ' ' || *str == '\t') {
		str++;
	}
	//count spcaces in the end of the va
	while (*strEnd == ' ' || *strEnd == '\t') {
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
	if (isEmpty(str)) {
			return false;
		}
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
	if (isEmpty(str)) {
			return false;
		}
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

	if (isEmpty(str)) {
			return false;
		}
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
	if (isEmpty(str)) {
			return;
		}
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
		return SP_CONFIG_SUCCESS;
	}

	if (strcmp(name, "spImagesPrefix") == 0) {
		strcpy(config->spImagesPrefix, value);
		return SP_CONFIG_SUCCESS;
	}

	if (strcmp(name, "spImagesSuffix") == 0) {
		if (strcmp(value, ".jpg") != 0 && strcmp(value, ".png") !=0 && strcmp(value, ".bmp") != 0  && strcmp(value, ".gif") != 0 ) {
			return SP_CONFIG_MISSING_SUFFIX;
		}
		strcpy(config->spImagesSuffix, value);
		return SP_CONFIG_SUCCESS;
	}

	if (strcmp(name, "spNumOfImages") == 0) {
		if (!isInt(value)) {
			return SP_CONFIG_INVALID_INTEGER;
		}
		tmpNum = atoi(value);
		config->spNumOfImages = tmpNum;
		return SP_CONFIG_SUCCESS;
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
		return SP_CONFIG_SUCCESS;
		}

	if (strcmp(name, "spPCAFilename") == 0){
		strcpy(config->spPCAFilename, value);
		return SP_CONFIG_SUCCESS;
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
			return SP_CONFIG_SUCCESS;
		}
	if (strcmp(name, "spExtractionMode") == 0) {
		if (!isBool(value, &tmpBool)) {
			return SP_CONFIG_INVALID_STRING;
		}
		config->spExtractionMode = tmpBool;
		return SP_CONFIG_SUCCESS;
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
		return SP_CONFIG_SUCCESS;
			}

	if (strcmp(name, "spKDTreeSplitMethod") == 0) {
		strToUpperCase(value);
		if(strcmp(value, "RANDOM") == 0) {
			config->spKDTreeSplitMethod = RANDOM;
			return SP_CONFIG_SUCCESS;
			}
		else if(strcmp(value, "MAX_SPREAD") == 0) {
			config->spKDTreeSplitMethod = MAX_SPREAD;
			return SP_CONFIG_SUCCESS;
			}
		else if(strcmp(value, "INCREMENTAL") == 0) {
			config->spKDTreeSplitMethod = INCREMENTAL;
			return SP_CONFIG_SUCCESS;
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
			return SP_CONFIG_SUCCESS;
				}
		if (strcmp(name, "spMinimalGUI") == 0) {
				if (!isBool(value, &tmpBool)) {
					return SP_CONFIG_INVALID_STRING;
				}
			config->spMinimalGUI = tmpBool;
			return SP_CONFIG_SUCCESS;
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
			return SP_CONFIG_SUCCESS;
			}

		if (strcmp(name, "spLoggerFilename") == 0) {
			strcpy(config->spLoggerFilename, value);
			return SP_CONFIG_SUCCESS;
			}


		return SP_CONFIG_INVALID_LINE;
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
	 config->spExtractionMode = true;
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
	char msgBuffer[1024] = {0};

	if (msg == SP_CONFIG_INVALID_LINE) {
		message = "Invalid configuration line";
	}
	else if (msg == SP_CONFIG_INVALID_STRING || msg == SP_CONFIG_INVALID_INTEGER) {
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
		sprintf(msgBuffer, "Parameter %s is not set", param);
		message = msgBuffer;
	}

	printf("File: %s\n",filename);
	printf("Line: %d\n", line);
	printf("Message: %s\n", message);
}

/*
 * checks if one of the parameters with no default value is not set
 */
SP_CONFIG_MSG validateConfig(SPConfig config){
	if (isEmpty(config->spImagesDirectory)) {
		return SP_CONFIG_MISSING_DIR;
	}
	if (isEmpty(config->spImagesPrefix)) {
		return SP_CONFIG_MISSING_PREFIX;
	}
	if (isEmpty(config->spImagesSuffix)){
		return SP_CONFIG_MISSING_SUFFIX;
	}
	if (config->spNumOfImages == -1) {
		return SP_CONFIG_MISSING_NUM_IMAGES;
	}
	return SP_CONFIG_SUCCESS;
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

	fp = fopen(filename, "r");
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
		if (isEmpty(line) || isComment(line)) {
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
		*msg = validateConfig(config);

cleanup:
	if (*msg == SP_CONFIG_CANNOT_OPEN_FILE ) {
		return config;
	};
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

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!= NULL);
	if (!config){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
	}else{
		*msg = SP_CONFIG_SUCCESS;
	}

	return config->spExtractionMode;
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!= NULL);
	if (!config){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
	}else{
		*msg = SP_CONFIG_SUCCESS;
	}

	return config->spMinimalGUI;
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!= NULL);
	if (!config){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1; 	// returns a negative number if not succeed
		}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfImages;
	}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!= NULL);
	if (!config){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1; // returns a negative number if not succeed
	}else{
		*msg = SP_CONFIG_SUCCESS;
	}
	if (config->spNumOfFeatures == -1){ // means no value was entered
		return 100; //default value
	}
	return config->spNumOfFeatures;// check it is fixed after integrate with shoshan
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!= NULL);
		if (!config){
			*msg = SP_CONFIG_INVALID_ARGUMENT;
			return -1; // returns a negative number if not succeed
		}else{
			*msg = SP_CONFIG_SUCCESS;
		}
		if (config->spPCADimension == -1){
			return 20; //default value
		}
	return config->spPCADimension;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config, int index){
	if (!config || !imagePath){
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	if (index >= config->spNumOfImages){
			return SP_CONFIG_INDEX_OUT_OF_RANGE;
		}

	sprintf(imagePath, "%s/%s%d%s",config->spImagesDirectory,config->spImagesPrefix, index, config->spImagesSuffix);
	return SP_CONFIG_SUCCESS;
}


SP_CONFIG_MSG spConfigCreateFeatsPath(char* featsPath, const SPConfig config, int index){
	if (!config || !featsPath){
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	if (index >= config->spNumOfImages){
			return SP_CONFIG_INDEX_OUT_OF_RANGE;
		}

	sprintf(featsPath, "%s/%s%d.feats", config->spImagesDirectory, config->spImagesPrefix, index);

	return SP_CONFIG_SUCCESS;
}


SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config){
	if (!config || !pcaPath){
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	strcat(pcaPath, config->spImagesDirectory);
	strcat(pcaPath , "/");
	strcat(pcaPath, config->spPCAFilename);

	return SP_CONFIG_SUCCESS;
}

void spConfigDestroy(SPConfig config){
	if (config == NULL) {
		return;
	}
	free(config);
}


