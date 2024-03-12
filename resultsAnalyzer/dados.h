#ifndef DADOS_H_INCLUDED
#define DADOS_H_INCLUDED

typedef struct _dados Dados;

Dados* Dados_create(char** argv, char *file_in);
void Dados_free(Dados* dados);
void prox_tag(FILE* gab, FILE* pag);

/* get */
int Dados_get_buscarCampoInstancia(Dados* dados, char *inst);
int Dados_get_tamCabecalho(Dados* dados);
int Dados_get_numCampoInstancia(Dados* dados);
int Dados_get_qntInstancias(Dados* dados);
int Dados_get_numCampoSolFinal(Dados* dados);
int Dados_get_numCampoNumSol(Dados* dados);
int Dados_get_numCampoSolInicial(Dados* dados);
int Dados_get_qntSolIniciais(Dados* dados);
int Dados_get_qntCampos(Dados* dados);
int Dados_get_qntLinhas(Dados* dados);
int Dados_get_qntMaxParametros(Dados* dados);
int Dados_get_qntParamEntrada(Dados* dados);
int Dados_get_qntParamSaida(Dados* dados);
int Dados_get_qntCamposIgnorar(Dados* dados);
int Dados_get_qntAnalysisExibirCampos(Dados* dados);
int Dados_get_qntResultsCamposCalcMediaDesvio(Dados* dados);
int Dados_get_qntresultsExibirCampos(Dados* dados);
int Dados_get_qntResultsDetailsExibirCampos(Dados* dados);
char* Dados_get_nomeAlgoritmo(Dados* dados);
char* Dados_get_idTeste(Dados* dados);
char* Dados_get_nomeArquivo(Dados* dados);
char* Dados_get_caminho(Dados* dados);
char* Dados_get_summaryStartDate(Dados* dados);
char* Dados_get_summaryEndDate(Dados* dados);
char* Dados_get_summaryAbout(Dados* dados);
char* Dados_get_descricao(Dados* dados);
char** Dados_get_campoInstancia(Dados* dados);
char* Dados_get_campoInstancia_index(Dados* dados, int index);
char** Dados_get_campoNomeSolucao(Dados* dados);
char* Dados_get_campoNomeSolucao_index(Dados* dados, int index);
char** Dados_get_nomeCampos(Dados* dados);
char* Dados_get_nomeCampos_index(Dados* dados, int index);
int* Dados_get_campoParamEntrada(Dados* dados);
int Dados_get_campoParamEntrada_index(Dados* dados, int index);
int* Dados_get_campoParamSaida(Dados* dados);
int* Dados_get_camposIgnorar(Dados* dados);
int* Dados_get_resultsCamposCalcMediaDesvio(Dados* dados);
int Dados_get_resultsCamposCalcMediaDesvio_index(Dados* dados, int index);
int* Dados_get_resultsExibirCampos(Dados* dados);
int* Dados_get_resultsDetailsExibirCampos(Dados* dados);
int* Dados_get_analysisExibirCampos(Dados* dados);
float** Dados_get_tabDados(Dados* dados);
float* Dados_get_tabDados_vet(Dados* dados, int index);
float Dados_get_tabDados_index(Dados* dados, int x, int y);
float** Dados_get_tabParametros(Dados* dados);
float* Dados_get_tabParametros_vet(Dados* dados, int index);
float Dados_get_tabParametros_index(Dados* dados, int x, int y);
char Dados_get_tipoParamEntradaSaida_index(Dados* dados, int index);
char* Dados_get_tipoParamEntradaSaida(Dados* dados);
char Dados_get_tipoParamEntrada_index(Dados* dados, int index);
char* Dados_get_tipoParamEntrada(Dados* dados);
int Dados_get_variacaoParametros(Dados* dados);
double** Dados_get_tabConjuntoParametros(Dados* dados);
double* Dados_get_tabConjuntoParametros_vet(Dados* dados, int index);
double Dados_get_tabConjuntoParametros_index(Dados* dados, int x, int y);
char* Dados_get_arquivoBestSolutions(Dados* dados);
float Dados_get_tmp_media(Dados* dados);
float Dados_get_tmp_desvio(Dados* dados);
char* Dados_get_tmp_nomePagina(Dados* dados );
int Dados_get_numCampoNomeSol(Dados* dados);
int Dados_get_numCampoCalcQuality(Dados* dados );
int Dados_get_numCampoDualBoundFinal(Dados* dados );
int Dados_get_qntParametrosUtilizados(Dados* dados );
float Dados_get_melhorMediaQuality(Dados* dados );
float Dados_get_melhorDesvioQuality(Dados* dados );
char* Dados_get_cor1(Dados* dados);
char* Dados_get_cor2(Dados* dados);
char* Dados_get_nomeProblema(Dados* dados);
int Dados_get_numCampoTempo(Dados* dados);
char Dados_get_tipoSolObj(Dados* dados);
char* Dados_get_corNotFound(Dados* dados);
char* Dados_get_corInfeasible(Dados* dados);
char* Dados_get_caminhoPastaSolucoes(Dados* dados);
int Dados_get_numCampoSubstituir_index(Dados* dados, int index);
int Dados_get_ignorarAusentesInfactiveis(Dados* dados);
int Dados_get_buscarBdBestSolutions(Dados* dados, char* instance, int col, char *str);

/* set */
void Dados_set_tmp_media(Dados* dados, float value);
void Dados_set_tmp_desvio(Dados* dados, float value);
void Dados_set_tmp_nomePagina(Dados* dados, char* str );
void Dados_set_melhorMediaQuality(Dados* dados, float value);
void Dados_set_melhorDesvioQuality(Dados* dados, float value);
void Dados_set_nomeProblema(Dados* dados, char* nome);
void Dados_set_arquivoBestSolutions(Dados* dados, char* nome);
void Dados_set_caminhoPastaSolucoes(Dados* dados, char* nome);

int chkInVet( int *vet, int tamVet, int value );
int verificador_de_tipo(float* vet, int tamVet);
char* intToChar(char* buffer, int num);
void verificacoes_iniciais(Dados* dados);
char* intToMes(int mesNUM);
int verificaVazioConfig(FILE* cfg);

#endif // DADOS_H_INCLUDED
