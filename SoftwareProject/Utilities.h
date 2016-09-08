#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "SPPoint.h"
#include "SPConfig.h"

struct image_rate_t{
	int imgIndex;
	int rate;
};

/*define a struct which include an image index and the rate, meaning the number
 * of times an image feature was among the k nearest feature to the query*/
typedef struct image_rate_t* imageRate;


/**
 * Allocates a new imageRate in the memory.
 * Given the image index and the image rate.
 *
 * @return
 * NULL in case allocation failure occurred
 * Otherwise, the imageRate is returned
 */
imageRate imageRateCreate(int imgIndex, int rate);

/**
 * Compare function for the qsort. compares two imageRates a and b according to their rate.
 *
 * @param a The first imageRate to be compared
 * @param b The second imageRate to be compared with
 *
 * @return
 * 1 if a->rate is less than b->rate or the rates are equal and a->index is more than b->index
 * 0 if a->rate is equal b->rate and a->index is equal b->index
 * -1 if a->rate is bigger than b->point at that coordinate or the rates are equal and a->index is less than b->index
 */
int rateCompare(const void* a, const void* b);

/**
 * given an array of points and it's size, the function frees the array memory
 * and fress the memory allocated for the points in the array
 */
void freePointsArray(SPPoint* points, int size);

/**
 * The function checks if the proper command line arguments were given to the program
 * and returns the created config
 *
 * @param argc - the first command line argument
 * @param argv - the second command line argument
 *
 * @return
 * SPConfig in case of success, NULL in case of error
 */
SPConfig checkArgs(int argc, char** argv);


#endif /* UTILITIES_H_ */
