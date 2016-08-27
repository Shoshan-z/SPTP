/*
 * SPKDArrayN.c
 *
 *  Created on: Aug 25, 2016
 *      Author: Lilach
 */
#include "SPKDArray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SPConfig.h"
#include <assert.h>
#include "SPPoint.h"
#include <math.h> //TODO SHOSHAN - we can use this, right?


typedef struct sp_pointAndIndex* SPPointInd;

typedef struct kdarray* SPKDArray;


int coord =0;
void chooseCoordinate(int coordinate){ // TODO SHOSHAN: The change Ben suggested-decide if we want to use it
	coord= coordinate;
}

SPPointInd spPointIndCreate(SPPoint point,int index){
	if(point == NULL || index<0){
		return NULL; //LOGGER
	}
	SPPointInd pointInd = (SPPointInd)malloc(sizeof(struct sp_pointAndIndex)); ////TODO FREE- Make sure we free this space
	if (pointInd == NULL) {
		return NULL;
	}
	pointInd->point = point;
	pointInd->index = index;
	return pointInd;
}
//SPPointInd spPointIndCopy(SPPointInd source) {
//	assert(source != NULL);
//	SPPointInd copy = spPointIndCreate(source->point,source->index);
//	if (copy == NULL) {
//		return NULL;
//	}
//	return copy;
//}

//void copyPointIndArray(SPPointInd* source, int size, SPPointInd* target){
//	int i=0;
//	for(i=0; i<size; i++){
//		target[i]= spPointIndCopy(source[i]);
//	}
//}
void PointIndToIntArray(SPPointInd* source, int size, int* target){
	assert(source!= NULL && target!=NULL && size>=0);//LOGGER + size>0 or 1?
	int i=0;
	for(i=0; i<size; i++){
		target[i]= source[i]->index;
	}
}

SPKDArray allocateKDArray(int dim, int size){
	SPKDArray KDArr = NULL;
	int** matrix = NULL;
	int i = 0;
	if(dim<0 || size<0){//LOGGER
		return NULL;
	}

	KDArr = (SPKDArray)malloc(sizeof(struct kdarray));
	if(KDArr ==NULL){
		return NULL;
	}
	matrix = (int**)malloc(sizeof (int*)*dim);
	if(matrix ==NULL){
		return NULL;
	}
	for(i=0; i<dim; i++){//TODO add the for loops
			matrix[i]= (int*)malloc(sizeof(int)*size);
			if(matrix[i] ==NULL){
				return NULL;
			}
		}
	KDArr->matrix = matrix;

	return KDArr;
}


SPKDArray init(SPPoint* arr, int size){
	int i = 0;
	int dim = 0;
	SPKDArray newKDArray=NULL;
	int** index;
	if (arr == NULL || size <=0){
		return NULL; //TODO check if need to print something to logger and if it should return null
	}
	newKDArray = (SPKDArray)malloc(sizeof(struct kdarray));

	SPPointInd* pointIndArray =NULL;
	dim = spPointGetDimension(arr[0]);
	pointIndArray = (SPPointInd*) malloc(sizeof(SPPointInd) * size);
	if (pointIndArray == NULL){
		return NULL;
	}
	for(i=0; i<size; i++){
		//pointArray[i]= spPointCopy(arr[i]);
		pointIndArray[i]= spPointIndCreate(arr[i],i); // copy point array into point-index array
		assert(dim == spPointGetDimension(arr[i]));// TODO check what happens if the assertion fails
	}
	index = (int**) malloc(dim*sizeof(int*));// TODO verify free this space
	if (index == NULL){
		return NULL;
	}
	for(i=0; i < dim; i++){  //allocate space
		index[i] = (int*) malloc(size*sizeof(int));
		if (index[i] == NULL){//TODO if we fail in middle of allocation need to free whats been allocated -destroy
			return NULL;
		}
	}

	for(i=0; i <dim; i++){ //runing over the matrix's rows
		qsort(pointIndArray , size , sizeof(SPPointInd), compare);// comparing the i'th row's SPPointInd->points by the i'th coordinate
		//chooseCoordinate(i); //TODO SHOSHAN: if we decide to use ben's idea we need to uncomment this and remove the coor++
		PointIndToIntArray(pointIndArray, size, index[coord]);
		coord++;    // coord is the coordinate we currently sorting according to
	}

newKDArray->matrix=index;
newKDArray->points = arr;
newKDArray->size=size;
newKDArray->dim=dim;
return newKDArray;
}

int compare (const void * a, const void * b)
{

	SPPointInd p1 = *(SPPointInd*) a;// TODO verify if needs to be SPPointInd* instead
	SPPointInd p2 = *(SPPointInd*) b;

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

void split(SPKDArray kdArr, int coor, SPKDArray leftKDArr, SPKDArray rightKDArr){//, int size){//, SPPoint* pointArray){//TODO !!! removed size SHOSHAN check pointers to point arrays left and right
	//int size =sizeof(kdArr[0])/sizeof(kdArr[0][0]);//SHOSHAN: if we decide size is not an arg well calculate it=num of cols
	int* LRArray = NULL;
	int middle =0;
	int size =0;
	int rows=0;
	int i = 0;
	int j = 0;
	int l = 0;// running index for left array
	int r = 0; // running index for right array
	int indexInOrigin = 0;
	int* map1 = {0};
	int* map2 = {0};
	SPPoint* p1 = NULL;
	SPPoint* p2 = NULL;
	assert(kdArr != NULL && coor>=0 && leftKDArr!=NULL && leftKDArr != NULL);//LOGGER
	rows= kdArr->dim;
	size= kdArr->size;
	middle =(int)(ceil((double)size/2));
	p1 = (SPPoint*)malloc(sizeof(SPPoint)*middle);
	p2 = (SPPoint*)malloc(sizeof(SPPoint)*(size-middle));
	LRArray = (int*)malloc(sizeof(int)*size);//SHOSHAN: =the X array, dynamically allocated because size is defined only in runtime
	assert(LRArray != NULL); //TODO couldn't return null in a void func
	map1 = (int*)malloc(sizeof(int)*size);
	assert(map1 != NULL); //TODO couldn't return null in a void func
	map2 = (int*)malloc(sizeof(int)*size);
	assert(map2 != NULL); //TODO couldn't return null in a void func

	for(i = 0; i < size; i++){ //SHOSHAN filling in the LRArray (X in doc)
		indexInOrigin = (kdArr->matrix)[coor][i];
		if( i < middle){
			LRArray[indexInOrigin]=0;
		}
		else{
			LRArray[indexInOrigin]=1;
		}
	}
	l=0;
	r=0;

	for(i = 0; i < size; i++){// split to leftArr= p1  and rightArr=p2
		if (LRArray[i]==0){
			p1[l]=(kdArr->points)[i];
			l++;
		}
		else{
			p2[r]=(kdArr->points)[i];
			r++;

		}
	}
	l=0;
	r=0;
	for(i = 0; i < size; i++){
		if (LRArray[i]==0){
			map1[i]= l;
			map2[i]= -1;
			l++;
		}
		else{   // LRArray[i]= 1
			map2[i]=r;
			map1[i]= -1;
			r++;
		}
	}


	l=0;
	r=0;
	for(i=0; i<rows; i++){ // the split into leftKDArray and rightKDArray
		for (j=0; j<size; j++){
			indexInOrigin=(kdArr->matrix)[i][j];
			if (LRArray[indexInOrigin] == 0){ //X array in doc
				(leftKDArr->matrix)[i][l]= map1[(kdArr->matrix)[i][j]];
				l++;
			}
			else if(LRArray[indexInOrigin]==1){
				(rightKDArr->matrix)[i][r]= map2[(kdArr->matrix)[i][j]];
				r++;
			}
		}
		l = 0;
		r = 0;
	}
	leftKDArr->points = p1;
	leftKDArr->dim = rows;
	leftKDArr->size= middle;

	rightKDArr->points = p2;
	rightKDArr->dim = rows;
	rightKDArr->size= size-middle;

}


