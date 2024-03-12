/* GOAL PSP Solver II
 * Multi-Mode Resource-Constrained Multi-Project Scheduling Problem (MRCMPSP)
 *
 * mip_compact: routines for the creation of compact MIP formulations
 *              for the whole problem or for just one project
 *
 * Develop as part of the D.Sc. thesis of Soares, Janniele A., with collaboration
 *                                   of Santos, H.G., Toffolo, T. and Baltar, D.
 */

#ifndef MIP_COMPACT
#define  MIP_COMPACT

#include "instance.h"
#include "solution.h"
#include "lp.h"
#include "parameters.h"
#include "results.h"
#include "cut_pool.h"

#define MIPC_ALL_PROJECTS -1

typedef struct _MIPCompact MIPCompact;

//-1 idxProj para todos, tpdSum e tmsSum passar o tpd e tms para todos os projetos.
MIPCompact *MipC_create( Instance *inst, Parameters *par, Results *res, const int idxProj, const int tpdSum, const int tmsSum, double timeLeft );

/* parses a name in the format prefix(idx1,idx2,...) */
int parseName( const char *name, char *prefix, int *idx );

double MipC_getSlack( MIPCompact *mipC );
double MipC_getMaxCut( MIPCompact *mipC );
int MipC_getRemoveCuts( MIPCompact *mipC );

/* solver the LP relaxation of the mip */
int MipC_linear_relaxation( MIPCompact *mipC, Solution *sol, Results *res,  double timeLeft);

/* solver the LP relaxation of the mip with cuts*/
int MipC_cutting_plane( MIPCompact *mipC, Solution *sol, Results *res,  double timeLeft);
int MipC_cutting_plane_cgraph( MIPCompact *mipC, Solution *sol, Results *res,  double timeLeft);

/*solve LP as exact*/
void MipC_solve( MIPCompact *mipp, double timeLeft );

/* tries to improve the current formulation by cutting the
   current fractional solution, returns the number of cuts added */
void MipC_setVariablesZero(MIPCompact *mipC);
//int MipC_getMaxInstant( MIPCompact *mipC );
int MipC_getJump( MIPCompact *mipC );
int MipC_getMaxConstraint( MIPCompact *mipC );


void MipC_checkArgs(MIPCompact *mipC, char **argv, int argc);
void MipC_writeLP( const MIPCompact *mipp, const char *fileName );
char MipC_hasSolution( MIPCompact *MipP );
void MipC_allocateSol(MIPCompact *mipC);
int MipC_saveSol(MIPCompact *mipC);
int MipC_isInteger( MIPCompact *mipC);

/* gets and sets*/
int MipC_getnRounds();
int MipC_getMaxElements(int idx);
int MipC_getSumAllVarWithConflicts(int idx);
int MipC_getSumAllElements(int idx);
Solution* MipC_getSol( MIPCompact *mipC );
int MipC_getNCut( MIPCompact *mipC, int idx );
double MipC_getTCut( MIPCompact *mipC, int idx );
int MipC_getContinuous(MIPCompact *mipC);
int MipC_getLifting(MIPCompact *mipC);
double MipC_getBestObj( MIPCompact *mipC );
double MipC_getCurrentObj( MIPCompact *mipC );
double MipC_getBestPossibleObj( MIPCompact *mipC);
int MipC_getMaxCutElements(int idx);
int MipC_getSumElementsCut(int idx);
int MipC_getSumTPD( MIPCompact *mipp);
double MipC_TPD( MIPCompact *MipP );
LinearProgram* MipC_mip( MIPCompact *mipC);
const Instance* MipC_inst( MIPCompact *mipC );
void MipC_setInitialSolution( MIPCompact *mipp, const Solution *_bestSol );
void MipC_setMaxSeconds( MIPCompact *MipP, int maxSeconds );
//double computeSumXAllModesSecondPart( const Instance *inst,  int j, int ij, int t, int ***TJ, int **nCont, int start, const double ***x );
//double computeSumXAllModes( const Instance *inst, int j, int ***TJ, int **nCont, int start, int end, const double ***x );
void MipC_parseParameters( int argc, const char **argv );
Parameters *MipC_getPar(MIPCompact *mipC);

/*backtracking functions*/

void MipC_strengthening(MIPCompact *mipp, double timelimit, int nJobs);

int MipC_subset_bt(const Instance *inst, VecInt *sj, VecInt *sm, VecInt* tj, VecInt* tm, VecInt *initsubset,
                   int sjsm_size, int *jobmodeinsubset, int *totalusesRR, int *totalusesN,
                   int *maxnumit, int ite, int nJobs, const int* capRR, const int* capNR);

void MipC_bt_clear_vectors(const Instance *inst, int *totalusesRR, int *totalusesN,  int *jobinsubset);

int MipC_bt_check_constraints(const Instance *inst, const Job *job, const Mode *mode,  int *jobmodeinsubset, int *totalusesRR, int *totalusesN, int nJobs, const int *capRR, const int *capNR);

void MipC_bt_undo_constraints(const Job *job, const Mode *mode, int *totalusesRR, int *totalusesN,  int *jobinsubset, int valid);

/*end backtracking functions*/

void MipC_help( );

void MipC_printConfig( );

void MipC_free( MIPCompact **mipp );

#endif // MIP_PROJECT

