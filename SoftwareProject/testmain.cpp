#include <cstdio>
#include<cmath>

extern "C" {
	#include "SPConfig.h"
    #include "SPPoint.h"
	#include "SPKDArray.h"
}

//int** matrix;
//SPPoint* points;
//int size;
//int dim;

SPKDArray CreateEmptyKDArray(int dim, int size) {
	SPKDArray kdArr = NULL;
	SPPoint* points = NULL;
	int** matrix = NULL;
	int i = 0;

	if (size==0 || dim ==0) {
		return NULL;
	}

	kdArr = (SPKDArray) malloc(sizeof(struct kdarray));

	points = (SPPoint*) malloc(sizeof(SPPoint)*size);

	matrix = (int**) malloc(sizeof(int*)*dim);
	for (i=0; i<dim; i++) {
		matrix[i] = (int*) malloc(sizeof(int)*size);
	}

	kdArr->points = points;
	kdArr->matrix = matrix;
	kdArr->size = size;
	kdArr->dim = dim;

	return kdArr;
}

void SPKDArrayDestroy(SPKDArray kdArr) {
	int dim = 0;
	int size = 0;
	int i = 0;

	if (kdArr == NULL) return;

	dim = kdArr->dim;
	size = kdArr->size;

	for (i=0; i<dim; i++) {
		free(kdArr->matrix[i]);
	}
	free(kdArr->matrix);
	for (i=0; i<size; i++) {
		spPointDestroy(kdArr->points[i]);
	}
	free(kdArr->points);
	free(kdArr);
}

int main() {
	SPConfig config = NULL;
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;


	config = spConfigCreate("config.txt", &msg);

	//printf("%d\n", msg);
/*
	if (msg == SP_CONFIG_SUCCESS ) {
		printf("%s\n", config->spImagesDirectory);
		printf("%s\n",config->spImagesPrefix);
		printf("%s\n",config->spImagesSuffix);
		printf("%d\n",config->spNumOfImages);
		printf("%d\n", config->spPCADimension);
		printf("%s\n",config->spPCAFilename);
		printf("%d\n", config->spNumOfFeatures);
		//config->spExtractionMode = false;
		printf("%d\n", config->spNumOfSimilarImages);
		// config->spKDTreeSplitMethod = NONE;
		printf("%d\n", config->spKNN);
		//config->spMinimalGUI = false;
		printf("%d\n",config->spLoggerLevel);
		printf("%s\n", config->spLoggerFilename);


	}*/

		char path[1024] = {0};
		spConfigGetImagePath(path, config, 3 );


		char PCApath[1024] = {0};
		spConfigGetPCAPath(PCApath, config );
		int i=0;
		int j=0;
		SPKDArray leftKDArr;
		SPKDArray rightKDArr;
		double point0[2]={1.0,2.0};
		SPPoint a= spPointCreate(point0, 2, 1);

		double point1[2]={123.0,70.0};
		SPPoint b= spPointCreate(point1, 2, 1);

		double point2[2]={2.0,7.0};
		SPPoint c= spPointCreate(point2, 2, 1);

		double point3[2]={9.0,11.0};
		SPPoint d= spPointCreate(point3, 2, 1);

		double point4[2]={3.0,4.0};
		SPPoint e= spPointCreate(point4, 2, 1);

		SPPoint p[5]={a, b, c, d, e};

		int** A = NULL;

		A = (int**) malloc(sizeof(int*)*2);
		for (int i =0; i<2; i++) {
			A[i] = (int*) malloc(sizeof(int)*5);
		}

		A[0][0] = 0;
		A[0][1] = 2;
		A[0][2] = 4;
		A[0][3] = 3;
		A[0][4] = 1;

		A[1][0] = 0;
		A[1][1] = 4;
		A[1][2] = 2;
		A[1][3] = 3;
		A[1][4] = 1;

		SPKDArray big =NULL;

		big=(SPKDArray)malloc(sizeof(struct kdarray));
		big->points =p;
		big->matrix = A;
		big->dim = 2;
		big->size = 5;



		int middle = (int)(ceil((double)big->size/2));//TODO add(double)


		leftKDArr = CreateEmptyKDArray(2, middle);

		rightKDArr = CreateEmptyKDArray(2, big->size-middle);

		split(big, 0, leftKDArr, rightKDArr);//, SPPoint* pointArray)

		printf("A1 (left KDARR)\n");
		for (i=0; i<leftKDArr->dim; i++) {
			for(j=0; j<leftKDArr->size; j++) {
				printf("%d ", leftKDArr->matrix[i][j]);
			}
			printf("\n");
		}

		printf("A2 (right KDARR)\n");
		for (i=0; i<rightKDArr->dim; i++) {
			for(j=0; j<rightKDArr->size; j++) {
				printf("%d ", rightKDArr->matrix[i][j]);
					}
			printf("\n");
			}


	return 0;
}
