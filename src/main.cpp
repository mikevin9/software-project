
/* The main()function */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "SPImageProc.h"
extern "C"{
#include "main_aux.h"
#include "SPLogger.h"
#include "SPConfig.h"
#include "SPKDArray.h"
#include "SPKDTree.h"
#include "SPBPriorityQueue.h"
}

using namespace sp;
#define BUFSIZE 1024
#define NO_FEAT_FILE_MESSAGE(x) message[0] = '\0'; strcat(message, "Feat. file: "); \
	strcat(message, x); strcat(message, " dosen't exist\n");

int main(int argc, char *argv[]){
	ImageProc *proc = NULL;
	SPConfig config = NULL;
	SP_CONFIG_MSG configMsg = SP_CONFIG_SUCCESS;
	SPBPQueue *allCandidates = NULL;
	SPKDArray featArray = NULL;
	SPKDTreeNode featTree = NULL;
	int numOfFeats, imageDim, imageIndex, allFeatsNum = 0, numOfQueryFeats, *bestCandidates;
	char picPath[BUFSIZE] = {'\0'}, featFileName[BUFSIZE] = {'\0'}, queryPath[BUFSIZE] = {'\0'}, message[BUFSIZE] = {'\0'};
	double *featFileData = NULL;
	SPPoint *tempFeatures = NULL, *allFeatures = NULL, *queryFeatures = NULL;
	FILE *featureFileRead = NULL, *featureFileWrite = NULL;

	// Configuration stage:
	if (argc != 1 && argv[2] != NULL){ // Get configuration file from command line argument
		config = spConfigCreate(argv[2], &configMsg);
	}
	else{ // Use default configuration file
		config = spConfigCreate("spcbir.config", &configMsg);
	}
	if((wasConfigurationSuccessful(configMsg, argv[2]) != true) || config == NULL){
		return -1;
	}

	// Creating the logger:
	if(spLoggerCreateFromConfig(spGetLoggerFileName(config, &configMsg), spGetLoggerLevel(config, &configMsg)) != SP_LOGGER_SUCCESS){
		return -1;
	}

	// Initiation From Configuration:
	proc = new ImageProc(config);

	// Extraction Stage:
	if(spConfigIsExtractionMode(config, &configMsg) && configMsg == SP_CONFIG_SUCCESS){ // ExtractionMode == true
		// For each image in imagePath: extract features and write them to a new .feat file. simultaneously, merge all the features into one list
		for(int i=0; i<spConfigGetNumOfImages(config, &configMsg); i++){
			// Extract feats from current pic:
			spConfigGetImagePath(picPath, config, i);
			tempFeatures = proc->getImageFeatures(picPath, i ,&numOfFeats);
			// Create the .feat file and write to it:
			spConfigGetFeatFilePath(featFileName, config, i);
			featureFileWrite = fopen(featFileName, "w+");
			fprintf(featureFileWrite, "%d\n", i); // imageNo
			fprintf(featureFileWrite, "%d\n", numOfFeats); // image num of features:
			fprintf(featureFileWrite, "%d\n", spPointGetDimension(*tempFeatures)); // imageDim
			for(int j=0; j<numOfFeats; j++){ // for each feat of the i'th pic
				for(int k=0; k<spPointGetDimension(*(tempFeatures+j)); k++){
					fprintf(featureFileWrite, "%g\t", spPointGetAxisCoor(*(tempFeatures+j),k));
				}
				fprintf(featureFileWrite, "\n");
			}
			allFeatures = mergeSPPoints(allFeatures, tempFeatures, &allFeatsNum, numOfFeats);
			//tempFeatures ands it's objects are freed in the function "mergeSPPoints" above
			fclose(featureFileWrite);
		}

	}
	else if(!spConfigIsExtractionMode(config, &configMsg) && configMsg == SP_CONFIG_SUCCESS) { // ExtractionMode == false
		// For each feat file: open file, read and create list of points, then merge all lists to one
		for(int i=0; i<spConfigGetNumOfImages(config, &configMsg); i++){
			spConfigGetFeatFilePath(featFileName, config, i);
			featureFileRead = fopen(featFileName, "r");
			if(featureFileRead == NULL){ // if feat. file dosen't exist
				NO_FEAT_FILE_MESSAGE(featFileName);
				spLoggerPrintInfo(message);
				continue;
			}
			fscanf(featureFileRead, "%d", &imageIndex);
			fscanf(featureFileRead, "%d", &numOfFeats);
			fscanf(featureFileRead, "%d", &imageDim);
			tempFeatures = (SPPoint*)malloc(numOfFeats*sizeof(SPPoint));
			for(int j=0; j<numOfFeats; j++){
				featFileData = (double*)malloc(imageDim*sizeof(double));
				for(int k=0; k<imageDim; k++){
					fscanf(featureFileRead, "%lf", (featFileData+k));
				}
				*(tempFeatures+j) = spPointCreate(featFileData, imageDim, imageIndex);
				free(featFileData);
			}
			allFeatures = mergeSPPoints(allFeatures, tempFeatures, &allFeatsNum, numOfFeats);
			//tempFeatures ands it's objects are freed in the function "mergeSPPoints" above
			fclose(featureFileRead);
		}
	}

	// Initiating Data Structure:
	featArray = KDArrayInit(allFeatures, allFeatsNum);
	featTree = KDTreeInit(featArray, config, -1);

	// Get Command:
	getCommandFromUser(queryPath);

	while((strcmp(queryPath, "<>") != 0)){
		// Find Similar Images: for each feature of the query image find SPKNN most similar features and then find the overall best for the hole query image
		queryFeatures = proc->getImageFeatures(queryPath, spConfigGetNumOfImages(config, &configMsg)+1 ,&numOfQueryFeats);
		allCandidates = (SPBPQueue*)malloc(numOfQueryFeats*sizeof(SPBPQueue));
		for(int i=0; i<numOfQueryFeats; i++){
			*(allCandidates+i) = spBPQueueCreate(spConfigGetSPKNN(config, &configMsg));
			KDTreeSearch(featTree, *(allCandidates+i), *(queryFeatures+i));
		}
		bestCandidates = (int*)malloc(spConfigGetNumOfSimilarImages(config, &configMsg)*sizeof(int));
		bestCandidates = mostSimilarImagesOfAll(allCandidates, numOfQueryFeats,	\
				spConfigGetNumOfSimilarImages(config, &configMsg), \
				spConfigGetNumOfImages(config, &configMsg), spConfigGetSPKNN(config, &configMsg));

		// Show Results:
		if(!spConfigMinimalGui(config, &configMsg)){ // Non-Minimal GUI (spMinimalGUI == false)
			printf("Best candidates for - %s - are:\n", queryPath);
			for(int i=0; i<spConfigGetNumOfSimilarImages(config, &configMsg); i++){
				spConfigGetImagePath(picPath, config, *(bestCandidates+i));
				printf("%s\n", picPath);
			}
		}
		else{ // Minimal GUI (spMinimalGUI == true)
			spConfigGetImagePath(picPath, config, *(bestCandidates));
			proc->showImage(picPath);
			for(int i=1; i<spConfigGetNumOfSimilarImages(config, &configMsg); i++){ // i = 0
				spConfigGetImagePath(picPath, config, *(bestCandidates+i));
				proc->showImage(picPath);
			}
		}
		freeResurces(bestCandidates, allCandidates, queryFeatures, numOfQueryFeats);

		// Get Command Again:
		getCommandFromUser(queryPath);
	}

	// Exit:
	exiting(allFeatures, allFeatsNum, featTree, config);
	delete proc;
	//free(proc);
	/* if i run the line above, i'll have no definite loss of memory but will have an error.
	   without it, i have no errors but do have a definite loss of 20 byts */
	return 0;
}
