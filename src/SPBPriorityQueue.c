/*
 * SPBPriorityQueue.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "SPBPriorityQueue.h"
#include "SPList.h"
#include "SPListElement.h"

struct sp_bp_queue_t {
	int maxSize;
	SPList queue;
};

SPBPQueue spBPQueueCreate(int maxSize){
	SPBPQueue newQueue = (SPBPQueue)malloc(sizeof(*newQueue));
	if(newQueue == NULL){
		return NULL;
	}
	if(maxSize <= 0){
		free(newQueue);
		return NULL;
	}
	newQueue->queue = spListCreate();
	if(newQueue->queue == NULL){
			free(newQueue);
			return NULL;
	}
	newQueue->maxSize = maxSize;
	return newQueue;
}

SPBPQueue spBPQueueCopy(SPBPQueue source){
	SPBPQueue copyQueue = NULL;
	assert(source!=NULL);
	copyQueue = (SPBPQueue)malloc(sizeof(*copyQueue));
	if(copyQueue == NULL){
			free(copyQueue);
			return NULL;
	}
	copyQueue->queue = spListCopy(source->queue);
	if(copyQueue->queue == NULL){
			free(copyQueue);
			return NULL;
	}
	copyQueue->maxSize = source->maxSize;
	return copyQueue;
}

void spBPQueueDestroy(SPBPQueue source){
	if(source!=NULL){
		spListDestroy(source->queue);
		free(source);
	}
}

void spBPQueueClear(SPBPQueue source){
	if(source!=NULL){
		spListClear(source->queue);
	}
}

int spBPQueueSize(SPBPQueue source){
	if(source!=NULL){
		return spListGetSize(source->queue);
	}
	return -1;
}

int spBPQueueGetMaxSize(SPBPQueue source){
	if(source!=NULL){
		return source->maxSize;
	}
	return -1;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element){
	SPListElement temp = NULL;
	int size;
	if(source == NULL || source->queue == NULL || element == NULL){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	size = spListGetSize(source->queue);

	if(size == 0){ // queue is empty
		spListInsertFirst(source->queue, element);
		return SP_BPQUEUE_SUCCESS;
	}
	if(spListGetSize(source->queue)==source->maxSize){ // queue is full
		temp = spListGetFirst(source->queue);
		for(int i=0; i<size-1; i++){ // going to the last element in the queue
			temp = spListGetNext(source->queue);
		}
		if(spListElementGetValue(temp) > spListElementGetValue(element)){ //check if new value is smaller then the maximum value in the queue
			spListRemoveCurrent(source->queue); // make room for new element
			size -= 1;
		}
		else{ // queue is full & the new value is bigger than the maximum value in the queue
			return SP_BPQUEUE_FULL;
		}
	}
	temp = spListGetFirst(source->queue);
	for(int i=0; i<size; i++){ // now inserting the element
		// queue is full-1 & new element value > queue last element's value ->> insert after last
		if(size == i+1 && spListElementGetValue(element) > spListElementGetValue(temp)){
			spListInsertAfterCurrent(source->queue, element);
			break;
		}
		// queue is full-1 & new element value < queue last element's value ->> insert before last
		else if(size == i+1 && spListElementGetValue(element) < spListElementGetValue(temp)){
			spListInsertBeforeCurrent(source->queue, element);
			break;
		}
		// new element value < current element's value ->> insert before current
		else if(spListElementGetValue(element) < spListElementGetValue(temp)){
			spListInsertBeforeCurrent(source->queue, element);
			break;
		}
		// new element's value == current element's value
		else if(spListElementGetValue(temp) == spListElementGetValue(element)){
			// new element's index < current element's index ->> insert before current
			if(spListElementGetIndex(element) < spListElementGetIndex(temp)){
				spListInsertBeforeCurrent(source->queue, element);
				break;
			}
			// new element's index >= current element's index ->> insert before current
			else{ //(spListElementGetIndex(element) >= spListElementGetIndex(temp))
				spListInsertAfterCurrent(source->queue, element);
				break;
			}
		}
		temp = spListGetNext(source->queue);
	}
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source){
	SPListElement temp = NULL;
	int size;
	if(source == NULL || source->queue == NULL){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	size = spListGetSize(source->queue);
	if(size == 0){
		return SP_BPQUEUE_EMPTY;
	}
	temp = spListGetFirst(source->queue);
	spListElementGetValue(temp);
	spListRemoveCurrent(source->queue);
	//spListElementDestroy(temp);
	return SP_BPQUEUE_SUCCESS;
}

SPListElement spBPQueuePeek(SPBPQueue source){
	SPListElement copy = NULL;
	if(source == NULL || source->queue == NULL){
		return NULL;
	}
	copy = spListElementCopy(spListGetFirst(source->queue));
	return copy;
}

SPListElement spBPQueuePeekLast(SPBPQueue source){
	SPListElement copy = NULL;
	if(source == NULL || source->queue == NULL){
		return NULL;
	}
	copy = spListElementCopy(spListGetLast(source->queue));
	return copy;
}

double spBPQueueMinValue(SPBPQueue source){
	SPListElement temp = NULL;
	double val = -1.0;
	if(source!= NULL && source->queue != NULL){
		temp = spListGetFirst(source->queue);
		val = spListElementGetValue(temp);
		return val;
	}
	return val;
}

double spBPQueueMaxValue(SPBPQueue source){
	SPListElement temp = NULL;
	double val = -1.0;
	if(source!= NULL && source->queue != NULL){
		temp = spListGetLast(source->queue);
		val = spListElementGetValue(temp);
		return val;
	}
	return val;
}

bool spBPQueueIsEmpty(SPBPQueue source){
	assert(source!= NULL);
	if(spListGetSize(source->queue) == 0){
		return true;
	}
	return false;
}

bool spBPQueueIsFull(SPBPQueue source){
	assert(source!= NULL);
	if(spListGetSize(source->queue) == source->maxSize){
		return true;
	}
	return false;
}

SPList spBPQueueGetQueue(SPBPQueue source){
	SPList temp = spListCopy(source->queue);
	return temp;
}

SP_BPQUEUE_MSG spBPQueueEnqueueToMaximumQueue(SPBPQueue source, SPListElement element){
	SPListElement temp = NULL;
	int size;
	if(source == NULL || source->queue == NULL || element == NULL){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	size = spListGetSize(source->queue);

	if(size == 0){ // queue is empty
		spListInsertFirst(source->queue, element);
		return SP_BPQUEUE_SUCCESS;
	}
	if(spListGetSize(source->queue)==source->maxSize){ // queue is full
		temp = spListGetFirst(source->queue);
		for(int i=0; i<size-1; i++){ // going to the last element in the queue
			temp = spListGetNext(source->queue);
		}
		if(spListElementGetValue(temp) < spListElementGetValue(element)){ //check if new value is bigger then the maximum value in the queue
			spListRemoveCurrent(source->queue); // make room for new element
			size -= 1;
		}
		else{ // queue is full & the new value is smaller than the maximum value in the queue
			return SP_BPQUEUE_FULL;
		}
	}
	temp = spListGetFirst(source->queue);
	for(int i=0; i<size; i++){ // now inserting the element
		// queue is full-1 & new element value > queue last element's value ->> insert before last
		if(size == i+1 && spListElementGetValue(element) > spListElementGetValue(temp)){
			spListInsertBeforeCurrent(source->queue, element);
			break;
		}
		// queue is full-1 & new element value < queue last element's value ->> insert after last
		else if(size == i+1 && spListElementGetValue(element) < spListElementGetValue(temp)){
			spListInsertAfterCurrent(source->queue, element);
			break;
		}
		// new element value > current element's value ->> insert before current
		else if(spListElementGetValue(element) > spListElementGetValue(temp)){
			spListInsertBeforeCurrent(source->queue, element);
			break;
		}
		// new element's value == current element's value
		else if(spListElementGetValue(temp) == spListElementGetValue(element)){
			// new element's index < current element's index ->> insert before current
			if(spListElementGetIndex(element) < spListElementGetIndex(temp)){
				spListInsertBeforeCurrent(source->queue, element);
				break;
			}
			// new element's index >= current element's index ->> insert before current
			else{ //(spListElementGetIndex(element) >= spListElementGetIndex(temp))
				spListInsertAfterCurrent(source->queue, element);
				break;
			}
		}
		temp = spListGetNext(source->queue);
	}
	return SP_BPQUEUE_SUCCESS;
}
