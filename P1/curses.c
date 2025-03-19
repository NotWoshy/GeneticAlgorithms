#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <math.h>
#include <time.h>
#include "functions.h"

void applyAlgorithm(int opc, int fun, int rep, int range, int tests);

int main() {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    init_colors();
    srand(time(NULL));

    int fun, rep, opc, range;
    int tests = 0;

    char *funOptions[] = {"x^2", "ABS | x-5 /2 +sen(x) |", "sen(x) / x+1"};
    char *repOptions[] = {"Binaria", "Gray"};
    char *evalOptions[] = {"Maximizar", "Minimizar"};
    char *rangeOptions[] = {"1", "2", "3", "4", "5", "6", "7"};

    fun = selectMenu(funOptions, 3, 1);
    rep = selectMenu(repOptions, 2, 2);
    opc = selectMenu(evalOptions, 2, 3);
    range = selectMenu(rangeOptions, 7, 4);
    tests = testsMenu();
    clear();
    
    applyAlgorithm(opc, fun, rep, range, tests);
    
    printw("\n");
    printw("Presiona cualquier tecla para salir...");
    getch();
    endwin();
    return 0;
}

void applyAlgorithm(int opc, int fun, int rep, int range, int tests) {

    float** matrix;
    int** binMatrix, **decMatrix;
    float* evalMatrix;
    float max, min;
    
    matrix = genMatrix(tests, range); // Genera matriz inicial
    binMatrix = genBinaryMatrix(matrix, tests, range, rep); //Genera matriz con binarios
    decMatrix = genDecimalMatrix(binMatrix, tests, range, rep); // Convierte a Decimal
    evalMatrix = evalFunction(decMatrix, tests, fun); // Evalua funcion

    printw("\n");

    showData("Matriz Generada", tests, range, matrix, NULL, NULL, NULL, 0, 0);
    showData("Interpretando Posibilidades | Conversión a Binario o Gray", tests, range, NULL, binMatrix, NULL, NULL, 0, 0);
    showData("Convirtiendo a Decimal", tests, range, NULL, NULL, decMatrix, evalMatrix, 0, 0);
    showData("Resultado de la Evaluación", tests, range, NULL, NULL, decMatrix, evalMatrix, findMaxMin(evalMatrix, tests, opc), opc);

    saveResult(evalMatrix, tests);
    graph(fun);

    for(int i = 0; i < tests; i++) {
        free(matrix[i]);
        free(binMatrix[i]);
        free(decMatrix[i]);
    }
    free(matrix);
    free(binMatrix);
    free(decMatrix);
    free(evalMatrix);

    return;
}