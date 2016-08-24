#include <cstdio>
#include<cmath>

extern "C" {
	#include "SPConfig.h"
    #include "SPPoint.h"
#include "SPKDArray.h"
}




int main() {
	SPConfig config = NULL;
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;


	config = spConfigCreate("config.txt", &msg);

	printf("%d\n", msg);

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

	}

		char path[1024] = {0};
		spConfigGetImagePath(path, config, 3 );


		char PCApath[1024] = {0};
		spConfigGetPCAPath(PCApath, config );
		printf("%s\n", path);
		printf("%s\n",PCApath);

		SP_CONFIG_MSG misg ;
		int t_f;
		t_f=spConfigMinimalGui(config,&misg);
		printf("%d\n", t_f);
	//printf("AAAAAAAA\n");
		int i=0;
		int j=0;
		//SPPointInd arri[2][5];
		SPPointInd** arri= (SPPointInd**)malloc(sizeof(SPPointInd*)*2);
		SPPointInd up[5];
		SPPointInd down[5];
		arri[0]=up;
		arri[1]=down;
		SPPointInd** leftKDArr;
		SPPointInd** rightKDArr;
		double point0[2]={1.0,2.0};
		SPPoint a= spPointCreate(point0, 2, 1);
		SPPointInd A= spPointIndCreate(a ,0);
		double point1[2]={123.0,70.0};
		SPPoint b= spPointCreate(point1, 2, 1);
		SPPointInd B= spPointIndCreate(b ,1);
		double point2[2]={2.0,7.0};
		SPPoint c= spPointCreate(point2, 2, 1);
		SPPointInd C= spPointIndCreate(c ,2);
		double point3[2]={9.0,11.0};
		SPPoint d= spPointCreate(point3, 2, 1);
		SPPointInd D= spPointIndCreate(d ,3);
		double point4[2]={3.0,4.0};
		SPPoint e= spPointCreate(point4, 2, 1);
		SPPointInd E= spPointIndCreate(e ,4);
		arri[0][0]=A;// A,C,E,   D,B
		arri[0][1]=C;
		arri[0][2]=E;
		arri[0][3]=D;
		arri[0][4]=B;
		arri[1][0]=A;// A,E,C,    D,B
		arri[1][1]=E;
		arri[1][2]=C;
		arri[1][3]=D;
		arri[1][4]=B;
		int size=5;//TODO pass this sizeof(arri[0])/sizeof(arri[0][0]);//=number of cols = number of different points
		int dim=2;//TODO get it from SPPointGETDIMENSION
		int middle = (int)(ceil((double)size/2));//TODO add(double)

		leftKDArr = (SPPointInd**)malloc(sizeof (SPPointInd*)*dim);//TODO check size?
		if(leftKDArr ==NULL){
						return NULL;
					}
		for(i=0; i<dim; i++){//TODO add the for loops
			leftKDArr[i]= (SPPointInd*)malloc(sizeof(SPPointInd)*middle);
			if(leftKDArr[i] ==NULL){
				return NULL;
			}
		}
		rightKDArr = (SPPointInd**)malloc(sizeof (SPPointInd*)*2);//TODO check size?

		if(rightKDArr ==NULL){
			return NULL;
		}
		for(i=0; i<dim; i++){
			rightKDArr[i]= (SPPointInd*)malloc(sizeof(SPPointInd)*(size-middle));
			if(rightKDArr[i] ==NULL){
				return NULL;
			}
		}
		split(arri, 0, leftKDArr, rightKDArr, 5);//, SPPoint* pointArray)


	return 0;
}
