/* RESULTS ANALYZER v0.1 (alpha)
 * Software utilizado para analisar e comparar testes feitos
 * por qualquer algoritmo.
 *
 * Ultima atualizacao: 05/10/2017
 *
 *                                     Autor: Davi D. Baltar
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "index.h"
#include "dados.h"
#include "results.h"
#include "analysis.h"
#include "parameters.h"
#include "best_solutions.h"
#include "results_details.h"
#include "quality_by_instance.h"
#include "quality_by_instance_all.h"


int main(int argc, char *argv[])
{
    Dados* dados;
    dados = Dados_create( argv, "in.txt" );

    verificacoes_iniciais( dados );

    pag_analysis( dados );
    pag_parameters( dados );
    pag_results( dados );
    pag_index( dados, 0 );

    printf("\nPronto!\n"); fflush(stdout);

    Dados_free( dados );

    return 0;
}
