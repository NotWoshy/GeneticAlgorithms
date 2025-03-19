#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

float** genMatrix(int numTest, int range);
int** genBinaryMatrix(float** matrix, int numTest, int range, int rep);
int binToDec(int* binMatrix, int range);
void binToGray(int* binMatrix, int* grayMatrix, int range);
void grayToBin(int* grayMatrix, int* binMatrix, int range);
float* evalFunction(int** decMatrix, int numTest, int fun);
int **genDecimalMatrix (int** binMatrix, int numTest, int range, int rep);
float findMaxMin(float* evalMatrix, int numTest, int opc);
void saveResult(float* evalMatrix, int numTest);
void graph(int fun);

void showData(const char *title, int numTest, int range, float **matrix, int **binaryMatrix, int **decMatrix, float *evalMatrix, float maxMin, int opc);
void printTitle ();
void printText(int select);
void init_colors(void);
int selectMenu(char *options[], int count, int select);
int writeText (char desp[], int pos, int ch);
int testsMenu();
void printBox (void);