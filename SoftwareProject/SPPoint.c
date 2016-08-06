#include "SPPoint.h"
#include <stdlib.h>
#include <assert.h>

struct sp_point_t{
	int imgIndex;
	int dim;
	double* coordinates;
};


/** Type for defining the point **/
//typedef struct sp_point_t* SPPoint;

SPPoint spPointCreate(double* data, int dim, int index) {
	int i = 0;

	if (data == NULL || dim <=0 || index <0) {
		return NULL;
	}

	SPPoint point = (SPPoint)malloc(sizeof(struct sp_point_t));
	if (point == NULL) {
		return NULL;
	}

	double* coords = (double*) malloc (sizeof(double)*dim);
	if (coords == NULL) {
		return NULL;
	}

	for (i=0; i<dim; i++) {
		coords[i] = data[i];
	}

	point->imgIndex = index;
	point->dim = dim;
	point->coordinates = coords;

	return point;
}

SPPoint spPointCopy(SPPoint source) {
	assert(source != NULL);
	SPPoint copy = spPointCreate(source->coordinates, source->dim, source->imgIndex);
	if (copy == NULL) {
		return NULL;
	}
	return copy;
}

void spPointDestroy(SPPoint point) {
	if (point != NULL) {
		free(point->coordinates);
		free(point);
	}
}

int spPointGetDimension(SPPoint point) {
	assert(point != NULL);
	return point->dim;
}

int spPointGetIndex(SPPoint point) {
	assert(point != NULL);
	return point->imgIndex;
}

double spPointGetAxisCoor(SPPoint point, int axis) {
	assert(point!=NULL && axis < point->dim);
	return point->coordinates[axis];
}

double spPointL2SquaredDistance(SPPoint p, SPPoint q){
	int i =0;
	double distance = 0.0;
	double tmpDistance;
	assert(p!=NULL && q!=NULL && p->dim == q->dim);
	for (i=0; i<p->dim; i++) {
		tmpDistance = p->coordinates[i] - q->coordinates[i];
		distance += tmpDistance*tmpDistance;
	}
	return distance;
}

