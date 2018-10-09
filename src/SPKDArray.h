
#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

#include"SPPoint.h"


typedef struct kd_array* SPKDArray;

/**
 * Builds an array from a copy of the given set of points.
 *
 * @param arr - the set of points used to build the array
 * @param size - the number of points in arr
 *
 * @return - a pointer to the new array.
 */
SPKDArray KDArrayInit(SPPoint* arr, int size);

/**
 * Splits the Array into two Arrays by a given dimension.
 *
 * @param kdArr - the array in question
 * @param coor - the dimension by which the split will be performed
 * @param medianVal - will store the median value of all the points in the splitting dimension
 *
 * @return - return an array of 2 KD-Arrays split by the given dimension
 */
SPKDArray* KDArraySplit(SPKDArray kdArr, int coor, double* medianVal);

/**
 * Free all memory allocation associated with an array.
 * if pointer is NULL nothing happens.
 *
 * @param kdArr - The array in question
 */
void KDArrayDestroy(SPKDArray kdArr);

/**
 * Returns the dimension of the points in the array.
 *
 * @param kdArr - The array in question
 *
 * @return - An int representing the arrays dimension.
 */
int KDArrayGetDim(SPKDArray kdArr);

/**
 * Returns the size of the array, i.e. how many points are in it.
 *
 * @param kdArr - The array in question
 *
 * @return - An int representing the arrays size.
 */
int KDArrayGetSize(SPKDArray kdArr);

/**
 * Returns all of the points of the array.
 *
 * @param kdArr - The array in question
 *
 * @return - A list of all the points of the array.
 */
SPPoint* KDArrayGetPoints(SPKDArray kdArr);

/**
 * Returns the table of the sorted points by each dimension.
 *
 * @param kdArr - The array in question
 *
 * @return - the table of the sorted points by each dimension.
 */
int** KDArrayGetDimSortedPoints(SPKDArray kdArr);

/**
 * Returns a specific point from the array.
 *
 * @param kdArr - The array in question
 * @param index - The index of the point to be returned
 *
 * @return - a point.
 */
SPPoint KDArrayGetSpecificPoint(SPKDArray kdArr, int index);

#endif /* SPKDARRAY_H_ */
