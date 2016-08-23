/*
 * KDTree.h
 *
 *  Created on: Aug 22, 2016
 *      Author: Lilach
 */

#ifndef KDTREE_H_
#define KDTREE_H_
struct kd_tree_node{
	int dim;//the splitting dimension
	double val;//The median value of the splitting dimension
	KDTreeNode left;//Pointer to the left subtree
	KDTreeNode right;//Pointer to the right subtree
	SPPoint data; //Pointer to a point (only if the current node is a leaf) otherwise this field value is NULL
};

typedef struct kd_tree_node* KDTreeNode;

/**
 *
 * Given data array, split Method and an incrementingDimension.
 * create a new KD Tree where split takes place according to the split method
 * @return
 * NULL in case allocation failure occurred OR kdarr is NULL OR SPLIT_METHOD is NONE  OR incrementingDimension <0
 * Otherwise, the new KDTree is returned
 */
KDTreeNode init(SPKDArray kdarr, SPLIT_METHOD spKDTreeSplitMethod, int incrementingDimension);
/**
 *
 * Given data array, split Method and an incrementingDimension.
 * create a new KDTree Node
 * @return
 * NULL in case allocation failure occurred OR left or right is NULL OR dim<0
 * Otherwise, the new KDTreeNode is returned
 */
KDTreeNode initNode(int dim, double val, SPKDArray left,SPKDArray right);//SHOSHAN ~~SPKDTree
/**
 *
 *create a new KDTree Node
 * @return
 * NULL in case allocation failure
 * Otherwise, the new KDTreeNode is returned
 */
KDTreeNode initEmptyNode();
/**
 * given KDArray kdArr
 * find the coordinate with the highest spread, where spread is the
 * difference between the maximum and minimum of the 𝑖𝑡ℎ coordinate of all points
 */
int findMaxSpread(SPKDArray kdArr);



#endif /* KDTREE_H_ */
