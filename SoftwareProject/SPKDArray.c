#include "SPKDArray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "SPConfig.h"
#include <assert.h>
#include "SPPoint.h"
#include <math.h>
#include "Utilities.h"

#define ALLOC_ERROR_MSG "Allocation error"
#define INVALID_ARG_ERROR "Invalid arguments"
#define POINTS_DIM_ERROR "Points dimensions mismatch"

typedef struct sp_pointAndIndex* SPPointInd;
typedef struct kdarray* SPKDArray;

int coord =0;

SPPointInd spPointIndCreate(SPPoint point,int index){
	if(point == NULL || index<0){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	SPPointInd pointInd = (SPPointInd)malloc(sizeof(struct sp_pointAndIndex));
	if (pointInd == NULL) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		return NULL;
	}
	pointInd->point = point;
	pointInd->index = index;
	return pointInd;
}

void PointIndToIntArray(SPPointInd* source, int size, int* target){
	if (source == NULL || target== NULL || size<0){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
	}
	int i=0;
	for(i=0; i<size; i++){
		target[i]= source[i]->index;
	}
}

SPKDArray allocateKDArray(int dim, int size){
	SPKDArray KDArr = NULL;
	int** matrix = NULL;
	int i = 0;
	if(dim<0 || size<0){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}

	KDArr = (SPKDArray)malloc(sizeof(struct kdarray));
	if(KDArr ==NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		return NULL;
	}
	matrix = (int**)malloc(sizeof (int*)*dim);
	if(matrix ==NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		return NULL;
	}
	for(i=0; i<dim; i++){
		matrix[i]= (int*)malloc(sizeof(int)*size);
		if(matrix[i] ==NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			goto error;
			}
		}
	KDArr->matrix = matrix;

	return KDArr;

error:
	while(i>=0) {
		free(matrix[i]);
		i--;
	}
	free(matrix);
	return NULL;
}

void SPKDArrayDestroy(SPKDArray kdArr) {
	int dim = 0;
	int size = 0;
	int i =0;

	if (kdArr == NULL) {
		return;
	}
	dim = kdArr->dim;
	size = kdArr->size;

	//free the matrix
	if (kdArr->matrix != NULL) {
		for(i=0; i<dim; i++){
			if (kdArr->matrix[i] != NULL) {
				free(kdArr->matrix[i]);
			}
		}
		free(kdArr->matrix);
	}

	//destroy all points
	if (kdArr->points != NULL) {
		for (i=0; i<size; i++){
			if (kdArr->points[i] != NULL) {
				spPointDestroy(kdArr->points[i]);
			}
		}
		free(kdArr->points);
	}

	free(kdArr);
}

SPKDArray init(SPPoint* arr, int size){
	int i = 0;
	int dim = 0;
	SPKDArray newKDArray=NULL;
	int** index = NULL;
	SPPointInd* pointIndArray =NULL;
	bool success = false;

	if (arr == NULL || size <=0){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	newKDArray = (SPKDArray)malloc(sizeof(struct kdarray));
	if (newKDArray == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		goto cleanup;
	}

	dim = spPointGetDimension(arr[0]);
	pointIndArray = (SPPointInd*) calloc(sizeof(SPPointInd),size);
	if (pointIndArray == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		goto cleanup;
	}
	for(i=0; i<size; i++){
		pointIndArray[i]= spPointIndCreate(arr[i],i); // copy point array into point-index array
		if (pointIndArray[i] == NULL) {
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			goto cleanup;
			}

		if (dim != spPointGetDimension(arr[i])){
					spLoggerPrintError(POINTS_DIM_ERROR, __FILE__, __func__, __LINE__);
					goto cleanup;
				}
		}

	index = (int**) malloc(dim*sizeof(int*));
	if (index == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		goto cleanup;
	}
	for(i=0; i < dim; i++){  //allocate space
		index[i] = (int*) malloc(size*sizeof(int));
		if (index[i] == NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			goto cleanup;
		}
	}

	for(i=0; i <dim; i++){ //runing over the matrix's rows
		qsort(pointIndArray , size , sizeof(SPPointInd), compare);// comparing the i'th row's SPPointInd->points by the i'th coordinate
		PointIndToIntArray(pointIndArray, size, index[coord]);
		coord++;    // coord is the coordinate we currently sorting according to
	}


	newKDArray->points = (SPPoint*) malloc(sizeof(SPPoint)*size);
	if (newKDArray->points == NULL) {
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		goto cleanup;
	}

	for (i=0; i<size; i++){
		newKDArray->points[i] = spPointCopy(arr[i]);
		if (newKDArray->points[i] == NULL) {
			spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
			goto cleanup;
		}
	}
	newKDArray->matrix=index;
	//newKDArray->points = arr;
	newKDArray->size=size;
	newKDArray->dim=dim;

	success = true;
cleanup:
	if (pointIndArray != NULL) {
		i=0;
		while(i<size && pointIndArray[i] != NULL){
			free(pointIndArray[i]);
			i++;
		}
		free(pointIndArray);
	}

	if (!success) {
		SPKDArrayDestroy(newKDArray);
		return NULL;
	}
	return newKDArray;
}

int compare (const void * a, const void * b)
{

	SPPointInd p1 = *(SPPointInd*) a;
	SPPointInd p2 = *(SPPointInd*) b;

	double v1 = 0.0;
	double v2 = 0.0;
	v1 = spPointGetAxisCoor(p1->point, coord);
	v2 = spPointGetAxisCoor(p2->point,coord);
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


void split(SPKDArray kdArr, int coor, SPKDArray leftKDArr, SPKDArray rightKDArr){
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
	bool success = false;

	if (kdArr == NULL || coor<0 || leftKDArr==NULL || rightKDArr == NULL ){
		spLoggerPrintError(INVALID_ARG_ERROR, __FILE__, __func__, __LINE__);
		goto cleanup;
	}

	rows= kdArr->dim;
	size= kdArr->size;
	middle =(int)(ceil((double)size/2));

	p1 = (SPPoint*)calloc(sizeof(SPPoint),middle);
	if (p1 == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		goto cleanup;
	}

	p2 = (SPPoint*)calloc(sizeof(SPPoint),(size-middle));
	if (p2 == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		goto cleanup;
	}
	LRArray = (int*)malloc(sizeof(int)*size);//the X array, dynamically allocated because size is defined only in runtime
	if (LRArray == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		goto cleanup;
	}

	map1 = (int*)malloc(sizeof(int)*size);
	if (map1 == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		goto cleanup;
	}
	map2 = (int*)malloc(sizeof(int)*size);
	if (map1 == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
		goto cleanup;
	}
	for(i = 0; i < size; i++){ //filling in the LRArray (X in doc)
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
			p1[l]=spPointCopy((kdArr->points)[i]);
			if (p1[l] == NULL) {
				spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
				goto cleanup;
			}
			l++;
		}
		else{
			p2[r]=spPointCopy((kdArr->points)[i]);
			if (p2[r] == NULL) {
				spLoggerPrintError(ALLOC_ERROR_MSG, __FILE__, __func__, __LINE__);
				goto cleanup;
			}
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
				(leftKDArr->matrix)[i][l]= map1[(kdArr->matrix)[i][j]]; //the index of the point in p1
				l++;
			}
			else if(LRArray[indexInOrigin]==1){
				(rightKDArr->matrix)[i][r]= map2[(kdArr->matrix)[i][j]]; //the index of the point in p2
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

	success = true;
cleanup:
	if (!success){
		freePointsArray(p1,middle);
		freePointsArray(p2, size-middle);
	}
	free(LRArray);
	free(map1);
	free(map2);

}
