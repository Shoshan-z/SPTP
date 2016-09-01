/*
 * KNNSearch.h
 *
 *  Created on: Aug 26, 2016
 *      Author: Lilach
 */

#include "SPBPriorityQueue.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "SPConfig.h"
#include "SPPoint.h"
#include "KDTree.h"
#include "math.h"
#ifndef KNNSEARCH_H_
#define KNNSEARCH_H_
/**This function finds the K nearest neighbors of the test point point:
 * @param curr-the KDTreenode the search is going through
 * @param bpq-the priority queue that is filled with the resulting nearest neighbors
 * @SPPoint point- the point of interest
 */
void kNearestNeighbors( KDTreeNode curr , SPBPQueue bpq, SPPoint point);




#endif /* KNNSEARCH_H_ */
