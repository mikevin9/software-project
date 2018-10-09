#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include"SPPoint.h"


struct sp_point_t{
	double* data;
	int dim;
	int index;
};

SPPoint spPointCreate(double* data, int dim, int index){
	SPPoint newPoint;
	double* copyData;
	assert(dim > 0 && index >= 0 && data != NULL);
	newPoint = (SPPoint)malloc(sizeof(*newPoint));
	if(newPoint == NULL){
		free(newPoint);
		return NULL;
	}
	copyData = (double*)malloc(dim * sizeof(double));
	if(copyData == NULL){
			free(newPoint);
			free(copyData);
			return NULL;
	}
	for(int i=0; i<dim; i++){
		copyData[i]=data[i];
	}
	newPoint->data = copyData;
	newPoint->dim = dim;
	newPoint->index = index;
	return newPoint;
}

SPPoint spPointCopy(SPPoint source){
	SPPoint copyPoint = NULL;
	double *copyData;
	assert(source!=NULL);
	copyPoint = (SPPoint)malloc(sizeof(*copyPoint));
	copyData = (double*)malloc((source->dim) * sizeof(double));
	if(copyPoint == NULL || copyData == NULL){
		free(copyPoint);
		free(copyData);
		return NULL;
	}
	for(int i=0; i<(source->dim); i++){
		copyData[i]=(source->data)[i];
	}
	copyPoint->data = copyData;
	copyPoint->dim = source->dim;
	copyPoint->index = source->index;
	return copyPoint;
}

void spPointDestroy(SPPoint point){
	if(point!=NULL){
		free(point->data);
		free(point);
	}
}

int spPointGetDimension(SPPoint point){
	assert(point!=NULL);
	return point->dim;
}

int spPointGetIndex(SPPoint point){
	assert(point!=NULL);
	return point->index;
}

double spPointGetAxisCoor(SPPoint point, int axis){
	assert((point!=NULL) && (axis >= 0) && (axis < (point->dim)));
	return point->data[axis];
}

double spPointL2SquaredDistance(SPPoint p, SPPoint q){
	double sum = 0;
	assert(p!=NULL && q!=NULL && (p->dim)==(q->dim));
	for(int i=0; i<(p->dim); i++){
		sum = sum + (((p->data)[i]-(q->data)[i])*((p->data)[i]-(q->data)[i])) ;
	}
	return sum;
}
