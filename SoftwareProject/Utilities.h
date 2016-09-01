#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "SPPoint.h"

struct image_rate_t{
	int imgIndex;
	int rate;
};

typedef struct image_rate_t* imageRate;

imageRate imageRateCreate(int imgIndex, int rate);

int rateCompare(const void* a, const void* b);

void freePointsArray(SPPoint* points, int size);


#endif /* UTILITIES_H_ */
