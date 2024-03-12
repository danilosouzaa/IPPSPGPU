#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parameters.h"
#include "util.h"

#define INF 999999999


void pag_parameters(Dados* dados)
{
    char strAux[7], simb = '-';
    char novoArquivo[500] = {""};
    int i, j;

    char* nomeAlgoritmoFiltrado = filtro_strings(Dados_get_nomeAlgoritmo(dados), -1, 0);

    FILE* gab = fopen("gab/pag_parameters.html", "r"); // Arquivo de gabarito

    if (!gab) {
        printf("\nErro ao criar/abrir arquivo:  gab/pag_parameters.html");
        exit(1);
    }

    strcat(novoArquivo, Dados_get_caminho(dados));
    strcat(novoArquivo, "pg/");
    strcat(novoArquivo, Dados_get_nomeAlgoritmo(dados));
    strcat(novoArquivo, "/");
    strcat(novoArquivo, Dados_get_idTeste(dados));
    strcat(novoArquivo, "/");
    strcat(novoArquivo, "parameters.html");

    FILE* pag = fopen(novoArquivo, "w");

    if (!pag) {
        printf("\nErro ao criar/abrir arquivo: pag");
        exit(1);
    }

    /* Copiando até o ponto [r01]: */
    while (simb != '~') {
        simb = (char)getc(gab);
        fputc(simb, pag);
    }

    /* Lendo e escrevendo: "-->" */
    fscanf(gab, "%s", (char*)&strAux);
    fprintf(pag, "%s ", strAux);
    fscanf(gab, "%s", (char*)&strAux);
    fprintf(pag, "%s \n", strAux);

    /* Escrevendo [r01] : Titulo Aba */
    fprintf(pag, "\n<title>Test %s: %s </title>\n", Dados_get_idTeste(dados), nomeAlgoritmoFiltrado);
    prox_tag(gab, pag);

    /* Writing [r02] : Titulo */
    fprintf(pag, "\nTest %s: %s (All Parameters)\n", Dados_get_idTeste(dados), nomeAlgoritmoFiltrado);
    prox_tag(gab, pag);

    /* Writing [r03] : Descricao */
    fprintf(pag, "\nAll parameters used in the test\n");
    prox_tag(gab, pag);

    /* Writing [r3b] : page_back */
    fprintf(pag, "\n<a href=\"../index_%s.html\">Index</a> - <a href=\"../../../index.html\">Home</a>\n", Dados_get_nomeAlgoritmo(dados));
    prox_tag(gab, pag);

    /* Writing [r04] : Data */
    fprintf(pag, "\nUpdate: %s\n", __DATE__);
    prox_tag(gab, pag);

    /* Writing [r05] : Parametros (entrada) utilizados no teste */
    for (i = 1; i < Dados_get_qntCampos(dados); ++i) {
        if (chkInVet(Dados_get_campoParamEntrada(dados), Dados_get_qntParamEntrada(dados), i)) {
            fprintf(pag, "\n<br/><br/><b>%s: </b>", Dados_get_nomeCampos_index(dados, i));
            if (verificador_de_tipo(Dados_get_tabParametros_vet(dados, i), (int)Dados_get_tabParametros_index(dados, i, 0)) == 0) {
                /* Se os parametros forem do tipo: int */
                for (j = 1; j <= (int)Dados_get_tabParametros_index(dados, i, 0); ++j)
                    if (j == (int)Dados_get_tabParametros_index(dados, i, 0))
                        fprintf(pag, "%d ", (int)Dados_get_tabParametros_index(dados, i, j));
                    else
                        fprintf(pag, "%d, ", (int)Dados_get_tabParametros_index(dados, i, j));

            } else {
                /* Se os parametros forem do tipo: float */
                for (j = 1; j <= (int)Dados_get_tabParametros_index(dados, i, 0); ++j)
                    if (j == (int)Dados_get_tabParametros_index(dados, i, 0))
                        fprintf(pag, "%.6f ", Dados_get_tabParametros_index(dados, i, j));
                    else
                        fprintf(pag, "%.6f, ", Dados_get_tabParametros_index(dados, i, j));
            }
        }
    }

    fprintf(pag, "\n<br/><br/><b>Type of problem: </b>%s", Dados_get_nomeProblema(dados));
    fprintf(pag, "\n\n<br/><br/>\n\n");

    prox_tag(gab, pag);

    fclose(gab);
    fclose(pag);

    free(nomeAlgoritmoFiltrado);

    printf("\nParameters - OK"); fflush(stdout);
}

