#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

	strcpy(lineCopy, line); //since strcpy changes it's input string
	tmpName = strtok(lineCopy, "=");
	strcpy(name, tmpName);
	tmpVal = strtok(NULL, "\n");
	strcpy(val, tmpVal);

	return;
}

char* trimSpaces(char* str) {
	int len = 0;
	char* strEnd = NULL;

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

	while (*str != '\n') {
		if (*str == ' ' || *str == '\t') {
			return false;
		}
	}
	return true;
}


SP_CONFIG_MSG matchValue(SPConfig config, char* name, char* value){

	if (!checkStrConstraint(name) || !checkStrConstraint(value)){
		return SP_CONFIG_INVALID_STRING;
	}

	if (strcmp(name, "spImagesDirectory") == 0) {
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


	return SP_CONFIG_SUCCESS;
}

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg){
	FILE* fp;
	char line[1024]; //1025? the zero terminating the string
	char name[1024] = {0};
	char val[1024] = {0};
	char* namePtr = NULL;
	char* valPtr = NULL;
	SPConfig config = NULL;

	if (filename == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}

	fp = fopen("config.txt", "r");
	if (fp == NULL) {
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}

	config = (SPConfig) malloc (sizeof(struct sp_config_t));
	if (config == NULL) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	while(fgets(line, sizeof(line), fp) != NULL){
		if (!isComment(line) && !(containsEqual(line))) {
				*msg = SP_CONFIG_INVALID_STRING;
				return NULL;
			}
			splitLine(line, name, val);
			namePtr = trimSpaces(name);
			valPtr = trimSpaces(val);
			//printf("name: %s]]]", namePtr);
			//printf("value: %s]]]\n", valPtr);
			}

	fclose(fp);

	return NULL;
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

