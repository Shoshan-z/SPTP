#include "SPList.h"
#include "SPListElement.h"
#include "SPPoint.h"
#include "SPBPriorityQueue.h"
#include <stdlib.h>
#include <assert.h>

struct sp_bp_queue_t{
	int capacity;
	SPList queue;
};

SPBPQueue spBPQueueCreate(int maxSize){

	if (maxSize <1){
		return NULL;
	}

	SPBPQueue newQ = (SPBPQueue) malloc(sizeof(struct sp_bp_queue_t));
	if (newQ == NULL) {
		return NULL;
	}
	newQ->capacity = maxSize;
	newQ->queue = spListCreate();
	if (newQ->queue == NULL) {
		return NULL;
	}
	return newQ;
}
SPBPQueue spBPQueueCopy(SPBPQueue source){
	if (source == NULL){
		return NULL;
	}
	SPBPQueue copyQ = (SPBPQueue) malloc(sizeof( struct sp_bp_queue_t));
	if (copyQ == NULL){
		return NULL;
	}
	copyQ -> capacity = source -> capacity;
	copyQ -> queue = spListCopy(source-> queue);
	if (copyQ -> queue == NULL){
		return NULL;
	}
	return copyQ;
}

void  spBPQueueDestroy(SPBPQueue source){
	if (source == NULL) {
		return;
	}
	spListDestroy(source -> queue);
	free(source);
}
void spBPQueueClear(SPBPQueue source){
	spListClear(source -> queue);
}

int spBPQueueSize(SPBPQueue source){
	if (source == NULL) {
		return -1;
	}
	return spListGetSize(source -> queue);
}

int spBPQueueGetMaxSize(SPBPQueue source) {
	if (source == NULL) {
		return -1;
	}
	return source -> capacity;
}


SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element){
	int counter =0;
	int size = spBPQueueSize(source);
	SPListElement curr =NULL;
	SPListElement next = NULL;
	SP_LIST_MSG msg;

	if (element == NULL || source == NULL) {
				return SP_BPQUEUE_INVALID_ARGUMENT;
			}

	if (size != 0){
	curr = spListGetFirst(source ->queue);

	}

	else { //the queue is empty - insert the first element
		spListInsertFirst(source->queue, element);//copyE);
		return SP_BPQUEUE_SUCCESS;
	}

	if (size < (source -> capacity)) { //case1: the queue is not full
		while (spListElementCompare(element,curr)>0 && counter< size-1){//elem > curr // ??? check size or size -1
			next = spListGetNext(source ->queue);//if it's the end of the queue next will be NULL
			curr = next;
			counter ++;
		}
		if (counter < size && spListElementCompare( element,curr)<0 ){ //there are element/s larger than this elem (we can insert before)
		 msg = spListInsertBeforeCurrent(source -> queue,  element);
		 if (msg == SP_LIST_OUT_OF_MEMORY){
			 return SP_BPQUEUE_OUT_OF_MEMORY;
		 }

		}
		else{// if elem to insert  is bigger than all elems in the queue
		msg = spListInsertAfterCurrent(source -> queue, element);
		if (msg == SP_LIST_OUT_OF_MEMORY){
					 return SP_BPQUEUE_OUT_OF_MEMORY;
				 }
		}
	}
	else{ //case2: queue is full
		if(spListElementGetValue(element) >= spBPQueueMaxValue(source)){ // value is bigger than the max val in queue
			return SP_BPQUEUE_FULL; // elem wasn't inserted
		}
		curr= spListGetFirst(source -> queue);
		while (spListElementCompare(element,curr)>0 && counter< size-1){//elem > curr // ??? check size or size -1
			next = spListGetNext(source -> queue);
			curr = next;
			counter ++;
		}
		msg = spListInsertBeforeCurrent(source -> queue, element);
		if (msg == SP_LIST_OUT_OF_MEMORY){
			return SP_BPQUEUE_OUT_OF_MEMORY;
		}
		while (counter < size-1){// check sizes
			next = spListGetNext(source -> queue);
			curr = next;
			counter ++;

		}

		spListRemoveCurrent(source-> queue);
	}

	//spListElementDestroy(copyE);
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source){
	int size = spBPQueueSize(source);
	if ( source == NULL) {
				return SP_BPQUEUE_INVALID_ARGUMENT;
			}

	if (size==0) { //the queue is empty
			 return SP_BPQUEUE_EMPTY;
		}
	else{
		spListGetFirst(source ->queue);// set the queue's iterator to the first element
		spListRemoveCurrent(source->queue);//remove the element with the lowest value;

	}
	return SP_BPQUEUE_SUCCESS;
}

SPListElement spBPQueuePeek(SPBPQueue source){
	int size = spBPQueueSize(source);
	//SPListElement copy = NULL;
	SPListElement curr = NULL;
	if (source == NULL) {
				return NULL;
			}
	curr = spListGetFirst(source ->queue);// set the queue's iterator to the first element
	if (size==0) { //the queue is empty
				 return NULL;
			}
	return spListElementCopy(curr); //create a copy of the first element
	//return copy;
}


SPListElement spBPQueuePeekLast(SPBPQueue source){
	int size = spBPQueueSize(source);
	SPListElement curr = NULL;
	SPListElement next = NULL;
	//SPListElement copy = NULL;
	int counter = 0;
	//SPListElement first = NULL;

	if (source == NULL) {
				return NULL;
			}

	curr =  spListGetFirst(source ->queue);// set the queue's iterator to the first element
	next = spListGetNext(source ->queue);

	if (size==0) { //the queue is empty
				 return NULL;
			}

	while (counter<size-1) { //forward current to the last element of the queue
		curr = next;
		next = spListGetNext(source -> queue);
		counter++;
	}
	//create a copy of the last element
	return spListElementCopy(curr);
	//spListElementDestroy(next);
	//return copy;
}

double spBPQueueMinValue(SPBPQueue source){
	int size = spBPQueueSize(source);
	double ans = 0;
	SPListElement first = spBPQueuePeek(source); //get the first element
	if (source == NULL) {
		return -1;
			}
	if (first == NULL) {
			return -1;
				}
	if (size==0) { //the queue is empty
		return -1;
			}
	 ans = spListElementGetValue(first);
	 spListElementDestroy(first);
	 return ans;
}

double spBPQueueMaxValue(SPBPQueue source){
	int size = spBPQueueSize(source);
	double ans = 0;
	SPListElement last = spBPQueuePeekLast(source ); //get the last element
	if (source == NULL) {
		return -1;
			}
	if (last == NULL) {
			return -1;
				}
	if (size==0) { //the queue is empty
		return -1;
			}

	 ans = spListElementGetValue(last);
	 spListElementDestroy(last);
	 return ans;
}


bool spBPQueueIsEmpty(SPBPQueue source){
	if (source == NULL) {
		return false;
	}
	return (spBPQueueSize(source)== 0);
}

bool spBPQueueIsFull(SPBPQueue source){

	if (source == NULL) {
			return false;
		}
		return (spBPQueueSize(source)== source -> capacity);
	}




