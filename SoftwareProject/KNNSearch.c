#include "KNNSearch.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define BPQ_ERROR "Could not enqueue element to BPQ"

bool isLeaf(KDTreeNode kdTreeNode){
	if(kdTreeNode==NULL){//LOGGER
		return NULL;
	}
	if(kdTreeNode->data == NULL){
		return false;
	}
	return true;
}

void kNearestNeighbors(KDTreeNode curr , SPBPQueue bpq, SPPoint point){
	SPListElement element = NULL;
	double distance =0.0;
	bool isLeftChosen;
	SP_BPQUEUE_MSG bpqMsg;

	if(curr == NULL){
		return;
	}
	/* Add the current point to the BPQ. Note that this is a no-op if the
	 * point is not as good as the points we've seen so far.*/
	if (isLeaf(curr)){
		distance =sqrt(spPointL2SquaredDistance(curr->data,point));
		element = spListElementCreate(spPointGetIndex(curr->data), distance);
		bpqMsg	= spBPQueueEnqueue(bpq, element);
		if (bpqMsg == SP_BPQUEUE_INVALID_ARGUMENT || bpqMsg == SP_BPQUEUE_OUT_OF_MEMORY) {
			spLoggerPrintError(BPQ_ERROR, __FILE__, __func__, __LINE__);
			//TODO verify if we when enqueue fails we should "termintate" - indicate
			//in main that the search failed. extren searchSucess?
		}
		return;
	}

	/* Recursively search the half of the tree that contains the test point. */
	if(spPointGetAxisCoor(point,curr->dim) <= curr->val){
		isLeftChosen = true;
		kNearestNeighbors(curr->left , bpq, point);
	}
	else{
		isLeftChosen = false;
		kNearestNeighbors(curr->right , bpq, point);
	}
	/* If the candidate hypersphere crosses this splitting plane, look on the
	 * other side of the plane by examining the other subtree*/

	if((!spBPQueueIsFull(bpq))|| abs(curr->val-spPointGetAxisCoor(point,curr->dim)) < spBPQueueMaxValue(bpq)){

		//if the if returns true -then recursively search the other subtree on the next axis

		if(isLeftChosen){
			if (curr->right != NULL){
				kNearestNeighbors(curr->right ,bpq , point);
			}
		}
		else{
			if (curr->left != NULL){
				kNearestNeighbors(curr->left , bpq, point);
			}
		}
	}
}
