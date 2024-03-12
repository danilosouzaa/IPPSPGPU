#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "results.h"
#include "results_details.h"
#include "best_solutions.h"
#include "util.h"
#include "dados.h"

#define INF 999999999


void pag_results(Dados* dados)
{
    char strAux[1000] = {""}, simb = '-', cor[8];
    char novoArquivo[500] = {""}, refTab[100], instanciaLinha[100], optLinha[10];
    int qnt, chave, referenciaMelhorParametro = -1, referenciaParametro = -1, referenciaMelhorCusto = -1;
    int qntLinhas, limitePorcentagem, cont, h, i, j, l;
    float bestDualBound = 0, porcentagem, runTimeBestSolLinha, runTimeBestDualBoundLinha, runTimeLitSolLinha;
    float bestDualBoundLinha;
    double media, melhorMedia = INF;
    long int bestCost = INF, literatureSolutionLinha, bestSolutionLinha;
    long int ubTPDLinha, ubTMSLinha, lbTMSLinha, bestRoudedDualBoundLinha;

    char* nomeAlgoritmoFiltrado = filtro_strings(Dados_get_nomeAlgoritmo(dados), -1, 0);
    float *vetMedia = (float*)malloc(Dados_get_qntInstancias(dados) * sizeof(float));
    float *vetDesvio = (float*)malloc(Dados_get_qntInstancias(dados) * sizeof(float));
    float *vetMelhorMedia = (float*)malloc(Dados_get_qntInstancias(dados) * sizeof(float));
    float *vetMelhorDesvio = (float*)malloc(Dados_get_qntInstancias(dados) * sizeof(float));

    strcpy(cor, Dados_get_cor1(dados));

    FILE* gab = fopen("gab/pag_results.html", "r"); // Arquivo de gabarito

    if (!gab) {
        printf("\nErro ao criar/abrir arquivo:  gab/pag_results.html");
        exit(1);
    }

    strcat(novoArquivo, Dados_get_caminho(dados));
    strcat(novoArquivo, "pg/");
    strcat(novoArquivo, Dados_get_nomeAlgoritmo(dados));
    strcat(novoArquivo, "/");
    strcat(novoArquivo, Dados_get_idTeste(dados));
    strcat(novoArquivo, "/");
    strcat(novoArquivo, "results.html");

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

    /* Escrevendo [r01] : Titulo Aba*/
    fprintf(pag, "\n<title>Test %s: %s </title>\n", Dados_get_idTeste(dados), nomeAlgoritmoFiltrado);
    prox_tag(gab, pag);

    /* Escrevendo [r02] : Titulo */
    fprintf(pag, "\nTest %s: %s (Results)\n", Dados_get_idTeste(dados), nomeAlgoritmoFiltrado);
    prox_tag(gab, pag);

    /* Escrevendo [r03] : Descricao */
    fprintf(pag, "\n%s\n", Dados_get_descricao(dados));
    prox_tag(gab, pag);

    /* Escrevendo [r04] : page_back */
    fprintf(pag, "\n<a href=\"../index_%s.html\">Index</a> - <a href=\"../../../index.html\">Home</a>\n", Dados_get_nomeAlgoritmo(dados));
    prox_tag(gab, pag);

    /* Escrevendo [r05] : Data */
    fprintf(pag, "\nUpdate: %s\n", __DATE__);
    prox_tag(gab, pag);

    /* Escrevendo [r06] : Cabecalho [ Parametros (entrada) utilizados no teste ] media e desvio padrao */

    /* Cabecalho: campos instancia */
    char* nomeCampoInstanceFiltrado = filtro_strings(Dados_get_nomeCampos_index(dados, 0), -1, -1);
    fprintf(pag, "\n<th bgcolor=\"#cecece\" align=\"center\" rowspan=\"2\"><div class='header'>&nbsp %s &nbsp</div></th>", nomeCampoInstanceFiltrado);

    /* Cabecalho: campos media e desvio */
    for (i=0; i < Dados_get_qntCampos(dados); ++i) {
        if (chkInVet(Dados_get_resultsCamposCalcMediaDesvio(dados), Dados_get_qntResultsCamposCalcMediaDesvio(dados), i)) {
            char* nomeFiltrado = filtro_strings(Dados_get_nomeCampos_index(dados, i), -1, -1);
            fprintf(pag, "\n<th bgcolor=\"#cecece\" align=\"center\" rowspan=\"2\"><div class='header'> aver.<br/>&nbsp %s &nbsp</div></th>", nomeFiltrado);
            fprintf(pag, "\n<th bgcolor=\"#cecece\" align=\"center\" rowspan=\"2\"><div class='header'> std. dev.<br/>&nbsp %s &nbsp</div></th>", nomeFiltrado);
            free(nomeFiltrado);
        }
    }

    char* nomeFiltrado1 = filtro_strings(Dados_get_nomeCampos_index(dados, Dados_get_numCampoCalcQuality(dados)), -1, -1);

    if (Dados_get_tipoSolObj(dados) == 's') {       /* solution */
        /* Cabecalho: campo best cost */
        fprintf(pag, "\n<th bgcolor=\"#cecece\" align=\"center\" rowspan=\"2\"><div class='header'> &nbsp best %s &nbsp<br/>&nbsp found &nbsp </div></th>", nomeFiltrado1);
        /* Cabecalho: campo best literature */
        fprintf(pag, "\n<th bgcolor=\"#cecece\" align=\"center\" rowspan=\"2\"><div class='header'> &nbsp best %s &nbsp<br/>&nbsp literature &nbsp </div></th>", nomeFiltrado1);
        /* Cabecalho: campo B/C */
        fprintf(pag, "\n<th bgcolor=\"#cecece\" align=\"center\" rowspan=\"2\"><div class='header'> &nbsp B/C &nbsp </div></th>");
    } else {                                        /* dual bound */

        /* Cabecalho: campo best dual bound */
        fprintf(pag, "\n<th bgcolor=\"#cecece\" align=\"center\" rowspan=\"2\"><div class='header'> &nbsp best &nbsp</br>&nbsp %s &nbsp<br/>&nbsp found &nbsp </div></th>", nomeFiltrado1);
        /* Cabecalho: campo best literature */
        fprintf(pag, "\n<th bgcolor=\"#cecece\" align=\"center\" rowspan=\"2\"><div class='header'> &nbsp UB TPD &nbsp </div></th>");
        /* Cabecalho: campo dual bound / UB TPD */
        fprintf(pag, "\n<th bgcolor=\"#cecece\" align=\"center\" rowspan=\"2\"><div class='header'> &nbsp %s / &nbsp </br> &nbsp UB TPD &nbsp </div></th>", nomeFiltrado1);
    }

    /* Cabecalho: [ Parametros (entrada) utilizados no teste ] restante */
    for (i=1; i < Dados_get_qntCampos(dados); ++i) {
        if (chkInVet(Dados_get_resultsExibirCampos(dados), Dados_get_qntresultsExibirCampos(dados), i)) {
            /* verif icando se o campo a exibir eh um parametro de entrada (obrigatorio ser) */
            if (chkInVet(Dados_get_campoParamEntrada(dados), Dados_get_qntParamEntrada(dados), i))
            {
                char* nomeFiltrado2 = filtro_strings(Dados_get_nomeCampos_index(dados, i), -1, -1);
                fprintf(pag, "\n<th bgcolor=\"#cecece\" align=\"center\" rowspan=\"2\"><div class='header'> &nbsp %s &nbsp</div></th>", nomeFiltrado2);
                free(nomeFiltrado2);
            }
            else
                printf("\nCabecalho| campo: %d nao eh um parametro de entrada. Ignorando campo.", i);
        }
    }

    /* Cabecalho: campo download solucao */
    if (Dados_get_numCampoNomeSol(dados) != -1)
        fprintf(pag, "\n<th bgcolor=\"#cecece\" align=\"center\" rowspan=\"2\"><div class='header'>&nbsp file *.sol &nbsp</div></th></th>");

    /* Cabecalho: campo details */
    fprintf(pag, "\n<th bgcolor=\"#cecece\" align=\"center\" rowspan=\"2\"><div class='header'>&nbsp details &nbsp</div></th></th>");

    fprintf(pag, "\n");
    prox_tag(gab, pag);

    /* Escrevendo [r07] : Resutados */
    limitePorcentagem = (Dados_get_qntInstancias(dados)/100)*10; // marcar de 10 em 10 a porcentagem.
    cont = 0; porcentagem = 0;

    printf("\n\nResults: 0%%_"); fflush(stdout);

    for (i = 0; i < Dados_get_qntInstancias(dados); ++i) {
        /* Marcador de porcentagem */
        if (cont > limitePorcentagem){
            cont = 0;
            porcentagem = (float)(i*100)/Dados_get_qntInstancias(dados);
            printf("%.0f%%_", porcentagem); fflush(stdout);
        } else
            ++cont;

        if (Dados_get_ignorarAusentesInfactiveis(dados) == 0 || Dados_get_buscarBdBestSolutions(dados, Dados_get_campoInstancia_index(dados, i), 20, "f") != -1) {
            melhorMedia = INF;
            for (int p = 0; p < Dados_get_variacaoParametros(dados); ++p) {
                media = 0; qnt = 0;
                for (int n = 0; n < Dados_get_qntResultsCamposCalcMediaDesvio(dados); ++n) /* zera o vetor referente a media e desvio dos campos escolhidos para serem exibidos no results */
                    vetMedia[n] = 0;

                for (l = 0; l < Dados_get_qntLinhas(dados); ++l) {
                    chave = 0;
                    if (floatcmp(i, Dados_get_tabDados_index(dados, l, 0)) == 0) {
                        for (h = 0; h < Dados_get_qntParamEntrada(dados); ++h) {
                            if (floatcmp(Dados_get_tabDados_index(dados, l, Dados_get_campoParamEntrada_index(dados, h)), Dados_get_tabConjuntoParametros_index(dados, p, h) ) != 0) {
                                chave = 1;
                                break;
                            }
                        }

                        /* Todos os parametros de vetConjuntoParametros[] batem com os presentes na linha l */
                        if (chave == 0) {
                            if (Dados_get_tipoSolObj(dados) == 's') {
                                media += Dados_get_tabDados_index(dados, l, Dados_get_numCampoSolFinal(dados));
                                ++qnt;
                                referenciaParametro = l;

                                for (int n = 0; n < Dados_get_qntResultsCamposCalcMediaDesvio(dados); ++n)
                                    vetMedia[n] += Dados_get_tabDados_index(dados, l, Dados_get_resultsCamposCalcMediaDesvio_index(dados, n));
                            } else if (Dados_get_tipoSolObj(dados) == 'o') {
                                    media += Dados_get_tabDados_index(dados, l, Dados_get_numCampoDualBoundFinal(dados));
                                    ++qnt;
                                    referenciaParametro = l;

                                    for (int n = 0; n < Dados_get_qntResultsCamposCalcMediaDesvio(dados); ++n)
                                        vetMedia[n] += Dados_get_tabDados_index(dados, l, Dados_get_resultsCamposCalcMediaDesvio_index(dados, n));
                            }

                        }
                    }
                }

                media = (float)media/qnt;

                if (media < melhorMedia) {
                    melhorMedia = media;
                    referenciaMelhorParametro = referenciaParametro;

                    for (int n = 0; n < Dados_get_qntResultsCamposCalcMediaDesvio(dados); ++n)
                        vetMelhorMedia[n] = (float)vetMedia[n]/qnt;
                }
            }

            /* Calculando Desvio */
            melhorMedia = INF;
            for (int p = 0; p < Dados_get_variacaoParametros(dados); ++p) {
                media = 0; qnt = 0;
                for (int n = 0; n < Dados_get_qntResultsCamposCalcMediaDesvio(dados); ++n) /* zera o vetor referente a media e desvio dos campos escolhidos para serem exibidos no results */
                    vetDesvio[n] = 0;

                for (l = 0; l < Dados_get_qntLinhas(dados); ++l) {
                    chave = 0;
                    if (floatcmp(i, Dados_get_tabDados_index(dados, l, 0)) == 0) {
                        for (h = 0; h < Dados_get_qntParamEntrada(dados); ++h) {
                            if (floatcmp(Dados_get_tabDados_index(dados, l, Dados_get_campoParamEntrada_index(dados, h)), Dados_get_tabConjuntoParametros_index(dados, p, h) ) != 0) {
                                chave = 1;
                                break;
                            }
                        }

                        /* Todos os parametros de vetConjuntoParametros[] batem com os presentes na linha l */
                        if (chave == 0) {
                            if (Dados_get_tipoSolObj(dados) == 's') {
                                media += Dados_get_tabDados_index(dados, l, Dados_get_numCampoSolFinal(dados));
                                ++qnt;

                                for (int n = 0; n < Dados_get_qntResultsCamposCalcMediaDesvio(dados); ++n)
                                    vetDesvio[n] += pow(Dados_get_tabDados_index(dados, l, Dados_get_resultsCamposCalcMediaDesvio_index(dados, n)) - vetMelhorMedia[n], 2);

                            } else if (Dados_get_tipoSolObj(dados) == 'o') {
                                media += Dados_get_tabDados_index(dados, l, Dados_get_numCampoDualBoundFinal(dados));
                                ++qnt;

                                for (int n = 0; n < Dados_get_qntResultsCamposCalcMediaDesvio(dados); ++n)
                                    vetDesvio[n] += pow(Dados_get_tabDados_index(dados, l, Dados_get_resultsCamposCalcMediaDesvio_index(dados, n)) - vetMelhorMedia[n], 2);
                            }
                        }
                    }
                }

                media = media/qnt;

                if (media < melhorMedia) {
                    melhorMedia = media;

                    for (int n = 0; n < Dados_get_qntResultsCamposCalcMediaDesvio(dados); ++n)
                        vetMelhorDesvio[n] = (float)sqrt(vetDesvio[n] / (qnt-1));
                }
            }

            /* Encontrando o best literature para substituir no banco de dados*/
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

            /* Encontrando o best cost */
            if (Dados_get_tipoSolObj(dados) == 's') {
                bestCost = INF;
                for (j = 0; j < Dados_get_qntLinhas(dados); ++j) {
                    if (floatcmp(Dados_get_tabDados_index(dados, j, 0), (float)i) == 0) {
                        if (Dados_get_tabDados_index(dados, j, Dados_get_numCampoSolFinal(dados)) < bestCost) {
                            bestCost = Dados_get_tabDados_index(dados, j, Dados_get_numCampoSolFinal(dados));
                            referenciaMelhorCusto = j;
                        }
                    }
                }
            } else if (Dados_get_tipoSolObj(dados) == 'o') {
                bestDualBound = 0.000000;
                for (j = 0; j < Dados_get_qntLinhas(dados); ++j) {
                    if (floatcmp(Dados_get_tabDados_index(dados, j, 0), (float)i) == 0) {
                        float auxFloat;
                        if (ubTPDLinha == 0)
                            auxFloat = 0.000000;
                        else
                            auxFloat = (float)Dados_get_tabDados_index(dados, j, Dados_get_numCampoDualBoundFinal(dados)) / ubTPDLinha;

                        if ((floatcmp(auxFloat, bestDualBound) == 1) || (floatcmp(auxFloat, 0.000000) == 0)) { // 1 = maior; 0 = igual
                            bestDualBound = Dados_get_tabDados_index(dados, j, Dados_get_numCampoDualBoundFinal(dados));
                            referenciaMelhorCusto = j;
                        }
                    }
                }
            }

            if (bestSolutionLinha == -1)
                bestSolutionLinha = INF;

            /* Verificando se foi melhor que a best solution ou best literature */
            if (chave == 1 && Dados_get_tipoSolObj(dados) == 's') {
                if (bestCost < bestSolutionLinha && Dados_get_numCampoNomeSol(dados) != -1) {
                    printf("\n\nNew best solution! %s : %ld < %ld\n", instanciaLinha, bestCost, bestSolutionLinha); fflush(stdout);
                    printf("\nPressione ENTER para continuar..."); fflush(stdout);  getchar();
                    pag_bestSolutions(dados, refTab, instanciaLinha, referenciaMelhorCusto);
                }
            } else if (chave == 1 && Dados_get_tipoSolObj(dados) == 'o') {
                if (ubTPDLinha != 0)
                    if (floatcmp2((float)(bestDualBound / ubTPDLinha), (float)(bestDualBoundLinha / ubTPDLinha), 4) == 1) {
                        printf("\n\nNew best dual bound! %s : %.4f to %.4f (%.4f > %.4f) \n", instanciaLinha, bestDualBoundLinha, bestDualBound, (bestDualBound / ubTPDLinha), (bestDualBoundLinha / ubTPDLinha)); fflush(stdout);
                        printf("\nPressione ENTER para continuar..."); fflush(stdout); getchar();
                        pag_bestSolutions(dados, refTab, instanciaLinha, referenciaMelhorCusto);
                    }
            }

            /* Imprimindo instancia */
            fprintf(pag, "\n<tr bgcolor=%s><td align=\"center\"> %s </td>", cor, Dados_get_campoInstancia_index(dados, i));

            /* Imprimindo media e desvio */
            for (int n = 0; n < Dados_get_qntResultsCamposCalcMediaDesvio(dados); ++n) {
                fprintf(pag, "<td align=\"right\"> %.3f </td>", vetMelhorMedia[n]);
                fprintf(pag, "<td align=\"right\"> %.3f </td>", vetMelhorDesvio[n]);
            }

            /* Imprimindo best cost ou best dual bound */
            if (chave == 1 && Dados_get_tipoSolObj(dados) == 's')
                fprintf(pag, "<td align=\"right\"> %ld </td>", bestCost);
            else if (chave == 1 && Dados_get_tipoSolObj(dados) == 'o')
                fprintf(pag, "<td align=\"right\"> %.6f </td>", bestDualBound);

            /* Imprimindo best da literatura */
            if (chave == 0)  /* nao encontrou best solution (not found) */
                fprintf(pag, "<td align=\"center\"> not found  </td>");
            else {
                if (Dados_get_tipoSolObj(dados) == 's')
                    fprintf(pag, "<td align=\"right\"> %ld </td>", literatureSolutionLinha);
                else if (Dados_get_tipoSolObj(dados) == 'o')
                    fprintf(pag, "<td align=\"right\"> %ld </td>", ubTPDLinha);
            }

            /* Imprimindo B/C */
            if (chave == 0) /* nao encontrou best solution */
                fprintf(pag, "<td align=\"center\"> -  </td>");
            else {
                if (Dados_get_tipoSolObj(dados) == 's') {
                    if ((float)literatureSolutionLinha/bestCost >= 1)
                        fprintf(pag, "<td align=\"right\"> <b> %.3f </b> </td>", (float)literatureSolutionLinha/bestCost);
                    else if ((float)literatureSolutionLinha/bestCost > 0.999 && (float)literatureSolutionLinha/bestCost < 1.000) /* devido ao arredondamento */
                        fprintf(pag, "<td align=\"right\"> %.3f </td>", 0.999);
                    else
                        fprintf(pag, "<td align=\"right\"> %.3f </td>", (float)literatureSolutionLinha/bestCost);
                } else if (Dados_get_tipoSolObj(dados) == 'o') {
                        //if (ubTPDLinha != 0 && bestDualBound != 0)
                        //{
                            if ((bestDualBound / ubTPDLinha) >= 1)
                                fprintf(pag, "<td align=\"right\"> <b> %.3f </b> </td>", (bestDualBound / ubTPDLinha));
                            else if ((bestDualBound / ubTPDLinha) > 0.999 && (bestDualBound / ubTPDLinha) < 1.000) /* devido ao arredondamento */
                                fprintf(pag, "<td align=\"right\"> %.3f </td>", 0.999);
                            else
                                fprintf(pag, "<td align=\"right\"> %.3f </td>", (bestDualBound / ubTPDLinha));
                        //}
                }
            }

            /* Imprimindo os demais campos */
            for (j = 1; j < Dados_get_qntCampos(dados); ++j) {
                if (chkInVet(Dados_get_resultsExibirCampos(dados), Dados_get_qntresultsExibirCampos(dados), j)) {
                    /* verificando se o campo a exibir eh um parametro de entrada (obrigatorio ser) */
                    if (chkInVet(Dados_get_campoParamEntrada(dados), Dados_get_qntParamEntrada(dados), j))
                        if (Dados_get_tipoParamEntradaSaida_index(dados, j) == 'i')
                            fprintf(pag, "<td align=\"right\"> %d </td>", (int)Dados_get_tabDados_index(dados, referenciaMelhorParametro, j));
                        else
                            fprintf(pag, "<td align=\"right\"> %.6f </td>", Dados_get_tabDados_index(dados, referenciaMelhorParametro, j));
                    else
                        printf("\nResultados| campo: %d nao eh um parametro de entrada. Ignorando campo.", j);
                }
            }

            /* Cabecalho: campo download solucao */
            if (Dados_get_numCampoNomeSol(dados) != -1) {
                fprintf(pag, "<td align=\"center\"><a href=\"best_solutions/%s\"> Download </a></td>", Dados_get_campoNomeSolucao_index(dados, referenciaMelhorCusto));

                /* copiando .sol para pasta na pagina */
                strcpy(strAux, "cp ");
                strcat(strAux, Dados_get_caminhoPastaSolucoes(dados));
                strcat(strAux, Dados_get_campoNomeSolucao_index(dados, referenciaMelhorCusto));
                strcat(strAux, " ");
                strcat(strAux, Dados_get_caminho(dados));
                strcat(strAux, "pg/");
                strcat(strAux, Dados_get_nomeAlgoritmo(dados));
                strcat(strAux, "/");
                strcat(strAux, Dados_get_idTeste(dados));
                strcat(strAux, "/best_solutions/");
                system(strAux);
            }

            /* botao (Details)*/
            fprintf(pag, "<td align=\"center\"><for m action=\"details/%s.html\" method=\"get\"><input type=\"submit\" value=\" Details \"/></for m></td></tr>", Dados_get_campoInstancia_index(dados, i));

            pag_resultsDetails(dados, referenciaMelhorParametro, referenciaMelhorCusto);

            if (!strcmp(cor, Dados_get_cor1(dados)))
                strcpy(cor,Dados_get_cor2(dados) );
            else
                strcpy(cor,Dados_get_cor1(dados) );

            /* Verif icando se a solucao foi melhor que a (best solution) ou (best literature) */
            //verificador_bestSolutions(Dados_get_arquivoBestSolutions(dados), Dados_get_campoInstancia_index(dados, i), bestCost);
        }
    }

    /* Copiar ate o final do gabarito */
    prox_tag(gab, pag);

    fclose(gab);
    fclose(pag);

    free(nomeAlgoritmoFiltrado);
    free(nomeCampoInstanceFiltrado);
    free(nomeFiltrado1);
    free(vetMedia);
    free(vetDesvio);
    free(vetMelhorMedia);
    free(vetMelhorDesvio);

    printf("100%%_[ OK ]\n"); fflush(stdout);
}
