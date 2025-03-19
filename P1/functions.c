#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ncurses.h>
#include "functions.h"
#define maxLines 50

void showData(const char *title, int numTest, int range, float **matrix, int **binaryMatrix, int **decMatrix, float *evalMatrix, float maxMin, int opc) {
    int scrollPos = 0, visibleLines, ch, maxX, maxY;
    getmaxyx(stdscr,maxY,maxX);
    visibleLines = maxY - 5;

    while (1) {
        clear();
        printw("%s\n\n", title);

        // Muestra datos en ventana
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

        // Max o Min
        if (scrollPos + visibleLines >= numTest && evalMatrix) {
            printw("\n");
            if (opc == 1) {
                printw("Máximo de la función: %.2f\n", maxMin);
            } else if (opc == 2) {
                printw("Mínimo de la función: %.2f\n", maxMin);
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

    //for (int i = 0; i < numTest; i++) {
        //printw("Cromosoma %d: ", i);
        //for (int j = 0; j<range; j++) {
            //printw("%d ", binaryMatrix[i][j]);
        //}
        //printw("\n");
    //}
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

void saveResult(float* evalMatrix, int numTest) {
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

    fprintf(extremos, "%d %.2f\n", maxX, max);
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
        } else if (pos < 4 - 1) {              //Escribe
            desp[pos++] = ch;
            addch(ch);
        }
        refresh();
    }
    numTest = atoi(desp);
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
    }
    attroff(COLOR_PAIR(1));
    refresh();
    return;
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

void printBox (void) {
    attron(COLOR_PAIR(1));
    mvprintw(15, 54, "+-----------+");
    mvprintw(16, 54, "|           |");
    mvprintw(17, 54, "+-----------+");
    attroff(COLOR_PAIR(1));
    return;
  }