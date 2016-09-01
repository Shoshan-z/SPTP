#include <stdio.h>
#include <stdlib.h>
#include "Utilities.h"

imageRate imageRateCreate(int imgIndex, int rate){

	imageRate imgR = (imageRate)malloc(sizeof(struct image_rate_t));
	if (imgR == NULL) {
		return NULL;
	}
	imgR->imgIndex = imgIndex;
	imgR->rate = rate;
	return imgR;
}

int rateCompare(const void* a, const void* b) {
	imageRate img1 = *(imageRate* )a;
	imageRate img2 = *(imageRate* )b;

	if ((img2->rate - img1->rate)>0) {
		return 1;
	}
	else if (img2->rate - img1->rate <0){
		return -1;
	}
	else { //if the rate is equal, the image with the smaller index should be returned
		return (img1->imgIndex - img2->imgIndex);
	}
}

void freePointsArray(SPPoint* points, int size) {
	int i = 0;
	for (i=0; i<size; i++) {
		if (points[i] != NULL) {
			spPointDestroy(points[i]);
		}
	}
	free(points);
}
