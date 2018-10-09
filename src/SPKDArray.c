#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<math.h>

#include"SPKDArray.h"
#include"SPPoint.h"

struct kd_array{
	int dim;
	int size;
	SPPoint* points;
	int** dimSortedPoints;
};


/**
 * Compare function
 *
 * @param a - First object to be compared
 * @param b - Second object to be compared
 * @return - 0 if a=b; 1 if a>b; -1 if a<b
 */
int compare(const void* a, const void* b);

SPKDArray KDArrayInit(SPPoint* arr, int size){
	SPKDArray newArray = NULL;
	double** temp = NULL;
	assert(size > 0 && arr != NULL);
	newArray = (SPKDArray)malloc(sizeof(*newArray));
	if(newArray == NULL){
		return NULL;
	}
	newArray->size = size;
	newArray->dim = spPointGetDimension(*arr);
	newArray->points = (SPPoint*)malloc(sizeof(SPPoint)*size);
	if(newArray->points == NULL){
		free(newArray);
		return NULL;
	}
	for(int i=0; i<size; i++){
		*(newArray->points+i) = spPointCopy(*(arr+i));
	}
	newArray->dimSortedPoints = (int**)malloc(sizeof(int*)*(newArray->dim));
	if(newArray->dimSortedPoints == NULL){
		for(int i = size-1; i>-1; i--){
			spPointDestroy(*(newArray->points+i));
		}
		free(newArray->points);
		free(newArray);
		return NULL;
	}
	temp = (double**)malloc(sizeof(double*)*size);
	for(int i= 0; i<newArray->dim; i++){
		for(int j= 0; j < size; j++){
			temp[j] = (double*)malloc(sizeof(double)*2);
			temp[j][0] = j;
			temp[j][1] = spPointGetAxisCoor(*(arr+j), i);
		}
		qsort(temp, size, sizeof(int*), compare);
		*(newArray->dimSortedPoints+i) = (int*)malloc(sizeof(int)*size);
		for(int j= 0; j < size; j++){
			*(*(newArray->dimSortedPoints+i)+j) = temp[j][0];
			free(*(temp+j));
		}
	}
	free(temp);
	return newArray;
}

SPKDArray* KDArraySplit(SPKDArray kdArr, int coor, double* medianVal){
	SPKDArray* newArrays = NULL;
	SPKDArray leftArray = NULL, rightArray = NULL;
	int *myHash = NULL;
	int leftSize, rightSize, leftCount = 0, rightCount = 0, temp;
	assert(coor >= 0 && kdArr != NULL);

	// setting up the left kdArray:
	leftSize = ceil((double)(kdArr->size)/2);
	leftArray = (SPKDArray)malloc(sizeof(*leftArray));
	leftArray->size = leftSize;
	leftArray->dim = kdArr->dim;
	leftArray->points = (SPPoint*)malloc(sizeof(SPPoint)*leftSize);
	leftArray->dimSortedPoints = (int**)malloc(sizeof(int*)*(kdArr->dim));
	if(leftArray == NULL || leftArray->points == NULL || leftArray->dimSortedPoints == NULL){
		return NULL;
	}

	// setting up the right kdArray:
	rightSize = (kdArr->size) - leftSize;
	rightArray = (SPKDArray)malloc(sizeof(*rightArray));
	rightArray->size = rightSize;
	rightArray->dim = kdArr->dim;
	rightArray->points = (SPPoint*)malloc(sizeof(SPPoint)*rightSize);
	rightArray->dimSortedPoints = (int**)malloc(sizeof(int*)*(kdArr->dim));
	if(rightArray == NULL || rightArray->points == NULL || rightArray->dimSortedPoints == NULL){
		return NULL;
	}

	// finding the median value
	int medianIndex = kdArr->dimSortedPoints[coor][leftSize-1];
	*medianVal = spPointGetAxisCoor(*(kdArr->points+medianIndex), coor);

	// setting up my "Hash" table
	myHash = (int*)calloc(kdArr->size, sizeof(int));
	for(int i= 0; i<kdArr->size; i++){
		if(i<leftSize){
			*(myHash+(kdArr->dimSortedPoints[coor][i])) = 1;
		}
		else{
			*(myHash+(kdArr->dimSortedPoints[coor][i])) = -1;
		}
	}
	for(int i= 0; i<kdArr->size; i++){
		if(*(myHash+i)>0){
			*(myHash+i) = leftCount+1;
			leftCount++;
		}
		else{
			*(myHash+i) = rightCount-1;
			rightCount--;
		}
	}

	// setting up the SPPoints of the sub-trees
	leftCount = 0;
	rightCount = 0;
	for(int i= 0; i<kdArr->size; i++){
		if(*(myHash+i) > 0){ // = point belongs to left sub-Array
			*(leftArray->points+leftCount) = spPointCopy(*(kdArr->points+i));
			leftCount++;
		}
		else{ // = point belongs to right sub-Array
			*(rightArray->points+rightCount) = spPointCopy(*(kdArr->points+i));
			rightCount++;
		}
	}

	// now sorting...
	for(int i= 0; i<kdArr->dim; i++){
		*(leftArray->dimSortedPoints+i) = (int*)malloc(sizeof(int)*leftSize);
		leftCount = 0;
		*(rightArray->dimSortedPoints+i) = (int*)malloc(sizeof(int)*rightSize);
		rightCount = 0;
		if((leftArray->dimSortedPoints+i)==NULL || (rightArray->dimSortedPoints+i)==NULL){
			return NULL;
		}
		for(int j= 0; j<kdArr->size; j++){
			temp = kdArr->dimSortedPoints[i][j];
			if(myHash[temp] > 0){ // = belongs to left sub-Array
				leftArray->dimSortedPoints[i][leftCount] = (myHash[temp])-1;
				leftCount++;
			}
			else{ // = point belongs to right sub-Array
				rightArray->dimSortedPoints[i][rightCount] = abs(myHash[temp])-1;
				rightCount++;
			}
		}
	}
	free(myHash);
	newArrays = (SPKDArray*)malloc(sizeof(SPKDArray)*2);
	newArrays[0] = leftArray;
	newArrays[1] = rightArray;
	return newArrays;
}

int KDArrayGetDim(SPKDArray kdArr){
	return kdArr->dim;
}

int KDArrayGetSize(SPKDArray kdArr){
	return kdArr->size;
}

SPPoint* KDArrayGetPoints(SPKDArray kdArr){
	return kdArr->points;
}

int** KDArrayGetDimSortedPoints(SPKDArray kdArr){
	return kdArr->dimSortedPoints;
}

void KDArrayDestroy(SPKDArray kdArr){
	for(int i= 0; i < kdArr->size; i++){
		spPointDestroy(*(kdArr->points+i));
	}
	free(kdArr->points);
	for(int i= 0; i < kdArr->dim; i++){
		free(*(kdArr->dimSortedPoints+i));
	}
	free(kdArr->dimSortedPoints);
	free(kdArr);
}

SPPoint KDArrayGetSpecificPoint(SPKDArray kdArr, int index){
	return *(kdArr->points+index);
}

int compare(const void* a, const void* b){
	double** j = (double**)a;
	double** k = (double**)b;

	double A = j[0][1];
	double B = k[0][1];

     if(A == B){return 0;}
     else if(A < B){return -1;}
     else{return 1;}
}
