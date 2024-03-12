#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "index.h"
#include "util.h"
#include "dados.h"

#define INF 999999999

/*  int option:
 *   0 = Acrescenta novo teste no index
 *   1 = Somente reconstroi index com base no bd_index_[problema]
 */
void pag_index(Dados* dados, int option)
{
    char dia[3], mes[4], ano[5];
    char strAux[10000], idTeste[1000], simb = '-', c, cor[8], novoArquivo[1000] = {""};
    int qntTestes, cont, idTesteInt = 0, i, j, tamBdIndexDados;

    char* nomeAlgoritmoFiltrado = filtro_strings(Dados_get_nomeAlgoritmo(dados), -1, 0);
    char** bdIndexDados;

    strcpy(cor, Dados_get_cor1(dados));

    /* Arquivo de gabarito */
    FILE* gab = fopen("gab/pag_index.html", "r");

    if (!gab) {
        printf("\nErro ao criar/abrir arquivo:  gab/pag_index.html");
        exit(1);
    }

    strcat(novoArquivo, Dados_get_caminho(dados));
    strcat(novoArquivo, "pg/");
    strcat(novoArquivo, Dados_get_nomeAlgoritmo(dados));
    strcat(novoArquivo, "/index_");
    strcat(novoArquivo, Dados_get_nomeAlgoritmo(dados));
    strcat(novoArquivo, ".html");

    FILE* pag = fopen(novoArquivo, "w");

    if (!pag) {
        printf("\nErro ao criar/abrir arquivo: %s", novoArquivo);
        exit(1);
    }

    /* Copiando até o ponto [r01]: */
    while (simb != '~') {
        simb = fgetc(gab);
        fputc(simb, pag);
    }

    /* Lendo e escrevendo: "-->" */
    fscanf(gab, "%s", (char*)&strAux);
    fprintf(pag, "%s ", strAux);
    fscanf(gab, "%s", (char*)&strAux);
    fprintf(pag, "%s \n", strAux);

    /* Escrevendo [r00] : Titulo Aba */
    fprintf(pag, "\n<title>Index(%s)</title>\n", nomeAlgoritmoFiltrado);
    prox_tag(gab, pag);

    /* Escrevendo [r01] : Titulo */
    fprintf(pag, "\nTests: %s (Index)\n", nomeAlgoritmoFiltrado);
    prox_tag(gab, pag);

    /* Escrevendo [r02] : Descricao */
    fprintf(pag, "\nResults obtained with the algorithm %s\n", nomeAlgoritmoFiltrado);
    prox_tag(gab, pag);

    /* Escrevendo [r03] : page_back */
    fprintf(pag, "\n<a href=\"../../index.html\">Home</a>\n");
    prox_tag(gab, pag);

    /* Escrevendo [r04] : Data */
    fprintf(pag, "\nUpdate: %s\n", __DATE__);
    prox_tag(gab, pag);

    /* Escrevendo [r05] : Dados */
    strcpy(novoArquivo, Dados_get_caminho(dados));
    strcat(novoArquivo, "pg/");
    strcat(novoArquivo, Dados_get_nomeAlgoritmo(dados));
    strcat(novoArquivo, "/bd_index_");
    strcat(novoArquivo, Dados_get_nomeAlgoritmo(dados));
    strcat(novoArquivo, ".txt");

    /* Lendo para a memoria: bd_index_[algoritmo].txt*/
    FILE* arqBdIndexLeitura0 = fopen(novoArquivo, "r");

    if (!arqBdIndexLeitura0) {
        printf("\nErro ao criar/abrir arquivo: %s", novoArquivo);
        exit(1);
    }

    /* qnt de testes gravados */
    //fgets(qntTestesChar, sizeof(qntTestesChar), arqBdIndexLeitura0);
    //qntTestes = atoi(qntTestesChar);
    fscanf(arqBdIndexLeitura0, "%d", &qntTestes);

    tamBdIndexDados = qntTestes;
    bdIndexDados = (char**)malloc(tamBdIndexDados*sizeof(char*));
    for (i = 0; i < tamBdIndexDados; ++i)
        bdIndexDados[i] = (char*)malloc(1000*sizeof(char));

    fgets(strAux, 1000, arqBdIndexLeitura0);

    for (i = 0; i < qntTestes; ++i)
        fgets(bdIndexDados[i], 1000, arqBdIndexLeitura0);

    fclose(arqBdIndexLeitura0);

    if (option == 0) {    /* Gravando teste no: bd_index_[algoritmo].txt*/
        FILE* arqBdIndexEscrita = fopen(novoArquivo, "w");

        if (!arqBdIndexEscrita) {
            printf("\nErro ao criar/abrir arquivo: %s", novoArquivo);
            exit(1);
        }

        fprintf(arqBdIndexEscrita, "%d", qntTestes+1);

        /* Infor macoes do novo teste */
        fprintf(arqBdIndexEscrita, "\n%.4d ; %.3f ; %.3f ; ", qntTestes, Dados_get_melhorMediaQuality(dados), Dados_get_melhorDesvioQuality(dados)); // ERRO Valgrind

        /* start date */
        /*
        if (strcmp(Dados_get_summaryStartDate(dados), "-1") != 0) {
            strcpy(dia, ""); strcpy(mes, ""); strcpy(ano, "");
            strncat(dia, Dados_get_summaryStartDate(dados), 2);
            strncat(mes, Dados_get_summaryStartDate(dados) +3, 2);
            strncat(ano, Dados_get_summaryStartDate(dados) +6, 4);
            fprintf(arqBdIndexEscrita, "%s, %s %s <br/>--h--</td> ; ", ano, intToMes(atoi(mes)), dia);
        }else{
            fprintf(arqBdIndexEscrita, "__/__/__<br/>--h--</td> ; ");
        }
        */

        fprintf(arqBdIndexEscrita, "%s ; ", stringMaiusculo(Dados_get_nomeProblema(dados)));

        /* end date (date) */
        if (strcmp(Dados_get_summaryStartDate(dados), "-1") != 0) {
            strcpy(dia, ""); strcpy(mes, ""); strcpy(ano, "");
            strncat(dia, Dados_get_summaryEndDate(dados), 2);
            strncat(mes, Dados_get_summaryEndDate(dados) +3, 2);
            strncat(ano, Dados_get_summaryEndDate(dados) +6, 4);
            fprintf(arqBdIndexEscrita, "%s, %s %s <br/>--h-- ; ", ano, intToMes(atoi(mes)), dia);
        } else {
            fprintf(arqBdIndexEscrita, "__/__/__<br/>--h-- ; ");
        }

        if (Dados_get_qntParametrosUtilizados(dados) <= 1)
            fprintf(arqBdIndexEscrita, "%d parameter<br/>(exec./param.): %d ;\n", Dados_get_qntParametrosUtilizados(dados),Dados_get_qntSolIniciais(dados));
        else
            fprintf(arqBdIndexEscrita, "%d parameters<br/>(exec./param.): %d ;\n", Dados_get_qntParametrosUtilizados(dados),Dados_get_qntSolIniciais(dados));

        for (i = 0; i < qntTestes; ++i)
            fprintf(arqBdIndexEscrita, "%s", bdIndexDados[i]);

        fclose(arqBdIndexEscrita);
    }

    /* Lendo: bd_index_[algoritmo].txt*/
    FILE* arqBdIndexLeitura = fopen(novoArquivo, "r");

    if (!arqBdIndexLeitura) {
        printf("\nErro ao criar/abrir arquivo: %s", novoArquivo);
        exit(1);
    }

    fscanf(arqBdIndexLeitura, "%d", &qntTestes);

    for (i = 0; i < qntTestes; ++i) {
        fprintf(pag, "<tr bgcolor=\"%s\">", cor);
        for (j = 0; j < 6; ++j) {               /* 6 = quantidade de campos no bd_index_[problema].txt*/
            c = ' '; cont = 0;
            while (c != ';'){
                c = getc(arqBdIndexLeitura);
                strAux[cont] = c;
                ++cont;
            }
            strAux[cont-2] = '\0';

            if (j == 0) {
                strcpy(idTeste, strAux);
                idTesteInt = atoi(idTeste);
                fprintf(pag, "<td align=\"center\"> %.4d </td>", idTesteInt);
            } else if (j > 2)
                fprintf(pag, "<td align=\"right\">%s</td>", strAux);
              else
                fprintf(pag, "<td align=\"center\">%s</td>", strAux);
        }

        /* botao */
        /*
        if (idTesteInt > 152)
        {
            fprintf(pag, "<td align=\"center\"><for m action=\"%s/results.html\" method=\"get\"><input type=\"submit\" class=\"inputB\" value=\" results \"/></for m></td>", idTeste);
            fprintf(pag, "<td align=\"center\"><for m action=\"%s/parameters.html\" method=\"get\"><input type=\"submit\" class=\"inputB\" value=\" paramet. \"/></for m></td>", idTeste);
            fprintf(pag, "<td align=\"center\"><for m action=\"%s/analysis.html\" method=\"get\"><input type=\"submit\" class=\"inputB\" value=\" analysis \"/></for m></td></tr>\n\n", idTeste);
        }
        else if (idTesteInt <= 152) // retrocompatibilidade
        {
            fprintf(pag, "<td align=\"center\"><for m action=\"%s/test%.3d.html\" method=\"get\"><input type=\"submit\" class=\"inputB\" value=\" results \"/></for m></td>", idTeste, idTesteInt);
            fprintf(pag, "<td align=\"center\"><for m action=\"%s/test%.3d_details.html\" method=\"get\"><input type=\"submit\" class=\"inputB\" value=\" paramet. \"/></for m></td>", idTeste, idTesteInt);
            fprintf(pag, "<td align=\"center\"><for m action=\"%s/test%.3d_param.html\" method=\"get\"><input type=\"submit\" class=\"inputB\" value=\" analysis \"/></for m></td></tr>\n\n", idTeste, idTesteInt);
        }
        */

        /* link */
        if (idTesteInt <= 152 && strcmp(Dados_get_nomeAlgoritmo(dados), "lslahc") == 0) // retrocompatibilidade
        {
            fprintf(pag, "<td align=\"center\"><a href=\"%.4d/test%.3d.html\" class=\"link1\"> results </a></td>", idTesteInt, idTesteInt);
            fprintf(pag, "<td align=\"center\"><a href=\"%.4d/test%.3d_details.html\" class=\"link1\"> paramet. </a></td>", idTesteInt, idTesteInt);
            fprintf(pag, "<td align=\"center\"><a href=\"%.4d/test%.3d_param.html\" class=\"link1\"> analysis </a></td></tr>\n\n", idTesteInt, idTesteInt);

            //fprintf(pag, "<td align=\"center\"><for m action=\"%.4d/test%.3d.html\" method=\"get\"><input type=\"submit\" value=\"&nbsp results &nbsp\"/></for m></td>", idTesteInt, idTesteInt);
            //fprintf(pag, "<td align=\"center\"><for m action=\"%.4d/test%.3d.html\" method=\"get\"><input type=\"submit\" value=\"&nbsp paramet. &nbsp\"/></for m></td>", idTesteInt, idTesteInt);
            //fprintf(pag, "<td align=\"center\"><for m action=\"%.4d/test%.3d.html\" method=\"get\"><input type=\"submit\" value=\"&nbsp analysis &nbsp\"/></for m></td>\n\n", idTesteInt, idTesteInt);
        }
        else
            {
            fprintf(pag, "<td align=\"center\"><a href=\"%.4d/results.html\" class=\"link1\"> results </a></td>", idTesteInt);
            fprintf(pag, "<td align=\"center\"><a href=\"%.4d/parameters.html\" class=\"link1\"> paramet. </a></td>", idTesteInt);
            fprintf(pag, "<td align=\"center\"><a href=\"%.4d/analysis.html\" class=\"link1\"> analysis </a></td></tr>\n\n", idTesteInt);
        }

        if (!strcmp(cor, Dados_get_cor1(dados) ))
            strcpy(cor,Dados_get_cor2(dados) );
        else
            strcpy(cor,Dados_get_cor1(dados) );
    }

    prox_tag(gab, pag);

    fclose(arqBdIndexLeitura);
    fclose(gab);
    fclose(pag);

    free(nomeAlgoritmoFiltrado);

    for (i = 0; i < tamBdIndexDados; ++i)
        free( bdIndexDados[i] );

    free( bdIndexDados );

    printf("\nIndex - OK\n"); fflush(stdout);
}
