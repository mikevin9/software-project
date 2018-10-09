#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include "SPConfig.h"

#define BUFSIZE 1024
#define FEAT_FILE_EXTENTION ".feat"

#define RETURN_NULL(x) free(configLine); fclose(file); free(variablePart); if(x==0){free(valuePart);} return NULL


struct sp_config_t {
	char* spImagesDirectory;
	char* spImagesPrefix;
	char* spImagesSuffix; // {.jpg, .png, .bmp, .gif}
	int spNumOfImages; // >0.
	int spPCADimension; // >0 && between [10,28]. [default = 20]
	char* spPCAFilename; // [default = pca.yml]
	int spNumOfFeatures; // >0. [default = 100]
	bool spExtractionMode; // [default = true]
	int spNumOfSimilarImages; // >0 [default = 1]
	SP_SPLIT_METHOD spKDTreeSplitMethod; //{RANDOM, MAX_SPREAD, INCREMENTAL}. [default = MAX_SPREAD]
	int spKNN; // >0 [default = 1]
	bool spMinimalGUI; // [default = false]
	int spLoggerLevel; // {1,2,3,4}. 1=error; 2=warning; 3=info; 4=debug. [default = 3]
	char* spLoggerFilename; // [default = stdout]
};


/*
 * Sets default values to a config's parameters.
 *
 * @param newConfig - pointer to the new configuration struct
 *
 */
void startUpCongifWithDefault(SPConfig* newConfig);

/*
 * Checks if in a given string represents an int.
 *
 * @param tempChar - the string
 * @return 0 if the string represents an int.
 * otherwise, returns 1.
 *
 */
int isInt(char* tempChar);

/*
 * Checks if in a given config all parameters without default values were set.
 *
 * @param config - the configuration structure
 * @return NULL if the configuration is OK,
 * otherwise, returns a string with a name of one not-set parameter.
 *
 */
char* isProperConfig(SPConfig newConfig, SP_CONFIG_MSG* msg);

/*
 * Checks if a given string has opening and ending spaces.
 *
 * @param *target - the string to check
 * @return new String without spaces
 *
 */
char* discardStartEndSpaces(char* target, int flag);


SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg){
	SPConfig newConfig = NULL;
	FILE *file = NULL;
	char *configLine = NULL, *variablePart = NULL, *equalChar = NULL, *valuePart = NULL;
	char *isProper = NULL, *temp = NULL;
	int tempInt, lineCounter, spaceFlag;
	assert(msg!=NULL);
	if(filename==NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}

	file = fopen(filename,"r");
	if(file == NULL){
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}
	newConfig = (SPConfig)malloc(sizeof(*newConfig));
	if(newConfig==NULL){
		*msg = SP_CONFIG_ALLOC_FAIL;
		fclose(file);
		return NULL;
	}

	startUpCongifWithDefault(&newConfig);

	// now i'll read the file
	configLine = (char*)malloc(BUFSIZE*sizeof(char)+1);
	lineCounter = 0;
	while(fgets(configLine, BUFSIZE, file)!= NULL){
		lineCounter++;
		// getting rid of spaces in the beginning of the variable part of current config line
		variablePart = configLine;
		variablePart = discardStartEndSpaces(variablePart, 0);
		// checking if the current config line is a comment/empty line
		if(*variablePart == '#' || *variablePart == '\n' || (strcmp(variablePart, "\r\n") == 0)){
			free(variablePart);
			continue;
		}
		// checking if variablePart has spaces between chars
		temp = variablePart;
		spaceFlag = 0;
		while(*temp != '=' && *temp != '\0'){
			if(spaceFlag == 1){ // had space before
				if(*temp != ' ' || *temp != '\t'){ // config line has a "cahr_space_char" sequence
					INVALID_LINE_ERROR(filename,lineCounter);
					*msg = SP_CONFIG_INVALID_INTEGER;
					RETURN_NULL(1);
				}
			}
			if(*temp == ' ' || *temp == '\t'){
				spaceFlag = 1;
			}
			temp++;
		}
		equalChar = strchr(configLine, '=');
		if(equalChar == NULL){ // current config line has no '=' char
			INVALID_LINE_ERROR(filename,lineCounter);
			*msg = SP_CONFIG_INVALID_INTEGER;
			RETURN_NULL(1);
		}
		equalChar++;
		if(*equalChar == '\n' || strcmp(equalChar, "\r\n") == 0){ // current config line is: "somthing="
			INVALID_LINE_ERROR(filename,lineCounter);
			*msg = SP_CONFIG_INVALID_INTEGER;
			RETURN_NULL(1);
		}
		// getting rid of spaces in the beginning of the value part of current config line
		while((*equalChar != '\n' || strcmp(equalChar, "\r\n") != 0) && (*equalChar == ' ' || *equalChar == '\t')){
			equalChar++;
		}
		// checking if valuePart has spaces between chars
		valuePart = equalChar;
		spaceFlag = 0;
		while(*valuePart != '\0'){
			if(spaceFlag == 1){ // had space before
				if(*valuePart != ' ' || *valuePart != '\t'){ // config line has a "cahr_space_char" sequence
					INVALID_LINE_ERROR(filename,lineCounter);
					*msg = SP_CONFIG_INVALID_INTEGER;
					RETURN_NULL(1);;
				}
			}
			if(*valuePart == ' ' || *valuePart == '\t'){
				spaceFlag = 1;
			}
			valuePart++;
		}
		valuePart = equalChar;
		valuePart = discardStartEndSpaces(valuePart, 1);

		// now i'll look for the relevant parameter; if there are spaces in the variablePart we'll find no match and there'll be an INVALID_LINE_ERROR
		if(strcmp(variablePart, "spImagesDirectory") == 0){
			strcpy(newConfig->spImagesDirectory, valuePart);
		}
		else if(strcmp(variablePart, "spImagesPrefix") == 0){
			strcpy(newConfig->spImagesPrefix, valuePart);
		}
		else if(strcmp(variablePart, "spImagesSuffix") == 0){
			if((strcmp(valuePart, ".jpg") != 0) &&\
					(strcmp(valuePart, ".png") != 0) &&\
					(strcmp(valuePart, ".bmp") != 0) &&\
					(strcmp(valuePart, ".gif") != 0)){
				CONSTRAINT_ERROR(filename,lineCounter);
				RETURN_NULL(0);
			}
			strcpy(newConfig->spImagesSuffix, valuePart);
		}
		else if(strcmp(variablePart, "spPCAFilename") == 0){
			strcpy(newConfig->spPCAFilename, valuePart);
		}
		else if(strcmp(variablePart, "spLoggerFilename") == 0){
			strcpy(newConfig->spLoggerFilename, valuePart);
		}
		else if(strcmp(variablePart, "spNumOfImages") == 0){
			if((isInt(valuePart) != 0)){
				INVALID_LINE_ERROR(filename,lineCounter);
				*msg = SP_CONFIG_INVALID_INTEGER;
				RETURN_NULL(0);
			}
			tempInt = atoi(valuePart);
			if(tempInt<=0){
				CONSTRAINT_ERROR(filename,lineCounter);
				RETURN_NULL(0);
			}
			newConfig->spNumOfImages = tempInt;
		}
		else if(strcmp(variablePart, "spPCADimension") == 0){
			if(isInt(valuePart) != 0){
				INVALID_LINE_ERROR(filename,lineCounter);
				*msg = SP_CONFIG_INVALID_INTEGER;
				RETURN_NULL(0);
			}
			tempInt = atoi(valuePart);
			if(!(tempInt>=10 && tempInt<=28)){
				CONSTRAINT_ERROR(filename,lineCounter);
				RETURN_NULL(0);
			}
			newConfig->spPCADimension = tempInt;
		}
		else if(strcmp(variablePart, "spNumOfFeatures") == 0){
			if(isInt(valuePart) != 0){
				INVALID_LINE_ERROR(filename,lineCounter);
				*msg = SP_CONFIG_INVALID_INTEGER;
				RETURN_NULL(0);
			}
			tempInt = atoi(valuePart);
			if(tempInt<=0){
				CONSTRAINT_ERROR(filename,lineCounter);
				RETURN_NULL(0);
			}
			newConfig->spNumOfFeatures = tempInt;
		}
		else if(strcmp(variablePart, "spNumOfSimilarImages") == 0){
			if(isInt(valuePart) != 0){
				INVALID_LINE_ERROR(filename,lineCounter);
				*msg = SP_CONFIG_INVALID_INTEGER;
				RETURN_NULL(0);
			}
			tempInt = atoi(valuePart);
			if(tempInt<=0){
				CONSTRAINT_ERROR(filename,lineCounter);
				RETURN_NULL(0);
			}
			newConfig->spNumOfSimilarImages = tempInt;
		}
		else if(strcmp(variablePart, "spKNN") == 0){
			if(isInt(valuePart) != 0){
				INVALID_LINE_ERROR(filename,lineCounter);
				*msg = SP_CONFIG_INVALID_INTEGER;
				RETURN_NULL(0);
			}
			tempInt = atoi(valuePart);
			if(tempInt<=0){
				CONSTRAINT_ERROR(filename,lineCounter);
				RETURN_NULL(0);
			}
			newConfig->spKNN = tempInt;
		}
		else if(strcmp(variablePart, "spLoggerLevel") == 0){
			if(isInt(valuePart) != 0){
				INVALID_LINE_ERROR(filename,lineCounter);
				RETURN_NULL(0);
			}
			tempInt = atoi(valuePart);
			if(!(tempInt>=1 && tempInt<=4)){
				CONSTRAINT_ERROR(filename,lineCounter);
				RETURN_NULL(0);
			}
			newConfig->spLoggerLevel = tempInt;
		}
		else if(strcmp(variablePart, "spExtractionMode") == 0){
			if(strcmp(valuePart, "true") == 0){
				newConfig->spExtractionMode = true;
			}
			else if(strcmp(valuePart, "false") == 0){
				newConfig->spExtractionMode = false;
			}
			else{
				CONSTRAINT_ERROR(filename,lineCounter);
				RETURN_NULL(0);
			}
		}
		else if(strcmp(variablePart, "spMinimalGUI") == 0){
			if(strcmp(valuePart, "true") == 0){
				newConfig->spMinimalGUI = true;
			}
			else if(strcmp(valuePart, "false") == 0){
				newConfig->spMinimalGUI = false;
			}
			else{
				CONSTRAINT_ERROR(filename,lineCounter);
				RETURN_NULL(0);
			}
		}
		else if(strcmp(variablePart, "spKDTreeSplitMethod") == 0){
			if(strcmp(valuePart, "RANDOM") == 0){
				newConfig->spKDTreeSplitMethod = RANDOM;
			}
			else if(strcmp(valuePart, "MAX_SPREAD") == 0){
				newConfig->spKDTreeSplitMethod = MAX_SPREAD;
			}
			else if(strcmp(valuePart, "INCREMENTAL") == 0){
				newConfig->spKDTreeSplitMethod = INCREMENTAL;
			}
			else{
				CONSTRAINT_ERROR(filename,lineCounter);
				RETURN_NULL(0);
			}
		}
		else{
			INVALID_LINE_ERROR(filename,lineCounter);
			RETURN_NULL(0);
		}
		free(variablePart);
		free(valuePart);
	}
	free(configLine);
	isProper = isProperConfig(newConfig, msg);
	if(isProper != NULL){
		VALUE_NOT_SET_ERROR(filename,lineCounter,isProper);
		fclose(file);
		return NULL;
	}
	*msg = SP_CONFIG_SUCCESS;
	fclose(file);
	return newConfig;
}

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!=NULL);
	if(config==NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spExtractionMode;
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!=NULL);
	if(config==NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spMinimalGUI;
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!=NULL);
	if(config==NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfImages;
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!=NULL);
	if(config==NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfFeatures;
}

int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!=NULL);
	if(config==NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfSimilarImages;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!=NULL);
	if(config==NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spPCADimension;
}

int spConfigGetSPKNN(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!=NULL);
	if(config==NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spKNN;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config, int index){
	if(imagePath == NULL || config == NULL){
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if(index >= config->spNumOfImages){
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	imagePath[0] = '\0';
	strcat(imagePath, config->spImagesDirectory);
	strcat(imagePath, config->spImagesPrefix);
	char indexAsString[BUFSIZE];
	sprintf(indexAsString, "%d", index);
	strcat(imagePath, indexAsString);
	strcat(imagePath, config->spImagesSuffix);
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetFeatFilePath(char* featFilePath, const SPConfig config, int index){
	if(featFilePath == NULL || config == NULL){
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if(index >= config->spNumOfImages){
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	featFilePath[0] = '\0';
	strcat(featFilePath, config->spImagesDirectory);
	strcat(featFilePath, config->spImagesPrefix);
	char indexAsString[BUFSIZE];
	sprintf(indexAsString, "%d", index);
	strcat(featFilePath, indexAsString);
	strcat(featFilePath, FEAT_FILE_EXTENTION);
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config){
	if(pcaPath == NULL || config == NULL){
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	pcaPath[0] = '\0';
	strcat(pcaPath, config->spImagesDirectory);
	strcat(pcaPath, config->spPCAFilename);
	return SP_CONFIG_SUCCESS;
}

void spConfigDestroy(SPConfig config){
	free(config->spImagesDirectory);
	free(config->spImagesPrefix);
	free(config->spImagesSuffix);
	free(config->spLoggerFilename);
	free(config->spPCAFilename);
	free(config);
}

int spGetLoggerLevel(SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!=NULL);
	if(config==NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spLoggerLevel;
}

char* spGetLoggerFileName(SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!=NULL);
	if(config==NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	*msg = SP_CONFIG_SUCCESS;
	if(strcmp(config->spLoggerFilename, "stdout") == 0){
		return NULL;
	}
	return config->spLoggerFilename;
}

//SP_SPLIT_METHOD spGetSplitMethode(SPConfig config, SP_CONFIG_MSG* msg){
int spGetSplitMethode(SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg!=NULL);
	if(config==NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return (int)config->spKDTreeSplitMethod;
}

void startUpCongifWithDefault(SPConfig* newConfig){
	(*newConfig)->spImagesDirectory = (char*)malloc(BUFSIZE*sizeof(char));
	strcpy((*newConfig)->spImagesDirectory, "NULL");
	(*newConfig)->spImagesPrefix = (char*)malloc(BUFSIZE*sizeof(char));
	strcpy((*newConfig)->spImagesPrefix, "NULL");
	(*newConfig)->spImagesSuffix = (char*)malloc(BUFSIZE*sizeof(char));
	strcpy((*newConfig)->spImagesSuffix, "NULL");
	(*newConfig)->spNumOfImages = -1;
	(*newConfig)->spPCADimension = 20;
	(*newConfig)->spPCAFilename = (char*)malloc(BUFSIZE*sizeof(char));
	strcpy((*newConfig)->spPCAFilename, "pca.yml");
	(*newConfig)->spNumOfFeatures = 100;
	(*newConfig)->spExtractionMode = true;
	(*newConfig)->spNumOfSimilarImages = 1;
	(*newConfig)->spKDTreeSplitMethod = MAX_SPREAD;
	(*newConfig)->spKNN = 1;
	(*newConfig)->spMinimalGUI = false;
	(*newConfig)->spLoggerLevel = 3;
	(*newConfig)->spLoggerFilename = (char*)malloc(BUFSIZE*sizeof(char));
	strcpy((*newConfig)->spLoggerFilename, "stdout");
	return;
}

int isInt(char* tempChar){
	int count = 0;
	if (isdigit(*(tempChar+count)) == 0 && *(tempChar+count) != '+'){
		return 1;
	}
	count++;
	while(*(tempChar+count) != '\0'){
		if(isdigit(*(tempChar+count)) == 0){
			return 1;
		}
		count++;
	}
	return 0;
}

char* isProperConfig(SPConfig newConfig, SP_CONFIG_MSG* msg){
	if(strcmp(newConfig->spImagesDirectory, "NULL") == 0){
		*msg = SP_CONFIG_MISSING_DIR;
		return "spImagesDirectory";
	}
	if(strcmp(newConfig->spImagesPrefix, "NULL") == 0){
		*msg = SP_CONFIG_MISSING_PREFIX;
		return "spImagesPrefix";
	}
	if(strcmp(newConfig->spImagesSuffix, "NULL") == 0){
		*msg = SP_CONFIG_MISSING_SUFFIX;
		return "spImagesSuffix";
	}
	if(newConfig->spNumOfImages == -1){
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		return "spNumOfImages";
	}
	return NULL;
}

char* discardStartEndSpaces(char* target, int flag){
	char *temp = (char*)malloc(BUFSIZE*sizeof(char));
	int i = 0;
	if(flag == 0){ // for the variablePart
		while(*target != '='){
			if(*target != ' ' && *target != '\t'){
				*(temp + i) = *target;
				i++;
			}
			target++;
		}
	}
	else{ // for the valuePart
		while(*target != '\n' && (strcmp(target, "\r\n") != 0)){
			if(*target != ' ' && *target != '\t'){
				*(temp + i) = *target;
				i++;
			}
			target++;
		}
	}
	*(temp + i) = '\0';
	//free(target);
	return temp;
}
