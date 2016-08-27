/*
 * SPKDArray.h
 *
 *  Created on: Aug 11, 2016
 *      Author: Lilach
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SPConfig.h"
#include <assert.h>
#include "SPPoint.h"
#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

struct kdarray{
	int** matrix;
	SPPoint* points;
	int size;
	int dim;
};
struct sp_pointAndIndex {
	SPPoint point;
	int index;
};
/** Type for defining the sp_pointAndIndex */
typedef struct sp_pointAndIndex* SPPointInd;
/** Type for defining the kdarray pointer */
typedef struct kdarray* SPKDArray;
/**A global variable holding an int that represents a coordinate*/
extern int coor;

void chooseCoordinate(int coordinate); //TODO check if we want to use it


/**
 * Allocates a new pointInd in the memory.
 * Creates a new SPPointInd with the given SPPoint point and an index.
 *
 * @return
 * NULL in case allocation failure ocurred OR point is NULL OR OR index <0
 * Otherwise, the new point is returned
 */
SPPointInd spPointIndCreate(SPPoint point,int index);

/**
 * Given a source pointInd array,creates an int array with the same size that hold only the indexes of each point ind
 * at the respective place on the array.
 * for example if the point ind array was[{point a,index:0}, {point b, index:2}, {point c, index:1}]
 * The array that would be stored at "target" would be: [0,2,1]
 * @param source- the source pointInd array from which the indices are taken
 * @param size- the size of the source array
 * @param traget-tne target array which is filled with the indexes according to the pointInd array
 *
 * @assert source and target are not null and size>=0.
 */
void PointIndToIntArray(SPPointInd* source, int size, int* target);



/** Allocates a new KDArray.
 *
 * This function creates a new KDArray.
 * @param dim- the number of rows allocated for the matrix field of the struct.
 * @param size- the number of cols allocated for the matrix field of the struct.
 * @return
 * 	NULL - If allocations failed or if dim<0 of size<0.
 * 	A new KDArray in case of success.
 */
SPKDArray allocateKDArray(int dim, int size);

/**
 * Initializes the kd-array with the data given by arr.
 * Complexity O(d*nlogn)
 */
SPKDArray init(SPPoint* arr, int size);


/**
 * Compare function for the qsort.compares two SPPointInds a and b according to their points in a certain coordinate.
 * The function asserts that both a and b are not NULL pointers.
 *
 * @param a The first SPPointInd to be compared
 * @param b The second SPPointInd to be compared with
 * @assert a!=NULL AND b!=NULL
 * @return
 * -1 if a->point in the specified coordinate  is less than b->point at that coordinate
 * 0 if a->point in the specified coordinate  is equal to b->point at that coordinate
 * -1 if a->point in the specified coordinate  is bigger than b->point at that coordinate
 */
int compare (const void * a, const void * b);

/**
 * Given aKDArray kdArr, a coordinate coor and  two kd-arrays pointers leftKDArr,rightKDArr-
 * This function stores in leftKDArr  the ⌈𝒏/𝟐⌉ points with respect to the coordinate coor
 * and the data related such as matrix of these points,the dim of the split, val of median at that coor
 * and the rest of the points so as their related data in rightKDArr.
 * @param kdArr- the KDArray to be split
 * @param coor- the coordinate the split is taking place according to
 * @param leftKDArr, rightKDArr- pointers to the resulting 2 KDArrays (from the splitting according to the coordinate coor)
 * @assert kdArr is not NULL, coor>0, leftArrP and rightArrP are not NULL
 */
void split(SPKDArray kdArr, int coor, SPKDArray leftKDArr, SPKDArray rightKDArr);//, SPPoint* pointArray);



#endif /* SPKDARRAY_H_ */
