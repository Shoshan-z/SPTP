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

typedef SPPoint** SPKDArray;
extern int coor;

/**
 * Initializes the kd-array with the data given by arr.
 * Complexity 𝑂(𝑑×𝑛𝑙𝑜𝑔(𝑛))
 */
SPKDArray init(SPPoint* arr, int size);



#endif /* SPKDARRAY_H_ */
