#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include "SPListElement.h"
#include <stdbool.h>
/**
 * SP Bounded Priority Queue summary
 *
 * TODO Complete documentation
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
 * This function creates an empty queue with a given maximum capacity.
 * @return
 * 		NULL if allocation failed
 * 		A new SPBQueue in case of success
 */
SPBPQueue spBPQueueCreate(int maxSize);

/**
 * This function creates a copy of a given queue
 *
 * The new copy's queue will contain all the elements from the source SPBQueue's queue in the same
 * order. The new copy's max will be the same as the source's max.
 *
 * @param source is The source SPBQueue to copy
 * @return
 * NULL if a NULL was sent or a memory allocation failed.
 * A new SPBQuere containing the same queue and the same max.
 */
SPBPQueue spBPQueueCopy(SPBPQueue source);
/**
 * This function frees all memory allocation associated with the queue.
 * spBPQueueDestroy: Deallocates an existing SPBPQueue. Clears all queue's elements by using the
 * stored free function.
 *
 * @param source Target SPBPQueue to be deallocated. If NULL nothing will be
 * done
 */
void spBPQueueDestroy(SPBPQueue source);

/**
 * This function removes all elements from target source's queue.
 *
 * The elements are deallocated using the spListClear function.
 * @param source Target sources to remove all element from it's queue.
 *
 */
void spBPQueueClear(SPBPQueue source);

/**
 * This function returns the number of elements in the source SPBPQueue.
 *
 * @param source
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise the number of elements in the queue.
 */
int spBPQueueSize(SPBPQueue source);

/**
 * This function returns the maximum capacity of the queue
 * @param source The target source which it's capacity is requested.
 * @return
 * -1 if source == NULL
 * Otherwise the capacity of the queue.
 */
int spBPQueueGetMaxSize(SPBPQueue source);

/**
 * Adds a new element to the queue, the new element will be inserted in a place according to it's value.
 * if  the queue is full: if elemnt's value is larger than all elems in queue, it will not be inserted.
 * else- the element will be inserted according to it's value, and the last element in the queue will be removed.
 * @param source The SPBPqueue for which to add an element
 * @param element The element to insert. A copy of the element will be
 * inserted
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if a NULL was sent as queue or element
 * SP_BPQUEUE_FULL if queue is full and the element's value is larger than the elems in the queue. it will not be inserted.
 * SP_BPQUEUE_OUT_OF_MEMORY if an allocation failed
 * SP_BPQUEUE_SUCCESS the element has been inserted successfully
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/**
 * Removes the  element  with the lowest value of the queue using SPList.spListRemoveCurrent
 * function.
 * @param source The SPBPQueue from which the element will be removed
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if source is NULL
 * SP_BPQUEUE_EMPTY if the queue is empty
 * SP_BPQUEUE_SUCCESS the first element was removed successfully
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);

/**
 * Returns a new copy of the element with the lowest value
 *
 * @param source The SPBPQueue to return the element with lowest value from
 * @return
 * NULL if source is NULL or the queue is empty.
 * The element with the lowest value in the queue (first) otherwise
 */
SPListElement spBPQueuePeek(SPBPQueue source);

/**
 * Returns a new copy of the element with the highest value
 *
 * @param source The SPBPQueue to return the element with highest value from
 * @return
 * NULL if source is NULL or the queue is empty.
 * The element with the highest value in the queue (last) otherwise
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);

/**
 * returns the minimum value in the queue
 *
 * @param source the SPBPQueue from which the min value is taken from
 * @return
 * -1 if an null pointer was sent or if the queue is empty
 * The minimum value of the queue otherwise
 */
double spBPQueueMinValue(SPBPQueue source);

/**
 * returns the maximum value in the queue
 *
 * @param source the SPBPQueue from which the max value is taken from
 * @return
 * -1 if a null pointer was sent or if the queue is empty
 * The maximum value of the queue otherwise
 */
double spBPQueueMaxValue(SPBPQueue source);

/**
 *  returns true if the queue is empty
 *
 *  @param source the SPBPQueue  checked
 *  @return
 *  true if the queue is empty
 *  false otherwise or if a null pointer was sent
 */
bool spBPQueueIsEmpty(SPBPQueue source);

/**
 *  returns true if the queue is full
 *
 *  @param source the SPBPQueue checked
 *  @return
 *  true if the queue is full
 *  false otherwise or if a null pointer was sent
 */
bool spBPQueueIsFull(SPBPQueue source);

#endif
