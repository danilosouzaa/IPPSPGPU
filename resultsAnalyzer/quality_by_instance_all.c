#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quality_by_instance_all.h"
#include "util.h"
#include "dados.h"

#define INF 999999999


void pag_qualityByInstanceAll(Dados* dados, int id, int numInstance)
{
    int chave, h, i;
    char strAux1[100], simb = '-', cor[8], novoArquivo[500] = {""}, strAux2[1000] = {""}, pagBack[500];

    char* nomeAlgoritmoFiltrado = filtro_strings(Dados_get_nomeAlgoritmo(dados), -1, 0);

    strcpy(cor, Dados_get_cor1(dados));

    /* Arquivo de gabarito */
    FILE* gab = fopen("gab/pag_quality_by_instance_all.html", "r");

    if (!gab) {
        printf("\nErro ao criar/abrir arquivo:  gab/pag_quality_by_instance_all.html");
        exit(1);
    }

    for (i = 0; i < Dados_get_qntParamEntrada(dados); ++i) {
        if (Dados_get_tipoParamEntrada_index(dados, i) == 'i')
            sprintf(strAux1, "%d_", (int)Dados_get_tabConjuntoParametros_index(dados, id, i));
        else if (Dados_get_tipoParamEntrada_index(dados, i) == 'f')
            sprintf(strAux1, "%.6f_", Dados_get_tabConjuntoParametros_index(dados, id, i));

        strcat(strAux2, strAux1);
    }

    /* para: page back */
    strcpy(pagBack, strAux2);

    intToChar(strAux1, numInstance);

    strcat(strAux2, strAux1);
    strcat(strAux2, "_all.html");

    strcat(novoArquivo, "../../../");
    strcat(novoArquivo, Dados_get_nomeAlgoritmo(dados));
    strcat(novoArquivo, "/");
    strcat(novoArquivo, Dados_get_idTeste(dados));
    strcat(novoArquivo, "/quality_by_instance/all/");
    strcat(novoArquivo, strAux2);

    Dados_set_tmp_nomePagina(dados, novoArquivo);

    strcpy(novoArquivo, "");
    strcat(novoArquivo, Dados_get_caminho(dados));
    strcat(novoArquivo, "pg/");
    strcat(novoArquivo, Dados_get_nomeAlgoritmo(dados));
    strcat(novoArquivo, "/");
    strcat(novoArquivo, Dados_get_idTeste(dados));
    strcat(novoArquivo, "/quality_by_instance/all/");
    strcat(novoArquivo, strAux2);

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
    fscanf(gab, "%s", (char*)&strAux1);
    fprintf(pag, "%s ", strAux1);
    fscanf(gab, "%s", (char*)&strAux1);
    fprintf(pag, "%s \n", strAux1);

    /* Escrevendo [r00] : Titulo Aba */
    fprintf(pag, "\n<title>Quality All (%s: %s)</title>\n", Dados_get_idTeste(dados), nomeAlgoritmoFiltrado);
    prox_tag(gab, pag);

    /* Escrevendo [r01] : Titulo */
    fprintf(pag, "\nTest %s: %s (Quality by instance)\n", Dados_get_idTeste(dados), nomeAlgoritmoFiltrado);
    prox_tag(gab, pag);

    /* Escrevendo [r02] : Descricao */
    fprintf(pag, "\nAnalyze the specif ic set of parameters by instance\n");
    prox_tag(gab, pag);

    /* Escrevendo [r03] : page_back */
    fprintf(pag, "\n<a href=\"../%s.html\">Back</a> - <a href=\"../../../index_%s.html\">Index</a> - <a href=\"../../../../../index.html\">Home</a>\n", pagBack, Dados_get_nomeAlgoritmo(dados));
    prox_tag(gab, pag);

    /* Escrevendo [r04] : Data */
    fprintf(pag, "\nUpdate: %s\n", __DATE__);
    prox_tag(gab, pag);

    /* Escrevendo [r05] : Cabecalho [ Parametros (saida) utilizados no teste ] */
    for (i = 0; i < Dados_get_qntCampos(dados); ++i) {
        if (chkInVet(Dados_get_camposIgnorar(dados), Dados_get_qntCamposIgnorar(dados), i) == 0) /* se nao pertence aos campos a ignorar */
            if (chkInVet(Dados_get_campoParamEntrada(dados), Dados_get_qntParamEntrada(dados), i) == 0) { /* 0 = nao eh parametro de entrada. (ou seja, parametro saida*/
                char* nomeCamposFiltrado = filtro_strings(Dados_get_nomeCampos_index(dados, i), -1, -1);
                fprintf(pag, "\n<th bgcolor=\"#CDC5BF\" align=\"center\">&nbsp %s &nbsp</th>", nomeCamposFiltrado);
                free(nomeCamposFiltrado);
            }
    }
    fprintf(pag, "\n");
    prox_tag(gab, pag);

    /* Escrevendo [r06]: Resultados */
    for (int l = 0; l < Dados_get_qntLinhas(dados); ++l) {
        chave = 0;
        if (floatcmp(numInstance, Dados_get_tabDados_index(dados, l, 0)) == 0) {
            for (h = 0; h < Dados_get_qntParamEntrada(dados); ++h) {
                if (floatcmp(Dados_get_tabDados_index(dados, l, Dados_get_campoParamEntrada_index(dados, h)), Dados_get_tabConjuntoParametros_index(dados, id, h)) != 0) {
                    chave = 1;
                    break;
                }
            }

            /* Todos os parametros de vetConjuntoParametros[] batem com os presentes na linha l */
            if (chave == 0) {
                /* Imprime linha (soh parametros de saida */
                fprintf(pag, "\n<tr bgcolor=%s><td align=\"center\">&nbsp %s &nbsp</td>", cor, Dados_get_campoInstancia_index(dados, l));
                for (h = 1; h < Dados_get_qntCampos(dados); ++h) {
                    if (chkInVet(Dados_get_camposIgnorar(dados), Dados_get_qntCamposIgnorar(dados), h) == 0) /* se nao pertence aos campos a ignorar */
                        if (chkInVet(Dados_get_campoParamEntrada(dados), Dados_get_qntParamEntrada(dados), h) == 0) { /* se for parametro de saida */
                            if (Dados_get_tipoParamEntradaSaida_index(dados, h) == 'i'){ /* se o campo for de inteiros */
                                if ((int)Dados_get_tabDados_index(dados, l, h) == -1)
                                    fprintf(pag, "<td align=\"right\"> - </td>");
                                else
                                    fprintf(pag, "<td align=\"right\">&nbsp %d </td>", (int)Dados_get_tabDados_index(dados, l, h));

                            } else {    /* se o campo for de floats (ou saida) */

                                if (floatcmp(Dados_get_tabDados_index(dados, l, h), -1.000000) == 0)
                                    fprintf(pag, "<td align=\"right\"> - </td>");
                                else
                                    fprintf(pag, "<td align=\"right\">&nbsp %.3f </td>", Dados_get_tabDados_index(dados, l, h));
                            }
                        }
                }
                fprintf(pag, "</tr>\n");
                if (!strcmp(cor, Dados_get_cor1(dados) ))
                    strcpy(cor,Dados_get_cor2(dados) );
                else
                    strcpy(cor,Dados_get_cor1(dados) );
            }
        }
    }

    prox_tag(gab, pag);

    fclose(gab);
    fclose(pag);

    free(nomeAlgoritmoFiltrado);
}
