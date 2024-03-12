#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analysis.h"
#include "quality_by_instance.h"
#include "util.h"
#include "dados.h"

//#pragma clang diagnostic ignored "-Wdate-time"

#define INF 999999999


void pag_analysis(Dados* dados)
{
    char strAux[7], simb = '-', cor[8], novoArquivo[500] = {""};
    int limitePorcentagem, cont, i, j;
    float porcentagem;

    char* nomeAlgoritmoFiltrado = filtro_strings(Dados_get_nomeAlgoritmo(dados), -1, 0);

    strcpy(cor, Dados_get_cor1(dados));

    FILE* gab = fopen("gab/pag_analysis.html", "r"); // Arquivo de gabarito

    if (!gab) {
        printf("\nErro ao criar/abrir arquivo:  gab/pag_analysis.html");
        exit(1);
    }

    strcat(novoArquivo, Dados_get_caminho(dados));
    strcat(novoArquivo, "pg/");
    strcat(novoArquivo, Dados_get_nomeAlgoritmo(dados));
    strcat(novoArquivo, "/");
    strcat(novoArquivo, Dados_get_idTeste(dados));
    strcat(novoArquivo, "/");
    strcat(novoArquivo, "analysis.html");

    FILE* pag = fopen(novoArquivo, "w");

    if (!pag) {
        printf("\nErro ao criar/abrir arquivo: pag");
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

    /* Writing [r01] : Titulo */
    fprintf(pag, "\nTest %s: %s (Analysis of parameters)\n", Dados_get_idTeste(dados), nomeAlgoritmoFiltrado);
    prox_tag(gab, pag);

    /* Writing [r02] : Descricao */
    fprintf(pag, "\nAnalyze the quality of each set of parameters\n");
    prox_tag(gab, pag);

    /* Writing [r03] : Links: Back e Home */
    fprintf(pag, "\n<a href=\"../index_%s.html\">Index</a> - <a href=\"../../../index.html\">Home</a>\n", Dados_get_nomeAlgoritmo(dados));
    prox_tag(gab, pag);

    /* Writing [r04] : Data */
    fprintf(pag, "\nUpdate: %s\n", __DATE__);
    prox_tag(gab, pag);

    /* Writing [r05] : Cabecalho [ Parametros (entrada) utilizados no teste ] */ // Lembrar de ignorar campos aki
    for (i = 1; i < Dados_get_qntCampos(dados); ++i)
        if (chkInVet(Dados_get_campoParamEntrada(dados), Dados_get_qntParamEntrada(dados), i))
            fprintf(pag, "\n<th bgcolor=\"#CDC5BF\" align=\"center\" rowspan=\"3\">&nbsp %s &nbsp</th>", Dados_get_nomeCampos_index(dados, i));

    fprintf(pag, "\n");
    prox_tag(gab, pag);

    /* Escrevendo [r06] : Resultados */

    limitePorcentagem = (Dados_get_variacaoParametros(dados)/100)*10;
    cont = 0; porcentagem = 0;

    printf("\n\nAnalysis: 0%%_"); fflush(stdout);

    Dados_set_melhorMediaQuality(dados, 0);

    for (i = 0; i < Dados_get_variacaoParametros(dados); ++i) {
        /* Marcador de porcentagem */
        if (cont > limitePorcentagem) {
            cont = 0;
            porcentagem = (float)(i*100)/Dados_get_qntInstancias(dados);
            printf("%.0f%%_", porcentagem); fflush(stdout);
        } else
            ++cont;

        /* Gerando: quality by instance */
        pag_qualityByInstance(dados, i);

        if (floatcmp(Dados_get_tmp_media(dados), Dados_get_melhorMediaQuality(dados)) == 1) { // >
            Dados_set_melhorMediaQuality(dados, Dados_get_tmp_media(dados));
            Dados_set_melhorDesvioQuality(dados, Dados_get_tmp_desvio(dados));
        }

        // Lembrar de ignorar campos aki
        fprintf(pag, "\n<tr bgcolor=\"%s\">", cor);
        fprintf(pag, "<td align=\"right\">&nbsp %.6f </td>", Dados_get_tmp_media(dados));
        fprintf(pag, "<td bgcolor=\"%s\" align=\"right\">&nbsp %.6f </td>", cor, Dados_get_tmp_desvio(dados)); // ERRO Valgrind

        for (j = 0; j < Dados_get_qntParamEntrada(dados); ++j)
            if (Dados_get_tipoParamEntrada_index(dados, j) == 'i')
                fprintf(pag, "<td bgcolor=\"%s\" align=\"right\">&nbsp %d</td>", cor, (int)Dados_get_tabConjuntoParametros_index(dados, i, j));
            else if( Dados_get_tipoParamEntrada_index( dados, j ) == 'f' )
                fprintf( pag, "<td bgcolor=\"%s\" align=\"right\">&nbsp %.6f</td>", cor, Dados_get_tabConjuntoParametros_index(dados, i, j));

        /* botao (by quality) */
        fprintf(pag, "<td bgcolor=\"%s\" align=\"center\"><form action=\"quality_by_instance/%s\" method=\"get\"><input type=\"submit\" value=\"&nbsp By Instance &nbsp\"/></form></td>", cor, Dados_get_tmp_nomePagina(dados));
        fprintf(pag, "</tr>\n");

        if (!strcmp(cor, Dados_get_cor1(dados)))
            strcpy( cor,Dados_get_cor2(dados));
        else
            strcpy( cor,Dados_get_cor1(dados));
    }

     /* Copiar ate o final do gabarito */
    prox_tag(gab, pag);

    fclose(gab);
    fclose(pag);

    free(nomeAlgoritmoFiltrado);

    printf("100%%_[ OK ]\n"); fflush(stdout);
}
