#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"


int par_impar(int num)
{
    if (num % 2 == 0)
        return 0;
    else
        return 1;
}

/*
 *  filtro_strings:
 *  underline: -1 = substitui "_" por " " ; 0 = mantem ; 1 = substitui " " por "_"
 *  quebraDeLinha: -1 = substitui "\n" por " " ; 0 = mantem ; 1 substitui "\n" por uma quebra de linha
 */
char* filtro_strings(char str[], int espacoUnderline, int quebraDeLinha)
{
    char c1 = '-';
    int tam = 0, i;

    char *newStr;

    for (i = 0; c1 != '\0'; ++i) {
        ++tam;
        c1 = str[i];
    }

    newStr = (char*)malloc((tam+1)*sizeof(char));
    if (espacoUnderline == -1) {
        c1 = '-';
        for (i = 0; c1 != '\0'; ++i) {
            c1 = str[i];
            if (c1 == '_')
                newStr[i] = ' ';
            else
                newStr[i] = str[i];
        }
    }

    if (quebraDeLinha == -1)
    {
        /*
        c1 = '-';
        for (i = 0; c1 != '\0'; ++i)
        {
            c1 = str[i];
            if (c1 == '_')
                newStr[i] = ' ';
            else
                newStr[i] = str[i];

            if (i*cnt == (i*4) && i != 0)
            {
                if (strcmp(strAux, "</br>") == 0)
                {

                }

                ++cnt;
                strcpy(strAux, "");
            }
            else
            {
                strAux2[0] = str[i];
                strcat(strAux, strAux2);
            }


        }
        */

    }
    return (char*)newStr;
}

char letraMaiuscula(char *str, int index)
{
    if (str[index] == 'a') return 'A';
    if (str[index] == 'b') return 'B';
    if (str[index] == 'c') return 'C';
    if (str[index] == 'd') return 'D';
    if (str[index] == 'e') return 'E';
    if (str[index] == 'f') return 'F';
    if (str[index] == 'g') return 'G';
    if (str[index] == 'h') return 'H';
    if (str[index] == 'i') return 'I';
    if (str[index] == 'j') return 'J';
    if (str[index] == 'k') return 'K';
    if (str[index] == 'l') return 'L';
    if (str[index] == 'm') return 'M';
    if (str[index] == 'n') return 'N';
    if (str[index] == 'o') return 'O';
    if (str[index] == 'p') return 'P';
    if (str[index] == 'q') return 'Q';
    if (str[index] == 'r') return 'R';
    if (str[index] == 's') return 'S';
    if (str[index] == 't') return 'T';
    if (str[index] == 'u') return 'U';
    if (str[index] == 'v') return 'V';
    if (str[index] == 'x') return 'X';
    if (str[index] == 'z') return 'Z';
    if (str[index] == 'w') return 'W';
    if (str[index] == 'y') return 'Y';
    //if (str[index] == 'ç') return 'Ç'; // pode causar overflow

    return '-';
}

char charMaiusculo(char c)
{
    if (c == 'a') return 'A';
    if (c == 'b') return 'B';
    if (c == 'c') return 'C';
    if (c == 'd') return 'D';
    if (c == 'e') return 'E';
    if (c == 'f') return 'F';
    if (c == 'g') return 'G';
    if (c == 'h') return 'H';
    if (c == 'i') return 'I';
    if (c == 'j') return 'J';
    if (c == 'k') return 'K';
    if (c == 'l') return 'L';
    if (c == 'm') return 'M';
    if (c == 'n') return 'N';
    if (c == 'o') return 'O';
    if (c == 'p') return 'P';
    if (c == 'q') return 'Q';
    if (c == 'r') return 'R';
    if (c == 's') return 'S';
    if (c == 't') return 'T';
    if (c == 'u') return 'U';
    if (c == 'v') return 'V';
    if (c == 'x') return 'X';
    if (c == 'z') return 'Z';
    if (c == 'w') return 'W';
    if (c == 'y') return 'Y';
    //if (c == 'ç') return 'Ç'; // pode causar overflow

    return '-';
}

char* stringMaiusculo(char *str)
{
    char c = '-';
    int i;
    for (i = 0; c != '\0'; ++i) {
        c = str[i];
        if (c != '\0')
            str[i] = charMaiusculo(str[i]);
    }

    return str;
}

int fileExists(char fileName[])
{
    FILE *fp = fopen(fileName,"r");

    if (fp) {
        fclose(fp);
        return 1;   /* arquivo encontrado */
    } else
        return 0;  /* arquivo nao encontrado */

    printf("\nErro in fileExists!");
    return -1;
}

int verificador_de_tipo2(float num)
{
    int aux = (int)num;
    if (aux==num)
        return 0;   // se for inteiro

    return 1;       // se for float
}

/* Compara dois floats */
int floatcmp(float num1, float num2)
{
    num1 = num1 + 0.0000001; /* realizando truncamento para correcao de bug de arredondamento...  */
    num2 = num2 + 0.0000001; /* ...devido a for ma como um float eh armazenado na memoria */

    long int numAux1 = num1*1000000;
    long int numAux2 = num2*1000000;

    //printf("\n|%ld| |%ld|", numAux1, numAux2);
    if (numAux1 == numAux2)
        return 0;
    else if (numAux1 > numAux2)
        return 1;
    else
        return -1;
}

int doublecmp(double num1, double num2)
{
    num1 = num1 + 0.0000001; /* realizando truncamento para correcao de bug de arredondamento...  */
    num2 = num2 + 0.0000001; /* ...devido a for ma como um float eh armazenado na memoria */

    long int numAux1 = num1*1000000;
    long int numAux2 = num2*1000000;

    //printf("\ndoublecmp: |%ld| |%ld|", numAux1, numAux2);
    if (numAux1 == numAux2)
        return 0;
    else if (numAux1 > numAux2)
        return 1;
    else
        return -1;
}

int floatcmp2(float num1, float num2, int casasDecimais)
{
    num1 = num1 + 0.0000001; /* realizando truncamento para correcao de bug de arredondamento...  */
    num2 = num2 + 0.0000001; /* ...devido a for ma como um float eh armazenado na memoria */

    int mult = 6;
    int numAux1;
    int numAux2;

    if (casasDecimais == 0) mult = 1;
    else if (casasDecimais == 1) mult = 10;
    else if (casasDecimais == 2) mult = 100;
    else if (casasDecimais == 3) mult = 1000;
    else if (casasDecimais == 4) mult = 10000;
    else if (casasDecimais == 5) mult = 100000;
    else if (casasDecimais == 6) mult = 1000000;

    numAux1 = num1*mult;
    numAux2 = num2*mult;

    if (numAux1 == numAux2)
        return 0;
    else if (numAux1 > numAux2)
        return 1;
    else
        return -1;
}
