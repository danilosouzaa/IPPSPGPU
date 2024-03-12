#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dados.h"
#include "best_solutions.h"
#include "util.h"
#include "index.h"

#define INF 999999999


struct _dados {
    int tamCabecalho, numCampoInstancia, qntInstancias, variacaoParametros, qntParametrosUtilizados;
    int numCampoNumSol, numCampoSolInicial, numCampoSolFinal, numCampoDualBoundFinal, numCampoTempo;
    int qntSolIniciais, qntCampos, qntLinhas, qntMaxParametros, numCampoCalcQuality, numCampoNomeSol;
    int qntParamEntrada, qntParamSaida, qntCamposIgnorar, qntAnalysisExibirCampos, numCampoSubstituir[2];
    int qntResultsCamposCalcMediaDesvio, qntresultsExibirCampos, qntResultsDetailsExibirCampos;
    int qntBdBestSolutions, ignorarAusentesInfactiveis;

    float melhorMediaQuality, melhorDesvioQuality;

    char nomeAlgoritmo[100], idTeste[30], nomeArquivo[100], caminho[300], arquivoBestSolutions[300];
    char summaryStartDate[11], summaryEndDate[11], summaryAbout[300], descricao[2000], caminhoPastaSolucoes[300];
    char nomeProblema[100], tipoSolObj, cor1[8], cor2[8], corNotFound[8], corInfeasible[8];
    char *tipoParamEntrada, *tipoParamEntradaSaida;
    char **campoInstancia, **nomeCampos, **campoNomeSolucao;
    int *campoParamEntrada, *campoParamSaida, *camposIgnorar, *resultsCamposCalcMediaDesvio, *resultsExibirCampos;
    int *resultsDetailsExibirCampos, *analysisExibirCampos;
    float **tabDados, **tabParametros;

    // testes com double. Corrigir erros
    double **tabConjuntoParametros;

    char ***bdBestSolutions;

    /* para valores temporarios*/
    float media, desvio;
    char nomePagina[1000];
};

Dados* Dados_create(char** argv, char *file_in)
{
    char c, strAux[100], strFlag[100], arquivo[1000], buffer[10000];
    int chave, cont, atrib, pos = 0, k = 0, intAux1, h, i, j, resp, qnt = 0, indexAux = 0;

    if (fileExists(file_in) == 0) {
        printf("\nArquivo %s nao foi encontrado. \n\n", file_in);
        exit(1);
    }

    FILE *cfg = fopen(file_in, "r");
    FILE *arq;

    Dados* dados = (Dados*)malloc(sizeof(Dados));

    strcpy(dados->caminho, "/home/davi/git/gpsp/ptests/");
    dados->numCampoInstancia = 0;               /* Padrao */
    dados->ignorarAusentesInfactiveis = 1;      /* 0 = nao, 1 = sim | Ignorar linhas com instancias ausentes ou infactiveis */

    /* Cor da linha das tabelhas (Zebrado) */
    strcpy(dados->cor1, "#FFFFFF");             /* Branco */
    strcpy(dados->cor2, "#DDEDF8");             /* Azul claro */

    strcpy(dados->corNotFound, "#0000DD");      /* Azul */
    strcpy(dados->corInfeasible, "#DD0000");    /* Vermelho */

    /* [arquivo_resultados] */
    fscanf(cfg, "%s = %s ;", strFlag, dados->nomeArquivo);

    if (fileExists(dados->nomeArquivo) == 0) {
        printf("\nArquivo %s nao foi encontrado.  \n\n", dados->nomeArquivo);
        exit(1);
    }

    arq = fopen(dados->nomeArquivo, "r");
    dados->qntCampos = 0;

    /* verif icando quantidade de campos tem uma linha */
    c = ' ';
    while (c != '\n') {
        c = (char)getc(arq);
        if (c == ';')
            ++dados->qntCampos;
    }

    fseek(arq, 0, SEEK_SET);

    dados->campoParamEntrada = (int*)malloc(dados->qntCampos*sizeof(int));
    dados->campoParamSaida = (int*)malloc(dados->qntCampos*sizeof(int));
    dados->camposIgnorar = (int*)malloc(dados->qntCampos*sizeof(int));
    dados->resultsCamposCalcMediaDesvio = (int*)malloc(dados->qntCampos*sizeof(int));
    dados->resultsExibirCampos = (int*)malloc(dados->qntCampos*sizeof(int));
    dados->resultsDetailsExibirCampos = (int*)malloc(dados->qntCampos*sizeof(int));
    dados->analysisExibirCampos = (int*)malloc(dados->qntCampos*sizeof(int));

    /* [nome_do_algoritmo] */
    fscanf(cfg, "%s = %s ;", strFlag, dados->nomeAlgoritmo);

    /* [nome_do_problema] */
    fscanf(cfg, "%s = %s ;", strFlag, dados->nomeProblema);

    /* Lendo idTeste: bd_index_[algoritmo].txt*/
    strcpy(arquivo, Dados_get_caminho(dados));
    strcat(arquivo, "pg/");
    strcat(arquivo, Dados_get_nomeAlgoritmo(dados));
    strcat(arquivo, "/bd_index_");
    strcat(arquivo, Dados_get_nomeAlgoritmo(dados));
    strcat(arquivo, ".txt");

    FILE* arqBdIndex = fopen(arquivo, "r");

    if (!arqBdIndex) {
        printf("\nErro ao criar/abrir arquivo: %s", arquivo);
        exit(1);
    }

    fscanf(arqBdIndex, "%d", &intAux1);
    sprintf(dados->idTeste, "%.4d", intAux1);

    fclose(arqBdIndex);

    dados->nomeCampos = (char**)malloc(dados->qntCampos*sizeof(char*));
    for (i = 0; i < dados->qntCampos; i++)
        dados->nomeCampos[i] = (char*)malloc(50*sizeof(char));

    /* [nome_dos_campos] */
    fscanf(cfg, "%s %s", strFlag, strAux);
    for (i = 0; i < dados->qntCampos; ++i)
        fscanf(cfg, "%s %s", dados->nomeCampos[i], strAux);

    /* [descricao] */
    fscanf(cfg, "%s %s", strFlag, strAux);
    c = ' '; cont = 0;
    while (c != ';')
    {
        c = (char)getc(cfg);
        dados->descricao[cont] = c;
        ++cont;
    } dados->descricao[cont-1] = '\0';

    /* [caminho_pasta_solucoes] */
    fscanf(cfg, "%s = %s ;", strFlag, dados->caminhoPastaSolucoes);

    /* [campo_nome_sol] */
    fscanf(cfg, "%s = %d ;", strFlag, &dados->numCampoNomeSol);

    /* [campo_run_time] */
    fscanf(cfg, "%s = %d ;", strFlag, &dados->numCampoTempo);

    /* [campos_parametros_de_entrada] */
    dados->qntParamEntrada = 0;
    fscanf(cfg, "%s %s", strFlag, strAux);
    while (strcmp(strAux, ";") != 0) {
        fscanf(cfg, "%d %s", &dados->campoParamEntrada[dados->qntParamEntrada], strAux);
        /* pula o campo da instancia*/
        if (dados->campoParamEntrada[dados->qntParamEntrada] != 0)
            ++dados->qntParamEntrada;
    }

    cont = 0;
    for (i = 0; i < dados->qntCampos; ++i) {
        chave = 0;
        for (j = 0; j < dados->qntParamEntrada; ++j)
            if (i == dados->campoParamEntrada[j])
                chave = 1;

        if (chave == 0) {
            dados->campoParamSaida[cont] = i;
            ++cont;
        }
    }

    /* [campos_ignorar] */
    dados->qntCamposIgnorar = 0;
    fscanf(cfg, "%s %s", strFlag, strAux);
    while (strcmp(strAux, ";") != 0) {
        fscanf(cfg, "%d %s", &dados->camposIgnorar[dados->qntCamposIgnorar], strAux);
        if (dados->camposIgnorar[0] == -1)
            break;
        ++dados->qntCamposIgnorar;
    }

    if (dados->numCampoNomeSol != -1) {      /* Ignora campo nome da solucao */
        dados->camposIgnorar[dados->qntCamposIgnorar] = dados->numCampoNomeSol;
        ++dados->qntCamposIgnorar;
    }

    /* [campo_dual_bound_final] */
    fscanf(cfg, "%s = %d ;", strFlag, &dados->numCampoDualBoundFinal);

    /* [campo_sol_final_FO] */
    fscanf(cfg, "%s = %d ;", strFlag, &dados->numCampoSolFinal);

    /* [campo_sol_inicial_FO] */
    fscanf(cfg, "%s = %d ;", strFlag, &dados->numCampoSolInicial);

    /* [campo_calc_quality] */
    fscanf(cfg, "%s = %d ;", strFlag, &dados->numCampoCalcQuality);

    /* [campo_substituir_orig_dest] */
    fscanf(cfg, "%s = %d , %d ;", strFlag, &dados->numCampoSubstituir[0], &dados->numCampoSubstituir[1]);

    /* #_PAGINA_SUMARIO */
    fscanf(cfg, "%s", strAux);

    /* [pag_summary_start_date] */
    //if (verif icaVazioConfig(cfg) == -1)  // terminar de implementar
    //    strcpy(dados->summaryStartDate, "-1");
    //else
    fscanf(cfg, "%s = %s ;", strFlag, dados->summaryStartDate);

    /* [pag_summary_end_date] */
    fscanf(cfg, "%s = %s ;", strFlag, dados->summaryEndDate);

    /* #_PAGINA_RESULTS */
    fscanf(cfg, "%s", strAux);

    /* [pag_results_campos_calc_media_e_desvio] */
    dados->qntResultsCamposCalcMediaDesvio = 0;
    fscanf(cfg, "%s %s", strFlag, strAux);

    while (strcmp(strAux, ";") != 0) {
        fscanf(cfg, "%d %s", &dados->resultsCamposCalcMediaDesvio[dados->qntResultsCamposCalcMediaDesvio], strAux);
        if (dados->resultsCamposCalcMediaDesvio[0] == -1)
            break;
        ++dados->qntResultsCamposCalcMediaDesvio;
    }

    /* [pag_results_exibir_campos] */
    dados->qntresultsExibirCampos = 0;
    fscanf(cfg, "%s %s", strFlag, strAux);

    while (strcmp(strAux, ";") != 0) {
        fscanf(cfg, "%d %s", &dados->resultsExibirCampos[dados->qntresultsExibirCampos], strAux);
        if (dados->resultsExibirCampos[0] == -1)
            break;
        ++dados->qntresultsExibirCampos;
    }

    /* Fim da leitura do arquivo: results.txt*/

    /* Construindo caminho para o banco de dados com as best solutions*/
    strcpy(dados->arquivoBestSolutions, dados->caminho);
    strcat(dados->arquivoBestSolutions, "pg/bests/bd_best_solutions_");
    strcat(dados->arquivoBestSolutions, Dados_get_nomeProblema(dados));
    strcat(dados->arquivoBestSolutions, ".txt");

    /* Verif icando se o teste eh do tipo s = solution ou o = obj*/
    if (Dados_get_numCampoCalcQuality(dados) == Dados_get_numCampoSolFinal(dados)) /* best solution */
        dados->tipoSolObj = 's'; /* s = solution*/
    else
        dados->tipoSolObj = 'o'; /* o = obj*/

    /* Trazendo results para a memoria */
    dados->qntLinhas = 0;
    while (c != EOF) {
        c = getc(arq);
        if (c == ';')
            ++dados->qntLinhas;
    }

    dados->qntLinhas = dados->qntLinhas/dados->qntCampos; /* Como vai contar TODOS os ";" entao divide pela ant de colunas (campos) corrigir */
    dados->campoNomeSolucao = (char**)malloc(dados->qntLinhas*sizeof(char*));

    for (i = 0; i < dados->qntLinhas; i++)
        dados->campoNomeSolucao[i] = (char*)malloc(1000*sizeof(char));

    dados->campoInstancia = (char**)malloc(dados->qntLinhas*sizeof(char*));
    for (i=0; i<dados->qntLinhas; ++i)
        dados->campoInstancia[i] = (char*)malloc(50*sizeof(char));

    dados->tabDados = (float**)malloc(dados->qntLinhas*sizeof(float*));
    for (i = 0; i<dados->qntLinhas; ++i)
        dados->tabDados[i] = (float*)malloc(dados->qntCampos*sizeof(float));

    /* Voltando para o inicio do arquivo */
    fseek(arq, 0, SEEK_SET);

    /* Trazendo results para a memoria */
    dados->qntMaxParametros = 0;
    for (i = 0; i<dados->qntLinhas; ++i) {
        for (j = 0; j<dados->qntCampos; ++j) {
            /* Lembrando que o campo: nomeSolucao, por ser do tipo (char), sempre estara no vetor de campos a ignorar */
            if (chkInVet(Dados_get_camposIgnorar(dados), Dados_get_qntCamposIgnorar(dados), j) == 1 && Dados_get_numCampoSubstituir_index(dados, 0) != j) {
                /* se pertence aos campos a ignorar */
                if (j == dados->numCampoNomeSol)
                    fscanf(arq, "%s", dados->campoNomeSolucao[i]);
                else
                    fscanf(arq, "%s", strAux);     /* caso: numCampoNomeSol == -1 */

                dados->tabDados[i][j] = -1.000000;
            } else if (j == dados->numCampoInstancia) {
                fscanf(arq, "%s", dados->campoInstancia[i]);
                if (strcmp(dados->campoInstancia[0], dados->campoInstancia[i]) == 0)
                    ++dados->qntMaxParametros;
                dados->tabDados[i][j] = -1.000000;
            } else
                fscanf(arq, "%f", &dados->tabDados[i][j]);

            fscanf(arq, "%s", strAux);             /* lendo o ";" */
        }
    }

    /*
    for (i = 0; i<dados->qntLinhas; ++i) {
        printf("\n");
        for (j = 0; j<dados->qntCampos; ++j) {
            printf("%.1f ", dados->tabDados[i][j]);
        }
    }getchar();
    */

    dados->tabParametros = (float**)malloc((dados->qntCampos)*sizeof(float*));
    for (i = 0; i < dados->qntCampos; ++i)
        dados->tabParametros[i] = (float*)malloc((dados->qntMaxParametros+1)*sizeof(float)); // Problema ?

    /* Quantidade de cada parametro esta na primeira posicao do vetor */
    for (i = 0; i < dados->qntCampos; ++i)
        for (j = 0; j < (dados->qntMaxParametros+1); ++j)
            dados->tabParametros[i][j] = 0.000000;

    /* Obtendo as variacoes de parametros e a quantidade de cada um(sem repeticao) */
    atrib = 1;
    for (j = 0; j < dados->qntCampos; ++j) {
        for (int l = 0; l < dados->qntLinhas; ++l) {
            atrib = 1;

            for (h = 0; h < (int)dados->tabParametros[j][0]; ++h)
                if (floatcmp(dados->tabParametros[j][h+1], dados->tabDados[l][j]) == 0)
                    atrib = 0;

            if (atrib == 1 && j != 0 && chkInVet(dados->campoParamEntrada, dados->qntParamEntrada, j) != 0) {
                dados->tabParametros[j][0] += 1;
                dados->tabParametros[j][(int)dados->tabParametros[j][0]] = dados->tabDados[l][j]; // Problema: escrita invalida
            }

            if (floatcmp(dados->tabParametros[j][0], (float)dados->qntMaxParametros) == 0)
                break;
        }
    }

    dados->qntInstancias = dados->qntLinhas/dados->qntMaxParametros;

    fseek(arq, 0, SEEK_SET);

    for (i=0; i<dados->qntLinhas; ++i) {
        for (j=0; j<dados->qntCampos; ++j) {
            if (j == dados->numCampoInstancia) {
                fscanf(arq, "%s", strAux);
                for (h = 0; h < dados->qntInstancias; ++h) {
                    if (strcmp(strAux, dados->campoInstancia[h]) == 0) {
                        dados->tabDados[i][j] = (float)h;
                        break;
                    }
                }
            } else
                fscanf(arq, "%s", strAux);

            fscanf(arq, "%s", strAux);             /* lendo o ";" */
        }
    }

    /*
    printf("\n\ntabDados[][]:");
    for (i = 0; i < dados->qntLinhas; ++i) {
        printf("\n[%d]: ", i);
        for (j = 0; j < dados->qntCampos; ++j)
            printf("%.2f ", dados->tabDados[i][j]);
    } getchar(); */

    /*
    printf("\n\nDepois: tabParametros[][]:");
    for (i = 0; i < dados->qntCampos; ++i) {
        printf("\n[%d]: ", i);
        for (j = 0; j < (dados->qntMaxParametros+1); ++j)
            printf("%.2f ", dados->tabParametros[i][j]);
    }
    */

    /* Obtendo a quantidade maxima de combinacoes entres os diferentes tipos de parametros */
    dados->variacaoParametros = 1; /* vai fazer sucessivas mutiplicacoes ref. a qnt. de cada parametro */
    for (i = 0; i < dados->qntCampos; ++i)
        if ((int)dados->tabParametros[i][0] != 0)
            dados->variacaoParametros *= (int)dados->tabParametros[i][0];

    /* construindo tabela com todas as variacoes de parametros (sem repetir) */
    /* tabConjuntoParametros[][] possui todos os conjuntos de parametros utilizados (sem repeticao) */
    dados->tabConjuntoParametros = (double**)malloc(dados->variacaoParametros*sizeof(double*));
    for (i = 0; i<dados->variacaoParametros; ++i)
        dados->tabConjuntoParametros[i] = (double*)malloc(dados->qntParamEntrada*sizeof(double));

    for (i = 0; i<dados->variacaoParametros; ++i)
        for (j = 0; j<dados->qntParamEntrada; ++j)
            dados->tabConjuntoParametros[i][j] = -999999.123456; // Conferir, pois, na comparacao aparece *.12500. E Avisar caso o usuario tiver parametro igual a este numero. O proposito dele eh ser dif irentes de todos.
    /*
    for (i = 0; i<dados->variacaoParametros; ++i){
        printf("\n");
        for (j = 0; j<dados->qntParamEntrada; ++j)
            printf("%.6f ", dados->tabConjuntoParametros[i][j]);
    } getchar();
    */

    float *vetAuxParam = (float*)malloc(dados->qntParamEntrada*sizeof(float));

    for (i = 0; i < dados->qntLinhas; ++i) {
        k = 0;
        for (j = 1; j < dados->qntCampos; ++j) {
            if (chkInVet(Dados_get_campoParamEntrada(dados), Dados_get_qntParamEntrada(dados), j)) {
                vetAuxParam[k] = dados->tabDados[i][j];
                ++k;
            }
        }

        for (int l = 0; l < dados->variacaoParametros; ++l) {
            chave = 0;
            for (h = 0; h < dados->qntParamEntrada; ++h) {
                if (floatcmp(vetAuxParam[h], dados->tabConjuntoParametros[l][h]) != 0) {
                    chave = 1;
                    if (l == pos) { /* Se foi diferente ate onde ja foi acrescido, grava conjunto de parametros ineditos  */
                        for (int t = 0; t < dados->qntParamEntrada; ++t)
                            dados->tabConjuntoParametros[pos][t] = vetAuxParam[t];

                        ++pos;
                        chave = 0;
                        break;
                    }
                }
            }

            if (chave == 0)
                break;
        }
    }

    dados->tipoParamEntrada = (char*)malloc(dados->qntParamEntrada * sizeof(char));
    dados->tipoParamEntradaSaida = (char*)malloc(dados->qntCampos * sizeof(char));
    dados->tipoParamEntradaSaida[0] = 'c';

    for (int l = 0; l < dados->qntCampos; ++l)
        dados->tipoParamEntradaSaida[l] = '-';

    k = 0;
    for (i = 0; i < dados->qntCampos; ++i) {
        chave = 0;
        for (j = 0; j < dados->qntLinhas; ++j) {
            if (i == Dados_get_numCampoInstancia(dados) || i == Dados_get_numCampoNomeSol(dados))
                chave = -1;
            else {
                resp = verificador_de_tipo2(dados->tabDados[j][i]);

                if (resp == 1)
                    chave = 1;
            }
        }

        if (chave == 0)             /* nao teve alteracoes nas casas decimais. Eh um inteiro*/
            dados->tipoParamEntradaSaida[i] = 'i';
        else if (chave == 1)
            dados->tipoParamEntradaSaida[i] = 'f';
        else
            dados->tipoParamEntradaSaida[i] = 'c';

        if (chkInVet(Dados_get_campoParamEntrada(dados), Dados_get_qntParamEntrada(dados), i) == 1) {
            if (chave == 0)         /* nao teve alteracoes nas casas decimais. Eh um inteiro*/
                dados->tipoParamEntrada[k] = 'i';
            else if (chave == 1)
                dados->tipoParamEntrada[k] = 'f';
            else
                dados->tipoParamEntrada[k] = 'c';

            ++k;
        }
    } // verificar

    /* Descobrindo a quantidade de solucoes iniciais */
    dados->qntSolIniciais = 0;

    for (int l = 0; l < Dados_get_qntLinhas(dados); ++l) {
        chave = 0;
        if (floatcmp(0, Dados_get_tabDados_index(dados, l, 0)) == 0) {
            for (h = 0; h < Dados_get_qntParamEntrada(dados); ++h) {
                if (floatcmp(Dados_get_tabDados_index(dados, l, Dados_get_campoParamEntrada_index(dados, h)), Dados_get_tabConjuntoParametros_index(dados, 0, h)) != 0) {
                    chave = 1;
                    break;
                }
            }

            /* Todos os parametros de vetConjuntoParametros[] batem com os presentes na linha l */
            if (chave == 0)
                ++dados->qntSolIniciais;
        }
    }

    /* Quantidade de parametros utilizados */
    dados->qntParametrosUtilizados = 1;
    for (i = 0; i < dados->qntCampos; ++i) {
        if (floatcmp(dados->tabParametros[i][0], 0.0) != 0)
            dados->qntParametrosUtilizados *= dados->tabParametros[i][0];
    }

    dados->qntBdBestSolutions = 0;

    /* Lendo BD best solutions para a memoria: bd_best_solutions_[problema].txt*/
    FILE* arqBestSolution = fopen(Dados_get_arquivoBestSolutions(dados), "r");

    if (!arqBestSolution) {
        printf("\nErro ao criar/abrir arquivo: %s", Dados_get_arquivoBestSolutions(dados));
        exit(1);
    }

    /* Descobrindo quantidade de linhas: bd_best_solutions_[problema].txt*/
    while ((fgets(buffer, sizeof(buffer), arqBestSolution)) != NULL)
        ++dados->qntBdBestSolutions;

    fseek(arqBestSolution, 0, SEEK_SET);
    dados->bdBestSolutions = (char***)malloc(dados->qntBdBestSolutions*sizeof(char**));

    for (i = 0; i < dados->qntBdBestSolutions; i++)
        dados->bdBestSolutions[i] = (char**)malloc(22*sizeof(char*)); // bd_best_sol tem 21 campos. +1 para refTab. Encontrar sozinho.

    for (i = 0; i < dados->qntBdBestSolutions; ++i)
        for (j = 0; j < 22; ++j)
            dados->bdBestSolutions[i][j] = (char*)malloc(50*sizeof(char));

    for (i = 0; i < dados->qntBdBestSolutions; ++i)
        for (j = 0; j < 22; ++j)
            strcpy(dados->bdBestSolutions[i][j], "-");

    fscanf(arqBestSolution, "%s ; %d ;", strAux, &qnt);

    while (strcmp(strAux, "[end]") != 0) {
        for (i = 0; i < qnt; ++i) {
            for (h = 0; h < 15; ++h)
                fscanf(arqBestSolution, " %s ;", dados->bdBestSolutions[indexAux][h]);

            fscanf(arqBestSolution, " %s %s ;", dados->bdBestSolutions[indexAux][15], dados->bdBestSolutions[indexAux][16]);
            fscanf(arqBestSolution, " %s %s ;", dados->bdBestSolutions[indexAux][17], dados->bdBestSolutions[indexAux][18]);

            for (h = 19; h < 21; ++h)
                fscanf(arqBestSolution, " %s ;", dados->bdBestSolutions[indexAux][h]);

            strcpy(dados->bdBestSolutions[indexAux][21], strAux); // refTab
            ++indexAux; /* Quantidade real */
        }
        fscanf(arqBestSolution, "%s ; %d ;", strAux, &qnt);
    }

    fclose(arqBestSolution);

    /*
    printf("\n\ntabConjuntoParametros[][]: \n");
    for (int l = 0; l < dados->variacaoParametros; ++l) {
        printf("\n");
        for (h = 0; h < dados->qntParamEntrada; ++h)
            printf("%.2f ", dados->tabConjuntoParametros[l][h]);
    }
    */

    printf("\n\nInformacoes do teste: \n");
    printf("\n   Qnt. de colunas: %d", dados->qntCampos);
    printf("\n   Qnt. de linhas: %d", dados->qntLinhas);
    printf("\n   Qnt. solucoes inicias: %d", dados->qntSolIniciais);
    printf("\n   Qnt. max. de param. : %d", dados->qntMaxParametros);
    printf("\n   Qnt. instancias: %d", dados->qntInstancias);
    printf("\n   Variacao conjunto de param.: %d", dados->variacaoParametros);
    printf("\n   Qnt. Param. utilizados: %d", dados->qntParametrosUtilizados);

    printf("\n\nATENCAO! Todas as alteracoes feitas a partir daqui poderao causar perda ou alteracao dos dados mantidos nos bancos da pagina. Faca um BKP da pasta ptests antes de proseguir (sistema de BKP automatico ainda nao foi implementado). Confira TUDO antes de executar a analise.");
    printf(" Sistema em construcao! ");

    fclose(arq);
    fclose(cfg);

    free(vetAuxParam);

    return dados;
}

/* FREE */
void Dados_free(Dados* dados)
{
    int i, j;

    for (i=0; i < dados->qntBdBestSolutions; i++)
        for (j=0; j < 22; j++)
            free(dados->bdBestSolutions[i][j]);

    for (i=0; i < dados->qntBdBestSolutions; i++)
        free(dados->bdBestSolutions[i]);

    for (i=0; i < dados->qntCampos; i++) {
        free(dados->nomeCampos[i]);
        free(dados->tabParametros[i]);
    }

    for (i = 0; i < dados->qntLinhas; ++i) {
        free(dados->campoInstancia[i]);
        free(dados->tabDados[i]);
        free(dados->campoNomeSolucao[i]);
    }

    for (i = 0; i < dados->variacaoParametros; ++i)
        free(dados->tabConjuntoParametros[i]);

    free(dados->bdBestSolutions);
    free(dados->nomeCampos);
    free(dados->tabParametros);
    free(dados->campoInstancia);
    free(dados->campoNomeSolucao);
    free(dados->tabDados);
    free(dados->tabConjuntoParametros);

    free(dados->campoParamEntrada);
    free(dados->campoParamSaida);
    free(dados->camposIgnorar);
    free(dados->resultsCamposCalcMediaDesvio);
    free(dados->resultsExibirCampos);
    free(dados->resultsDetailsExibirCampos);
    free(dados->analysisExibirCampos);
    free(dados->tipoParamEntrada);
    free(dados->tipoParamEntradaSaida);

    free(dados);
}

void verificacoes_iniciais(Dados* dados)
{
    char caminhoArquivo[1000], comando[1000], op = '-';
    int ret;

    /* MENU */
    while (op == '-') {
        //printf("\n\nMenu:\n\n  [1] - Executar analise\n  [2] - Apagar teste\n  [3] - Apagar categoria\n  [4] - Reconstruir pagina Best Solutions\n  [5] - Reconstruir index\n  [6] - Substituir valores no best solutions\n  [7] - Sair\n\nOpcao: ");
        printf("\n\nMenu:\n\n  [1] - Executar analise\n  [2] - Reconstruir paginas: Best Solutions\n  [3] - Reconstruir Index\n  [4] - Sair\n\nOpcao: ");

        scanf("%c", &op);
        //op = '1';
        switch(op) {

            case '1':
                printf("\nExecutando analise...\n");
                /* codigo ao final do swich. [colocar em uma funcao]*/
                break;

            case '2':
                BResults_reconstruirPagina(dados, "mmrcmpsp");
                BResults_reconstruirPagina(dados, "mrcpsp");
                BResults_reconstruirPagina(dados, "rcpsp");

                printf("\n\nPronto!\n");
                exit(0);

            case '3':
                pag_index(dados, 1);
                printf("\n\nIndex reconstruido!");
                exit(0);

            case '7':
                printf("\nEm construcao...");
                break;

            case '8':
                printf("\nEm construcao...");
                break;

            case '9':
                substituir_pagBestSolutions(dados);
                printf("\n\nSubstituicao. Pronto!");
                exit(0);

            case '4':
                printf("\nSaindo...");
                exit(0);

            default:
                printf("\nOpcao inválida! Tente novamente...\n");
                op = '-';
                break;
        }
    }

    /* Verificando se a pasta do teste ja existe */
    strcpy(comando, "mkdir ");
    strcpy(caminhoArquivo, Dados_get_caminho(dados));
    strcat(caminhoArquivo, "pg/");
    strcat(caminhoArquivo, Dados_get_nomeAlgoritmo(dados));
    strcat(caminhoArquivo, "/");
    strcat(caminhoArquivo, Dados_get_idTeste(dados));
    strcat(comando, caminhoArquivo);
    ret = system(comando);

    /* Se a pasta ja existe */
    if (ret != 0) {
        while (op == '-') {
            printf("\nTeste ja sumarizado: (%s : %s)\n", Dados_get_nomeAlgoritmo(dados), Dados_get_idTeste(dados));
            printf("\nO que deseja fazer? [ (s)ubstituir, (c)cancelar ]: ");
            scanf("%c", &op);

            switch(op) {
                case 's':
                    /* Removendo pasta idTeste */
                    strcpy(comando, "rm -r ");
                    strcat(comando, caminhoArquivo);
                    system(comando);

                    /* Criando pasta idTeste */
                    strcpy(comando, "mkdir ");
                    strcat(comando, caminhoArquivo);
                    system(comando);

                    /* Criando pasta idTeste/quality_by_instance */
                    strcat(comando, "/quality_by_instance");
                    system(comando);

                    /* Criando pasta idTeste/quality_by_instance/all */
                    strcat(comando, "/all");
                    system(comando);

                    /* Criando pasta idTeste/details */
                    strcpy(comando, "mkdir ");
                    strcat(comando, caminhoArquivo);
                    strcat(comando, "/details");
                    system(comando);

                    /* Criando pasta idTeste/best_solutions */
                    strcpy(comando, "mkdir ");
                    strcat(comando, caminhoArquivo);
                    strcat(comando, "/best_solutions");
                    system(comando);
                    break;

                case 'c':
                    printf("\nSaindo...\n");
                    exit(0);

                default:
                    printf("\nOpcao inválida! Tente novamente...");
                    op = '-';
                    break;
            }
        }
    } else {

        /* Criando pasta idTeste/quality_by_instance */
        strcat(comando, "/quality_by_instance");
        system(comando);

        /* Criando pasta idTeste/quality_by_instance/all */
        strcat(comando, "/all");
        system(comando);

        /* Criando pasta idTeste/details */
        strcpy(comando, "mkdir ");
        strcat(comando, caminhoArquivo);
        strcat(comando, "/details");
        system(comando);

        /* Criando pasta idTeste/best_solutions */
        strcpy(comando, "mkdir ");
        strcat(comando, caminhoArquivo);
        strcat(comando, "/best_solutions");
        system(comando);
    }
}

/* verifica se os elementos de um vetor sao todos inteiros ou nao */
int verificador_de_tipo(float* vet, int tamVet)
{
    int chave = 0, i;
    for (i = 1; i <= tamVet; ++i) {
        int num1 = vet[i]*1000000, num2 = (int)vet[i]; num2 *= 1000000;
        if (num1 > num2)
            chave = 1;
    }
    return chave;
}

void prox_tag(FILE* gab, FILE* pag)
{
    char simb, strAux[10];
    simb = (char)getc(gab);

    while (simb != '~') {
        simb = (char)getc(gab);
        putc(simb, pag);
    }

    fscanf(gab, "%s", (char*)&strAux);
    fprintf(pag, "%s ", strAux);
    fscanf(gab, "%s", (char*)&strAux);
    fprintf(pag, "%s \n", strAux);
}

/* Verif ica valor em um vetor */
int chkInVet(int *vet, int tamVet, int value)
{
    int i;
    for (i = 0; i < tamVet; ++i)
        if (vet[i] == value)
            return 1;

    return 0;
}

/* Converter inteiro para char */
char* intToChar(char* buffer, int num)
{
    sprintf(buffer, "%d", num);
    return buffer;
}

/* Converter inteiro para nome do mes */
char* intToMes(int mesNUM)
{
    if (mesNUM == 1)
        return (char*)"Jan";
    else if (mesNUM == 2)
        return (char*)"Feb";
    else if (mesNUM == 3)
        return (char*)"Mar";
    else if (mesNUM == 4)
        return (char*)"Apr";
    else if (mesNUM == 5)
        return (char*)"May";
    else if (mesNUM == 6)
        return (char*)"June";
    else if (mesNUM == 7)
        return (char*)"July";
    else if (mesNUM == 8)
        return (char*)"Aug";
    else if (mesNUM == 9)
        return (char*)"Sept";
    else if (mesNUM == 10)
        return (char*)"Oct";
    else if (mesNUM == 11)
        return (char*)"Nov";
    else if (mesNUM == 12)
        return (char*)"Dec";

    printf("\nERROR: Mes nao reconhecido! %d\n", mesNUM);
    return (char*)"ERROR";
}

int verificaVazioConfig(FILE* cfg)
{
    char strFlag[1000], strAux[100];
    long int posArq = ftell(cfg);

    fscanf(cfg, "%s = %s", strFlag, strAux);
    fseek(cfg, posArq, SEEK_SET);

    if (strcmp(strAux, "-1") == 0)
        return -1;

    return 0;
}

int Dados_get_buscarCampoInstancia(Dados* dados, char *inst)
{
    int i;
    for (i = 0; i < Dados_get_qntLinhas(dados); ++i)
        if (strcmp(inst, dados->campoInstancia[i]) == 0)
            return i;
    return -1;
}

int Dados_get_buscarBdBestSolutions(Dados* dados, char* instance, int col, char *str)
{
    int i;
    for (i = 0; i < dados->qntBdBestSolutions; ++i)
        if (strcmp(dados->bdBestSolutions[i][0], instance) == 0)
            if (strcmp(dados->bdBestSolutions[i][col], str) == 0)
                return i;

    return -1;
}

int Dados_get_tamCabecalho(Dados* dados) {
    return dados->tamCabecalho;
}

int Dados_get_numCampoInstancia(Dados* dados) {
    return dados->numCampoInstancia;
}

int Dados_get_qntInstancias(Dados* dados) {
    return dados->qntInstancias;
}

int Dados_get_numCampoSolFinal(Dados* dados) {
    return dados->numCampoSolFinal;
}

int Dados_get_numCampoNumSol(Dados* dados) {
    return dados->numCampoNumSol;
}

int Dados_get_numCampoSolInicial(Dados* dados) {
    return dados->numCampoSolInicial;
}

int Dados_get_qntSolIniciais(Dados* dados) {
    return dados->qntSolIniciais;
}

int Dados_get_qntCampos(Dados* dados) {
    return dados->qntCampos;
}

int Dados_get_qntLinhas(Dados* dados) {
    return dados->qntLinhas;
}

int Dados_get_qntMaxParametros(Dados* dados) {
    return dados->qntMaxParametros;
}

int Dados_get_qntParamEntrada(Dados* dados) {
    return dados->qntParamEntrada;
}

int Dados_get_qntParamSaida(Dados* dados) {
    return dados->qntParamSaida;
}

int Dados_get_qntCamposIgnorar(Dados* dados) {
    return dados->qntCamposIgnorar;
}

int Dados_get_qntAnalysisExibirCampos(Dados* dados) {
    return dados->qntAnalysisExibirCampos;
}

int Dados_get_qntResultsCamposCalcMediaDesvio(Dados* dados) {
    return dados->qntResultsCamposCalcMediaDesvio;
}

int Dados_get_qntresultsExibirCampos(Dados* dados) {
    return dados->qntresultsExibirCampos;
}

int Dados_get_qntResultsDetailsExibirCampos(Dados* dados) {
    return dados->qntResultsDetailsExibirCampos;
}

char* Dados_get_nomeAlgoritmo(Dados* dados) {
    return dados->nomeAlgoritmo;
}

char* Dados_get_idTeste(Dados* dados) {
    return dados->idTeste;
}

char* Dados_get_nomeArquivo(Dados* dados) {
    return dados->nomeArquivo;
}

char* Dados_get_caminho(Dados* dados) {
    return dados->caminho;
}

char* Dados_get_summaryStartDate(Dados* dados) {
    return dados->summaryStartDate;
}

char* Dados_get_summaryEndDate(Dados* dados) {
    return dados->summaryEndDate;
}

char* Dados_get_summaryAbout(Dados* dados) {
    return dados->summaryAbout;
}

char* Dados_get_descricao(Dados* dados) {
    return dados->descricao;
}

char** Dados_get_campoInstancia(Dados* dados) {
    return dados->campoInstancia;
}

char* Dados_get_campoInstancia_index(Dados* dados, int index) {
    return dados->campoInstancia[index];
}

char** Dados_get_campoNomeSolucao(Dados* dados) {
    return dados->campoNomeSolucao;
}

char* Dados_get_campoNomeSolucao_index(Dados* dados, int index) {
    return dados->campoNomeSolucao[index];
}

char** Dados_get_nomeCampos(Dados* dados) {
    return dados->nomeCampos;
}

char* Dados_get_nomeCampos_index(Dados* dados, int index) {
    return dados->nomeCampos[index];
}

int* Dados_get_campoParamEntrada(Dados* dados) {
    return dados->campoParamEntrada;
}

int Dados_get_campoParamEntrada_index(Dados* dados, int index) {
    return dados->campoParamEntrada[index];
}

int* Dados_get_campoParamSaida(Dados* dados) {
    return dados->campoParamSaida;
}

int* Dados_get_camposIgnorar(Dados* dados) {
    return dados->camposIgnorar;
}

int* Dados_get_resultsCamposCalcMediaDesvio(Dados* dados) {
    return dados->resultsCamposCalcMediaDesvio;
}

int Dados_get_resultsCamposCalcMediaDesvio_index(Dados* dados, int index) {
    return dados->resultsCamposCalcMediaDesvio[index];
}

int* Dados_get_resultsExibirCampos(Dados* dados) {
    return dados->resultsExibirCampos;
}

int* Dados_get_resultsDetailsExibirCampos(Dados* dados) {
    return dados->resultsDetailsExibirCampos;
}

int* Dados_get_analysisExibirCampos(Dados* dados) {
    return dados->analysisExibirCampos;
}

float** Dados_get_tabDados(Dados* dados) {
    return dados->tabDados;
}

float* Dados_get_tabDados_vet(Dados* dados, int index) {
    return dados->tabDados[index];
}

float Dados_get_tabDados_index(Dados* dados, int x, int y) {
    return dados->tabDados[x][y];
}

float** Dados_get_tabParametros(Dados* dados) {
    return dados->tabParametros;
}

float* Dados_get_tabParametros_vet(Dados* dados, int index) {
    return dados->tabParametros[index];
}

float Dados_get_tabParametros_index(Dados* dados, int x, int y) {
    return dados->tabParametros[x][y];
}

char Dados_get_tipoParamEntradaSaida_index(Dados* dados, int index) {
    return dados->tipoParamEntradaSaida[index];
}

char* Dados_get_tipoParamEntradaSaida(Dados* dados) {
    return dados->tipoParamEntradaSaida;
}

char Dados_get_tipoParamEntrada_index(Dados* dados, int index) {
    return dados->tipoParamEntrada[index];
}

char* Dados_get_tipoParamEntrada(Dados* dados) {
    return dados->tipoParamEntrada;
}

int Dados_get_variacaoParametros(Dados* dados) {
    return dados->variacaoParametros;
}

double** Dados_get_tabConjuntoParametros(Dados* dados) {
    return dados->tabConjuntoParametros;
}

double* Dados_get_tabConjuntoParametros_vet(Dados* dados, int index) { // Possivel erro. Evitar usar. Verif icar!
    return dados->tabConjuntoParametros[index];
}

double Dados_get_tabConjuntoParametros_index(Dados* dados, int x, int y) {
    return dados->tabConjuntoParametros[x][y];
}

float Dados_get_tmp_media(Dados* dados) {
    return dados->media;
}

float Dados_get_tmp_desvio(Dados* dados) {
    return dados->desvio;
}

char* Dados_get_arquivoBestSolutions(Dados* dados) {
    return dados->arquivoBestSolutions;
}

char* Dados_get_tmp_nomePagina(Dados* dados) {
    return dados->nomePagina;
}

int Dados_get_numCampoCalcQuality(Dados* dados) {
    return dados->numCampoCalcQuality;
}

int Dados_get_numCampoNomeSol(Dados* dados) {
    return dados->numCampoNomeSol;
}

int Dados_get_numCampoDualBoundFinal(Dados* dados) {
    return dados->numCampoDualBoundFinal;
}

int Dados_get_qntParametrosUtilizados(Dados* dados) {
    return dados->qntParametrosUtilizados;
}

float Dados_get_melhorMediaQuality(Dados* dados) {
    return dados->melhorMediaQuality;
}

float Dados_get_melhorDesvioQuality(Dados* dados) {
    return dados->melhorDesvioQuality;
}

char* Dados_get_cor1(Dados* dados) {
    return dados->cor1;
}

char* Dados_get_cor2(Dados* dados) {
    return dados->cor2;
}

char* Dados_get_corNotFound(Dados* dados) {
    return dados->corNotFound;
}

char* Dados_get_corInfeasible(Dados* dados) {
    return dados->corInfeasible;
}

char* Dados_get_nomeProblema(Dados* dados) {
    return dados->nomeProblema;
}

int Dados_get_numCampoTempo(Dados* dados) {
    return dados->numCampoTempo;
}

char Dados_get_tipoSolObj(Dados* dados) {
    return dados->tipoSolObj;
}

char* Dados_get_caminhoPastaSolucoes(Dados* dados) {
    return dados->caminhoPastaSolucoes;
}

int Dados_get_numCampoSubstituir_index(Dados* dados, int index) {
    return dados->numCampoSubstituir[index];
}

int Dados_get_ignorarAusentesInfactiveis(Dados* dados) {
    return dados->ignorarAusentesInfactiveis;
}

/* set */
void Dados_set_tmp_media(Dados* dados, float value) {
    dados->media = value;
}

void Dados_set_tmp_desvio(Dados* dados, float value) {
    dados->desvio = value;
}

void Dados_set_tmp_nomePagina(Dados* dados, char* str) {
    strcpy(dados->nomePagina, str);
}

void Dados_set_melhorMediaQuality(Dados* dados, float value) {
    dados->melhorMediaQuality = value;
}

void Dados_set_melhorDesvioQuality(Dados* dados, float value) {
    dados->melhorDesvioQuality = value;
}

void Dados_set_nomeProblema(Dados* dados, char* nome) {
    strcpy(dados->nomeProblema, nome);
}

void Dados_set_arquivoBestSolutions(Dados* dados, char* nome) {
    strcpy(dados->arquivoBestSolutions, nome);
}

void Dados_set_caminhoPastaSolucoes(Dados* dados, char* nome) {
    strcpy(dados->caminhoPastaSolucoes, nome);
}
