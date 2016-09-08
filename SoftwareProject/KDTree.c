#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SPConfig.h"
#include <assert.h>
#include "SPPoint.h"
#include "KDTree.h"
#include "SPKDArray.h"
#include <math.h>
#include <time.h>

#define ALLOC_ERROR_MSG "Allocation error"
#define INVALID_ARG_ERROR "Invalid arguments"
#define MAX_SPREAD_ERROR "Could not find MAX_SPREAD"

bool treeSuccess = true;

KDTreeNode initTree(SPKDArray kdarr, SPLIT_METHOD spKDTreeSplitMethod, int incrementingDimension){//maybe instead of kdarr we need do send features/sppoints array and use init of kdarray
	KDTreeNode returnNode = NULL;
	int spread = 0, random = 0, splitDim = 0, middle = 0, size=0, coordinates =0;
	int inc = 0; // will hold the new incrementingDimension
	KDTreeNode node = NULL;
	SPKDArray leftKDArr = NULL;
	SPKDArray rightKDArr = NULL;

	if (kdarr == NULL || spKDTreeSplitMethod == NONE || incrementingDimension < 0){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		treeSuccess = false;
		return NULL;
	}

	size= kdarr->size;
	coordinates = kdarr->dim;// =num of rows = num of coordinates in each point
	middle = (int)(ceil((double) size/2));

	if (size==1){ // stop criteria
		returnNode= initLeaf(-1, INFINITY, NULL, NULL);
		if (returnNode == NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			treeSuccess = false;
			goto cleanup;
		}
		SPPoint p = spPointCopy((kdarr->points)[0]);
		if (p == NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			treeSuccess = false;
			goto cleanup;
		}

		returnNode->data= p;
		node = returnNode;
		goto cleanup;
	}

	leftKDArr = allocateKDArray(coordinates, middle);
	if (leftKDArr == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		treeSuccess = false;
		goto cleanup;
	}
	rightKDArr = allocateKDArray(coordinates, size-middle);
	if (rightKDArr == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		treeSuccess = false;
		goto cleanup;
	}

	if (spKDTreeSplitMethod == MAX_SPREAD){
		spread= findMaxSpread(kdarr);
		if (spread == -1) {
			spLoggerPrintError(MAX_SPREAD_ERROR, __FILE__, __func__, __LINE__);
			treeSuccess = false;
			goto cleanup;
		}
		splitDim= spread;
	}
	if (spKDTreeSplitMethod == RANDOM){
		srand(time(NULL));
		random = rand()%coordinates;// random int between 0 and last coordinates index
		splitDim = random;
	}
	if(spKDTreeSplitMethod == INCREMENTAL){
		inc= (incrementingDimension)%coordinates;
		splitDim=inc;
	}
	split(kdarr, splitDim, leftKDArr, rightKDArr);
	node = initEmptyNode();
	if (node == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		treeSuccess = false;
		goto cleanup;
	}

	node->dim = splitDim;
	node->val = spPointGetAxisCoor((kdarr->points)[(kdarr->matrix)[splitDim][middle-1]], splitDim);
	node->left = initTree(leftKDArr,spKDTreeSplitMethod, incrementingDimension+1);
	node->right = initTree(rightKDArr,spKDTreeSplitMethod, incrementingDimension+1);
	node-> data = NULL;

cleanup:
	SPKDArrayDestroy(leftKDArr);
	SPKDArrayDestroy(rightKDArr);

	return node;
}

void KDTreeDestroy(KDTreeNode kdTree) {
	if (kdTree == NULL) {
		return;
	}

	if (isLeaf(kdTree))
	{
		if (kdTree->data != NULL)
			spPointDestroy(kdTree->data);
	}

	KDTreeDestroy(kdTree->right);
	KDTreeDestroy(kdTree->left);

	free(kdTree);
}

KDTreeNode initLeaf(int dim, double val, KDTreeNode left,KDTreeNode right){// ~~SPKDTree
		KDTreeNode newNode=NULL;

		newNode= (struct kd_tree_node*) malloc(sizeof (struct kd_tree_node));
		if (newNode ==NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			return NULL;
		}
		newNode->dim = dim;
		newNode->val = val;
		newNode->left =left;
		newNode->right =right;
		newNode->data = NULL;

		return newNode;
}

KDTreeNode initEmptyNode(){// ~~ SPKDTree
	    KDTreeNode newNode=NULL;
		newNode= (struct kd_tree_node*) malloc(sizeof (struct kd_tree_node));
		if (newNode ==NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			return NULL;
		}
		return newNode;
}

int findMaxSpread(SPKDArray kdArr){
	double min = 0.0, max = 0.0;
	int i = 0;
	int size=0;
	double maxSpread = 0.0;
	int spreadDim = 0;// the dimension we'll spread according to (i.e x=0, y=1 etc)
	double* spreadByDim = NULL;//array of spread of each dim
	int coordinates = kdArr->dim;// =num of rows = num of coordinates in each point
	int cols;//=number of cols = number of different points

	size=kdArr->size;
	cols=size;
	spreadByDim = (double*)malloc(coordinates*sizeof(double));
	if (spreadByDim == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		return -1;
	}

	for(i=0; i<coordinates; i++){
		max = spPointGetAxisCoor((kdArr->points)[(kdArr->matrix)[i][cols-1]],i);
		min =spPointGetAxisCoor((kdArr->points)[(kdArr->matrix)[i][0]],i);
		spreadByDim[i] = max-min;
	}
	maxSpread = spreadByDim[0];
	for(i=1; i<coordinates; i++){// find the first coordinate with the max spread
		if(spreadByDim[i]> maxSpread){
			spreadDim  =  i;
		}
	}
	free(spreadByDim);

	return spreadDim;
}

bool isLeaf(KDTreeNode kdTreeNode){
	if(kdTreeNode==NULL){
		return NULL;
	}
	if(kdTreeNode->data == NULL){
		return false;
	}
	return true;
}
