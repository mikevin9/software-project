#include<stdio.h>
#include<stdlib.h>
#include<assert.h>

#include"SPKDTree.h"
#include"SPKDArray.h"
#include"SPConfig.h"
#include"SPBPriorityQueue.h"
#include"SPListElement.h"
#include"SPPoint.h"

struct kd_tree{
	int splitDim; // The splitting dimension
	double medianVal; // The median value of the splitting dimension
	SPKDTreeNode left; // Pointer to the left subtree
	SPKDTreeNode right; // Pointer to the right subtree
	SPPoint point; // Pointer to a point (only if the current node is a leaf) otherwise this field value is NULL
};


/**
 * An aux func for initTree
 *
 * @param arr - The source array
 * @return
 * the dimension for which maxVal(P[dim])-minVal(P[dim]) is the maximal
 */
int findMaxSpread(SPKDArray arr);


SPKDTreeNode KDTreeInit(SPKDArray arr, SPConfig config, int prevSplitDim){
	SPKDArray* temp = NULL;
	SPKDTreeNode newNode = NULL;
	int method;
	int currSplitDim;
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
	newNode = (SPKDTreeNode)malloc(sizeof(*newNode));
	if(newNode == NULL){
		return NULL;
	}
	if(KDArrayGetSize(arr) == 1){
		newNode->splitDim = -1;
		newNode->medianVal = -1;
		newNode->left = NULL;
		newNode->right = NULL;
		newNode->point = spPointCopy(*KDArrayGetPoints(arr));
		KDArrayDestroy(arr);
		return newNode;
	}
	else{
		method = spGetSplitMethode(config, &msg);
		if(method == MAX_SPREAD){
			currSplitDim = findMaxSpread(arr);
		}
		else if(method == RANDOM){
			currSplitDim = (rand() % (KDArrayGetDim(arr)-1));
		}
		else{ // method == INCREMENTAL
			currSplitDim = (prevSplitDim+1)%(KDArrayGetDim(arr));
		}
		newNode->splitDim = currSplitDim;
		temp = KDArraySplit(arr, currSplitDim, &(newNode->medianVal));
		newNode->left = KDTreeInit(temp[0], config, currSplitDim);
		newNode->right = KDTreeInit(temp[1], config, currSplitDim);
		newNode->point = NULL;
		free(temp);
		KDArrayDestroy(arr);
		return newNode;
	}
}

void KDTreeSearch(SPKDTreeNode node,SPBPQueue queue, SPPoint queryPoint){
	if(node == NULL){
		return;
	}
	if(node->splitDim == -1){
		if(!spBPQueueIsFull(queue) || spPointL2SquaredDistance(node->point, queryPoint) <= spBPQueueMaxValue(queue)){
			SPListElement element = spListElementCreate(spPointGetIndex(node->point),
				spPointL2SquaredDistance(node->point, queryPoint));
			spBPQueueEnqueue(queue, element); // enqueuing process make a copy of element
			spListElementDestroy(element);
		}
		return;
	}
	int left = 0;
	if((spPointGetAxisCoor(queryPoint, node->splitDim) <= node->medianVal)){
		KDTreeSearch(node->left, queue, queryPoint);
		left = 1;
	}
	else{
		KDTreeSearch(node->right, queue, queryPoint);
	}
	double prox = (node->medianVal - spPointGetAxisCoor(queryPoint, node->splitDim));
	prox = prox*prox;
	if(!spBPQueueIsFull(queue) || prox < spBPQueueMaxValue(queue)){
		if(left==1){
			KDTreeSearch(node->right, queue, queryPoint);
		}
		else{
			KDTreeSearch(node->left, queue, queryPoint);
		}
	}
}

void KDTreeDestroy(SPKDTreeNode node){
	if(node == NULL){
		return;
	}
	KDTreeDestroy(node->left);
	KDTreeDestroy(node->right);
	if(node->point != NULL){
		spPointDestroy(node->point);
	}
	free(node);
	return;
}

int findMaxSpread(SPKDArray arr){
	SPPoint min, max;
	int** sortedArr = KDArrayGetDimSortedPoints(arr);
	int maxDim = -1, size;
	double maxVal = 0, temp;
	size = KDArrayGetSize(arr)-1;

	for(int i=0;i<KDArrayGetDim(arr);i++){
		max = KDArrayGetSpecificPoint(arr, sortedArr[i][size]);
		min = KDArrayGetSpecificPoint(arr, sortedArr[i][0]);
		temp = spPointGetAxisCoor(max,i) - spPointGetAxisCoor(min,i);
		if(maxVal<temp){
			maxVal = temp;
			maxDim = i;
		}
	}
	return maxDim;
}
