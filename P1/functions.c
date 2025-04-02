#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ncurses.h>
#include "functions.h"
#define maxLines 50

void initCurses() {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    init_colors();
}

void showData(const char *title, int numTest, int range, float **matrix, int **binaryMatrix, int **decMatrix, float *evalMatrix, float maxMin, int opc, int** selectedChromosomes, int* selectedIndex, float* fitnessValues) {
    int scrollPos = 0, visibleLines, ch, maxX, maxY;
    getmaxyx(stdscr,maxY,maxX);
    visibleLines = maxY - 5;

    while (1) {
        clear();
        printw("%s\n\n", title);

        //Por alguna razón solo funciona si esto se pone al principio (???)
        if (selectedChromosomes && selectedIndex) {
            if (strcmp(title, "Cromosomas seleccionados para el cruce") == 0) {
                for (int i = 0; i < 2; i++) {
                    if (selectedIndex[i] >= 0 && selectedIndex[i] < numTest) { 
                        printw("Cromosoma %d: ", selectedIndex[i]);
                        for (int j = 0; j < range; j++) {
                            printw("%d ", selectedChromosomes[i][j]);
                        }
                        printw("\n");
                    }
                }
            } else if (strcmp(title, "Evaluación en función fitness") == 0 && fitnessValues) { 
                for (int i = 0; i < 2; i++) {
                    if (selectedIndex[i] >= 0 && selectedIndex[i] < numTest) { 
                        printw("Cromosoma %d: ", selectedIndex[i]);
                        for (int j = 0; j < range; j++) {
                            printw("%d ", selectedChromosomes[i][j]);
                        }
                        printw(" | Valor decimal: %d | Aptitud: %.2f", decMatrix[selectedIndex[i]][0], fitnessValues[i]);
                        printw("\n");
                    }
                }
            }
        }

        // Muestra datos en ventana
        if (!selectedChromosomes || !selectedIndex) {
            for (int i = 0; i < visibleLines && (i + scrollPos) < numTest; i++) {
                printw("Cromosoma %d: ", i + scrollPos);

                if (matrix) { // Matriz generada
                    for (int j = 0; j < range; j++) {
                        printw("%.2f ", matrix[i + scrollPos][j]);
                    }
                } 
                else if (binaryMatrix) { // Matriz binaria o Gray
                    for (int j = 0; j < range; j++) {
                        printw("%d ", binaryMatrix[i + scrollPos][j]);
                    }
                } 

                else if (evalMatrix && decMatrix) { // Evaluación de la función
                    if (strcmp(title,"Convirtiendo a Decimal") == 0)
                        printw("x = %d", decMatrix[i + scrollPos][0]);

                    else if (strcmp(title,"Resultado de la Evaluación") == 0)
                        printw("f(%d) = %.2f", decMatrix[i + scrollPos][0], evalMatrix[i + scrollPos]);
                }

                printw("\n");
            }
        

            if (scrollPos + visibleLines >= numTest && evalMatrix) { // Max o Min
                printw("\n");
                if (opc == 1) {
                    printw("Máximo de la función: %.2f\n", maxMin);
                } else if (opc == 2) {
                    printw("Mínimo de la función: %.2f\n", maxMin);
                }
                printw("\n");
            }
        }

        printw("\nUsa las flechas para desplazarte, Enter para seguir.\n");

        refresh();
        ch = getch();

        if (ch == '\n') break;
        else if (ch == KEY_UP && scrollPos > 0) scrollPos--;
        else if (ch == KEY_DOWN && scrollPos < numTest - visibleLines) scrollPos++;
    }
}

float** genMatrix(int numTest, int range) {
    float **matrix = (float **)malloc(numTest*sizeof(float*));

        for (int i = 0; i < numTest; i++)
            matrix[i] = (float *)malloc(range*sizeof(float));
        
        for (int i = 0; i<numTest; i++) {
            for (int j = 0; j<range; j++) {
                matrix[i][j] = ((float)rand()/(float)(RAND_MAX)); // 0% o 100%
            }
        }
        return matrix;
}

int** genBinaryMatrix(float** matrix, int numTest, int range, int rep) {
    
    int **binaryMatrix = (int **)malloc(numTest*sizeof(int*));
    
    //printw("\n");
    //printw("Interpretando posibilidades: \n");
    for (int i = 0; i < numTest; i++)
        binaryMatrix[i] = (int *)malloc(range*sizeof(int));
    
    for (int i = 0; i < numTest; i++) {
        for (int j = 0; j<range; j++) {
            if (matrix[i][j] > 0.5)
                binaryMatrix[i][j] = 1;
            else
                binaryMatrix[i][j] = 0;
        }

        if (rep == 2) { // Si se quiere gray
            int* grayMatrix = (int*)malloc(range*sizeof(int));
            binToGray(binaryMatrix[i], grayMatrix, range);

            for (int j = 0; j<range; j++)
                binaryMatrix[i][j] = grayMatrix[j]; //Utiliza la matriz binaria y copia el gray a ella
            free(grayMatrix);
        }
    }
    return binaryMatrix;
}

int binToDec(int* binMatrix, int range) {
    int number = 0;
    for (int i = 0; i < range; i++)
        number = (number << 1) | binMatrix[i];
    return number;
}

void binToGray(int* binMatrix, int* grayMatrix, int range) {
    grayMatrix[0] = binMatrix[0];

    for (int i = 1; i< range; i++)
        grayMatrix[i] = binMatrix[i-1] ^ binMatrix[i]; //XOR
}

void grayToBin(int* grayMatrix, int* binMatrix, int range) { //Decodificado para graficar
    binMatrix[0] = grayMatrix[0];
    for(int i = 1; i < range;i++ )
        binMatrix[i] = binMatrix[i-1] ^ grayMatrix[i];
}

float* evalFunction(int** decMatrix, int numTest, int fun) {
    float* result = (float*)malloc(numTest*sizeof(float));
    float num, op;

    switch (fun) {
        case 1:
        for (int i = 0; i < numTest; i++) {
            num = (float)decMatrix[i][0];
            op = pow(num,2);
            result[i] = op;
        }
        break;
        
        case 2:
        for (int i = 0; i < numTest; i++) {
            num = (float)decMatrix[i][0];
            op = fabs((num - 5) / (2 + sin(num)));
            result[i] = op;
        }
        break;

        case 3:
        for (int i = 0; i < numTest; i++) {
            num = (float)decMatrix[i][0];
            op = (sin(num)) / (num + 1);
            result[i] = op;
        }
        break;

        default:
        printw("Ingresa una opción valida\n");
        break;

    }
    
    return result;
}

int **genDecimalMatrix (int** binMatrix, int numTest, int range, int rep)
{
    int **decimalMatrix = (int **)malloc(numTest*sizeof(int*));
    
    for (int i = 0; i < numTest; i++)
        decimalMatrix[i] = (int *)malloc(range*sizeof(int));
    
    for (int i = 0; i < numTest; i++) {
        if (rep == 2) {
            int* convertBin = (int*)malloc(range*sizeof(int));
            grayToBin(binMatrix[i], convertBin, range);
            decimalMatrix[i][0] = binToDec(convertBin, range);
            free(convertBin);
        } else 
            decimalMatrix[i][0] = binToDec(binMatrix[i], range);

    }
    return decimalMatrix;
}

float findMaxMin(float* evalMatrix, int numTest, int opc) {
    float max = evalMatrix[0];
    float min = evalMatrix[0];
    for (int i = 0; i < numTest; i++) {
        if (evalMatrix[i] > max)
            max = evalMatrix[i];
        if (evalMatrix[i] < min)
            min = evalMatrix[i];
    }
    if (opc == 1)
        return max;
    else if (opc == 2)
        return min;
}

void saveResult(float* evalMatrix, int numTest, int opc) {
    FILE *fichero = fopen("resultados.txt", "w");
    FILE *extremos = fopen("extremos.txt","w");
    float max = evalMatrix[0], min = evalMatrix[0];
    int maxX = 0, minX = 0;
    
    if (fichero == NULL) {
        printw("Error al abrir el archivo.\n");
        exit(1);
    }

    for (int i = 0; i < numTest; i++) {
        fprintf(fichero, "%d %.2f\n", i, evalMatrix[i]); 

        if (evalMatrix[i] > max) {
            max = evalMatrix[i];
            maxX=i;
        }
        if (evalMatrix[i] < min) {
            min = evalMatrix[i];
            minX=i;
        }
    }

    if (opc == 1) 
        fprintf(extremos, "%d %.2f\n", maxX, max);
    else if (opc == 2)
        fprintf(extremos, "%d %.2f\n", minX, min);
        
    fclose(fichero);
    fclose(extremos);
}

void graph(int fun) {
    FILE *gnuplot = popen("gnuplot -persist", "w");
    if (gnuplot == NULL) {
        printw("Error al abrir gnuplot.\n");
        exit(1);
    }

    switch (fun) {
        case 1:
        fprintf(gnuplot, "set term x11\n");  
        fprintf(gnuplot, "set title 'F(x) = x^2'\n");
        fprintf(gnuplot, "set xlabel 'Cromosoma'\n");
        fprintf(gnuplot, "set ylabel 'f(x)'\n");
        fprintf(gnuplot, "plot 'resultados.txt' with linespoints title 'f(x)', ");
        fprintf(gnuplot, "'extremos.txt' with points pointtype 7 pointsize 2 lc rgb 'red' title 'Maximo/Minimo'\n");
        break;

        case 2:
        fprintf(gnuplot, "set term x11\n");  
        fprintf(gnuplot, "set title 'F(x) = ABS( (x - 5) / (2 + sen(x)) )'\n");
        fprintf(gnuplot, "set xlabel 'Cromosoma'\n");
        fprintf(gnuplot, "set ylabel 'f(x)'\n");
        fprintf(gnuplot, "plot 'resultados.txt' with linespoints title 'f(x)', ");
        fprintf(gnuplot, "'extremos.txt' with points pointtype 7 pointsize 2 lc rgb 'red' title 'Maximo/Minimo'\n");
        break;

        case 3:
        fprintf(gnuplot, "set term x11\n");  
        fprintf(gnuplot, "set title 'F(x) = sen(x) / x+1'\n");
        fprintf(gnuplot, "set xlabel 'Cromosoma'\n");
        fprintf(gnuplot, "set ylabel 'f(x)'\n");
        fprintf(gnuplot, "plot 'resultados.txt' with linespoints title 'f(x)', ");
        fprintf(gnuplot, "'extremos.txt' with points pointtype 7 pointsize 2 lc rgb 'red' title 'Maximo/Minimo'\n");
        break;
        
        default:
        printw("Ingresa una opción valida\n");
        break;
    }
    pclose(gnuplot);
}

void init_colors(void) {
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Titulo | Verde
    init_pair(2, COLOR_RED, COLOR_BLACK);   // Error | Rojo
}

int writeText (char desp[], int pos, int ch) {
    int numTest;
    while ((ch = getch()) != '\n') {
        if (ch == KEY_BACKSPACE || ch == 127) { //Borra
            if (pos > 0) {
                pos--;
                desp[pos] = '\0';
                mvaddch(16, 60 + pos, ' ');
                move(16, 60 + pos);
            }
        } else if (pos < 4 - 1 && ch >= '0' && ch <= '9') {  //Escribe
            desp[pos++] = ch;
            addch(ch);
        }
        refresh();
    }
    numTest = atoi(desp);
    return numTest;
}

float writeTextFloat (char desp[], int pos, int ch) {
    float numTest = 0.0;
    int hasDot = 0;

    while ((ch = getch()) != '\n') {
        if (ch == KEY_BACKSPACE || ch == 127) { //Borra
            if (pos > 0) {
                if (desp[pos - 1] == '.')
                    hasDot = 0;
                pos--;
                desp[pos] = '\0';
                mvaddch(16, 60 + pos, ' ');
                move(16, 60 + pos);
            }
        } else if ((ch >= '0' && ch <= '9') || (ch == '.' && !hasDot && pos > 0)) {              //Escribe
            if (ch == '.')
                hasDot = 1;
            desp[pos++] = ch;
            addch(ch);
        }
        refresh();
    }
    numTest = atof(desp);
    return numTest;
}

int selectMenu(char *options[], int count, int select) {
    int highlight = 0;
    int ch;
    while (1) {
        clear();
        printTitle();
        printText(select);
        attron(COLOR_PAIR(1));

        for (int i = 0; i < count; i++) {
            if (i == highlight)
                attron(A_REVERSE);
            mvprintw(13 + 2*i, 55, "%s", options[i]);
            if (i == highlight)
                attroff(A_REVERSE);
        }
        attroff(COLOR_PAIR(1));
        refresh();

        ch = getch();
        switch (ch) {
            case KEY_UP:
                if (highlight > 0) highlight--;
                break;
            case KEY_DOWN:
                if (highlight < count - 1) highlight++;
                break;
            case '\n':
                return highlight + 1;
        }
    }
}

void printTitle () {
    attron(COLOR_PAIR(1));
    box(stdscr, 0, 0);
    mvprintw(5,40, "   ___                _   _           _ ");
    mvprintw(6,40,"  / _ \\_ __ __ _  ___| |_(_) ___ __ _/ |");
    mvprintw(7,40," / /_)/ '__/ _` |/ __| __| |/ __/ _` | |");
    mvprintw(8,40,"/ ___/| | | (_| | (__| |_| | (_| (_| | |");
    mvprintw(9,40,"\\/    |_|  \\__,_|\\___|\\__|_|\\___\\__,_|_|");
    attroff(COLOR_PAIR(1));
    refresh();
}

void printText(int select) {
    attron(COLOR_PAIR(1));
    box(stdscr, 0, 0);
    switch (select) {
        case 1:
        mvprintw(11,44,"Selecciona la función fitness");
        break;

        case 2:
        mvprintw(11,48,"Tipo de representación");
        break;
        
        case 3:
        mvprintw(11,48,"Evaluación de la función");
        break;

        case 4:
        mvprintw(11,45,"Rango de la población inicial");
        break;
        
        case 5:
        mvprintw(11,51,"Número de individuos");
        break;

        case 6:
        mvprintw(11,50,"Probabilidad de cruce");
        break;

        case 7:
        mvprintw(11,50,"Probabilidad de mutación");
        break;
    }
    attroff(COLOR_PAIR(1));
    refresh();
    return;
}

int rangeMenu() {
    int range = 0;
    char desp[3] = {0};
    clear();
    printTitle();
    printText(4);
    printBox();
    refresh();
    move(16, 60);
    range = writeText(desp, 0, 0);
    return range;
}  

int testsMenu() {
    int tests = 0;
    char desp[3] = {0};
    clear();
    printTitle();
    printText(5);
    printBox();
    refresh();
    move(16, 60);
    tests = writeText(desp, 0, 0);
    return tests;
}

float crossMenu() {
    float crossProb = 0.0;
    char desp[10] = {0};
    clear();
    printTitle();
    printText(6);
    printBox();
    refresh();
    move(16, 60);
    crossProb = writeTextFloat(desp, 0, 0);
    return crossProb;
}

float mutateMenu() {
    float mutationProb = 0;
    char desp[10] = {0};
    clear();
    printTitle();
    printText(7);
    printBox();
    refresh();
    move(16, 60);
    mutationProb = writeTextFloat(desp, 0, 0);
    return mutationProb;
}

void printBox (void) {
    attron(COLOR_PAIR(1));
    mvprintw(15, 54, "+-----------+");
    mvprintw(16, 54, "|           |");
    mvprintw(17, 54, "+-----------+");
    attroff(COLOR_PAIR(1));
    return;
  }

void dataCorrection(float** matrix,int** binMatrix,int **decMatrix,int binRange,int range,int numTest,int rep){
    int outRange=1;
    for (int i = 0; i < numTest; i++) {
        if(decMatrix[i][0]>range){
            while(outRange==1){
                for(int j=0;j<=binRange;j++){
                    matrix[i][j]= ((float)rand()/(float)(RAND_MAX));
                }
                for(int j=0;j<=binRange;j++){
                    if (matrix[i][j] > 0.5)
                        binMatrix[i][j] = 1;
                    else
                        binMatrix[i][j] = 0;
                    if (rep == 2) { // Si se quiere gray
                        int* grayMatrix = (int*)malloc(binRange*sizeof(int));
                        binToGray(binMatrix[i], grayMatrix, binRange);
                        for (int j = 0; j<binRange; j++)binMatrix[i][j] = grayMatrix[j]; //Utiliza la matriz binaria y copia el gray a ella
                        free(grayMatrix);
                    }
                }
                if (rep == 2) {
                    int* convertBin = (int*)malloc(binRange*sizeof(int));
                    grayToBin(binMatrix[i], convertBin, binRange);
                    decMatrix[i][0] = binToDec(convertBin, binRange);
                    free(convertBin);
                } else
                    decMatrix[i][0] = binToDec(binMatrix[i], binRange);
                if(decMatrix[i][0]<=range){
                    outRange=0;
                }
            }
            outRange=1;
        }
    }
}

int bitCounter(int num) {
    if (num == 0) return 1;
    int bits = 0;
    while (num) {
    bits++;
    num >>= 1;
    }
    return bits;
}

int** selectChromosomes(float* evalMatrix, int** binMatrix, int tests, int opc, int range, int* selectedIndex){
    int** selected = (int**)malloc(2 * sizeof(int*)); //Dos comosomas elegidos
    int firstChosenId = -1, secondChosenId = -1;
    float bestValue, secondBestValue; //Max o Min
    
    if (opc == 1) {
        bestValue = -99999.0;
        secondBestValue = -99999.0;
    } else if (opc == 2) {
        bestValue = 99999.0;
        secondBestValue = 99999.0;
    }

    for (int i = 0; i < 2; i++) {
        selected[i] = (int*)malloc(range * sizeof(int));
        if (!selected[i]) {
            printw("Error al reservar memoria.\n");
            free(selected[i]);
            free(selected);
            exit(1);
        }
    }

    for (int i = 0; i < tests; i++) {
        float value = evalMatrix[i];
        if ((opc == 1 && value > bestValue) || (opc == 2 && value < bestValue)) {
            secondBestValue = bestValue;
            secondChosenId = firstChosenId;
            bestValue = value;
            firstChosenId = i;
        } else if (((opc == 1 && value > secondBestValue) || (opc == 2 && value < secondBestValue)) && i != firstChosenId) {
            secondBestValue = value;
            secondChosenId = i;
        }
    }

    if (firstChosenId == -1 || secondChosenId == -1) {
        printw("No hay suficientes cromosomas válidos.\n");
        free(selected[0]);
        free(selected[1]);
        free(selected);
        return NULL;
    }

    selectedIndex[0] = firstChosenId;
    selectedIndex[1] = secondChosenId;

    for (int j = 0; j < range; j++) {
        selected[0][j] = binMatrix[firstChosenId][j];
        selected[1][j] = binMatrix[secondChosenId][j];
    };

    return selected;
}

float* applyFitnessFun (int** selectedChromosomes, int range, float crossChance, int** decMatrix, int* selectedIndex, float randomNumber) {

    float* fitnessResults = (float*)malloc(2 * sizeof(float));
    int dec1 = 0, dec2 = 0;
    
    if (randomNumber > crossChance) { //No cruza
        printw("\nNo ocurre cruza en esta generación.\n");
        refresh(); 
        free(fitnessResults);
        return NULL;
    }

    //Fitness
    dec1 = (selectedIndex[0] >= 0) ? binToDec(selectedChromosomes[0], range) : 0;
    dec2 = (selectedIndex[1] >= 0) ? binToDec(selectedChromosomes[1], range) : 0;

    if (dec1 + dec2 == 0) {
        printw("Error: División por 0 en función fitness.\n");
        refresh();
        free(fitnessResults);
        return NULL;
    }

    fitnessResults[0] = (float)dec1 / (dec1+dec2);
    fitnessResults[1] = (float)dec2 / (dec1+dec2);

   return fitnessResults;
}

float randomNumber () {
    float number = ((float)rand()/(float)(RAND_MAX));
    return number;
}

void initialGeneration(float** matrix, int** binMatrix, int** decMatrix, float* evalMatrix, int tests, int range, int fun, int rep, int opc){
    showData("Matriz Generada", tests, range, matrix, NULL, NULL, NULL, 0, 0, NULL, NULL, NULL);
    showData("Interpretando Posibilidades | Conversión a Binario o Gray", tests, range, NULL, binMatrix, NULL, NULL, 0, 0, NULL, NULL, NULL);
    showData("Convirtiendo a Decimal", tests, range, NULL, NULL, decMatrix, evalMatrix, 0, 0, NULL, NULL, NULL);
    showData("Resultado de la Evaluación", tests, range, NULL, NULL, decMatrix, evalMatrix, findMaxMin(evalMatrix, tests, opc), opc, NULL, NULL, NULL);
    saveResult(evalMatrix, tests, opc);
    graph(fun);
}
    
int crossAvailable(float* fitnessValues) {

    float rnd1, rnd2, firstFitnessLimit, secondFitnessLimit;
    int firstValid, secondValid;

    rnd1 = randomNumber(); 
    rnd2 = randomNumber();

    firstFitnessLimit = fitnessValues[0];   
    secondFitnessLimit = firstFitnessLimit + fitnessValues[1]; 

    //Para propósitos de prueba, se asegura que siempre se crucen. Quitar código comentado para simulación real
    firstValid = (rnd1 >= 0.0);
    //firstValid = (rnd1 >= 0.0 && rnd1 <= firstFitnessLimit);
    secondValid = (rnd2 >= 0.0);
    //secondValid = (rnd2 > firstFitnessLimit && rnd2 <= secondFitnessLimit);

    return (firstValid && secondValid); // 1 si ambos aptos
}

void geneticAlgorithm(int tests, int range, int** decMatrix, float* evalMatrix, float maxMin, int opc, float crossChance, float mutateChance, int** selectedChromosomes, float* fitnessValues, int* selectedIndex) {
    showData("Cromosomas seleccionados para el cruce", tests, range, NULL, NULL, decMatrix, evalMatrix, findMaxMin(evalMatrix, tests, opc), opc, selectedChromosomes, selectedIndex, NULL);
    if (fitnessValues == NULL || crossAvailable(fitnessValues) == 0) {
        printw("\nNo ocurre cruza en esta generación.\n");
        //mutateAvailable() - determina qué bit del cromosoma puede mutar (Se aplica a todos los bits) usando un aleatorio que esté dentro del rango preguntado al inicio
        //                  - Si bit muta, intercambia 0 con 1, y visceversa
        //Usa nuevamente evalFun() para testear los nuevos individuos
        //Grafica para ver nuevos resultados
        return;
    }
    showData("Evaluación en función fitness", tests, range, NULL, NULL, decMatrix, NULL, 0, 0, selectedChromosomes, selectedIndex, fitnessValues);
    printw("\nAmbos cromosomas son aptos para cruzar\n");
    //getCrossPoints() - debe mostrar el punto de corte tipo 0 0 | 1 0
    //crossChromosomes() - cruza los cromosomas usando el punto de corte anterior
    //mutateAvailable() - determina qué bit del cromosoma puede mutar (Se aplica a todos los bits) usando un aleatorio que esté dentro del rango preguntado al inicio
    //                  - Si bit muta, intercambia 0 con 1, y visceversa
    //Usa nuevamente evalFun() para testear los nuevos individuos
    //Grafica para ver nuevos resultados
}   