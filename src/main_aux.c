
#include<stdlib.h>

#include "main_aux.h"
#include "SPConfig.h"
#include "SPBPriorityQueue.h"
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPKDTree.h"


bool wasConfigurationSuccessful(SP_CONFIG_MSG configMsg, char* commandLineInput){
	if(configMsg == SP_CONFIG_CANNOT_OPEN_FILE){
		printf("The configuration file %s couldn't be open\n", commandLineInput);
		return false;
	}
	if(configMsg == SP_CONFIG_INVALID_ARGUMENT){
		printf("Invalid command line : use -c <config_filename>\n");
		return false;
	}
	if(configMsg == SP_CONFIG_CANNOT_OPEN_FILE){
		printf("The default configuration file spcbir.config couldn’t be open\n");
		return false;
	}
	if(configMsg == SP_CONFIG_ALLOC_FAIL){
		printf("Couldn't allocate memory for configuration\n");
		return false;
	}
	return true;
}

SPPoint* mergeSPPoints(SPPoint* allPoints, SPPoint* newPoints, int* allPointsNum, int newPointsNum){
//void mergeSPPoints(SPPoint* allPoints, SPPoint* newPoints, int* allPointsNum, int newPointsNum){
	SPPoint* temp = (SPPoint*)malloc((*allPointsNum+newPointsNum)*sizeof(SPPoint));
	if(allPoints != NULL){
		for(int i=0; i<*allPointsNum;i++){
			*(temp+i) =  spPointCopy(*(allPoints+i));
			spPointDestroy(*(allPoints+i));
		}
		free(allPoints);
	}
	for(int i=0; i<newPointsNum;i++){
		*(temp+*allPointsNum+i) =  spPointCopy(*(newPoints+i));
		spPointDestroy(*(newPoints+i));
	}
	free(newPoints);
	*allPointsNum = *allPointsNum + newPointsNum;
	return temp;
}

void getCommandFromUser(char* queryPath){
	printf("Please enter an image path:\n");
	fflush(NULL);
	queryPath[0] = '\0';
	scanf("%s", queryPath);
}

int* mostSimilarImagesOfAll(SPBPQueue* allCandidates, int numOfQueryFeats, \
		int numOfSimilarImages, int numOfImages, int spKNN){

	int *nearest = (int*)malloc(numOfSimilarImages * sizeof(int));
	int *all = (int*)calloc(numOfImages, sizeof(int));
	SPList tempList = NULL;
	SPListElement tempListElement = NULL;
	SPBPQueue mostSimilar = spBPQueueCreate(numOfSimilarImages);
	for(int i=0; i<numOfQueryFeats; i++){
		tempList = spBPQueueGetQueue(*(allCandidates+i));
		tempListElement = spListGetFirst(tempList);
		for(int j=0; j<spKNN; j++){
			all[spListElementGetIndex(tempListElement)]++;
			tempListElement = spListGetNext(tempList);
		}
		spListDestroy(tempList);
	}
	for(int i=0; i<numOfImages; i++){
		tempListElement = spListElementCreate(i, all[i]);
		spBPQueueEnqueueToMaximumQueue(mostSimilar, tempListElement);
		spListElementDestroy(tempListElement);
	}
	for(int i=0; i<numOfSimilarImages; i++){
		tempListElement = spBPQueuePeek(mostSimilar);
		nearest[i] = spListElementGetIndex(tempListElement);
		spListElementDestroy(tempListElement);
		spBPQueueDequeue(mostSimilar);
	}
	spBPQueueDestroy(mostSimilar);
	free(all);
	return nearest;
}

void freeResurces(int* bestCandidates, SPBPQueue* allCandidates, SPPoint* queryFeatures, int numOfQueryFeats){
	free(bestCandidates);
	for(int i=0; i<numOfQueryFeats; i++){
		spBPQueueDestroy(*(allCandidates+i));
	}
	free(allCandidates);
	for(int i=0; i<numOfQueryFeats; i++){
		spPointDestroy(*(queryFeatures+i));
	}
	free(queryFeatures);
}

void exiting(SPPoint* allFeatures, int allFeatsNum, SPKDTreeNode featTree, SPConfig config){
	printf("Exiting...\n");
	for(int i=0; i<allFeatsNum; i++){
		spPointDestroy(*(allFeatures+i));
	}
	free(allFeatures);
	KDTreeDestroy(featTree); // in practice this also dose KDArrayDestroy(featArray)
	spConfigDestroy(config);
	spLoggerDestroy();
}
