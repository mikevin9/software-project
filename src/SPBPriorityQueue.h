#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_

#include <stdbool.h>
#include "SPList.h"
#include "SPListElement.h"

/**
 * SP Bounded Priority Queue summary
 *
 * Implements a bounded priority queue container type.
 * The queue contains a SPList which elements are of type SPListElement, please refer
 * to SPList.h and SPListElement.h for usage.
 * The list can be of a pre-given maximum size.
 *
 * The following functions are available:
 *
 *   spBPQueueCreate               	- Creates a new empty queue of a specific maximum size
 *   spBPQueueCopy                 	- Copies an existing queue
 *   spBPQueueDestroy              	- Deletes an existing queue and frees all resources
 *   spBPQueueClear		      	   	- Clears all the data from the queue
 *   spBPQueueSize                	- Returns the current size of a given queue
 *   spBPQueueMAxSize				- Returns the maximum size of a given queue
 *   spBPQueueEnqueue 				- Inserts a NEW COPY element to the queue
 *   spBPQueueDequeue 				- Removes the element with the lowest value from the queue
 *   spBPQueuePeek 					- Returns a NEW COPY of the element with the lowest value
 *   spBPQueuePeekLast 				- Returns a NEW COPY of the element with the highest value
 *   spBPQueueMinValue 				- Returns the minimum value in the queue
 *   spBPQueueMaxValue 				- Returns the maximum value in the queue
 *   spBPQueueIsEmpty				- Returns true if the queue is full
 *   spBPQueueIsFull				- Returns true if the queue is full
 *
 */

/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t* SPBPQueue;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates a new Queue with a given maximum capacity.
 *
 * This function creates a new empty queue with a given maximum capacity.
 * @param maxSize - The maxSize of the queue (maxSize >= 0)
 * @return
 * 	NULL If memory allocations failed.
 * 	A new Queue with the corresponding maxSize in case of success.
 */
SPBPQueue spBPQueueCreate(int maxSize);

/**
 * Creates a copy of target queue.
 *
 * The new copy will contain all the elements from the source queue in the same
 * order. The internal iterator for both the new copy and the target list will not be
 * defined afterwards.
 *
 * @param source - The target queue to copy
 * @return
 * NULL if a NULL was sent or a memory allocation failed.
 * A Queue containing the same elements with same order as list otherwise.
 */
SPBPQueue spBPQueueCopy(SPBPQueue source);

/**
 * Deallocates a given queue.
 *
 * Clears all elements by using the stored free function.
 *
 * @param source - Target queue to be deallocated. If queue is NULL nothing will happen.
 */
void spBPQueueDestroy(SPBPQueue source);

/**
 * Removes all elements from target queue.
 *
 * The elements are deallocated using the stored freeing function
 *
 * @param source - Target queue to remove all element from.
 * If queue is NULL nothing will happen.
 */
void spBPQueueClear(SPBPQueue source);

/**
 * Returns the number of elements in a queue.
 *
 * @param source - The target queue which size is requested.
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise the number of elements in the queue.
 */
int spBPQueueSize(SPBPQueue source);

/**
 * Returns the maximum capacity of the queue.
 *
 * @param source - The target queue which size is requested.
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise the maximum capacity of the queue.
 */
int spBPQueueGetMaxSize(SPBPQueue source);

/**
 * Inserts a NEW COPY element to the queue.
 *
 * @param source - The queue for which to add an element
 * @param element - The element to insert. A copy of the element will be inserted
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT - if a NULL was sent as queue or element
 * SP_BPQUEUE_FULL - if the queue is full and (the elements value is larger then the
 * largest value in the queue or elements value is equal to the largest value in the queue
 * but it's index is bigger)
 * SP_BPQUEUE_SUCCESS - the element has been inserted successfully
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/**
 * Removes the element with the lowest value from the queue.
 *
 * @param source - The target queue we want to peek
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT - if a NULL was sent as queue
 * SP_BPQUEUE_EMPTY - if the queue is empty
 * SP_BPQUEUE_SUCCESS - the element has been removed successfully
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);

/**
 * Returns a NEW COPY of the element with the lowest value
 *
 * @param source - The target queue we want to peek
 * @return
 * NULL if a NULL was sent or a memory allocation failed.
 * A NEW COPY of the element with the lowest value.
 */
SPListElement spBPQueuePeek(SPBPQueue source);

/**
 * Returns a NEW COPY of the element with the highest value
 *
 * @param source - The target queue we want to peek
 * @return
 * NULL if a NULL was sent or a memory allocation failed.
 * A NEW COPY of the element with the highest value.
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);

/**
 * Returns the minimum value in the queue.
 *
 * @param source - The target queue in question.
 * @return
 * -1.0 in case source==NULL
 * otherwise, the minimum value in the queue.
 */
double spBPQueueMinValue(SPBPQueue source);

/**
 * Returns the maximum value in the queue.
 *
 * @param source - The target queue in question.
 * @return
 * -1.0 in case source==NULL
 * otherwise, the maximum value in the queue.
 */
double spBPQueueMaxValue(SPBPQueue source);

/**
 * Returns true if the queue is empty.
 *
 * @param source - The target queue in question.
 * @assert source != NULL
 * @return
 * true if the queue is empty. false otherwise.
 */
bool spBPQueueIsEmpty(SPBPQueue source);

/**
 * Returns true if the queue is full.
 *
 * @param source - The target queue in question.
 * @assert source != NULL
 * @return
 * true if the queue is full. false otherwise.
 */
bool spBPQueueIsFull(SPBPQueue source);

/**
 * Returns a copy of the SPList of the queue.
 *
 * @param source - The target queue in question.
 * @assert source != NULL
 * @return - a copy of the SPList of the queue.
 */
SPList spBPQueueGetQueue(SPBPQueue source);

/**
 * Same as spBPQueueEnqueue just that the queue will be a maximum queue.
 *
 * @param source - The queue for which to add an element
 * @param element - The element to insert. A copy of the element will be inserted
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT - if a NULL was sent as queue or element
 * SP_BPQUEUE_FULL - if the queue is full and (the elements value is larger then the
 * largest value in the queue or elements value is equal to the largest value in the queue
 * but it's index is bigger)
 * SP_BPQUEUE_SUCCESS - the element has been inserted successfully
 */
SP_BPQUEUE_MSG spBPQueueEnqueueToMaximumQueue(SPBPQueue source, SPListElement element);


#endif
