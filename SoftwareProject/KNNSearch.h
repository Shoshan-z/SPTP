/*
 * KNNSearch.h
 *
 *  Created on: Aug 26, 2016
 *      Author: Lilach
 */

#include "SPBPriorityQueue.h"
#include <stdlib.h>
#include "SPConfig.h"
#include "SPPoint.h"
#include "KDTree.h"
#include "math.h"
#ifndef KNNSEARCH_H_
#define KNNSEARCH_H_
void kNearestNeighbors( KDTreeNode curr , SPBPQueue bpq, SPPoint point);
bool isLeaf(KDTreeNode kdTreeNode);




#endif /* KNNSEARCH_H_ */
