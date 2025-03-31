#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

void initCurses();

float** genMatrix(int numTest, int range);
int** genBinaryMatrix(float** matrix, int numTest, int range, int rep);
int binToDec(int* binMatrix, int range);
void binToGray(int* binMatrix, int* grayMatrix, int range);
void grayToBin(int* grayMatrix, int* binMatrix, int range);
float* evalFunction(int** decMatrix, int numTest, int fun);
int **genDecimalMatrix (int** binMatrix, int numTest, int range, int rep);
float findMaxMin(float* evalMatrix, int numTest, int opc);
int bitCounter(int num);
void dataCorrection(float** matrix,int** binMatrix,int **decMatrix,int binRange,int range,int numTest,int rep);
void saveResult(float* evalMatrix, int numTest, int opc);
void graph(int fun);

void showData(const char *title, int numTest, int range, float **matrix, int **binaryMatrix, int **decMatrix, float *evalMatrix, float maxMin, int opc, int** selectedChromosomes, int* selectedIndex, float* fitnessValues);
void printTitle ();
void printText(int select);
void init_colors(void);
int selectMenu(char *options[], int count, int select);
int writeText (char desp[], int pos, int ch);
float writeTextFloat (char desp[], int pos, int ch);
int testsMenu();
int rangeMenu();
float crossMenu();
float mutateMenu();
void printBox (void);

int** selectChromosomes(float* evalMatrix, int** binMatrix, int tests, int opc, int range, int* selectedIndex);
float randomNumber ();
float* applyFitnessFun (int** selectedChromosomes, int range, float crossChance, int** decMatrix, int* selectedIndex, float randomNumber);
void initialGeneration(float** matrix, int** binMatrix, int** decMatrix, float* evalMatrix, int tests, int range, int fun, int rep, int opc);
void geneticAlgorithm(int tests, int range, int** decMatrix, float* evalMatrix, float maxMin, int opc, float crossChance, float mutateChance, int** selectedChromosomes, float* fitnessValues, int* selectedIndex);