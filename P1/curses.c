#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <math.h>
#include <time.h>
#include "functions.h"

void applyAlgorithm(int opc, int fun, int rep,int bitRange, int range, int tests, float crossChance, float mutateChance);

int main() {

    srand(time(NULL));
    initCurses();

    int fun, rep, opc, range, bitRange;
    float crossChance, mutateChance;
    int tests = 0;

    char *funOptions[] = {"x^2", "ABS | x-5 /2 +sen(x) |", "sen(x) / x+1"};
    char *repOptions[] = {"Binaria", "Gray"};
    char *evalOptions[] = {"Maximizar", "Minimizar"};
    char *algorithmOptions[] = {"Estacionario", "Generacional"}; //Para uso futuro

    fun = selectMenu(funOptions, 3, 1);
    rep = selectMenu(repOptions, 2, 2);
    opc = selectMenu(evalOptions, 2, 3);
    range = rangeMenu();
    bitRange = bitCounter(range);
    tests = testsMenu();
    crossChance = crossMenu();
    mutateChance = mutateMenu();
    clear();

    applyAlgorithm(opc, fun, rep,range, bitRange, tests, crossChance, mutateChance);

    printw("\n");
    printw("Presiona cualquier tecla para salir...");
    getch();
    endwin();
    return 0;
}
void applyAlgorithm(int opc, int fun, int rep,int bitRange, int range, int tests, float crossChance, float mutateChance) {

    float** matrix;
    int** binMatrix, **decMatrix, **selectedChromosomes;
    int selectedIndex[2];
    float* evalMatrix, *fitnessValues;
    float max, min;
    
    matrix = genMatrix(tests, range); // Genera matriz inicial
    binMatrix = genBinaryMatrix(matrix, tests, range, rep); //Genera matriz con binarios
    decMatrix = genDecimalMatrix(binMatrix, tests, range, rep); // Convierte a Decimal
    dataCorrection(matrix,binMatrix,decMatrix,range,bitRange,tests,rep);
    evalMatrix = evalFunction(decMatrix, tests, fun); // Evalua funcion
    
    printw("\n");

    initialGeneration(matrix, binMatrix, decMatrix, evalMatrix, tests, range, fun, rep, opc);

    selectedChromosomes = selectChromosomes(evalMatrix, binMatrix, tests, opc, range, selectedIndex); // Selecciona individuos para cruzar
    fitnessValues = applyFitnessFun(selectedChromosomes, range, crossChance, decMatrix, selectedIndex, randomNumber()); // Aplica la función de fitness y determina si hay cruza en la generación

    geneticAlgorithm(tests, range, decMatrix, evalMatrix, findMaxMin(evalMatrix, tests, opc), opc, crossChance, mutateChance, selectedChromosomes, fitnessValues, selectedIndex);
    
    
    for(int i = 0; i < tests; i++) {
        free(matrix[i]);
        free(binMatrix[i]);
        free(decMatrix[i]);
    }
    if (fitnessValues) 
        free(fitnessValues);    
    free(matrix);
    free(binMatrix);
    free(decMatrix);
    free(evalMatrix);
    free(selectedChromosomes[0]);
    free(selectedChromosomes[1]);
    free(selectedChromosomes);
    return;
}