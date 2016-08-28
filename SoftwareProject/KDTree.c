
/*
 * KDTree.c
 *
 *  Created on: Aug 22, 2016
 *      Author: Lilach
 */
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

typedef struct kd_tree_node* KDTreeNode;



KDTreeNode initTree(SPKDArray kdarr, SPLIT_METHOD spKDTreeSplitMethod, int incrementingDimension){//maybe instead of kdarr we need do send features/sppoints array and use init of kdarray
	//TODO we first need to build a kd-array with all features stored in it
	//and then we recursively apply the following steps:
//!!! the recursive call is already there after the split: in the creation of the node at the bottom- in it's left and right fields..
	KDTreeNode returnNode = NULL;
	int spread = 0;
	int random = 0;
	int splitDim = 0;
	int middle = 0;
	int size=0;
	int coordinates =0;
	int inc = 0; // will hold the new incrementingDimension
	KDTreeNode node = NULL;
	SPKDArray leftKDArr = NULL;
	SPKDArray rightKDArr = NULL;

	if (kdarr == NULL || spKDTreeSplitMethod == NONE || incrementingDimension < 0){
		return NULL;// TODO verify if split method = NONE should we return null or what?
	}

	size= kdarr->size;
	coordinates = kdarr->dim;// =num of rows = num of coordinates in each point
	middle = (int)(ceil((double) size/2));


	leftKDArr = allocateKDArray(coordinates, middle);
	rightKDArr = allocateKDArray(coordinates, size-middle);

	if (size==1){ // stop criteria
		returnNode= initLeaf(-1, INFINITY, NULL, NULL);// TODO verify what is invalid in val
		SPPoint p = spPointCopy((kdarr->points)[0]);
		returnNode->data= p;//TODO!!! maybe we can directly pass points for it has only one point in it
		return returnNode;
	}
	if (spKDTreeSplitMethod == MAX_SPREAD){
		spread= findMaxSpread(kdarr);
		splitDim= spread;
	}
	if (spKDTreeSplitMethod == RANDOM){
		srand(time(NULL));
		random = rand()%coordinates;// random int between 0 and last coordinates index
		splitDim = random;
	}
	if(spKDTreeSplitMethod == INCREMENTAL){
		inc= (incrementingDimension)%coordinates;
		//TODO incrementing dim?need to send with 0 at first
		splitDim=inc;
	}
	split(kdarr, splitDim, leftKDArr, rightKDArr);
	node = initEmptyNode();
	node->dim = splitDim;
	node->val = spPointGetAxisCoor((kdarr->points)[(kdarr->matrix)[splitDim][middle-1]], splitDim);// TODO is this what val means? the middle according to the split coordinate
	node->left = initTree(leftKDArr,spKDTreeSplitMethod, incrementingDimension+1);
	node->right = initTree(rightKDArr,spKDTreeSplitMethod, incrementingDimension+1);
	node-> data = NULL;

return node;
}



KDTreeNode initLeaf(int dim, double val, KDTreeNode left,KDTreeNode right){// ~~SPKDTree
		KDTreeNode newNode=NULL;

		newNode= (struct kd_tree_node*) malloc(sizeof (struct kd_tree_node));//TODO verify dereference
		if (newNode ==NULL){
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
		newNode= (struct kd_tree_node*) malloc(sizeof (struct kd_tree_node));//TODO verify dereference
		if (newNode ==NULL){
					return NULL;
		}
		return newNode;
}


int findMaxSpread(SPKDArray kdArr){
	double min = 0.0;
	double max = 0.0;
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
	if (spreadByDim == NULL){//TODO print to logger!! terminate program?
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


