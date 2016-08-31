/*
 * KDTree.h
 *
 *  Created on: Aug 22, 2016
 *      Author: Lilach
 */

#ifndef KDTREE_H_
#define KDTREE_H_
#include "SPKDArray.h"
struct kd_tree_node{
	int dim;//the splitting dimension- positive
	double val;//The median value of the splitting dimension
	struct kd_tree_node* left;//Pointer to the left subtree
	struct kd_tree_node* right;//Pointer to the right subtree
	SPPoint data; //Pointer to a point (only if the current node is a leaf) otherwise this field value is NULL
};
/** Type for defining the kd_tree_node */
typedef struct kd_tree_node* KDTreeNode;

/**
 *
 * Given data KDarray, split Method and an incrementingDimension.
 * create a new KD Tree  from the KDArray kdarr where each level split is according to the split method
 * @param- kdarr- The KDArray is the data structure that holds the data that the tree building relays on
 * @param split Method-MAX_SPREAD split according to the coordinate with the highest spread, where spread
 * is the difference between the maximum and minimum of the ith coordinate of all points
 * RANDOM split according to a random dimension
 * INCREMENTAL split on increasing dimension (if the upper level split dim was i, this level it will be (i+1)%d)
 * @param incrementingDimension- is relevant only if INCREMENTAL method is chosen, used to supply the former level
 * splitting dimension. if INCREMENATL is not in use- pass 0 (int) in this arg.
 * @return
 * NULL in case allocation failure occurred OR kdarr is NULL OR SPLIT_METHOD is NONE  OR incrementingDimension <0
 * Otherwise, the new KDTree is returned
 */
KDTreeNode initTree(SPKDArray kdarr, SPLIT_METHOD spKDTreeSplitMethod, int incrementingDimension);
/**
 * Node constructor:
 * Given dimension dim, value val and KDTreeNode left,KDTreeNode right.
 * create a new KDTreeNode Node
 * with the given data in it.
 * @param dim- The splitting dimension, a positive int.
 * @param val- The median value of the splitting dimension
 * @param left-Pointer to the left subtree
 * @param right- Pointer to the right subtree
 * @return
 * NULL in case allocation failure occurred OR left or right is NULL OR dim<0
 * Otherwise, the new KDTreeNode is returned
 */
KDTreeNode initLeaf(int dim, double val, KDTreeNode left,KDTreeNode right);
/**
 *An empty node constructor:
 *create a new empty(no data in fields) KDTree Node
 * @return
 * NULL in case allocation failure
 * Otherwise, the new KDTreeNode is returned
 */
KDTreeNode initEmptyNode();
/**
 * this function finds the max spread of a given KDArray
 * @param- KDArray kdArr the KDArray that its MAX_SPREAD is wanted.
 * @return the coordinate with the highest spread, where spread is the
 * difference between the maximum and minimum of the ith coordinate of all points
 * OR -1 if kdArr is NULL
 */
int findMaxSpread(SPKDArray kdArr);


/**
 * this function recursively free all the nodes in the KDTree
 * @param- KDTreeNode kdTree the KDTree to be freed
 *
 * if the kdTree is NULL nothing happens
 */
void KDTreeDestroy(KDTreeNode kdTree);


#endif /* KDTREE_H_ */
