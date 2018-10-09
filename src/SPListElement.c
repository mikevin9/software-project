#include "SPListElement.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct sp_list_element_t {
	int index;
	double value;
};

SPListElement spListElementCreate(int index, double value) {
	SPListElement temp = NULL;
	if(index < 0 || value <0.0){
		return NULL;
	}
	temp = (SPListElement) malloc(sizeof(*temp));
	if (temp == NULL) { //Allocation Fails
		return NULL;
	}
	temp->index = index;
	temp->value = value;
	return temp;
}

SPListElement spListElementCopy(SPListElement element) {
	SPListElement elementCopy = NULL;
	if (element == NULL) {
		return NULL;
	}
	elementCopy = (SPListElement) malloc(sizeof(*elementCopy));
	if (elementCopy == NULL) {
		return NULL;
	}
	elementCopy->index = element->index;
	elementCopy->value = element->value;
	return elementCopy;
}

void spListElementDestroy(SPListElement element) {
	if (element == NULL) {
		return;
	}
	free(element);
}

SP_ELEMENT_MSG spListElementSetIndex(SPListElement element, int index) {
	if (element == NULL || index <0) {
		return SP_ELEMENT_INVALID_ARGUMENT;
	}
	element->index = index;
	return SP_ELEMENT_SUCCESS;
}

int spListElementGetIndex(SPListElement element) {
	if(element == NULL){
		return -1;
	}
	return element->index;
}

SP_ELEMENT_MSG spListElementSetValue(SPListElement element, double newValue) {
	if (element == NULL || newValue < 0) {
		return SP_ELEMENT_INVALID_ARGUMENT;
	}
	element->value = newValue;
	return SP_ELEMENT_SUCCESS;
}

double spListElementGetValue(SPListElement element) {
	if(element == NULL){
		return -1.0;
	}
	return element->value;
}

int spListElementCompare(SPListElement e1, SPListElement e2){
	assert(e1!=NULL && e2!=NULL);
	if(e1->value == e2->value){
		return e1->index - e2->index;
	}else if(e1->value > e2->value){
		return 1;
	}
	return -1;
}
