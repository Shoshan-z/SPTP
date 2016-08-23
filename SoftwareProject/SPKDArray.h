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

typedef struct sp_pointAndIndex* SPPointInd;

typedef SPPointInd** SPKDArray;
extern int coor;

/**
 * Initializes the kd-array with the data given by arr.
 * Complexity 𝑂(𝑑×𝑛𝑙𝑜𝑔(𝑛))
 */
SPKDArray init(SPPoint* arr, int size);
int compare (const void * a, const void * b);
void split(SPKDArray kdArr, int coor, SPPointInd** leftArrP, SPPointInd** rightArrP, int size);//, SPPoint* pointArray);
SPPointInd spPointIndCreate(SPPoint point,int index);
SPPointInd spPointIndCopy(SPPointInd source);
void chooseCoordinate(int coordinate); //TODO check if we want to use it
#endif /* SPKDARRAY_H_ */
