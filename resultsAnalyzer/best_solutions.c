#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "best_solutions.h"
#include "util.h"

#define INF 999999999


void pag_bestSolutions(Dados* dados, char* refTab, char* instancia, int referenciaMelhorCusto)
{
    char novoArquivo[1000] = {""}, buffer[10000], strAux[1000], simb = '-', cor[8];
    char algoritmoLinhaSol[100], idTesteLinhaSol[100], algoritmoLinhaObj[100], idTesteLinhaObj[100];
    char caminhoSolLinha[1000], optLinha[10], situacaoInstanciaLinha[10], status;
    int qntLinhasArqBest = 0, chave = 0, nInfact = 0, nExite = 0, i, j, k, idTesteInt;
    float runTimeBestSolLinha, runTimeLitSolLinha, divBC, bestDualBoundLinha, runTimeBestDualBoundLinha;
    double B_C_sol, dual_bound_UB_TPD, rounded_dual_bound_UB_TPD;
    long int bestRoudedDualBoundLinha, literatureSolutionLinha, bestSolutionLinha;
    long int ubTPDLinha, ubTMSLinha, lbTMSLinha, posArq, posArqR05, qntLinhas = 0;

    char **tabBests, **tabBestsCol0;

    strcpy(cor, Dados_get_cor1(dados));

    /* Arquivo de gabarito */
    FILE* gab = fopen("gab/pag_best_solutions.html", "r");

    if (!gab) {
        printf("\nErro ao criar/abrir arquivo:  gab/pag_best_solutions.html");
        exit(1);
    }

    strcat(novoArquivo, Dados_get_caminho(dados));
    strcat(novoArquivo, "pg/bests/");
    strcat(novoArquivo, Dados_get_nomeProblema(dados));
    strcat(novoArquivo, "/");
    strcat(novoArquivo, refTab);
    strcat(novoArquivo, ".html");
    //printf("\nnovoArquivo: |%s|", novoArquivo);

    FILE* pag = fopen(novoArquivo, "w");

    if (!pag) {
        printf("\nErro ao criar/abrir arquivo: %s", novoArquivo);
        exit(1);
    }

    /* Copiando até o ponto [r01]: */
    while (simb != '~') {
        simb = (char)fgetc(gab);
        fputc(simb, pag);
    }

    /* Lendo e escrevendo: "-->" */
    fscanf(gab, "%s", (char*)&strAux);
    fprintf(pag, "%s ", strAux);
    fscanf(gab, "%s", (char*)&strAux);
    fprintf(pag, "%s \n", strAux);

    /* Escrevendo [r00] : Titulo Aba */
    fprintf(pag, "\n<title>Best Solutions for %s</title>", refTab);
    prox_tag(gab, pag);

    /* Escrevendo [r01] : Titulo */
    fprintf(pag, "\nBest Solutions (%s) for %s", Dados_get_nomeProblema(dados), refTab);
    prox_tag(gab, pag);

    /* Escrevendo [r02] : Descricao */
    fprintf(pag, "\nThe best solutions in respect of all analyzes");
    prox_tag(gab, pag);

    /* Escrevendo [r03] : page_back */
    fprintf(pag, "\n<a href=\"../%s.html\">Back</a> - <a href=\"../../../index.html\">Home</a>", Dados_get_nomeProblema(dados));
    prox_tag(gab, pag);

    /* Escrevendo [r04] : Data */
    fprintf(pag, "\n<em> Update: %s </em>", __DATE__);
    fprintf(pag, "\n</br></br></br>");
    fprintf(pag, "\n");

    posArqR05 = ftell(pag); /* Escrevendo: Registered instances, Not found, Infeasible */

    fprintf(pag, "\n                                                                                                                                  ");
    fprintf(pag, "\n                                                                                                                                  ");
    fprintf(pag, "\n                                                                                                                                  ");

    prox_tag(gab, pag);

    /* Escrevendo [r05] : *_all_solutions.zip */
    fprintf(pag, "\n<a style=\"text-align: right; color:#000000;\" href=\"../../files_best_sol/%s/%s_all_solutions.zip\"><b>%s_all_solutions.zip</b></a>\n", Dados_get_nomeProblema(dados), refTab, refTab);
    prox_tag(gab, pag);

    /* Lendo para a memoria: bd_best_solutions_[problema].txt*/
    FILE* arqBestSolution = fopen(Dados_get_arquivoBestSolutions(dados), "r");

    if (!arqBestSolution) {
        printf("\nErro ao criar/abrir arquivo: %s", Dados_get_arquivoBestSolutions(dados));
        exit(1);
    }

    /* Descobrindo quantidade de linhas: bd_best_solutions_[problema].txt*/
    while ((fgets(buffer, sizeof(buffer), arqBestSolution)) != NULL)
        ++qntLinhasArqBest;

    fseek(arqBestSolution, 0, SEEK_SET);

    tabBests = (char**)malloc((unsigned long)qntLinhasArqBest*sizeof(char*));
    tabBestsCol0 = (char**)malloc(qntLinhasArqBest*sizeof(char*));

    for (i = 0; i < qntLinhasArqBest; i++) {
        tabBests[i] = (char*)malloc(10000*sizeof(char));
        tabBestsCol0[i] = (char*)malloc(300*sizeof(char));
    }

    if (strcmp(instancia, "r") != 0) {              /* r = somente refazer as paginas */
        /* Encontrando linha referente a best */
        for (i = 0; i < qntLinhasArqBest; ++i) {
            posArq = ftell(arqBestSolution);
            fscanf(arqBestSolution, "%s ; %s ;", tabBestsCol0[i], strAux);

            if (strcmp(tabBestsCol0[i], "[end]") == 0) {
                sprintf(tabBestsCol0[i], "%s ; %s", tabBestsCol0[i], strAux);
                break;
            }

            if (strcmp(tabBestsCol0[i], instancia) == 0) {
                fseek(arqBestSolution, posArq, SEEK_SET);

                fscanf(arqBestSolution, "%s ;", tabBestsCol0[i]);
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
                fscanf(arqBestSolution, "%lf ; %lf ; %lf ;", &B_C_sol, &dual_bound_UB_TPD, &rounded_dual_bound_UB_TPD);
                fscanf(arqBestSolution, "%s %s ;", algoritmoLinhaSol, idTesteLinhaSol);
                fscanf(arqBestSolution, "%s %s ;", algoritmoLinhaObj, idTesteLinhaObj);
                fscanf(arqBestSolution, "%s ;", caminhoSolLinha);
                fscanf(arqBestSolution, "%s ;", situacaoInstanciaLinha); // f, i ou -
            } else {
                fgets(tabBests[i], 10000*sizeof(char), arqBestSolution);
                sprintf(tabBestsCol0[i], "%s ; %s", tabBestsCol0[i], strAux);
            }
        }
    }
    fclose(arqBestSolution);

    /* Quando for soh reconstruir a pagina, instancia = "r" */
    if  (strcmp(instancia, "r") != 0) {

        /* Reescrevendo arquivo bd_bests_solutions_[problema] com a melhor solucao encontrada */
        FILE* arqBestSolutionNew = fopen(Dados_get_arquivoBestSolutions(dados), "w");

        if (!arqBestSolutionNew) {
            printf("\nErro ao criar/abrir arquivo: %s", Dados_get_arquivoBestSolutions(dados));
            exit(1);
        }

        for (i = 0; i < qntLinhasArqBest-1; ++i) {
            /* Reescrevendo linha com a nova solucao encontrada */
            if  (strcmp(tabBestsCol0[i], instancia) == 0) {
                if  (Dados_get_tipoSolObj(dados) == 's') {       /* solution */
                    long int newBestCost = Dados_get_tabDados_index(dados, referenciaMelhorCusto, Dados_get_numCampoSolFinal(dados));                                                                                            //tabBestsCol0[i], bestSolutio, runTimeBestSolLinha                                                                       , bestDualBoundLinha, bestRoudedDualBoundLinha, runTimeBestDualBoundLinha, literatureSolutionLinha, runTimeLitSolLinha, ubTPDLinha, ubTMSLinha, lbTMSLinha, optLinha, (float)literatureSolutionLinha/newBestCost, dual_bound_UB_TPD, rounded_dual_bound_UB_TPD, algoritmoLinhaSol               , idTesteLinhaSol           , algoritmoLinhaObj, idTesteLinhaObj, caminhoSolLinha, situacaoInstanciaLinha
                    fprintf(arqBestSolutionNew, "%s ; %ld ; %.3f ; %.4f ; %ld ; %.3f ; %ld ; %.3f ; %ld ; %ld ; %ld ; %s ; %.3f ; %.3f ; %.3f ; %s %s ; %s %s ; %s ; %s ;\n", tabBestsCol0[i], newBestCost, Dados_get_tabDados_index(dados, referenciaMelhorCusto, Dados_get_numCampoTempo(dados)), bestDualBoundLinha, bestRoudedDualBoundLinha, runTimeBestDualBoundLinha, literatureSolutionLinha, runTimeLitSolLinha, ubTPDLinha, ubTMSLinha, lbTMSLinha, optLinha, (float)literatureSolutionLinha/newBestCost, dual_bound_UB_TPD, rounded_dual_bound_UB_TPD, Dados_get_nomeAlgoritmo(dados), Dados_get_idTeste(dados), algoritmoLinhaObj, idTesteLinhaObj, caminhoSolLinha, situacaoInstanciaLinha);
                } else {                                        /* dual bound */
                    double newBestDualBound = Dados_get_tabDados_index(dados, referenciaMelhorCusto, Dados_get_numCampoDualBoundFinal(dados));
                    long int newbestRoudedDualBound = ceil(newBestDualBound); // arredondar para cima
                    fprintf(arqBestSolutionNew, "%s ; %ld ; %.3f ; %.4f ; %ld ; %.3f ; %ld ; %.3f ; %ld ; %ld ; %ld ; %s ; %.3f ; %.3f ; %.3f ; %s %s ; %s %s ; %s ; %s ;\n", tabBestsCol0[i], bestSolutionLinha, runTimeBestSolLinha, newBestDualBound, newbestRoudedDualBound, Dados_get_tabDados_index(dados, referenciaMelhorCusto, Dados_get_numCampoTempo(dados)), literatureSolutionLinha, runTimeLitSolLinha, ubTPDLinha, ubTMSLinha, lbTMSLinha, optLinha, B_C_sol, (double)newBestDualBound/ubTPDLinha, (double)newbestRoudedDualBound/ubTPDLinha, algoritmoLinhaSol, idTesteLinhaSol, Dados_get_nomeAlgoritmo(dados), Dados_get_idTeste(dados), caminhoSolLinha, situacaoInstanciaLinha);
                }
            } else
                fprintf(arqBestSolutionNew, "%s ;%s", tabBestsCol0[i], tabBests[i]);
        }
        fprintf(arqBestSolutionNew, "[end] ; 0 ;\n");
        fclose(arqBestSolutionNew);
    }

    /* Escrevendo [r06] : Dados */
    FILE* arqBestSolutionLeitura = fopen(Dados_get_arquivoBestSolutions(dados), "r");

    if (!arqBestSolutionLeitura) {
        printf("\nErro ao criar/abrir arquivo: %s", Dados_get_arquivoBestSolutions(dados));
        exit(1);
    }

    /* Copiando conteudo de bd_best_solutions_[nomeProblema].txt para a pagina */
    /* Obs.: No campo [literature obj]: -1 = Infactivel : -2 = Nao existe : ??? conferir isso */
    chave = 0;
    for (i = 0; i < qntLinhasArqBest; ++i) {
        fscanf(arqBestSolutionLeitura, "%s ; %ld ;", strAux, &qntLinhas);
        if (strcmp(strAux, refTab) == 0) {
            chave = 1;
            for (j = 0; j < qntLinhas; ++j) {
                posArq = ftell(arqBestSolutionLeitura);

                /* pulando para o final */
                for (k = 0; k < 15; ++k)
                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);

                /* pulando para o final */
                fscanf(arqBestSolutionLeitura, "%s %s ; %s %s ; ", strAux, strAux, strAux, strAux);
                fscanf(arqBestSolutionLeitura, "%s ; ", strAux);
                fscanf(arqBestSolutionLeitura, "%c ; ", &status);

                fseek(arqBestSolutionLeitura, posArq, SEEK_SET);
                fscanf(arqBestSolutionLeitura, "%s ; ", strAux);

                if (status == 'i') {    /* Infactivel */
                    ++nInfact;
                    fprintf(pag, "\n<tr bgcolor=%s style=\"color:%s;\"><td align=\"center\">&nbsp %s &nbsp</td>", cor, Dados_get_corInfeasible(dados), strAux);
                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);

                    if (!strcmp(cor, Dados_get_cor1(dados)))
                        strcpy(cor,Dados_get_cor2(dados));
                    else
                        strcpy(cor,Dados_get_cor1(dados));

                    fprintf(pag, "<td align=\"center\">&nbsp infeasible &nbsp</td>");

                    for (k = 1; k < 14; ++k) {
                        fscanf(arqBestSolutionLeitura, "%s ; ", strAux);

                        if  (k == 10) /* Campos para centralizar: (opt) */
                            fprintf(pag, "<td align=\"center\"> - </td>");
                        else
                            fprintf(pag, "<td align=\"right\"> -</td>");
                    }

                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);

                    /* Referencia do melhor teste (solution) */
                    fscanf(arqBestSolutionLeitura, "%s %s ; ", algoritmoLinhaSol, idTesteLinhaSol);
                    fscanf(arqBestSolutionLeitura, "%s %s ; ", algoritmoLinhaSol, idTesteLinhaSol);

                    fprintf(pag, "<td align=\"center\"> - </td>");
                    fprintf(pag, "<td align=\"center\"> - </td>");
                } else if (status == '-') {     /* nao existe */
                    ++nExite;
                    fprintf(pag, "\n<tr bgcolor=%s style=\"color:%s;\"><td align=\"center\">&nbsp %s &nbsp</td>", cor, Dados_get_corNotFound(dados), strAux);

                    if  (!strcmp(cor, Dados_get_cor1(dados)))
                        strcpy(cor,Dados_get_cor2(dados));
                    else
                        strcpy(cor,Dados_get_cor1(dados));

                    fprintf(pag, "<td align=\"center\">&nbsp not found &nbsp</td>");

                    for (k = 1; k < 14; ++k) {
                        fscanf(arqBestSolutionLeitura, "%s ; ", strAux);

                        if  (k == 10) /* Capos para centralizar: (opt) */
                            fprintf(pag, "<td align=\"center\"> - </td>");
                        else
                            fprintf(pag, "<td align=\"right\"> -</td>");
                    }

                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);

                    /* Referencia test (solution) */
                    fscanf(arqBestSolutionLeitura, "%s %s ; ", algoritmoLinhaSol, idTesteLinhaSol);
                    fscanf(arqBestSolutionLeitura, "%s %s ; ", algoritmoLinhaSol, idTesteLinhaSol);

                    fprintf(pag, "<td align=\"center\"> - </td>");
                    fprintf(pag, "<td align=\"center\"> - </td>");
                } else {    /* f : factivel */
                    fprintf(pag, "\n<tr bgcolor=\"%s\"><td align=\"center\" width=\"94px\" height=\"12px\"> %s </td>", cor, strAux);

                    /* best solution */
                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);
                    if (strcmp(strAux, "-3") == 0)
                        fprintf(pag, "<td align=\"right\" width=\"78px\"> FAILED </td>");
                    else
                        fprintf(pag, "<td align=\"right\" width=\"78px\"> %s </td>", BResults_filtroChar(strAux));

                    /* run time best sol. */
                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);
                    fprintf(pag, "<td align=\"right\" width=\"75px\"> %s </td>", BResults_filtroChar(strAux));

                    /* dual bound */
                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);
                    fprintf(pag, "<td align=\"right\" width=\"78px\"> %s </td>", BResults_filtroChar(strAux));

                    /* rounded dual bound */
                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);
                    fprintf(pag, "<td align=\"right\" width=\"78px\"> %s </td>", BResults_filtroChar(strAux));

                    /* run time</br>dual bound */
                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);
                    fprintf(pag, "<td align=\"right\" width=\"75px\"> %s </td>", BResults_filtroChar(strAux));

                    /* literature solution */
                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);
                    fprintf(pag, "<td align=\"right\" width=\"78px\"> %s </td>", BResults_filtroChar(strAux));

                    /* run time lit. sol. */
                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);
                    fprintf(pag, "<td align=\"right\" width=\"75px\"> %s </td>", BResults_filtroChar(strAux));

                    /* UB TPD */
                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);
                    fprintf(pag, "<td align=\"right\" width=\"43px\"> %s </td>", BResults_filtroChar(strAux));

                    /* UB TMS */
                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);
                    fprintf(pag, "<td align=\"right\" width=\"43px\"> %s </td>", BResults_filtroChar(strAux));

                    /* LB TMS */
                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);
                    fprintf(pag, "<td align=\"right\" width=\"43px\"> %s </td>", BResults_filtroChar(strAux));

                    /* Campo opt. */
                    fscanf(arqBestSolutionLeitura, "%s ; ", strAux);
                    fprintf(pag, "<td align=\"center\" width=\"36px\"> %s </td>", BResults_filtroChar(strAux));

                    if  (!strcmp(cor, Dados_get_cor1(dados)))
                        strcpy(cor,Dados_get_cor2(dados));
                    else
                        strcpy(cor,Dados_get_cor1(dados));

                    /* B/C */
                    for (k = 0; k < 3; ++k) {
                        fscanf(arqBestSolutionLeitura, "%f ; ", &divBC);
                        if  (divBC < 0.000)
                            fprintf(pag, "<td align=\"right\" width=\"54px\"> - </td>");
                        else if (divBC >= 1)
                            fprintf(pag, "<td align=\"right\" width=\"54px\"> <b>%.3f</b> </td>", divBC);
                        else if (divBC > 0.999 && divBC < 1.000) /* correcao devido ao arredondamento */
                            fprintf(pag, "<td align=\"right\" width=\"54px\"> %.3f </td>", 0.999);
                        else
                            fprintf(pag, "<td align=\"right\" width=\"54px\"> %.3f </td>", divBC);
                    }

                    /* Referencia melhor teste (solution) */
                    fscanf(arqBestSolutionLeitura, "%s %s ; ", algoritmoLinhaSol, idTesteLinhaSol);

                    idTesteInt = atoi(idTesteLinhaSol);

                    if (idTesteInt <= 152 && strcmp( Dados_get_nomeAlgoritmo(dados), "lslahc") == 0)  /* Para ser compativel com as paginas antigas do lslahc */
                        fprintf(pag, "<td align=\"center\" width=\"62px\"><a href=\"../../lslahc/%.4d/test%.3d.html\"> %c%s </a></td>", idTesteInt, idTesteInt, letraMaiuscula(algoritmoLinhaSol, 0), idTesteLinhaSol);
                    else
                        fprintf(pag, "<td align=\"center\" width=\"62px\"><a href=\"../../%s/%s/results.html\"> %c%s </a></td>", algoritmoLinhaSol, idTesteLinhaSol, letraMaiuscula(algoritmoLinhaSol, 0), idTesteLinhaSol);

                    /* Referencia melhor teste (obj) */
                    fscanf(arqBestSolutionLeitura, "%s %s ; ", algoritmoLinhaObj, idTesteLinhaObj);
                    fprintf(pag, "<td align=\"center\" width=\"62px\"><a href=\"../../%s/%s/results.html\"> %c%s </a></td>", algoritmoLinhaObj, idTesteLinhaObj, letraMaiuscula(algoritmoLinhaObj, 0), idTesteLinhaObj);
                }

                /* Caminho do *.sol */
                fscanf(arqBestSolutionLeitura, "%s ;", caminhoSolLinha);

                /* status : i, f, ou - */
                fscanf(arqBestSolutionLeitura, "%s ; ", strAux);
                fprintf(pag, "<td align=\"center\" width=\"38px\"><a download href=\"../../files_best_sol/%s/%s\"> <img width=\"32px\" height=\"15px\" src=\"../../_img/simb_down_alfa.png\"/> </a> </td></tr>\n", Dados_get_nomeProblema(dados), caminhoSolLinha);
            }
        } else
            fgets(buffer, sizeof(buffer), arqBestSolutionLeitura);

        if (chave == 1)
            break;
    }

    prox_tag(gab, pag);

    /* Escrevendo [r07] : page_back final da pagina */
    fprintf(pag, "\n<a href=\"../%s.html\">Back</a> - <a href=\"../../../index.html\">Home</a>\n", Dados_get_nomeProblema(dados));

    fclose(arqBestSolutionLeitura);
    posArq = ftell(pag);

    /* Escrevendo [r05] : Registered instances, Not found e Infeasible */
    /* Gravando quantidade de nao encontrados e infactiveis */
    fseek(pag, posArqR05, SEEK_SET); /* Voltando no local para gravar */

    fprintf(pag, "\n<b style=\"text-align:left; \">Registered instances: </b> %ld </p>", qntLinhas);
    fprintf(pag, "\n<b style=\"text-align:left; color:%s;\">Not found: </b> %d </p>", Dados_get_corNotFound(dados), nExite);
    fprintf(pag, "\n<b style=\"text-align:left; color:%s;\">Infeasible: </b> %d</p></br>", Dados_get_corInfeasible(dados), nInfact);

    fseek(pag, posArq, SEEK_SET);

    /* Copiando arquivo *.sol para a pasta files_best_sol/... */
    //system("cd $USERNAM");  /* soh para LINUX */

    if (strcmp(instancia, "r") != 0 && Dados_get_tipoSolObj(dados) == 's') {
        /* copiando *.sol para pasta das best solutions */
        strcpy(buffer, "cp ");
        strcat(buffer, Dados_get_caminhoPastaSolucoes(dados));
        strcat(buffer, Dados_get_campoNomeSolucao_index(dados, referenciaMelhorCusto));
        strcat(buffer, " ");
        strcat(buffer, Dados_get_caminho(dados));
        strcat(buffer, "pg/files_best_sol/");
        strcat(buffer, Dados_get_nomeProblema(dados));
        strcat(buffer, "/");
        strcat(buffer, instancia);
        strcat(buffer, "_best.sol");

        system(buffer);
    }

    prox_tag(gab, pag);

    for (i = 0; i < qntLinhasArqBest; i++) {
        free(tabBests[i]);
        free(tabBestsCol0[i]);
    }

    free(tabBests);
    free(tabBestsCol0);

    fclose(gab);
    fclose(pag);
}

void BResults_reconstruirPagina(Dados* dados, char* nomeProblema)
{
    char arquivo[1000], buffer[10000], refTab[300], auxNomeProblema[300], auxArquivoBestSolutions[500];
    int qntLinhas, i;

    printf("\nReconstruido bests: %s", nomeProblema);

    strcpy(arquivo, Dados_get_caminho(dados));
    strcat(arquivo, "pg/bests/bd_best_solutions_");
    strcat(arquivo, nomeProblema);
    strcat(arquivo, ".txt");

    strcpy(auxNomeProblema, Dados_get_nomeProblema(dados));
    Dados_set_nomeProblema(dados, nomeProblema);

    strcpy(auxArquivoBestSolutions, Dados_get_arquivoBestSolutions(dados));
    Dados_set_arquivoBestSolutions(dados, arquivo);

    FILE* arqBestSolution = fopen(arquivo, "r");

    if (!arqBestSolution) {
        printf("\nErro ao criar/abrir arquivo: %s", arquivo);
        exit(1);
    }

    fscanf(arqBestSolution, "%s ; %d", refTab, &qntLinhas);
    fgets(buffer, sizeof(buffer), arqBestSolution);

    while (strcmp(refTab, "[end]") != 0) {
        pag_bestSolutions(dados, refTab, "r", 0);                  /* r = refazer as paginas */

        for (i = 0; i < qntLinhas; ++i)
            fgets(buffer, sizeof(buffer), arqBestSolution);

        fscanf(arqBestSolution, "%s ; %d", refTab, &qntLinhas);
        fgets(buffer, sizeof(buffer), arqBestSolution);
    }

    fclose(arqBestSolution);

    Dados_set_nomeProblema(dados, auxNomeProblema);
    Dados_set_arquivoBestSolutions(dados, auxArquivoBestSolutions);
}

char* BResults_filtroInt(char* str, long int num)
{
    /* Filtro: Caso receba -1, retorna "-" */
    if (num == -1) {
        strcpy(str, "-");
        return str;
    }
    sprintf(str, "%ld", num);
    return str;
}

char* BResults_filtroFloat(char* str, float num)
{
    /* Filtro: Caso receba -1, retorna "-" */
    if (floatcmp(num, -1) == 0) {
        strcpy(str, "-");
        return str;
    }
    sprintf(str, "%.3f", num);
    return str;
}

char* BResults_filtroChar(char* str)
{
    /* Filtro: Caso receba -1, retorna "-" */
    if (strcmp(str, "-1") == 0 || strcmp(str, "-1.0") == 0 || strcmp(str, "-1.00") == 0 || strcmp(str, "-1.000") == 0 || strcmp(str, "-1.0000") == 0 || strcmp(str, "-1.00000") == 0 || strcmp(str, "-1.000000") == 0) {
        strcpy(str, "-");
        return str;
    }
    return str;
}

void substituir_pagBestSolutions(Dados* dados)
{
    char algoritmoLinhaSol[100], idTesteLinhaSol[100], algoritmoLinhaObj[100], idTesteLinhaObj[100], refTab[300];
    char caminhoSolLinha[1000], optLinha[10], instanciaLinha[100], situacaoInstanciaLinha[10];
    int ref;
    float runTimeBestSolLinha, runTimeLitSolLinha, bestDualBoundLinha, runTimeBestDualBoundLinha;
    float B_C_sol, dual_bound_UB_TPD, rounded_dual_bound_UB_TPD;
    long int bestRoudedDualBoundLinha, literatureSolutionLinha, bestSolutionLinha;
    long int ubTPDLinha, ubTMSLinha, lbTMSLinha, qntLinhas = 0;

    FILE* arqNewBestSolution = fopen("bd_best.txt", "w");
    FILE* arqBestSolution = fopen(Dados_get_arquivoBestSolutions(dados), "r");

    if (!arqBestSolution) {
        printf("\nErro ao criar/abrir arquivo:  %s", Dados_get_arquivoBestSolutions(dados));
        exit(1);
    }

    strcpy(refTab, "-");
    ref = 0;

    while (strcmp(refTab, "[end]") != 0) {
        fscanf(arqBestSolution, "%s ; %ld ;", refTab, &qntLinhas);
        fprintf(arqNewBestSolution, "%s ; %ld ;\n", refTab, qntLinhas);

        for (int l = 0; l < qntLinhas; ++l) {

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
            fscanf(arqBestSolution, "%f ; %f ; %f ;", &B_C_sol, &dual_bound_UB_TPD, &rounded_dual_bound_UB_TPD);
            fscanf(arqBestSolution, "%s %s ;", algoritmoLinhaSol, idTesteLinhaSol);
            fscanf(arqBestSolution, "%s %s ;", algoritmoLinhaObj, idTesteLinhaObj);
            fscanf(arqBestSolution, "%s ;", caminhoSolLinha);
            fscanf(arqBestSolution, "%s ;", situacaoInstanciaLinha); // f, i ou -

            ref = Dados_get_buscarCampoInstancia(dados, instanciaLinha);
            if (ref != -1) {
                /* (8) ubTPDLinha */
                if (Dados_get_numCampoSubstituir_index(dados, 1) == 8) {
                    ubTPDLinha = (long int)Dados_get_tabDados_index(dados, ref, Dados_get_numCampoSubstituir_index(dados, 0));

                    if (floatcmp(bestDualBoundLinha, -1.000) == 0)
                        fprintf(arqNewBestSolution, "%s ; %ld ; %.3f ; %.4f ; %ld ; %.3f ; %ld ; %.3f ; %ld ; %ld ; %ld ; %s ; %.3f ; %.3f ; %.3f ; %s %s ; %s %s ; %s ; %s ;\n", instanciaLinha, bestSolutionLinha, runTimeBestSolLinha, bestDualBoundLinha, bestRoudedDualBoundLinha, runTimeBestDualBoundLinha, literatureSolutionLinha, runTimeLitSolLinha, ubTPDLinha, ubTMSLinha, lbTMSLinha, optLinha, B_C_sol, -1.000, -1.000, algoritmoLinhaSol, idTesteLinhaSol, algoritmoLinhaObj, idTesteLinhaObj, caminhoSolLinha, situacaoInstanciaLinha);
                    else
                        fprintf(arqNewBestSolution, "%s ; %ld ; %.3f ; %.4f ; %ld ; %.3f ; %ld ; %.3f ; %ld ; %ld ; %ld ; %s ; %.3f ; %.3f ; %.3f ; %s %s ; %s %s ; %s ; %s ;\n", instanciaLinha, bestSolutionLinha, runTimeBestSolLinha, bestDualBoundLinha, bestRoudedDualBoundLinha, runTimeBestDualBoundLinha, literatureSolutionLinha, runTimeLitSolLinha, ubTPDLinha, ubTMSLinha, lbTMSLinha, optLinha, B_C_sol, (float)bestDualBoundLinha/ubTPDLinha, (float)bestRoudedDualBoundLinha/ubTPDLinha, algoritmoLinhaSol, idTesteLinhaSol, algoritmoLinhaObj, idTesteLinhaObj, caminhoSolLinha, situacaoInstanciaLinha);
                } else
                    printf("\nNao implementado. best_solutions.c | (%d) %s \n", Dados_get_numCampoSubstituir_index(dados, 1), Dados_get_nomeCampos_index(dados, Dados_get_numCampoSubstituir_index(dados, 0))); getchar();
            } else
                fprintf(arqNewBestSolution, "%s ; %ld ; %.3f ; %.4f ; %ld ; %.3f ; %ld ; %.3f ; %ld ; %ld ; %ld ; %s ; %.3f ; %.3f ; %.3f ; %s %s ; %s %s ; %s ; %s ;\n", instanciaLinha, bestSolutionLinha, runTimeBestSolLinha, bestDualBoundLinha, bestRoudedDualBoundLinha, runTimeBestDualBoundLinha, literatureSolutionLinha, runTimeLitSolLinha, ubTPDLinha, ubTMSLinha, lbTMSLinha, optLinha, B_C_sol, dual_bound_UB_TPD, rounded_dual_bound_UB_TPD, algoritmoLinhaSol, idTesteLinhaSol, algoritmoLinhaObj, idTesteLinhaObj, caminhoSolLinha, situacaoInstanciaLinha);
        }
    }
    fclose(arqBestSolution);
    fclose(arqNewBestSolution);
}
