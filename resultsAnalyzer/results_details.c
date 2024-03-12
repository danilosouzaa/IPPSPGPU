#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "results_details.h"
#include "util.h"

#define INF 999999999


void pag_resultsDetails(Dados* dados, int referenciaMelhorParametro, int referenciaMelhorCusto)
{
    int i, j;
    char strAux[100], simb = '-', novoArquivo[500] = {""};

    char* nomeAlgoritmoFiltrado = filtro_strings(Dados_get_nomeAlgoritmo(dados), -1, 0);

    /* Arquivo de gabarito */
    FILE* gab = fopen("gab/pag_results_details.html", "r");

    if (!gab) {
        printf("\nErro ao criar/abrir arquivo:  gab/pag_results_details.html");
        exit(1);
    }

    strcat(novoArquivo, Dados_get_caminho(dados));
    strcat(novoArquivo, "pg/");
    strcat(novoArquivo, Dados_get_nomeAlgoritmo(dados));
    strcat(novoArquivo, "/");
    strcat(novoArquivo, Dados_get_idTeste(dados));
    strcat(novoArquivo, "/details/");
    strcat(novoArquivo, Dados_get_campoInstancia_index(dados, (int)Dados_get_tabDados_index(dados, referenciaMelhorParametro, 0)));
    strcat(novoArquivo, ".html");

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

    /* Escrevendo [r00] : Titulo Aba */
    fprintf(pag, "\n<title>Details (%s: %s)</title>\n", Dados_get_idTeste(dados), nomeAlgoritmoFiltrado);
    prox_tag(gab, pag);

    /* Escrevendo [r01] : Titulo */
    fprintf(pag, "\nTest %s: %s (Details)\n", Dados_get_idTeste(dados), nomeAlgoritmoFiltrado);
    prox_tag(gab, pag);

    /* Escrevendo [r02] : Descricao */
    fprintf(pag, "\nDetails of instance: %s\n", Dados_get_campoInstancia_index(dados, (int)Dados_get_tabDados_index(dados, referenciaMelhorParametro, 0)));
    prox_tag(gab, pag);

    /* Escrevendo [r03] : page_back */
    fprintf(pag, "\n<a href=\"../results.html\">Back</a> - <a href=\"../../index_%s.html\">Index</a> - <a href=\"../../../../index.html\">Home</a>\n", Dados_get_nomeAlgoritmo(dados));
    prox_tag(gab, pag);

    /* Escrevendo [r04] : Data */
    fprintf(pag, "\nUpdate: %s\n", __DATE__);
    prox_tag(gab, pag);

    /* Escrevendo [r05] : Cabecalho [ Parametros da melhor media para a instancia ] */

    /* Cabecalho: campo instancia */
    char* nomeInstanciaFiltrado = filtro_strings(Dados_get_nomeCampos_index(dados, 0), -1, -1);
    fprintf(pag, "\n<th bgcolor=\"#CDC5BF\" align=\"center\">&nbsp %s &nbsp</th>", nomeInstanciaFiltrado);

    /* Cabecalho: campos de entrada */
    for (i = 0; i < Dados_get_qntCampos(dados); ++i) {
        if (chkInVet(Dados_get_campoParamEntrada(dados), Dados_get_qntParamEntrada(dados), i) == 1) { /* 0 = nao eh parametro de entrada. (ou seja, parametro saida*/
            char* nomeCampoParamEntradaFiltrado = filtro_strings(Dados_get_nomeCampos_index(dados, i), -1, -1);
            fprintf(pag, "\n<th bgcolor=\"#CDC5BF\" align=\"center\">&nbsp %s &nbsp</th>", nomeCampoParamEntradaFiltrado);
            free(nomeCampoParamEntradaFiltrado);
        }
    }

    fprintf(pag, "\n");
    prox_tag(gab, pag);

    /* Escrevendo [r06]: Dados [ Parametros da melhor media para a instancia ] */
    fprintf(pag, "\n<tr bgcolor=\"#FFFFFF\"><td align=\"center\">&nbsp %s &nbsp</td>", Dados_get_campoInstancia_index(dados, Dados_get_tabDados_index(dados, referenciaMelhorParametro, 0)));

    /* Dados: campos de entrada */
    for (j = 0; j < Dados_get_qntParamEntrada(dados); ++j)
        if (Dados_get_tipoParamEntrada_index(dados, j) == 'i')
            fprintf(pag, "<td align=\"right\">&nbsp %d</td>", (int)Dados_get_tabDados_index(dados, referenciaMelhorParametro, Dados_get_campoParamEntrada_index(dados, j)));
        else if (Dados_get_tipoParamEntrada_index(dados, j) == 'f')
            fprintf(pag, "<td align=\"right\">&nbsp %.6f</td>", (float)Dados_get_tabDados_index(dados, referenciaMelhorParametro, Dados_get_campoParamEntrada_index(dados, j)));

    fprintf(pag, "</tr>");

    prox_tag(gab, pag);

    /* Escrevendo [r07] : Cabecalho [ Parametros da melhor solucao para a instancia ] */

    /* Cabecalho: campo instancia */
    fprintf(pag, "\n<th bgcolor=\"#CDC5BF\" align=\"center\">&nbsp %s &nbsp</th>", nomeInstanciaFiltrado);

    /* Cabecalho: campo solucao inicial */
    if (Dados_get_numCampoSolInicial(dados) != -1) {
        char* nomeCampoSolInicialFiltrado = filtro_strings(Dados_get_nomeCampos_index(dados, Dados_get_numCampoSolInicial(dados)), -1, -1);
        fprintf(pag, "\n<th bgcolor=\"#CDC5BF\" align=\"center\">&nbsp %s &nbsp</th>", nomeCampoSolInicialFiltrado);
        free(nomeCampoSolInicialFiltrado);
    }

    /* Cabecalho: campo solucao final */  //Dados_get_numCampoSolFinal Dados_get_numCampoCalcQuality
    char* nomeCampoCalcQualityFiltrado = filtro_strings(Dados_get_nomeCampos_index(dados, Dados_get_numCampoCalcQuality(dados)), -1, -1);
    fprintf(pag, "\n<th bgcolor=\"#CDC5BF\" align=\"center\">&nbsp %s &nbsp</th>", nomeCampoCalcQualityFiltrado);

    /* Cabecalho: campos de entrada */
    for (i = 0; i < Dados_get_qntCampos(dados); ++i) {
        if (chkInVet(Dados_get_campoParamEntrada(dados), Dados_get_qntParamEntrada(dados), i) == 1) { /* 0 = nao eh parametro de entrada. (ou seja, parametro saida*/
            char* nomeCampoParamEntradaFiltrado = filtro_strings(Dados_get_nomeCampos_index(dados, i), -1, -1);
            fprintf(pag, "\n<th bgcolor=\"#CDC5BF\" align=\"center\">&nbsp %s &nbsp</th>", Dados_get_nomeCampos_index(dados, i));
            free(nomeCampoParamEntradaFiltrado);
        }
    }

    fprintf(pag, "\n");
    prox_tag(gab, pag);

    /* Escrevendo [r08] : Dados [ Parametros da melhor solucao para a instancia ]  */

    /* Dados: campo instancia */
    fprintf(pag, "\n<tr bgcolor=\"#FFFFFF\"><td align=\"center\">&nbsp %s &nbsp</td>", Dados_get_campoInstancia_index(dados, Dados_get_tabDados_index(dados, referenciaMelhorCusto, 0)));

    /* Dados: campo solucao inicial */
    if (Dados_get_numCampoSolInicial(dados) != -1)
        fprintf(pag, "<td align=\"right\">&nbsp %d &nbsp</td>", (int)Dados_get_tabDados_index(dados, referenciaMelhorCusto, Dados_get_numCampoSolInicial(dados)));

    /* Dados: campo solucao final */
    if (Dados_get_tipoSolObj(dados) == 's')
        fprintf(pag, "<td align=\"right\">&nbsp %d &nbsp</td>", (int)Dados_get_tabDados_index(dados, referenciaMelhorCusto, Dados_get_numCampoCalcQuality(dados)));
    else if (Dados_get_tipoSolObj(dados) == 'o')
        fprintf(pag, "<td align=\"right\">&nbsp %f &nbsp</td>", Dados_get_tabDados_index(dados, referenciaMelhorCusto, Dados_get_numCampoCalcQuality(dados)));

    /* Dados: campos de entrada */
    for (j = 0; j < Dados_get_qntParamEntrada(dados); ++j)
        if (Dados_get_tipoParamEntrada_index(dados, j) == 'i')
            fprintf(pag, "<td align=\"right\">&nbsp %d</td>", (int)Dados_get_tabDados_index(dados, referenciaMelhorCusto, Dados_get_campoParamEntrada_index(dados, j)));
        else if (Dados_get_tipoParamEntrada_index(dados, j) == 'f')
            fprintf(pag, "<td align=\"right\">&nbsp %.6f</td>", Dados_get_tabDados_index(dados, referenciaMelhorCusto, Dados_get_campoParamEntrada_index(dados, j)));

    fprintf(pag, "</tr>");

    prox_tag(gab, pag);

    fclose(gab);
    fclose(pag);

    free(nomeAlgoritmoFiltrado);
    free(nomeInstanciaFiltrado);
    free(nomeCampoCalcQualityFiltrado);
}
