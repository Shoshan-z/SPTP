#include "SPKDArray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SPConfig.h"
#include <assert.h>
#include "SPPoint.h"
#include <math.h> //TODO SHOSHAN - we can use this, right?
struct sp_pointAndIndex {
	SPPoint point;
	int index;
};

typedef SPPointInd** SPKDArray;
int coord =0;
void chooseCoordinate(int coordinate){ // TODO SHOSHAN: The change Ben suggested-decide if we want to use it
	coord= coordinate;
}

SPPointInd spPointIndCreate(SPPoint point,int index){
	SPPointInd pointInd = (SPPointInd)malloc(sizeof(struct sp_pointAndIndex)); ////TODO FREE- Make sure we free this space
	if (pointInd == NULL) {
		return NULL;
	}
	pointInd->point = point;
	pointInd->index = index;
	return pointInd;
}
SPPointInd spPointIndCopy(SPPointInd source) {
	assert(source != NULL);
	SPPointInd copy = spPointIndCreate(source->point,source->index);
	if (copy == NULL) {
		return NULL;
	}
	return copy;
}

//void copyPointArray(SPPoint* source, int size, SPPoint* target){
//	int i=0;
//	for(i=0; i<size; i++){
//			target[i]= spPointCopy(source[i]);
//		}
//}
void copyPointIndArray(SPPointInd* source, int size, SPPointInd* target){
	int i=0;
	for(i=0; i<size; i++){
		target[i]= spPointIndCopy(source[i]);
	}
}

SPKDArray init(SPPoint* arr, int size){
	int i = 0;
	int dim = 0;
	SPKDArray kdArray;
	if (arr == NULL || size <=0){
		return NULL; //TODO check if need to print something to logger and if it should return null
	}
	//SPPoint* pointArray =NULL;
	//pointArray = (SPPoint*) malloc(sizeof(SPPoint) * size);
	SPPointInd* pointIndArray =NULL;
	dim = spPointGetDimension(arr[0]);
	pointIndArray = (SPPointInd*) malloc(sizeof(SPPointInd) * size);
	if (pointIndArray == NULL){
		return NULL;
	}
	for(i=0; i<size; i++){ // allocate space for cols
		//pointArray[i]= spPointCopy(arr[i]);
		pointIndArray[i]= spPointIndCreate(arr[i],i); // copy point array into point-index array
		assert(dim == spPointGetDimension(arr[i]));// TODO check what happens if the assertion fails
	}
	kdArray = (SPPointInd**) malloc(sizeof(SPPointInd*)*dim);// TODO verify free this space
	if (kdArray == NULL){
		return NULL;
	}
	for(i=0; i < dim; i++){  //allocate space
		kdArray[i] = (SPPointInd*) malloc(sizeof(SPPointInd) * size);
		if (kdArray[i] == NULL){//TODO if we fail in middle of allocation need to free whats been allocated -destroy
			return NULL;
		}

		for(i=0; i <dim; i++){ //runing over the matrix's rows
			qsort( pointIndArray , size , sizeof(SPPointInd), compare);// comparing the i'th row's SPPointInd->points by the i'th coordinate
			//chooseCoordinate(i); //TODO SHOSHAN: if we decide to use ben's idea we need to uncomment this and remove the coor++
			copyPointIndArray(pointIndArray, size, kdArray[coord]);
			coord++;    // coord is the coordinate we currently sorting according to
		}
	}

	return kdArray;
}

int compare (const void * a, const void * b)
{

	SPPointInd p1 = (SPPointInd)a;// TODO verify if needs to be SPPointInd* instead
	SPPointInd p2 = (SPPointInd)b;
	double v1 = 0.0;
	double v2 = 0.0;
	v1 = spPointGetAxisCoor(p1->point, coord);
	v2 = spPointGetAxisCoor(p2->point,coord);
	//	SPPoint p1 = NULL;
	//	SPPoint p2 = NULL;
	//	p1 = (SPPoint)a;
	//	p2 = (SPPoint)b;
	//	v1 = spPointGetAxisCoor(a,coord);
	//	v2 = spPointGetAxisCoor(b,coord);
	if (v1-v2 > 0){
		return 1;
	}
	else if( v1-v2 < 0){
		return -1;
	}
	else{
		return 0;
	}
}

void split(SPKDArray kdArr, int coor, SPKDArray* leftKDArr, SPKDArray* rightKDArr, int size){//, SPPoint* pointArray){//TODO !!! SHOSHAN check pointers to point arrays left and right
	//int size =sizeof(kdArr[0])/sizeof(kdArr[0][0]);//SHOSHAN: if we decide size is not an arg well calculate it=num of cols
	int* LRArray = NULL;
	int middle = (int)(ceil(size/2));
	int i = 0;
	int j = 0;
	int l = 0;// running index for left array
	int r = 0; // running index for right array
	int indexInOrigin = 0;
	int* map1;
	int* map2;
	int rows= sizeof(kdArr)/sizeof(kdArr[0]);
	SPPointInd* leftArr = NULL;
	SPPointInd* rightArr = NULL;
	//SPPointInd* cpyArrayInds = NULL;
	SPPointInd* leftArr = (SPPointInd*)malloc(sizeof(SPPointInd)*middle);
	SPPointInd* rightArr = (SPPointInd*)malloc(sizeof(SPPointInd)*(size-middle));
	//	cpyArrayInds = (SPPointInd*)malloc(sizeof(SPPointInd)*size);
	LRArray = (int*)malloc(sizeof(int)*size);//SHOSHAN: =the X array, dynamically allocated because size is defined only in runtime
	assertNotNull(LRArray); //TODO culdnt return null in a void func
	map1 = (int*)malloc(sizeof(int)*size);
	assertNotNull(map1); //TODO culdnt return null in a void func
	map2 = (int*)malloc(sizeof(int)*size);
	assertNotNull(map2); //TODO culdnt return null in a void func
	//	for(i = 0; i < size; i++){ //SHOSHAN making a copy of pointArray-NOT sure we still need this copy
	//	cpyArrayInds[i] = spPointIndCreate(pointArray[i],i);
	//}
	for(i = 0; i < size; i++){ //SHOSHAN filling in the LRArray (X in doc)
		indexInOrigin = kdArr[coor][i]->index;
		if( i < middle){
			LRArray[indexInOrigin]=0;
		}
		else{
			LRArray[indexInOrigin]=1;
		}
	}
	//TODO verify if we have to use p1 p2 and map1 map2
	for(i = 0; i < middle; i++){// SHOHAN split to leftArr(p1 in doc) and rightArr(p2 in doc)
		leftArr[i]=kdArr[coor][i];
	}
	for(i = middle, j = 0 ; i < size ; i++, j++){
		rightArr[j]= kdArr[coor][i];
	}
	for(i = 0; i < size; i++){
		if (LRArray[i]==0){
			map1[i]= leftArr[l]->index;
			map2[i]= -1;
			l++;
		}
		else{   // LRArray[i]= 1
			map2[i]= rightArr[r]->index;
			map1[i]= -1;
			r++;
		}
	}
	l=0;
	r=0;
	for(i=0; i<rows; i++){
		for (j=0; j<size; j++){
			if(LRArray[kdArr[i][j]->index]==0){
				*leftKDArr[i][l]= kdArr[i][j];
				l++;
			}
			if(LRArray[kdArr[i][j]->index]==1){
				*rightKDArr[i][r]= kdArr[i][j];
				r++;
			}
		}
		l = 0;
		r = 0;
	}
}
