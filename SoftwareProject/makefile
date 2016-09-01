CC = gcc
CPP = g++
#put your object files here
OBJS = main.o SPImageProc.o SPPoint.o 
OUR_OBJS = KDTree.o KNNSearch.o SPBPriorityQueue.o SPConfig.o SPFeaturesFile.o SPKDArray.o SPList.o SPListElement.o SPLogger.o Utilities.o

#The executabel filename
EXEC = SPCBIR
INCLUDEPATH=/usr/local/lib/opencv-3.1.0/include/
LIBPATH=/usr/local/lib/opencv-3.1.0/lib/
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core


CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

C_COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

$(EXEC): $(OBJS) $(OUR_OBJS)
	$(CPP) $(OBJS) $(OUR_OBJS) -L$(LIBPATH) $(LIBS) -o $@
main.o: main.cpp #put dependencies here!
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
SPImageProc.o: SPImageProc.cpp SPImageProc.h SPConfig.h SPPoint.h SPLogger.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
#a rule for building a simple c souorce file
#use gcc -MM SPPoint.c to see the dependencies
SPPoint.o: SPPoint.c SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c
	
$(OUR_OBJS):
	$(CC) $(C_COMP_FLAG) -c $*.c
	
clean:
	rm -f $(OBJS) $(OUR_OBJS) $(EXEC)
