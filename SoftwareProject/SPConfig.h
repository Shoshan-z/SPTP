#ifndef SPCONFIG_H_
#define SPCONFIG_H_

#include <stdbool.h>
#include <stdio.h>
#include "SPLogger.h"

/**
 * A data-structure which is used for configuring the system.
 */


typedef enum sp_config_msg_t {
	SP_CONFIG_MISSING_DIR,
	SP_CONFIG_MISSING_PREFIX,
	SP_CONFIG_MISSING_SUFFIX,
	SP_CONFIG_MISSING_NUM_IMAGES,
	SP_CONFIG_CANNOT_OPEN_FILE,
	SP_CONFIG_ALLOC_FAIL,
	SP_CONFIG_INVALID_INTEGER,
	SP_CONFIG_INVALID_STRING,
	SP_CONFIG_INVALID_ARGUMENT,
	SP_CONFIG_INDEX_OUT_OF_RANGE,
	SP_CONFIG_INVALID_LINE,
	SP_CONFIG_SUCCESS
} SP_CONFIG_MSG;

typedef enum sp_KDTree_Split_Method_t {
	NONE,
	RANDOM,
	MAX_SPREAD,
	INCREMENTAL
}SPLIT_METHOD;

struct sp_config_t {
	char spImagesDirectory[1024];
	char spImagesPrefix[1024];
	char spImagesSuffix[5];
	int spNumOfImages;
	int spPCADimension;
	char spPCAFilename[1024];
	int spNumOfFeatures;
	bool spExtractionMode;
	int spNumOfSimilarImages;
	SPLIT_METHOD spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI;
	SP_LOGGER_LEVEL spLoggerLevel;
	char spLoggerFilename[1024];
};

typedef struct sp_config_t* SPConfig;

/**
 * Creates a new system configuration struct. The configuration struct
 * is initialized based on the configuration file given by 'filename'.
 * 
 * @param filename - the name of the configuration file
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return NULL in case an error occurs. Otherwise, a pointer to a struct which
 * 		   contains all system configuration.
 * 
 * The resulting value stored in msg is as follow:
 * - SP_CONFIG_INVALID_ARGUMENT - if filename == NULL
 * - SP_CONFIG_CANNOT_OPEN_FILE - if the configuration file given by filename cannot be open
 * - SP_CONFIG_ALLOC_FAIL - if an allocation failure occurred
 * - SP_CONFIG_INVALID_INTEGER - if a line in the config file contains invalid integer
 * - SP_CONFIG_INVALID_STRING - if a line in the config file contains invalid string
 * - SP_CONFIG_MISSING_DIR - if spImagesDirectory is missing
 * - SP_CONFIG_MISSING_PREFIX - if spImagesPrefix is missing
 * - SP_CONFIG_MISSING_SUFFIX - if spImagesSuffix is missing 
 * - SP_CONFIG_MISSING_NUM_IMAGES - if spNumOfImages is missing
 * - SP_CONFIG_SUCCESS - in case of success
 *
 *
 */
SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg);

/*
 * Returns true if spExtractionMode = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns true if spMinimalGUI = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the number of images set in the configuration file, i.e the value
 * of spNumOfImages.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the number of features to be extracted. i.e the value
 * of spNumOfFeatures.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Returns the dimension of the PCA. i.e the value of spPCADimension.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Given an index 'index' the function stores in imagePath the full path of the
 * ith image.
 *
 * For example:
 * Given that the value of:
 *  spImagesDirectory = "./images/"
 *  spImagesPrefix = "img"
 *  spImagesSuffix = ".png"
 *  spNumOfImages = 17
 *  index = 10
 *
 * The functions stores "./images/img10.png" to the address given by imagePath.
 * Thus the address given by imagePath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @param index - the index of the image.
 *
 * @return
 * - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 * - SP_CONFIG_INDEX_OUT_OF_RANGE - if index >= spNumOfImages
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index);

/**
 * The function stores in pcaPath the full path of the pca file.
 * For example given the values of:
 *  spImagesDirectory = "./images/"
 *  spPcaFilename = "pca.yml"
 *
 * The functions stores "./images/pca.yml" to the address given by pcaPath.
 * Thus the address given by pcaPath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @return
 *  - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 *  - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config);

/**
 * Given an index 'index' the function stores in featsPath the full path of the
 * ".feats" file of the ith image.
 *
 * For example:
 * Given that the value of:
 *  spImagesDirectory = "./images/"
 *  spImagesPrefix = "img"
 *  spNumOfImages = 17
 *  index = 10
 *
 * The functions stores "./images/img10.feats" to the address given by imagePath.
 * Thus the address given by imagePath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @param index - the index of the image.
 *
 * @return
 * - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 * - SP_CONFIG_INDEX_OUT_OF_RANGE - if index >= spNumOfImages
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigCreateFeatsPath(char* featsPath, const SPConfig config, int index);

/**
 * Frees all memory resources associate with config. 
 * If config == NULL nothig is done.
 */
void spConfigDestroy(SPConfig config);


/**
 * The function determines if str is considered an empty string: new line, NULL string or zeros string
 *
 * @param str - the string to be checked
 *
 * @return
 *  - true - in case of an empty string (one of the conditions above is true)
 *  - false - otherwise
 */
bool isEmpty(char* str);

/**
 * The function determines if str, stripped from spaces, starts with #
 *
 * @param str - the string to be checked
 *
 * @return
 *  - true - if str starts with # and is therefore a comment
 *  - false - otherwise
 */
bool isComment(char* str);


/**
 * The function returns true if the line contains an equal sign
 *
 * @param str - the string to be checked
 *
 * @return
 *  - true - if str contains an equal sign
 *  - false - otherwise
 */
bool containsEqual(char* str);


/**
 * The function splits lines by '=' sign and stores both parts in "name" and "val"
 *
 * @param line - the line to be split
 * @param name - will hold what appeared before the "="
 * @parm val - will hold what appeared after the "="
 *
 * if one of the tokens doesn't exist, the pointers will contain "NULL"
 *
 */
void splitLine(char* line, char* name, char* val);

/**
 * The function returns a pointer to the string with no spaces in the start and end
 *
 * @param str - the string to be trimmed
 *
 * @return
 * a pointer the string without spaces, NULL if the str was empty
 */
char* trimSpaces(char* str);


/**
 * The function returns true if the string contains no spaces
 *
 * @param str - the string to be checked
 *
 * @return
 *  - true - if str doesn't contain any spaces
 *  - false - otherwise
 */
bool checkStrConstraint(char* str);

/**
 * The function returns false if the string doesn't represent a positive integer
 *
 * @param str - the string to be checked
 *
 * @return
 *  - true - if str represent a positive integer
 *  - false - otherwise
 */
bool isInt(char* str);

/**
 * The function returns"false" if the string is not "true" or "false"
 *
 * @param str - the string to be checked
 * @param b - will hold the actual boolean value that str represents
 *
 * @return
 *  - true - if str represents the words "true" or "false"
 *  - false - otherwise
 */
bool isBool(char* str, bool* b);

/**
 * the function changes a string to be all upper case
 */
void strToUpperCase(char* str);

/*
 * the functions takes each line and fill in the correspondent value in the configuration struct
 *
 * @param config - a config struct
 * @param name - the param name from the configuration file
 * @param value - the corresponding value from the configuration file
 *
 * @return:
 * - SP_CONFIG_SUCCESS in case of success
 * - SP_CONFIG_INVALID_STRING - if the string given in name ore value is invalid
 * - SP_CONFIG_INVALID_INTEGER - if one of the given integers doesn't meet the relevant constraint
 * - SP_CONFIG_MISSING_SUFFIX - if the image suffix doesn't meet the relevant constraint
 * - SP_CONFIG_INVALID_LINE - if the name doesn't match any of the config's fields
 *
 */
SP_CONFIG_MSG matchValue(SPConfig config, char* name, char* value);

/**
 * the function returns a pointer to a configuration struct with empty fields,
 * or NULL in case of an allocation error
 *
 */
SPConfig createEmptyConfig();

/*
 * the function prints an error message to stdout
 *
 * @param filename - the configiraton file name
 * @param line - the line in the configuration file
 * @param msg - the relevant error message
 *
 */
void printError(const char* filename, int line, SP_CONFIG_MSG msg);

/*
 * the function checks if one of the parameters with no default value is not set
 *
 * @param config - the configuration file to be checked
 *
 * @return
 * - SP_CONFIG_SUCCESS - in case all mandatory parameters are set
 * - SP_CONFIG_MISSING_DIR - if images directory is missing
 * - SP_CONFIG_MISSING_PREFIX - if "image prefix" is missing
 * - SP_CONFIG_MISSING_SUFFIX - if "image suffix" is missing
 * - SP_CONFIG_MISSING_NUM_IMAGES - if "num of images" is missing
 */
SP_CONFIG_MSG validateConfig(SPConfig config);
#endif /* SPCONFIG_H_ */
