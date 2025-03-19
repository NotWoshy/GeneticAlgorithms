#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "functions.h"

void menu(int opc, int fun, int rep, int range, int tests);

void main (void) {
    int opc, fun, rep, tests, range;
    srand(time(NULL));

    printf("Selecciona la función fitness:\n");
    printf("1. x^2\n");
    printf("2. ABS | x-5 /2 +sen(x) | \n");
    printf("3. sen(x) / x+1 \n");
    printf("Seleccione una opción: ");
    scanf("%d", &fun);
    if(fun < 0 || fun > 3) {
        printf("Ingresa una opción válida\n");
        return;
    }
    printf("\n");

    printf("Tipo de representación:\n");
    printf("1. Binaria\n");
    printf("2. Gray\n");
    printf("Seleccione una opción: ");
    scanf("%d", &rep);
    if(rep < 0 || rep > 2) {
        printf("Ingresa una opción válida\n");
        return;
    }
    printf("\n");
    
    printf("Evaluación de la función:\n");
    printf("1 - Maximizar\n");
    printf("2 - Minimizar\n");
    printf("Seleccione una opción: ");
    scanf("%d", &opc);
    if(opc < 0 || opc > 2) {
        printf("Ingresa una opción válida\n");
        return;
    }
    printf("\n");

    printf("Rango de la población inicial (1 a 7): ");
    scanf("%d", &range);
    if(range <= 0 || range > 7) {
        printf("Número inválido\n");
        return;
    }
    printf("\n");

    printf("Número de individuos: ");
    scanf("%d", &tests);
 
    menu(opc, fun, rep, range, tests);
    return;
}

void menu(int opc, int fun, int rep, int range, int tests) {

    float** matrix;
    int** binMatrix, **decMatrix;
    float* evalMatrix;
    float max, min;

    matrix = genMatrix(tests, range); // Genera matriz inicial
    binMatrix = genBinaryMatrix(matrix, tests, range, rep); //Genera matriz con binarios
    decMatrix = genDecimalMatrix(binMatrix, tests, range, rep); // Convierte a Decimal
    evalMatrix = evalFunction(decMatrix, tests, fun); // Evalua funcion

    printf("\n");
    if (rep == 1)
        printf("Convirtiendo binario a decimal:\n");
    else if (rep == 2)
        printf("Convirtiendo gray a decimal:\n");
        
    for(int i = 0; i < tests; i++)
        printf("Cromosoma %d: x = %d\n", i+1, decMatrix[i][0]);

    printf("\n");
    printf("Resultado de la evaluación:\n");

    for(int i = 0; i < tests; i++)
        printf("Cromosoma %d: f(%d) = %.2f\n", i+1, decMatrix[i][0], evalMatrix[i]);

    printf("\n");

    if (opc == 1) {
        max = findMaxMin(evalMatrix, tests, opc);
        printf("Máximo de la función: %.2f\n", max);
    } else if (opc == 2) {
        min = findMaxMin(evalMatrix, tests, opc);
        printf("Mínimo de la función: %.2f\n", min);
    }

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