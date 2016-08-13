
#include "SPKDArray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SPConfig.h"
#include <assert.h>
#include "SPPoint.h"


typedef SPPoint** SPKDArray;
int coor =0;

SPKDArray init(SPPoint* arr, int size){
	int i = 0;
	int dim = 0;
	SPKDArray KDArray;
	if (arr == NULL){
		return NULL; //TODO check if need to print something to logger and if it should return null
	}
	dim = spPointGetDimension(arr[0]);
	SPPoint* pointArray =NULL;
	pointArray = (SPPoint*) malloc(sizeof(SPPoint * size));
	for(i=0; i<size; i++){ // allocate space for cols
		pointArray[i]= spPointCopy(arr[i]);
		assert(dim ==spPointGetDimension(arr[i]));// TODO check what happens if the assertion fails
	}
	KDArray = (SPPoint**) malloc(sizeof(SPPoint*)*dim);// TODO verify free this space
	for(i=0; i < dim; i++){  //allocate space for rows
		KDArray[i] = (SPPoint*) malloc(SPPoint * size);
	}

	for(i=0; i <dim; i++){ //runing over the matrix's rows
		qsort( pointArray , size , sizeof(SPPoint), compare);
		copyPointArray(pointArray, size, KDArray[coor]);
		coor++;    // coor is the coordinate we currently sorting according to
	}



return NULL;
}

void copyPointArray(SPPoint* source, int size, SPPoint* target){
	int i=0;
	for(i=0; i<size; i++){
			target[i]= spPointCopy(source[i]);
		}

}

int compare (const void * a, const void * b)
{
	double v1 = 0.0;
	double v2 = 0.0;
	SPPoint p1 = NULL;
	SPPoint p2 = NULL;
	p1 = (SPPoint)a;
	p2 = (SPPoint)b;
	v1 = spPointGetAxisCoor(a,coor);
	v2 = spPointGetAxisCoor(b,coor);
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

