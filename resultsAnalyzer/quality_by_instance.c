#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "quality_by_instance.h"
#include "quality_by_instance_all.h"
#include "util.h"
#include "dados.h"

#define INF 999999999


void pag_qualityByInstance(Dados* dados, int id)
{
    char strAux[1000] = {""}, nomeArquivo[1000] = {""};
    char novoArquivo[500] = {""}, refTab[100], optLinha[10], simb = '-', cor[8], instanciaLinha[100];
    int chave, qntTotal = 0, qnt = 0, h, i, l, qntLinhas;
    float bestDualBoundLinha, runTimeBestSolLinha, runTimeBestDualBoundLinha, runTimeLitSolLinha;
    double media = 0, mediaQuality = 0, desvio = 0;
    long int literatureSolutionLinha, bestRoudedDualBoundLinha, bestSolutionLinha;
    long int ubTMSLinha, lbTMSLinha, ubTPDLinha;

    char* nomeAlgoritmoFiltrado = filtro_strings(Dados_get_nomeAlgoritmo(dados), -1, 0);
    double *vetMedias = (double*)malloc(Dados_get_qntInstancias(dados) * sizeof(double));

    strcpy(cor, Dados_get_cor1(dados));

    /* Arquivo de gabarito */
    FILE* gab = fopen("gab/pag_quality_by_instance.html", "r");

    if (!gab) {
        printf("\nErro ao criar/abrir arquivo:  gab/pag_quality_by_instance.html");
        exit(1);
    }

    for (i = 0; i < Dados_get_qntParamEntrada(dados); ++i) {
        if (Dados_get_tipoParamEntrada_index(dados, i) == 'i')
            sprintf(strAux, "%d_", (int)Dados_get_tabConjuntoParametros_index(dados, id, i));
        else if (Dados_get_tipoParamEntrada_index(dados, i) == 'f')
            sprintf(strAux, "%.6f_", Dados_get_tabConjuntoParametros_index(dados, id, i));

        strcat(nomeArquivo, strAux);
    }

    strcat(nomeArquivo, ".html");
    strcat(novoArquivo, Dados_get_caminho(dados));
    strcat(novoArquivo, "pg/");
    strcat(novoArquivo, Dados_get_nomeAlgoritmo(dados));
    strcat(novoArquivo, "/");
    strcat(novoArquivo, Dados_get_idTeste(dados));
    strcat(novoArquivo, "/quality_by_instance/");
    strcat(novoArquivo, nomeArquivo);

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

    /* Escrevendo [r00] : Titulo Aba*/
    fprintf(pag, "\n<title>Quality by instance(%s: %s)</title>\n", Dados_get_idTeste(dados), nomeAlgoritmoFiltrado);
    prox_tag(gab, pag);

    /* Escrevendo [r01] : Titulo */
    fprintf(pag, "\nTest %s: %s (Quality by instance)\n", Dados_get_idTeste(dados), nomeAlgoritmoFiltrado);
    prox_tag(gab, pag);

    /* Escrevendo [r02] : Descricao */
    fprintf(pag, "\nAnalyze the quality of each set of parameters by instance\n");
    prox_tag(gab, pag);

    /* Escrevendo [r03] : page_back */
    fprintf(pag, "\n<a href=\"../analysis.html\">Back</a> - <a href=\"../../index_%s.html\">Index</a> - <a href=\"../../../../index.html\">Home</a>\n", Dados_get_nomeAlgoritmo(dados));
    prox_tag(gab, pag);

    /* Escrevendo [r04] : Data */
    fprintf(pag, "\nUpdate: %s\n", __DATE__);
    prox_tag(gab, pag);

    /* Escrevendo [r05] : Cabecalho [ Parametros (entrada) utilizados no teste ] */
    fprintf(pag, "\n<th bgcolor=\"#CDC5BF\" align=\"center\" rowspan=\"3\">&nbsp %s &nbsp</th>", Dados_get_nomeCampos_index(dados, 0));
    fprintf(pag, "\n<th bgcolor=\"#87CEFA\" align=\"center\" rowspan=\"2\"> &nbsp quality &nbsp</th>");
    char* nomeFiltrado = filtro_strings(Dados_get_nomeCampos_index(dados, Dados_get_numCampoCalcQuality(dados)), -1, -1);
    fprintf(pag, "\n<th bgcolor=\"#CDC5BF\" align=\"center\" rowspan=\"3\"> aver.<br/>&nbsp %s &nbsp</th>", nomeFiltrado); //solution:  Dados_get_nomeCampos_index(dados, Dados_get_numCampoSolFinal(dados))
    fprintf(pag, "\n<th bgcolor=\"#CDC5BF\" align=\"center\" rowspan=\"3\"> best<br/>&nbsp literature &nbsp</th>");
    fprintf(pag, "\n<th bgcolor=\"#CDC5BF\" align=\"center\" rowspan=\"3\">&nbsp All &nbsp</th>");
    fprintf(pag, "\n");

    prox_tag(gab, pag);

    /* Escrevendo [r06]: Resultados */

    for (i = 0; i < Dados_get_qntInstancias(dados); ++i) {
        if (Dados_get_ignorarAusentesInfactiveis(dados) == 0 || Dados_get_buscarBdBestSolutions(dados, Dados_get_campoInstancia_index(dados, i), 20, "f") != -1) {
            media = 0; desvio = 0; qnt = 0;
            for (l = 0; l < Dados_get_qntLinhas(dados); ++l) {
                chave = 0;
                if (floatcmp(i, Dados_get_tabDados_index(dados, l, 0)) == 0) {
                    for (h = 0; h < Dados_get_qntParamEntrada(dados); ++h) {
                        if (doublecmp(Dados_get_tabDados_index(dados, l, Dados_get_campoParamEntrada_index(dados, h)), Dados_get_tabConjuntoParametros_index(dados, id, h)) != 0) {
                            chave = 1;
                            break;
                        }
                    }
                    /* Todos os parametros de vetConjuntoParametros[] batem com os presentes na linha l */
                    if (chave == 0) {
                        media += Dados_get_tabDados_index(dados, l, Dados_get_numCampoCalcQuality(dados));
                        ++qnt;
                    }
                }
            }

            /* Procurando best da literatura (nao para substituir no banco de dados, somente para calcular o B/C local) */
            FILE* arqBestSolution = fopen(Dados_get_arquivoBestSolutions(dados), "r");

            if (!arqBestSolution) {
                printf("\nErro ao criar/abrir arquivo:  %s", Dados_get_arquivoBestSolutions(dados));
                exit(1);
            }

            chave = 0;
            strcpy(refTab, "-");

            while (strcmp(refTab, "[end]") != 0) {
                fscanf(arqBestSolution, "%s ; %d ;", refTab, &qntLinhas);
                for (l = 0; l < qntLinhas; ++l) {
                    fscanf(arqBestSolution, "%s ;", instanciaLinha);
                    fscanf(arqBestSolution, "%ld ;", &bestSolutionLinha);
                    fscanf(arqBestSolution, "%f ;", &runTimeBestSolLinha);
                    fscanf(arqBestSolution, "%f ;", &bestDualBoundLinha);
                    fscanf(arqBestSolution, "%ld ;", &bestRoudedDualBoundLinha);
                    fscanf(arqBestSolution, "%f ;", &runTimeBestDualBoundLinha);
                    fscanf(arqBestSolution, "%ld ;", &literatureSolutionLinha);
                    fscanf(arqBestSolution, "%f ;", &runTimeLitSolLinha);
                    fscanf(arqBestSolution, "%ld ;", &ubTPDLinha);
                    fscanf(arqBestSolution, "%ld ;", &ubTMSLinha);
                    fscanf(arqBestSolution, "%ld ;", &lbTMSLinha);
                    fscanf(arqBestSolution, "%s ;", optLinha);
                    fscanf(arqBestSolution, "%s ;", strAux);
                    fscanf(arqBestSolution, "%s ;", strAux);
                    fscanf(arqBestSolution, "%s ;", strAux);
                    fscanf(arqBestSolution, "%s %s ;", strAux, strAux);
                    fscanf(arqBestSolution, "%s %s ;", strAux, strAux);
                    fscanf(arqBestSolution, "%s ;", strAux);
                    fscanf(arqBestSolution, "%s ;", strAux);

                    if (strcmp(instanciaLinha, Dados_get_campoInstancia_index(dados, i)) == 0) {
                        chave = 1;
                        break;
                    }
                }
                if (chave == 1)
                    break;
            }

            fclose(arqBestSolution);

            media = media/qnt;

            pag_qualityByInstanceAll(dados, id, i);

            if (chave == 0)         /* nao encontrou best solution */
                fprintf(pag, "\n<tr bgcolor=%s><td align=\"center\">%s</td><td align=\"center\"> - </td><td align=\"right\">&nbsp %.3f </td><td align=\"right\">&nbsp not found &nbsp</td>", cor, Dados_get_campoInstancia_index(dados, i) , media);
            else {
                if (Dados_get_tipoSolObj(dados) == 's') {
                    fprintf(pag, "\n<tr bgcolor=%s><td align=\"center\">%s</td><td align=\"right\">&nbsp %.6f </td><td align=\"right\">&nbsp %.3f </td><td align=\"right\">&nbsp %ld </td>", cor, Dados_get_campoInstancia_index(dados, i), (float)literatureSolutionLinha/media, media, literatureSolutionLinha);
                    vetMedias[qntTotal] = (float)literatureSolutionLinha/media;
                    mediaQuality += (float)literatureSolutionLinha/media;
                    ++qntTotal;
                } else {              /* tipo: obj = dual bound*/
                    fprintf(pag, "\n<tr bgcolor=%s><td align=\"center\">%s</td><td align=\"right\">&nbsp %.6f </td><td align=\"right\">&nbsp %.3f </td><td align=\"right\">&nbsp %ld </td>", cor, Dados_get_campoInstancia_index(dados, i), media/ubTPDLinha, media, ubTPDLinha);

                    if (ubTPDLinha != 0) {
                        vetMedias[qntTotal] = media/ubTPDLinha;
                        mediaQuality += media/ubTPDLinha;
                    }else
                        vetMedias[qntTotal] = 0.0;

                    ++qntTotal;
                }
            }

            /* botao (All) */
            fprintf(pag, "<td bgcolor=\"%s\" align=\"center\"><for m action=\"%s\" method=\"get\"><input type=\"submit\" value=\"&nbsp All &nbsp\"/></for m></td></tr>", cor, Dados_get_tmp_nomePagina(dados));

            if (!strcmp(cor, Dados_get_cor1(dados) ))
                strcpy(cor,Dados_get_cor2(dados) );
            else
                strcpy(cor,Dados_get_cor1(dados) );
        }
    }
    mediaQuality = mediaQuality/qntTotal;
    Dados_set_tmp_media(dados, mediaQuality);

    /* calculando o desvio padrao */
    for (h = 0; h < Dados_get_qntInstancias(dados); ++h)
        desvio += pow(vetMedias[h] - mediaQuality, 2); // ERRO Valgrind

    desvio = (float)sqrt(desvio / (Dados_get_qntInstancias(dados)-1)); // ERRO Valgrind
    Dados_set_tmp_desvio(dados, desvio);

    prox_tag(gab, pag);

    Dados_set_tmp_nomePagina(dados, nomeArquivo);

    fclose(gab);
    fclose(pag);

    free(nomeFiltrado);
    free(nomeAlgoritmoFiltrado);
    free(vetMedias);
}
