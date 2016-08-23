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
	//TODO we first need to build a kd-array with all features stored in it (this takes 𝑂(𝑑×𝑛𝑙𝑜𝑔(𝑛))
	//and then we recursively apply the following steps:
	KDTreeNode root = NULL;
	int spread = 0;
	int random = 0;
	int splitDim = 0;
	int middle = 0;
	int size =0;
	int coordinates =0;
	int inc = 0; // will hold the new incrementingDimension
	KDTreeNode node = NULL;
	SPPointInd** leftKDArr;
	SPPointInd** rightKDArr;
	//SPPoint* pointArray;//TODO where do we get it from if needed at all
	coordinates = sizeof (kdarr) / sizeof (kdarr[0]);// =num of rows = num of coordinates in each point
	size=sizeof(kdarr[0])/sizeof(kdarr[0][0]);//=number of cols = number of different points
	middle = (int)(ceil(size/2));

	leftKDArr = (SPPointInd**)malloc(sizeof (SPPointInd*)*middle);//TODO check size?
	if(leftKDArr ==NULL){
		return NULL;
	}
	rightKDArr = (SPPointInd**)malloc(sizeof (SPPointInd*)*(size-middle));//TODO check size?
	if(rightKDArr ==NULL){
			return NULL;
		}
	if (kdarr == NULL || spKDTreeSplitMethod == NONE || incrementingDimension < 0){
		return NULL;// TODO verify if split method = NONE should we return null or what?
	}

	if (size==1){ // stop criteria
		root= initNode(-1, -1.0, NULL, NULL);// TODO verify what is invalid in val
		root->data= kdarr[0][0]->point;
		return root;
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
	split(kdarr, splitDim, &leftKDArr, &rightKDArr, size);
	node = initEmptyNode();
	node->dim = splitDim;
	node->val = spPointGetAxisCoor(kdarr[splitDim][middle]->point, splitDim);// TODO is this what val means? the middle according to the split coordinate
	node->left = initTree(leftKDArr,spKDTreeSplitMethod, incrementingDimension+1);
	node->right = initTree(rightKDArr,spKDTreeSplitMethod, incrementingDimension+1);
	node-> data = NULL;

return NULL; //TODO not supposed to get here
}



KDTreeNode initNode(int dim, double val, KDTreeNode left,KDTreeNode right){// ~~SPKDTree
		KDTreeNode newNode=NULL;
		newNode= (struct kd_tree_node*) malloc(sizeof (struct kd_tree_node));//TODO verify dereference
		if (newNode ==NULL){
			return NULL;
		}
		if(dim<0 ){//TODO check if valid: val<0
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
	int min = 0;
	int max = 0;
	int i = 0;
	int maxSpread = 0;
	int spreadDim = 0;// the dimension we'll spread according to (i.e x=0, y=1 etc)
	int* spreadByDim = NULL;//array of spread of each dim
	int coordinates = sizeof(kdArr)/sizeof(kdArr[0]);// =num of rows = num of coordinates in each point
	int cols=sizeof(kdArr[0])/ sizeof(kdArr[0][0]);//=number of cols = number of different points
	spreadByDim = (int*)malloc(coordinates*sizeof(int));

	for(i=0; i<coordinates; i++){ // go over each coordinate
//		for (j=0; j<cols; j++){ // go over the different points and compare the same coordinate, find min and max
//			if (spPointGetAxisCoor(kdArr[0][j]->point,i) < min){
//				min = kdArr[j][i];
//			}
//			if(spPointGetAxisCoor(kdArr[0][j]->point,i)> max){
//				max = kdArr[j][i];
//			}
//		}
//		spreadByDim[i] = max-min;
//		min=0;
//		max=0;
		max = spPointGetAxisCoor(kdArr[i][cols-1]->point,i);
		min =spPointGetAxisCoor(kdArr[i][0]->point,i);
		spreadByDim[i] = max-min;
	}
	maxSpread = spreadByDim[0];
	for(i=1; i<coordinates; i++){// find the first coordinate with the max spread
		if( spreadByDim[i]> maxSpread){
			spreadDim  =  i;
		}
	}
	return spreadDim;
}
