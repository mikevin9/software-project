
#ifndef KDTREE_H_
#define KDTREE_H_

#include"SPKDArray.h"
#include"SPConfig.h"
#include"SPBPriorityQueue.h"

typedef struct kd_tree* SPKDTreeNode;

/**
 * Builds a kd-tree from given set of points.
 *
 * @param arr - A KD-Array which is used to build the tree
 * @param config - The configuration object
 * @param prevSplitDim - Used in the recursions of the building process to decide
 * 		on which dimension to split the tree. at first call will be -1.
 *
 * @ return - a pointer to the root of the tree.
 */
SPKDTreeNode KDTreeInit(SPKDArray arr, SPConfig config, int prevSplitDim);

/**
 * Searches the tree for N closest points to the query point.
 * the closest points are stored in a passed priority queue in which N is defined.
 *
 * @param node - The tree in question
 * @param queue - The priority queue that the closest points will be stored in
 * @param queryPoint - The query point
 *
 */
void KDTreeSearch(SPKDTreeNode node,SPBPQueue queue, SPPoint queryPoint);

/**
 * Free all memory allocation associated with the hole tree.
 * if pointer is NULL nothing happens.
 *
 * @param node - The tree in question
 *
 */
void KDTreeDestroy(SPKDTreeNode node);

#endif /* KDTREE_H_ */
