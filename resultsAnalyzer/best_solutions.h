#ifndef BEST_SOLUTIONS_H_INCLUDED
#define BEST_SOLUTIONS_H_INCLUDED

#include "dados.h"

void pag_bestSolutions(Dados* dados, char* refTab, char* instancia, int referenciaMelhorCusto );
char* BResults_filtroInt(char* str, long int num);
char* BResults_filtroFloat(char* str, float num);
char* BResults_filtroChar(char* str);
void BResults_reconstruirPagina(Dados* dados, char* nomeProblema);
void substituir_pagBestSolutions(Dados* dados);

#endif // BEST_SOLUTIONS_H_INCLUDED
