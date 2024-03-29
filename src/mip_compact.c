/* GOAL PSP Solver II
 * Multi-Mode Resource-Constrained Multi-Project Scheduling Problem (MRCMPSP)
 *
 * Develop as part of the D.Sc. thesis of Soares, Janniele A., with collaboration
 *                                   of Santos, H.G., Toffolo, T. and Baltar, D.
 */

/* precedence constraint type :
 *   0:  y_u + duration_u <= y_j                           V (u,j) in P
 *   1:  x_umt + sum{m',t' : t' <= t+d_um-1 } xjm't' <= 1  V u in J, m in Mj, t in est_u, j in Succ_u
 *   2:  sum{m,t} t+d_uj x_umt <= sum{m',t'} t' x_jm't'    V (u,j) in P
 * */
static int precConsType = 2;


#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <omp.h>
#include <time.h>
#include "mip_compact.h"
#include "ms_solver_mip.h"

#include "cut_pool.h"
#include "cut_cg.h"
#include "cut_clique.h"
#include "cut_oddholes.h"
#include "cut_cover.h"
#include "cut_precedence.h"

#define VERBOSE  1

/* store mip solving options (i.e. timeout) */
VecStr *mipCParams = NULL;


struct _MIPCompact
{
    const Instance *inst;
    LinearProgram *mip;
    Solution *bestSol;
    Parameters *par;
    char hasIniSol;

    /* best tpd and tms known for this problem */
    int tpdSum;
    int tmsSum;

    /* null if we are working with all projects,
     * pointer to the project otherwise */
    const Project *project;
    /* 0 if working with all projects */
    int idxFirstJob;
    /* number of jobs */
    int nJobs;

    /* maximum time */
    int **maxTJM;
    int *maxTJ;
    int maxT;

    /* maximum number of modes in instance */
    int maxModes;

    VecInt ***idxResR;
    VecDbl ***coefResR;

    double ****coefR;
    LinearProgram ****mipstrengthening;
    // LinearProgram ***mipstrengthening;
    /* all variables of a job*/
    VecInt **jIdx;
    /* duration (mode depending) of a given x variable*/
    VecInt **dJ;

    /* all variables of a job at a time */
    VecInt ***jtIdx;

    /* variables of a job and mode */
    VecInt ***jmIdx;


    /* using non renewable resources */
    VecInt **idxResNR;
    VecDbl **coefResNR;

    int *yIdx;
    int *dIdx;
    int **uIdx;

    DictInt *dnames;

    int kiterations;

    Solution *sol;

    /* if the last solution method invoked produced
     * a feasible solution */
    char hasSolution;

    double tpd;

    int idxProj;

    /* if job is integral 1 else 0*/
    int *jobIntegral;
    int *jobFixed;

    IntTriple *jmtIntegral;
    IntTriple *jmtFractional;
    IntTriple *jmtFixed;

    int nFixed;
    int nFractional;
    int nIntegral;

    MSM_Solver *msm;

    float minRoundUp;

};

int cmp_int_triple_t( const void *v1, const void *v2);
int cmp_int_triple_time( const void *v1, const void *v2 );
int cmp_int_triple_value( const void *v1, const void *v2);
int cmp_int_triple_j( const void *v1, const void *v2 );
int cmp_int_dbl_pair_a( const void *v1, const void *v2 );
int cmp_int_dbl_pair_b( const void *v1, const void *v2 );


/*if job j on mode m starts at time t and if is being processed at time t*/
static void createXZ( MIPCompact *mipp, double timeLeft);
void createX( MIPCompact *mipp, double timeLeft);

static void createY( MIPCompact *mipp, double timeLeft );

static void createD( MIPCompact *mipp, double timeLeft );


/* mode used */
static void createU( MIPCompact *mipp, double timeLeft );

static void addFOConstants( MIPCompact *mipp, double timeLeft );


/* renewable resources */
static void createResConsR( MIPCompact *mipp, double timeLeft);
void createResConsRByZ( MIPCompact *mipp, double timeLeft);
void createResConsRByZX( MIPCompact *mipp, double timeLeft);

void createResConsRLiftByZ( MIPCompact *mipp, double timeLeft);

/* non renewable resources */
static void createResConsNR( MIPCompact *mipp, double timeLeft );

/* mode selection */
static void createModeSelCons( MIPCompact *mipp, double timeLeft );

/* link x and y */
static void createConsLinkXY( MIPCompact *mipp, double timeLeft );

/* link x and Z */
void createConsLinkXZ( MIPCompact *mipp, double timeLeft );

/* link x and d */
static void createConsLinkXD( MIPCompact *mipp, double timeLeft );

/* precedence constraints */
static void createPrecCons0( MIPCompact *mipp,double timeLeft );
static void createPrecCons1( MIPCompact *mipp,double timeLeft );
static void createPrecCons2( MIPCompact *mipp,double timeLeft );

static void createConsLnkXU( MIPCompact *mipp, double timeLeft );

static void createConsSelU( MIPCompact *mipp, double timeLeft );

static int getXIdx( const MIPCompact *mipp, int j, int m, int t );

/* for fast search after model creation */
static void storeVarNames( int n, const char **names, MIPCompact *mipp );


#ifdef CBC
int lp_callbacks( LinearProgram *lp, int whereFrom, const int *origCols, LinearProgram *origLP, void *data );
int lp_callback_prec( LinearProgram *lp, int whereFrom, const int *origCols, LinearProgram *origLP, void *data );
int lp_callback_rr( LinearProgram *lp, int whereFrom, const int *origCols, LinearProgram *origLP, void *data );
int lp_callback_CLIQUE( LinearProgram *lp, int whereFrom, const int *origCols, LinearProgram *origLP, void *data);
#endif // CBC


#ifdef CBC

int lp_callback_prec( LinearProgram *lp, int whereFrom, const int *origCols, LinearProgram *origLP, void *data )
{

    double _time=0;

    const MIPCompact *mipC = data;
    assert(mipC!=NULL );
    //    CutPool *cutP = mipC->cutP;
    //   Results *res = mipC->res;
    //    const Instance *inst = mipC->inst;
    //assert(inst!=NULL );
    //    assert(cutP!=NULL );

    //  lp_add_cuts_precedence( lp, origCols, origLP, mipC->inst, _time, 0, mipC->lifting, mipC->maxTJM, mipC->maxTJ, mipC->maxcuts, cutP, res );

    return 0;
}

int lp_callback_rr( LinearProgram *lp, int whereFrom, const int *origCols, LinearProgram *origLP, void *data)
{

    double _time=0;

    MIPCompact *mipC = data;
    assert(mipC!=NULL );
    //    CutPool *cutP = mipC->cutP;
    //    Results *res = mipC->res;
    //    const Instance *inst = mipC->inst;
    // assert(inst!=NULL );
    //    assert(cutP!=NULL );

    //    lp_add_cuts_cover( lp, origCols, origLP, MipC_inst(mipC), _time, MipC_getContinuous(mipC), MipC_getLifting(mipC), cutP, res );

    return 0;
}

int lp_callback_CLIQUE( LinearProgram *lp, int whereFrom, const int *origCols, LinearProgram *origLP, void *data)
{

    double _time=0;

    MIPCompact *mipC = data;
    assert(mipC!=NULL );
    //    CutPool *cutP = mipC->cutP;
    //    Results *res = mipC->res;
    //    const Instance *inst = mipC->inst;
    //assert(inst!=NULL );
    //    assert(cutP!=NULL );

    //    lp_add_cuts_conflicts_clique( lp, origCols, origLP, MipC_inst(mipC), _time,  MipC_getContinuous(mipC), mipC->maxTJM, mipC->maxcuts, cutP,res );

    return 0;
}

int lp_callbacks( LinearProgram *lp, int whereFrom, const int *origCols, LinearProgram *origLP, void *data)
{

    double _time=0;

    MIPCompact *mipC = data;
    assert(mipC!=NULL );
    //    CutPool *cutP = mipC->cutP;
    //    Results *res = mipC->res;
    //    const Instance *inst = mipC->inst;
    //assert(inst!=NULL );
    //    assert(cutP!=NULL );

    //    lp_add_cuts_cover( lp, origCols, origLP, MipC_inst(mipC), _time, MipC_getContinuous(mipC),  MipC_getLifting(mipC), cutP, res);
    //    lp_add_cuts_precedence( lp, origCols, origLP, MipC_inst(mipC), _time,  MipC_getContinuous(mipC), MipC_getLifting(mipC), mipC->maxTJM, mipC->maxTJ, mipC->maxcuts, cutP,res );
    //    lp_add_cuts_conflicts_clique( lp, origCols, origLP, MipC_inst(mipC), _time,  MipC_getContinuous(mipC), mipC->maxTJM, mipC->maxcuts, cutP, res );

    return 0;
}
#endif // CBC

void MipC_setVariablesZero(MIPCompact *mipC)
{
    //lp_write_lp(mipC->mip,"A-3before.lp");
    double *rc = lp_reduced_cost(mipC->mip);
    double value = 0.0;
    int tms = 0;
    if(Par_getTypePSP(MipC_getPar(mipC)) == 2)
        tms = mipC->tmsSum;

    for(int c = 0 ; c < lp_cols(mipC->mip); c++)
    {
        if(lp_obj_value(mipC->mip)+rc[c] >= mipC->tpdSum+0.00001+(tms*1e-5))
        {
            char name[256];
            lp_col_name( mipC->mip, c, name );
            if (tolower(name[0])=='x')
            {
                // printf("\n name[0] %s, lp_obj_value(mipC->mip) %f +rc[c] %f > mipC->tpdSum %d",name,lp_obj_value(mipC->mip),rc[c],mipC->tpdSum);
                lp_fix_col(mipC->mip,c, value);
            }
        }
    }
    //lp_write_lp(mipC->mip,"A-3after.lp");


}

MIPCompact *MipC_create( Instance *inst, Parameters *par, Results *res, const int idxProj, const int tpdSum, const int tmsSum, double timeLeft )
{
    assert(inst!=NULL);
    double _time = 0;
    double startT = omp_get_wtime();

    MIPCompact *mipp;
    ALLOCATE_INI( mipp, MIPCompact );
    Inst_setSumTPD(inst, tpdSum);
    Inst_setSumTMS(inst, tmsSum);
    mipp->inst = inst;
    mipp->mip  = lp_create();
    mipp->tpdSum = tpdSum;
    mipp->tmsSum = tmsSum;
    mipp->idxProj = idxProj;
    mipp->dnames = DInt_create( 7919, -1 );
    mipp->bestSol = Sol_create(inst);
    mipp->hasIniSol = False;
    mipp->par = par;
#ifdef CBC
    if(par->continuous)
        lp_set_ownCutPool(mipp->mip,  '1');
#endif // CBC

    int nJobs = Inst_nJobs(inst);

    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0)
    {
        printf( "\nMSM Create: Time is over %f Time Left %f \n", _time, timeLeft);
        exit(0);
    }

    mipp->msm = MSM_create(inst,0,nJobs-1, _time);

    //  mipp->res = Res_create();


    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0)
    {
        printf( "\nMSM Create: Time is over %f Time Left %f \n", _time, timeLeft);
        exit(0);
    }


    int maxT = 0;



    if (idxProj>=0)
    {
        assert( idxProj<Inst_nProjects(inst) );
        mipp->project = Inst_project( inst, idxProj );
        mipp->idxFirstJob = Project_idxFirstJob( mipp->project );
        mipp->nJobs = Project_nJobs( mipp->project );
        maxT = MAX( maxT, Project_releaseDate(mipp->project )+Project_criticalPath(mipp->project )+tpdSum);//janniele
    }
    else
    {
        mipp->project = NULL;
        mipp->idxFirstJob = 0;
        mipp->nJobs = Inst_nJobs( inst );
        for ( int j=0 ; (j<Inst_nProjects(inst)) ; ++j )
        {
            const Project *proj = Inst_project( inst, j );
            maxT = MAX( maxT, Project_releaseDate(proj)+Project_criticalPath(proj)+tpdSum );
        }
    }
    mipp->maxT = maxT;

    ALLOCATE_VECTOR( mipp->mipstrengthening, LinearProgram***,Inst_nProjects(mipp->inst) );
    for(int p = 0; p < Inst_nProjects(mipp->inst) ; p++)
    {
        ALLOCATE_VECTOR( mipp->mipstrengthening[p], LinearProgram***,(maxT+1) );
        for(int t = 0; t <= maxT ; t++)
        {
            ALLOCATE_VECTOR( mipp->mipstrengthening[p][t], LinearProgram**,Inst_nResR(inst));
        }
    }
    /* ALLOCATE_VECTOR( mipp->mipstrengthening, LinearProgram**,Inst_nProjects(mipp->inst) );
     for(int p = 0; p < Inst_nProjects(mipp->inst) ; p++){
         ALLOCATE_VECTOR( mipp->mipstrengthening[p], LinearProgram**,(maxT+1) );
     }*/

    ALLOCATE_VECTOR( mipp->idxResR, VecInt**, Inst_nResR(inst) );
    ALLOCATE_VECTOR_INI( mipp->idxResR[0], VecInt*, Inst_nResR(inst)*(maxT+1) );
    for ( int i=1 ; (i<Inst_nResR(inst)) ; ++i )
        mipp->idxResR[i] = mipp->idxResR[i-1] + (maxT+1);

    for ( int r=0 ; (r<Inst_nResR(inst)) ; ++r )
        for ( int t=0 ; (t<(maxT+1)) ; ++t )
            mipp->idxResR[r][t] = VInt_create();

    ALLOCATE_VECTOR( mipp->coefResR, VecDbl**, Inst_nResR(inst) );
    ALLOCATE_VECTOR_INI( mipp->coefResR[0], VecDbl*, Inst_nResR(inst)*(maxT+1) );
    for ( int i=1 ; (i<Inst_nResR(inst)) ; ++i )
        mipp->coefResR[i] = mipp->coefResR[i-1] + (maxT+1);

    for ( int r=0 ; (r<Inst_nResR(inst)) ; ++r )
        for ( int t=0 ; (t<(maxT+1)) ; ++t )
            mipp->coefResR[r][t] = VDbl_create();

    ALLOCATE_VECTOR( mipp->jIdx, VecInt*, nJobs );
    for ( int i=0 ; (i<nJobs) ; ++i )
        mipp->jIdx[i] = VInt_create();

    ALLOCATE_VECTOR( mipp->dJ, VecInt*, nJobs );
    for ( int i=0 ; (i<nJobs) ; ++i )
        mipp->dJ[i] = VInt_create();

    ALLOCATE_VECTOR( mipp->jtIdx, VecInt **, nJobs );
    ALLOCATE_VECTOR( mipp->jtIdx[0], VecInt *, nJobs*(maxT+1) );
    for ( int j=1 ; (j<nJobs) ; ++j )
        mipp->jtIdx[j] = mipp->jtIdx[j-1]+(maxT+1);

    int maxModes = 0;
    for ( int j=0 ; (j<nJobs) ; ++j )
    {
        for ( int t=0 ; (t<(maxT+1)) ; ++t )
            mipp->jtIdx[j][t] = VInt_create();

        maxModes = MAX( maxModes, Job_nModes( Inst_job( inst, j ) ) );
    }

    mipp->maxModes = maxModes;

    ALLOCATE_VECTOR( mipp->maxTJM, int*, nJobs );
    ALLOCATE_VECTOR_INI( mipp->maxTJM[0], int, nJobs*maxModes );
    for ( int i=1 ; (i<nJobs) ; ++i )
        mipp->maxTJM[i] = mipp->maxTJM[i-1]+maxModes;

    ALLOCATE_VECTOR_INI( mipp->maxTJ, int, nJobs );

    ALLOCATE_VECTOR( mipp->idxResNR, VecInt*, Inst_nResN( inst ) );
    ALLOCATE_VECTOR( mipp->coefResNR, VecDbl*, Inst_nResN( inst ) );
    for ( int i=0 ; i<Inst_nResN( inst ) ; ++i )
    {
        mipp->idxResNR[i]  = VInt_create();
        mipp->coefResNR[i] = VDbl_create();
    }

    ALLOCATE_VECTOR( mipp->yIdx, int, nJobs );
    ALLOCATE_VECTOR( mipp->dIdx, int, nJobs );
    ALLOCATE_VECTOR( mipp->uIdx, int*, nJobs );
    ALLOCATE_VECTOR( mipp->uIdx[0], int, nJobs*maxModes );
    FILL(mipp->uIdx[0], 0, Inst_nJobs(inst)*maxModes, -1 );
    for ( int j=1 ; (j<Inst_nJobs(inst)) ; ++j )
        mipp->uIdx[j] = mipp->uIdx[j-1] + maxModes;

    ALLOCATE_VECTOR( mipp->jmIdx, VecInt **, nJobs);
    ALLOCATE_VECTOR( mipp->jmIdx[0], VecInt *, nJobs*(maxModes) );

    for ( int j=1 ; (j<nJobs) ; ++j )
        mipp->jmIdx[j] = mipp->jmIdx[j-1]+(maxModes);


    ALLOCATE_VECTOR( mipp->coefR, double***, nJobs );
    for ( int j=0 ; (j<nJobs) ; ++j )
    {
        ALLOCATE_VECTOR( mipp->coefR[j], double**, maxModes );
        for ( int m=0 ; (m<(maxModes)) ; ++m )
        {
            ALLOCATE_VECTOR( mipp->coefR[j][m], double*, Inst_nResR( inst ) );
            mipp->jmIdx[j][m] = VInt_create();
            for ( int r=0 ; r<Inst_nResR( inst ) ; ++r )
                ALLOCATE_VECTOR( mipp->coefR[j][m][r], double, (maxT+1) );
        }
    }

    ALLOCATE_VECTOR_INI( mipp->jobIntegral, int, nJobs );
    ALLOCATE_VECTOR_INI( mipp->jobFixed, int,nJobs);
    ALLOCATE_VECTOR_INI( mipp->jmtIntegral, IntTriple,nJobs );
    ALLOCATE_VECTOR_INI( mipp->jmtFractional, IntTriple, nJobs*maxModes*(maxT+1) );
    ALLOCATE_VECTOR_INI( mipp->jmtFixed, IntTriple, nJobs);

    for(int j = 0 ; j < Inst_nJobs(inst) ; j++)
        mipp->jmtIntegral[j].j = -1;


    mipp->minRoundUp = 0.005;
    mipp->nIntegral = 0;
    mipp->nFractional = 0;
    mipp->nFixed = 0;
    mipp->kiterations = 100;


    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0)
    {
        printf( "\nAlocations: Time is over %f Time Left %f \n", _time, timeLeft);
        exit(0);
    }

    if(Par_getVariablesZ(par)>=1){
        createXZ( mipp, _time );
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreateXZ: Time is over %f \n", _time);
            exit(0);
        }

        double timestrengthening = omp_get_wtime();
        MipC_strengthening(mipp, _time, nJobs);
        timestrengthening = (double) (omp_get_wtime()-timestrengthening);
        Res_setTimestrengthening(res,timestrengthening);
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\n MipC_strengthening: Time is over %f \n", _time);
            exit(0);
        }

       //createResConsRLiftByZ( mipp, _time );
//createResConsRByZ
        if(Par_getVariablesZ(par)==1){
            createResConsRByZ( mipp, _time );
        }else
            createResConsRLiftByZ( mipp, _time );

        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\n createResConsRLiftByZ: Time is over %f \n", _time);
            exit(0);
        }
    }else{

        createX( mipp, _time );
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreateX: Time is over %f \n", _time);
            exit(0);
        }

        createResConsR( mipp, _time );
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\n createResConsR: Time is over %f \n", _time);
            exit(0);
        }

    }



    createModeSelCons( mipp, _time);
    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0)
    {
        printf( "\n createModeSelCons: Time is over %f \n", _time);
        exit(0);
    }

    createY( mipp, _time );
    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0)
    {
        printf( "\n createY: Time is over %f \n", _time);
        exit(0);
    }

    createD( mipp, _time );
    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0)
    {
        printf( "\ncreateD: Time is over %f \n", _time);
        exit(0);
    }

    createU( mipp, _time);
    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0)
    {
        printf( "\ncreateU: Time is over %f \n", _time);
        exit(0);
    }

    createConsLnkXU( mipp,_time );
    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0)
    {
        printf( "\ncreateConsLnkXU: Time is over %f \n", _time);
        exit(0);
    }

    createResConsNR( mipp,_time );
    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0)
    {
        printf( "\ncreateResConsNR: Time is over %f \n", _time);
        exit(0);
    }

    createConsLinkXY( mipp, _time );
    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0)
    {
        printf("\ncreateConsLinkXY: Time is over %f \n", _time);
        exit(0);
    }

    if(Par_getVariablesZ(par)==1){
        createConsLinkXZ( mipp, _time );
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf("\ncreateConsLinkXZ: Time is over %f \n", _time);
            exit(0);
        }
    }

    createConsLinkXD( mipp, _time );
    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0)
    {
        printf( "\ncreateConsLinkXD: Time is over %f \n", _time);
        exit(0);
    }

    switch ( precConsType )
    {
    case 0 :
    {
        createPrecCons0( mipp, _time );
        break;
    }
    case 1 :
    {
        createPrecCons1( mipp, _time );
        break;
    }
    case 2 :
    {
        createPrecCons2( mipp, _time );
        break;
    }
    }

    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0)
    {
        printf( "\ncreatePrecCons: Time is over %f \n", _time);
        exit(0);
    }

    addFOConstants( mipp, _time);

    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0)
    {
        printf( "\naddFOConstants: Time is over %f \n", _time);
        exit(0);
    }

    createConsSelU( mipp, _time);

    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0)
    {
        printf( "\ncreateConsSelU: Time is over %f \n", _time);
        exit(0);
    }

    mipp->hasSolution = False;
    mipp->tpd = -1.0;
    mipp->sol = Sol_create(inst);

    if(Par_getTypePSP(par)==2) lp_set_abs_mip_gap(mipp->mip, 1e-5);
    else lp_set_abs_mip_gap(mipp->mip, 0.99);

    if(tpdSum>0)
    {
        double relativeGap = (double) 1-( ((double)tpdSum-0.00001)/(double)tpdSum  );
        lp_set_rel_mip_gap( mipp->mip, relativeGap);
    }


    lp_set_max_reduced_cost(mipp->mip, par->maxReducedCost);
    if(par->maxNode)
        lp_set_max_nodes(mipp->mip, par->maxNode);

//    computeUnitsResourcesES(inst,mipp->maxT,mipp->maxTJM);

#ifdef CBC
    /*  if(mipp->cutPrec&&mipp->cutRR&&mipp->cutCLIQUE)
          lp_set_callback( mipp->mip, lp_callbacks, (void *) inst );
      else if(mipp->cutPrec)
          lp_set_callback( mipp->mip, lp_callback_prec, (void *) inst );
      else if(mipp->cutRR)
          lp_set_callback( mipp->mip, lp_callback_rr, (void *) inst );
      else if(mipp->cutCLIQUE)
          lp_set_callback( mipp->mip, lp_callback_CLIQUE, (void *) inst );*/
#endif // CBC



    return mipp;
}

int MipC_bt_check_constraints(const Instance *inst, const Job *job, const Mode *mode,  int *jobmodeinsubset, int *totalusesRR, int *totalusesN, int nJobs, const int *capRR, const int *capNR)
{

    int valid = 1;


    if(jobmodeinsubset[Job_index(job)]!= -1)
    {
      //   printf("Job %d already in subset with mode %d \n", Job_index(job), jobmodeinsubset[Job_index(job)]); fflush(stdout);
        //valid = 0;
        return -1;
    }


    for(int ii = 0 ; ii < nJobs; ii++)
    {
        if(jobmodeinsubset[ii]!=-1)
        {
            if(Job_hasIndSucc(job,ii) || Job_hasIndPred(job,ii))
            {
     //            printf("Job %d, suc/pred %d valid ==0\n", Job_index(job),ii); fflush(stdout);
                //valid = 0;
                //break;
                return -2;
            }
        }
    }

    for(int r = 0; r< Mode_nResR(mode) ; r++)
    {
        int idxR =  Mode_idxResR(mode,r);
        int uses =  Mode_useResR(mode, r);
        totalusesRR[idxR] += uses;
        //if(totalusesRR[idxR] > Inst_capResR(inst,idxR) )
        if(totalusesRR[idxR] > capRR[idxR] )
        {
       //     printf(" job %d mode %d totalusesRR[idxR] %d + uses %d > Inst_capResR(inst,idxR) %d\n", Job_index(job), Mode_index(mode), totalusesRR[idxR], uses, Inst_capResR(inst,idxR)); fflush(stdout);
         //   printf(" RR valid ==0\n"); fflush(stdout);
            valid =  0;
        }
    }
    if(valid == 0 ) return -3;

    for(int rn = 0; rn< Mode_nResN(mode) ; rn++)
    {
        int idxRN = Mode_idxResN(mode,rn);
        int uses =  Mode_useResN(mode, rn);
        totalusesN[idxRN] +=uses;
        if(totalusesN[idxRN]  > capNR[idxRN] )
        {
           // printf(" RN valid ==0\n"); fflush(stdout);
            valid = 0;
        }

    }
    if ( valid == 0) return -4;

    return valid;
}

void MipC_bt_undo_constraints(const Job *job, const Mode *mode, int *totalusesRR, int *totalusesN,  int *jobinsubset, int valid)
{
    if(valid == 1 || valid==-3 || valid==-4){
        for(int r = 0; r< Mode_nResR(mode) ; r++)
        {
            int idxR =  Mode_idxResR(mode,r);
            int uses =  Mode_useResR(mode, r);
            totalusesRR[idxR] -= uses;
        }
    }

    if(valid == 1 || valid==-4){
        for(int rn = 0; rn< Mode_nResN(mode) ; rn++)
        {
            int idxRN = Mode_idxResN(mode,rn);
            int uses =  Mode_useResN(mode, rn);
            totalusesN[idxRN] -=uses;
        }
    }
}

void MipC_bt_clear_vectors(const Instance *inst, int *totalusesRR, int *totalusesN,  int *jobinsubset)
{
    FILL(totalusesRR,0,Inst_nResR(inst),0.0);
    FILL(totalusesN,0,Inst_nResN(inst),0.0);
    FILL(jobinsubset,0,Inst_nJobs(inst),0);
}

/* inputs backtracking
 sj                                     - set job vector
 sm                                     - set mode vector
 tj                                     - save partial solutions job
 tm                                     - save partial solutions mode
 sjsm_size                              - set job mode size
 t_size                                 - tuplet size so far
 totalusesRR, totalusesN,  jobinsubset  -vectors to compute constraints
 ite                                    - element node
*/
int MipC_subset_bt(const Instance *inst, VecInt *sj, VecInt *sm, VecInt* tj, VecInt* tm, VecInt *initsubset,
                   int sjsm_size, int *jobmodeinsubset, int *totalusesRR, int *totalusesN,
                   int *maxnumit, int ite, int nJobs, const int* capRR, const int* capNR)
{

    // double _time = omp_get_wtime();
    int value = 0;
    *maxnumit = *maxnumit+1;
    // printf("maxnumit %d", *maxnumit); getchar();
    if(*maxnumit >= 200000) //1000000
    {
        //printf("max number of iterations achived\n");
       // fflush(stdout); //getchar();
        value = -1;
        return value;
    }
    else if( ite == sjsm_size )
    {
        value = 1;
        return value;

    }
    else
    {

        for( int i = ite; i < sjsm_size; i++ )
        {

            const Job *job = Inst_job(inst,VInt_get(sj,i));
            const Mode *mode = Job_mode(job,VInt_get(sm,i));

            //int valid = 1;
            int valid = MipC_bt_check_constraints(inst, job, mode, jobmodeinsubset, totalusesRR, totalusesN, nJobs, capRR,capNR);

            if(valid==1)
            {


                int oldvaluejmss = jobmodeinsubset[Job_index(job)];
                jobmodeinsubset[Job_index(job)]= Mode_index(mode);
                // printf("(%d,%d)  sb %d\n", Job_index(job) , Mode_index(mode), VInt_size(initsubset));

                // consider next level node (along depth)
                value = MipC_subset_bt(inst, sj, sm, tj, tm, initsubset, sjsm_size, jobmodeinsubset, totalusesRR, totalusesN, maxnumit,  i+1, nJobs, capRR, capNR);

                if(value==-1)
                {
                 //   printf("stopping by max iterations ");
                   // MipC_bt_undo_constraints(job, mode, totalusesRR, totalusesN,  jobmodeinsubset);
                    break;
                }

                VInt_pushBack(initsubset,VInt_size(tj));
                for(int j = 0 ; j < Inst_nJobs(inst) ; j++ )
                {
                    if(jobmodeinsubset[j]!=-1)
                    {
                        VInt_pushBack(tj, j);
                        VInt_pushBack(tm, jobmodeinsubset[j]);
                        // printf("-(%d,%d)  sb %d\n", j ,jobmodeinsubset[j], VInt_size(initsubset));// getchar();
                    }
                }
                jobmodeinsubset[Job_index(job)]= oldvaluejmss;
            }

            MipC_bt_undo_constraints(job, mode, totalusesRR, totalusesN,  jobmodeinsubset, valid);

        }
    }

    return value;
}

void MipC_strengthening(MIPCompact *mipp, double timelimit, int nJobs)
{
    double initT = omp_get_wtime();
    double _time =  (double) timelimit - ( (double)omp_get_wtime() - initT );
    int stopbytime = 0;
    const int *capRR = Inst_getCapRR(mipp->inst);
    const int *capNR = Inst_getCapNR(mipp->inst);
    for(int p = 0 ; p < Inst_nProjects(mipp->inst); p++)
    {
//        const Project *proj = Inst_project(mipp->inst,p);
        VecInt **setPJ;
        VecInt **setPM;
        ALLOCATE_VECTOR(setPJ,VecInt*,mipp->maxT);
        ALLOCATE_VECTOR(setPM,VecInt*,mipp->maxT);
      //  printf("Project %d:\n", p);  fflush(stdout);


        int *equals;
        ALLOCATE_VECTOR_INI(equals,int,mipp->maxT);
        int ***z;
        ALLOCATE_VECTOR(z,int**,mipp->maxT);
        int **ptra;
        ALLOCATE_VECTOR_INI(ptra,int*,mipp->maxT);

        for(int t = 0 ; t < mipp->maxT; t++)
        {
            setPJ[t]  = VInt_create();
            setPM[t]  = VInt_create();
            ALLOCATE_VECTOR(z[t],int*,nJobs);
            ALLOCATE_VECTOR_INI(ptra[t],int,Inst_nResR(mipp->inst));

            //   printf("\nsetP[%d]:\n", t);  fflush(stdout);
            int cont = 0;
            int equal = 1;

            // for(int j = 0 ; j < Project_nJobs(proj) ; j++)
            for(int j = 0 ; j < nJobs ; j++)
            {

                const Job *job = Inst_job(mipp->inst,j);
                int pp = Job_project(job);
                ALLOCATE_VECTOR(z[t][j],int,Job_nModes(job));
                FILL(z[t][j],0,Job_nModes(job),-1);
                if(pp == p)
                {

                    for(int m = 0 ; m < Job_nModes(job) ; m++)
                    {
                        char name[256];
                        sprintf( name, "z(%d,%d,%d)", Job_index(job),m,t);
                        int idxZ = lp_col_index(mipp->mip,name);
                        if(idxZ !=-1)
                        {
                            VInt_pushBack(setPJ[t],Job_index(job));
                            VInt_pushBack(setPM[t],m);
                            //  printf("(%d,%d) ", Job_index(job), m);  fflush(stdout);
                            if( equal==1 && t>0)
                            {
                                if( (VInt_size(setPJ[t-1]) <= cont) )
                                    equal = 0;
                                else if(VInt_get(setPJ[t-1],cont) != VInt_get(setPJ[t],cont) ||  (VInt_get(setPM[t-1],cont) != VInt_get(setPM[t],cont)))
                                    equal = 0;
                            }
                            cont++;

                        }
                    }
                }
            }
            if(equal)
                equals[t] = 1;
        }





        //   getchar();

        int contf = 0;
        int t;
        VecInt *idxresrz = VInt_create();
        VecInt *startsresrz = VInt_create();
        VecDbl *coefresrz = VDbl_create();
        VecDbl *rhsresrz = VDbl_create();
        VecStr *namesresrz = VStr_create(20);
        char  *senseresrz;
        int contconstraints = 0;
        int contc = 0;


        for(t = 0 ; t < mipp->maxT; t++)
        {
            int size = VInt_size(setPJ[t]);
            _time =  (double) timelimit - ( (double)omp_get_wtime() - initT );

            if(_time <=0)
            {
                stopbytime = 1;
                // printf("time is over computing adding row to mip lift\n");
                goto TIMEOVER;
            }

            if(size<=0)
            {
            //    printf("size %d", size); fflush(stdout);
                equals[t]=-1;
                continue;
            }

             if(contf == 0)
                equals[t] = 0;

            contf++;


            int value = -1;
            if(equals[t]==1)
            {

                if(equals[t-1]!=-1){
                  //  printf("equal P[%d]: %d ---\n", t, equals[t]); fflush(stdout);
                    int contrg = 0;
                    int end = Inst_nResR(mipp->inst);
                    if(p+1<Inst_nProjects(mipp->inst))
                        end = Inst_idxResRProj(mipp->inst,p+1);

                    int contOneResource = 0;
                    for(int r = 0 ; r < end ; r++)
                    {
                        mipp->mipstrengthening[p][t][r] = mipp->mipstrengthening[p][t-1][r];
                        contrg++;
                        if(Par_getTypePSP(mipp->par)==2)
                        {
                            if(contrg == Inst_nResRGlobal(mipp->inst) && Inst_nResRGlobal(mipp->inst) < Inst_nResR(mipp->inst))
                            {
                                r=Inst_idxResRProj(mipp->inst,p)-1;
                                if(contOneResource==0)
                                {
                                    int idR = contrg;
                                    contOneResource=1;
                                    if(contrg==r+1) idR = end;
                                    mipp->mipstrengthening[p][t][idR] = mipp->mipstrengthening[p][t-1][idR];
                                }
                            }
                        }
                    }
                    value = 1;
                }else{
                    equals[t]=-1;
                    value = -1;
                }
            }
            else
            {
                if (equals[t]==0)
                {
                 //   printf("not equal P[%d]: %d\n", t,  equals[t]);

                    int *jobmodeinsubset;
                    ALLOCATE_VECTOR(jobmodeinsubset,int,nJobs);
                    FILL(jobmodeinsubset,0,nJobs,-1);

                    VecInt *combJ =VInt_create();
                    VecInt *combM =VInt_create();
                    VecInt *initsubset =VInt_create();

                    int *totalusesN;
                    int *totalusesRR;

                    ALLOCATE_VECTOR_INI(totalusesN,int,Inst_nResN(mipp->inst));
                    ALLOCATE_VECTOR_INI(totalusesRR,int,Inst_nResR(mipp->inst));


                  //  printf("Starting Backtracking t %d:\n", t); fflush(stdout);

                    int maxnumit = 0;
                    int ite = 0;
                    value = MipC_subset_bt(mipp->inst, setPJ[t], setPM[t], combJ, combM, initsubset, size, jobmodeinsubset, totalusesRR, totalusesN, &maxnumit, ite, nJobs, capRR, capNR);
                  //  printf("value %d\n", value); fflush(stdout);

                    if(value!=-1)
                    {

                        int contrg = 0;
                        int end = Inst_nResR(mipp->inst);
                        if(p+1<Inst_nProjects(mipp->inst))
                            end = Inst_idxResRProj(mipp->inst,p+1);
                        int contOneResource = 0;
                        for(int r = 0 ; r < end ; r++)
                        {
                            int capacity = Inst_capResR(mipp->inst,r);
                            mipp->mipstrengthening[p][t][r] = lp_create();
                            ptra[t][r] = 1;
                            //mipp->mipstrengthening[p][t][r] = lp_create();
                            VecStr *namesZ = VStr_create( STR_SIZE );

                            double valueobjz[size], lb[size], ub[size];
                            char integer[size];
                            FILL( integer, 0, size, 1.0 );

                            char name[STR_SIZE];
                            //  printf("   size %d",  size); fflush(stdout);
                            for(int i = 0 ; i < size ; i++)
                            {
                                sprintf(name, "z(%d,%d)", VInt_get(setPJ[t],i), VInt_get(setPM[t],i) );
                                VStr_pushBack( namesZ, name );
                                int j =  VInt_get(setPJ[t],i);
                                int m = VInt_get(setPM[t],i);
                                const Job *jobsb = Inst_job(mipp->inst,j);
                                const Mode *modesb = Job_mode(jobsb,m);
                                int idxR = Mode_idxResROnMode(mipp->inst, modesb,r);
                                lb[i] = 0;
                                if(idxR != -1) lb[i] = Mode_useResR(modesb,idxR);
                                ub[i] = capacity;
                                valueobjz[i]= -1.0;
                                z[t][j][m] = i;
                            }
                            lp_add_cols( mipp->mipstrengthening[p][t][r], size, valueobjz, lb, ub, integer, VStr_ptr(namesZ) );

                            //  printf("   VInt_size(initsubset) %d  VInt_size(combJ) %d",  VInt_size(initsubset), VInt_size(combJ)); fflush(stdout); getchar();
                            VStr_free(&namesZ);
                            int nrows = VInt_size(initsubset);
                            int *starts;//[nrows+1];
                            ALLOCATE_VECTOR(starts,int,nrows+1);
                            int *idx;
                            ALLOCATE_VECTOR(idx,int,VInt_size(combJ));
                            double *coef;//[ VInt_size(combJ)];
                            ALLOCATE_VECTOR(coef,double,VInt_size(combJ));
                           // FILL( coef, 0,  VInt_size(combJ), 1.0 );
                            char *senses;//[nrows];
                            ALLOCATE_VECTOR(senses,char,nrows);
                            double *rhss;//[nrows];
                            ALLOCATE_VECTOR(rhss,double,nrows);
                         //   printf("nrows %d  VInt_size(combJ) %d ", nrows,  VInt_size(combJ)); fflush(stdout);
                            char **names;
                            ALLOCATE_VECTOR(names, char*, nrows);
                            int cont = 0;

                            for(int c = 0 ; c <  nrows ; c++)
                            {
                                _time =  (double) timelimit - ( (double)omp_get_wtime() - initT );
                                if(_time <=0)
                                {
                                    free(totalusesN);
                                    free(totalusesRR);

                                    free(jobmodeinsubset);
                                    free(idx);
                                    free(coef);
                                    free(senses);
                                    free(rhss);
                                    free(starts);
                                    for(int i = 0 ; i < c ; i++) free(names[i]);
                                    free(names);
                                    VInt_free(&combJ);
                                    VInt_free(&combM);
                                    VInt_free(&initsubset);
                                    equals[t]=-1;
                                    stopbytime = 1;
                                    //    printf("time is over computing adding row to mip lift\n");
                                    goto TIMEOVER;
                                }
                                ALLOCATE_VECTOR(names[c], char, 20);
                                int sizesb = 0;
                                if(c==VInt_size(initsubset)-1)
                                    sizesb = VInt_size(combJ)-VInt_get(initsubset,c);
                                else
                                    sizesb = VInt_get(initsubset,c+1)-VInt_get(initsubset,c);
                               // int idx[sizesb];
                                //double coef[sizesb];
                                //FILL( coef, 0, sizesb, 1.0 );

                                sprintf( names[c], "ccomb(%d)", c);
                              //  printf("\nnames[%d] %s ", c, names[c]); fflush(stdout);
                                starts[c] = cont;

                                senses[c] = 'L';
                                rhss[c] = capacity;


                                for(int i = VInt_get(initsubset,c) ; i < VInt_get(initsubset,c)+sizesb ; i++ )
                                {
                                    int j = VInt_get(combJ,i);
                                    int m = VInt_get(combM,i);
                                    //char name[256];
                                    //sprintf( name, "z(%d,%d)", j,m);
                                    int idxZ = z[t][j][m]; //lp_col_index(mipp->mipstrengthening[p][t][r],name);
                                    if(idxZ !=-1)
                                    {
                                        idx[cont] = idxZ;
                                        coef[cont] = 1.0;
                                      //  printf("%f * %d ", coef[cont],idx[cont] );
                                        cont++;
                                    }
                                }

                            }
                            starts[nrows] = cont;
                            lp_add_rows(mipp->mipstrengthening[p][t][r], nrows,starts,idx,coef,senses,rhss,names);
                            lp_set_print_messages(mipp->mipstrengthening[p][t][r],0);
                            _time =  (double) timelimit - ( (double)omp_get_wtime() - initT );
                            lp_set_max_seconds(mipp->mipstrengthening[p][t][r], _time);
                            //printf("lp_optimize_as_continuous\n"); fflush(stdout);
                            free(idx);
                            free(coef);
                            free(senses);
                            free(rhss);
                            free(starts);
                            for(int c = 0 ; c <  nrows ; c++) free(names[c]);
                            free(names);

                            lp_set_nThreads(mipp->mipstrengthening[p][t][r],5);
                            lp_optimize_as_continuous(mipp->mipstrengthening[p][t][r]);
                            // int status = lp_optimize(mipp->mipstrengthening[p][t][r]);
                            //lp_set_nThreads(mipp->mipstrengthening[p][t][r],1);
                            //lp_set_max_nodes(mipp->mipstrengthening[p][t][r],3);
                            //lp_set_max_seconds(mipp->mipstrengthening[p][t][r],1);

                           // printf("p %d, t %d, r %d end %d\n", p,t,r,end);  fflush(stdout);
                            int oldr = r;
                            contrg++;
                            //printf("Inst_nResRGlobal(mipp->inst) %d Inst_nResR(mipp->inst) %d" , Inst_nResRGlobal(mipp->inst),Inst_nResR(mipp->inst)); getchar();
                            if(Par_getTypePSP(mipp->par)==2)
                            {
                                if(contrg == Inst_nResRGlobal(mipp->inst) && Inst_nResRGlobal(mipp->inst) < Inst_nResR(mipp->inst))
                                {
                                    r=Inst_idxResRProj(mipp->inst,p)-1;
                                    if(contOneResource==0)
                                    {
                                        int idR = contrg;
                                        contOneResource=1;

                                        if(contrg==r+1) idR = end;
                                        mipp->mipstrengthening[p][t][idR] = lp_clone(mipp->mipstrengthening[p][t][oldr]);
                                        ptra[t][idR] = 1;
                                        int cap = Inst_capResR(mipp->inst, idR);

                                        for ( int i=0 ; (i<lp_rows(mipp->mipstrengthening[p][t][idR])) ; ++i )
                                        {
                                            lp_set_rhs(mipp->mipstrengthening[p][t][idR],i,cap);
                                        }
                                        for ( int i=0 ; (i<lp_cols(mipp->mipstrengthening[p][t][idR])) ; ++i )
                                        {
                                            lp_set_col_bounds(mipp->mipstrengthening[p][t][idR],i,0,cap);
                                        }
                                        _time =  (double) timelimit - ( (double)omp_get_wtime() - initT );
                                        lp_set_max_seconds(mipp->mipstrengthening[p][t][idR], _time);
                                        lp_set_print_messages(mipp->mipstrengthening[p][t][idR],0);
                                        lp_set_nThreads(mipp->mipstrengthening[p][t][idR],5);
                                        lp_optimize_as_continuous(mipp->mipstrengthening[p][t][idR]);
                                       // printf("--- p %d, t %d, idR %d \n", p,t,idR); fflush(stdout);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        equals[t]=-1;
                  //      printf("invalid by it P[%d]: %d\n", t,  equals[t]);
                    }

                    free(totalusesN);
                    free(totalusesRR);

                    free(jobmodeinsubset);

                    VInt_free(&combJ);
                    VInt_free(&combM);
                    VInt_free(&initsubset);
                }
            }

            if(value !=-1 && equals[t] !=-1)
            {

                //for(int r = 0 ; r < Inst_nResR(mipp->inst) ; r++){

                int contrg = 0;
                int end = Inst_nResR(mipp->inst);
                if(p+1<Inst_nProjects(mipp->inst))
                    end = Inst_idxResRProj(mipp->inst,p+1);

                int contOneResource = 0;
                for(int r = 0; r < end ; r++)
                {
                    //   if(mipp->mipstrengthening[p][t][r]==NULL) continue;

                    //int status = lp_optimize(mipp->mipstrengthening);
                    //int hasSolution = ( ( status == LP_OPTIMAL) || ( status == LP_FEASIBLE ) );
                    // if ( 1 )
                    //{



                    //lp_set_col_bounds(mipp->mipstrengthening[p][t][r], );
                    //lp_set_rhs()


                    const double *zf = lp_x(mipp->mipstrengthening[p][t][r]);
                    char prefix[STR_SIZE];
                    int id[3];
                    int cont2 = 0 ;
                    for ( int i=0 ; (i<lp_cols(mipp->mipstrengthening[p][t][r])) ; ++i )
                    {
                        char name[STR_SIZE];
                        lp_col_name(mipp->mipstrengthening[p][t][r],i,name);
                        parseName( name, prefix, id );
                        if( zf[i]>0)
                        {
//                            if (prefix[0]=='z') {
                            int j = id[0];
                            int m = id[1];
                            if(Par_getTypePSP(mipp->par)==2)
                            {
                                if(Par_getVariablesZ(mipp->par)==1){
                                    char namev[STR_SIZE];
                                    sprintf(namev,"z(%d,%d,%d)",j,m,t);
                                    int idz = lp_col_index(mipp->mip,namev);
                                    VInt_pushBack(idxresrz,idz);
                                    VDbl_pushBack(coefresrz, zf[i]);

                                    cont2++;
                                }else if(Par_getVariablesZ(mipp->par)==2){

                                    const Job*job = Inst_job(mipp->inst,j);
                                    const Mode*mode = Job_mode(job,m);
                                    int dur = t-Mode_duration(mode)+1;
                                    if(dur<Job_est(job)) dur = Job_est(job);
                                    for( int dd = dur;dd<=t;dd++){
                                        char namex[STR_SIZE];
                                        sprintf(namex,"x(%d,%d,%d)",j,m,dd);
                                        int indxx = lp_col_index(mipp->mip,namex);
                                        if(indxx != -1){
                                            VInt_pushBack(idxresrz,indxx);
                                            VDbl_pushBack(coefresrz, zf[i]);
                                            cont2++;
                                        }
                                    }



                                }
                            }
                            mipp->coefR[j][m][r][t] = zf[i];
//                          }
                        }
                    }

                    if(Par_getTypePSP(mipp->par)==2)
                    {

                    if(cont2>0){
                        char namec[20];
                         if(Par_getVariablesZ(mipp->par)==1)
                            sprintf(namec,"resRZ(%d,%d,%d)",r,t,p);
                         else  if(Par_getVariablesZ(mipp->par)==2)
                            sprintf(namec,"resRZX(%d,%d,%d)",r,t,p);
                        VStr_pushBack(namesresrz, namec );

                        VDbl_pushBack(rhsresrz,  Inst_capResR(mipp->inst, r));
                        VInt_pushBack(startsresrz,contconstraints);

                        contconstraints +=cont2;
                        contc++;
                    }

                        // printf(" resRZ(%d,%d,%d) \n",r,t,p);
                        //lp_add_row( mipp->mip, cont2, idx, coef, namec, 'L', Inst_capResR(mipp->inst, r) );
                 //   }
                    //printf("-p %d, t %d, r %d: ", p,t,r);fflush(stdout);
                    //         if(r==0 && t==13){
                    //lp_write_lp(mipp->mipstrengthening[p][t][r], "mipstrengthening.lp");
                    //lp_write_sol(mipp->mipstrengthening[p][t][r],"mipstrengthening.sol");
                    //lp_write_lp(mipp->mip,"mipstrengtheningoori.lp");
                    //printf("print lp...\n");
                   //getchar();
                    // }
                    // } // has solution
                    //  lp_free(&mipstrengthening);
                    contrg++;
                  //  if(Par_getTypePSP(mipp->par)==2)
                   // {
                    if(contrg == Inst_nResRGlobal(mipp->inst) && Inst_nResRGlobal(mipp->inst) < Inst_nResR(mipp->inst))
                    {
                        r=Inst_idxResRProj(mipp->inst,p)-1;
                        if(contOneResource==0)
                        {
                            int idR = contrg;
                            contOneResource=1;

                            if(contrg==r+1) idR = end;
                            const double *zf = lp_x(mipp->mipstrengthening[p][t][idR]);
                            char prefix[STR_SIZE];
                            int id[3];
                            int cont3 = 0 ;
                            for ( int i=0 ; (i<lp_cols(mipp->mipstrengthening[p][t][idR])) ; ++i )
                            {
                                char name[STR_SIZE];
                                lp_col_name(mipp->mipstrengthening[p][t][idR],i,name);
                                parseName( name, prefix, id );
                                if( zf[i]>0)
                                {
                                    //                            if (prefix[0]=='z') {
                                    int j = id[0];
                                    int m = id[1];
                                   // if(Par_getTypePSP(mipp->par)==2)
                                   // {
                                   if(Par_getVariablesZ(mipp->par)==1){
                                        char namev[STR_SIZE];
                                        sprintf(namev,"z(%d,%d,%d)",j,m,t);
                                    //    printf("z(%d,%d,%d)",j,m,t);
                                        int idz = lp_col_index(mipp->mip,namev);
                                        VInt_pushBack(idxresrz, idz);
                                        VDbl_pushBack(coefresrz, zf[i]);
                                      //  printf("%\nidx %d coef %f \n", idx[cont3],coef[cont3]);
                                        cont3++;
                                   // }
                                   }else if(Par_getVariablesZ(mipp->par)==2){

                                    const Job*job = Inst_job(mipp->inst,j);
                                    const Mode*mode = Job_mode(job,m);
                                    int dur = t-Mode_duration(mode)+1;
                                    if(dur<Job_est(job)) dur = Job_est(job);
                                        for( int dd = dur;dd<=t;dd++){

                                            char namex[STR_SIZE];
                                            sprintf(namex,"x(%d,%d,%d)",j,m,dd);
                                            int indxx = lp_col_index(mipp->mip,namex);

                                            if(indxx != -1){
                                                VInt_pushBack(idxresrz, indxx);
                                                VDbl_pushBack(coefresrz, zf[i]);
//                                                printf("%\nidx %d coef %f \n", idx[cont3],coef[cont3]);
                                                cont3++;
                                            }
                                        }
                                   }
                                   mipp->coefR[j][m][idR][t] = zf[i];
                                    //                          }
                                }
                            }
                            //if(Par_getTypePSP(mipp->par)==2)
                            //{
                            if(cont3>0){
                                char namec[20];
                                 if(Par_getVariablesZ(mipp->par)==1)
                                    sprintf(namec,"resRZ(%d,%d,%d)",idR,t,p);
                                 else if(Par_getVariablesZ(mipp->par)==2)
                                    sprintf(namec,"resRZX(%d,%d,%d)",idR,t,p);

                                VStr_pushBack(namesresrz, namec );

                                VDbl_pushBack(rhsresrz,  Inst_capResR(mipp->inst, idR));
                                VInt_pushBack(startsresrz,contconstraints);

                                contconstraints +=cont3;
                                contc++;
                            }
                                 //    printf(" Other project resRZ(%d,%d,%d) \n",r,t,p);
                               // lp_add_row( mipp->mip, cont3, idx, coef, namec, 'L', Inst_capResR(mipp->inst, idR) );
                            //}
                           // printf("--p %d, t %d, idR %d: ", p,t,idR);fflush(stdout);
                             //lp_write_lp(mipp->mipstrengthening[p][t][idR], "mipstrengthening.lp");
                           // lp_write_sol(mipp->mipstrengthening[p][t][idR],"mipstrengthening.sol");
                           // lp_write_lp(mipp->mip,"mipstrengtheningoori.lp");
                           // printf("entrou print lp...\n");
                        }
                    }
                    }
                }
            }
        }

TIMEOVER:

        ALLOCATE_VECTOR(senseresrz,char,contc);

        if(Par_getTypePSP(mipp->par)==2)
        {
            VInt_pushBack(startsresrz,contconstraints);
            FILL(senseresrz,0,contc,'L');
            lp_add_rows( mipp->mip, contc, VInt_getPtr(startsresrz), VInt_getPtr(idxresrz), VDbl_getPtr(coefresrz), senseresrz, VDbl_getPtr(rhsresrz),  VStr_ptr(namesresrz));
        }

        VInt_free(&idxresrz);
        VInt_free(&startsresrz);
        VDbl_free(&coefresrz);
        VDbl_free(&rhsresrz);
        VStr_free(&namesresrz);
        free(senseresrz);

        for(int t2 = 0 ; t2 <  mipp->maxT; t2++)
        {
            VInt_free(&setPJ[t2]);
            VInt_free(&setPM[t2]);
            if(equals[t2]==0)
            {

                for(int r = 0; r < Inst_nResR(mipp->inst) ; ++r)
                {
                    if(ptra[t2][r]==1)
                        lp_free(&mipp->mipstrengthening[p][t2][r]);
                }
            }
            for(int j = 0 ; j < nJobs ; j++){
                free(z[t2][j]);
            }
            free(z[t2]);
            free(ptra[t2]);
        }
        free(z);
        free(equals);
        free(ptra);

        free(setPJ);
        free(setPM);

         if(stopbytime == 1){
            p = Inst_nProjects(mipp->inst);
            printf("time is over computing adding row to mip lift\n");
            fflush(stdout);
        }
    }


}
/*
void MipC_strengthening(MIPCompact *mipp, double timelimit)
{
    double initT = omp_get_wtime();
    double _time =  (double) timelimit - ( (double)omp_get_wtime() - initT );

    for(int p = 0 ; p < Inst_nProjects(mipp->inst); p++)
    {
        const Project *proj = Inst_project(mipp->inst,p);
        VecInt **setPJ;
        VecInt **setPM;
        ALLOCATE_VECTOR(setPJ,VecInt*,mipp->maxT);
        ALLOCATE_VECTOR(setPM,VecInt*,mipp->maxT);
      //  printf("Project %d:\n", p);  fflush(stdout);


        int *equals;
        ALLOCATE_VECTOR_INI(equals,int,mipp->maxT);

        for(int t = 0 ; t < mipp->maxT; t++)
        {
            setPJ[t]  = VInt_create();
            setPM[t]  = VInt_create();
            //   printf("\nsetP[%d]:\n", t);  fflush(stdout);
            int cont = 0;
            int equal = 1;
            // for(int j = 0 ; j < Project_nJobs(proj) ; j++)
            for(int j = 0 ; j < Inst_nJobs(mipp->inst) ; j++)
            {

                const Job *job = Inst_job(mipp->inst,j);
                int pp = Job_project(job);
                if(pp == p)
                {
                    for(int m = 0 ; m < Job_nModes(job) ; m++)
                    {
                        char name[256];
                        sprintf( name, "z(%d,%d,%d)", Job_index(job),m,t);
                        int idxZ = lp_col_index(mipp->mip,name);
                        if(idxZ !=-1)
                        {
                            VInt_pushBack(setPJ[t],Job_index(job));
                            VInt_pushBack(setPM[t],m);
                            //  printf("(%d,%d) ", Job_index(job), m);  fflush(stdout);
                            if( equal==1 && t>0)
                            {
                                if( (VInt_size(setPJ[t-1]) <= cont) )
                                    equal = 0;
                                else if(VInt_get(setPJ[t-1],cont) != VInt_get(setPJ[t],cont) ||  (VInt_get(setPM[t-1],cont) != VInt_get(setPM[t],cont)))
                                    equal = 0;
                            }
                            cont++;

                        }
                    }
                }
            }
            if(equal)
                equals[t] = 1;
        }





        //   getchar();

        int contf = 0;
        for(int t = 0 ; t < mipp->maxT; t++)
        {
            int size = VInt_size(setPJ[t]);
            _time =  (double) timelimit - ( (double)omp_get_wtime() - initT );

            if(_time <=0)
            {
                VInt_free(&setPJ[t]);
                VInt_free(&setPM[t]);
                // printf("time is over computing adding row to mip lift\n");
                goto TIMEOVER;
            }
            if(contf == 0)
                equals[t] = 0;

            contf++;

            if(size<=0)
            {
            //    printf("size %d", size); fflush(stdout);
                equals[t]=-1;
                VInt_free(&setPJ[t]);
                VInt_free(&setPM[t]);
                continue;
            }

            int value = -1;
            int cond = -1;
            if((t-1)>=0)
                cond = equals[t-1];

            if(equals[t]==1 && cond !=-1)
            {

              //  printf("equal P[%d]: %d ---\n", t, equals[t]);
                fflush(stdout);
                int contrg = 0;
                int end = Inst_nResR(mipp->inst);
                if(p+1<Inst_nProjects(mipp->inst))
                    end = Inst_idxResRProj(mipp->inst,p+1);

                int contOneResource = 0;
                for(int r = 0 ; r < end ; r++)
                {
                    mipp->mipstrengthening[p][t][r] = mipp->mipstrengthening[p][t-1][r];
                    contrg++;
                    if(contrg == Inst_nResRGlobal(mipp->inst))
                    {
                        r=Inst_idxResRProj(mipp->inst,p)-1;
                        if(contOneResource==0)
                        {
                            int idR = contrg;
                            contOneResource=1;
                            if(contrg==r+1) idR = end;
                            mipp->mipstrengthening[p][t][idR] = mipp->mipstrengthening[p][t-1][idR];
                        }
                    }
                }
                value = 1;

            }
            else
            {
                value = -1;
                if (equals[t]==0)
                {
                 //   printf("not equal P[%d]: %d\n", t,  equals[t]);

                    // CutP_quick_sort_vec_int(VInt_getPtr(setPJ[t]),VInt_getPtr(setPM[t]),size);
                    int *jobmodeinsubset;
                    ALLOCATE_VECTOR(jobmodeinsubset,int,Inst_nJobs(mipp->inst));
                    FILL(jobmodeinsubset,0,Inst_nJobs(mipp->inst),-1);

                    VecInt *combJ =VInt_create();
                    VecInt *combM =VInt_create();
                    VecInt *initsubset =VInt_create();

                    int *totalusesN;
                    int *totalusesRR;

                    ALLOCATE_VECTOR_INI(totalusesN,int,Inst_nResN(mipp->inst));
                    ALLOCATE_VECTOR_INI(totalusesRR,int,Inst_nResR(mipp->inst));


                 //   printf("Starting Backtracking t %d:\n", t);
                   // fflush(stdout);
                    // if(size>0)
                    //{
                    int maxnumit = 0;
                    int ite = 0;
                    value = MipC_subset_bt(mipp->inst, setPJ[t], setPM[t], combJ, combM, initsubset, size, jobmodeinsubset, totalusesRR, totalusesN, &maxnumit, ite);
                    //printf("value %d\n", value); fflush(stdout);
                    //}
                    if(value!=-1)
                    {

                        int contrg = 0;
                        int end = Inst_nResR(mipp->inst);
                        if(p+1<Inst_nProjects(mipp->inst))
                            end = Inst_idxResRProj(mipp->inst,p+1);
                        int contOneResource = 0;
                        for(int r = 0 ; r < end ; r++)
                        {
                            int capacity = Inst_capResR(mipp->inst,r);
                            mipp->mipstrengthening[p][t][r] = lp_create();
                            //  printf("p %d, t %d, r %d end %d\n", p,t,r,end);  fflush(stdout);
                            //mipp->mipstrengthening[p][t][r] = lp_create();
                            VecStr *namesZ = VStr_create( STR_SIZE );

                            double valueobjz[size], lb[size], ub[size];
                            char integer[size];
                            FILL( integer, 0, size, 1.0 );

                            char name[STR_SIZE];
                            //  printf("   size %d",  size); fflush(stdout);
                            for(int i = 0 ; i < size ; i++)
                            {
                                sprintf(name, "z(%d,%d)", VInt_get(setPJ[t],i), VInt_get(setPM[t],i) );
                                VStr_pushBack( namesZ, name );
                                int j =  VInt_get(setPJ[t],i);
                                int m = VInt_get(setPM[t],i);
                                const Job *jobsb = Inst_job(mipp->inst,j);
                                const Mode *modesb = Job_mode(jobsb,m);
                                int idxR = Mode_idxResROnMode(mipp->inst, modesb,r);
                                lb[i] = 0;
                                if(idxR != -1) lb[i] = Mode_useResR(modesb,idxR);
                                ub[i] = capacity;
                                valueobjz[i]= -1.0;
                            }
                            lp_add_cols( mipp->mipstrengthening[p][t][r], VStr_size(namesZ), valueobjz, lb, ub, integer, VStr_ptr(namesZ) );

                            //  printf("   VInt_size(initsubset) %d  VInt_size(combJ) %d",  VInt_size(initsubset), VInt_size(combJ)); fflush(stdout); getchar();
                            int nrows = VInt_size(initsubset);
                            int starts[nrows+1];
                            int idx[ VInt_size(combJ)];
                            int coef[ VInt_size(combJ)];
                            FILL( coef, 0,  VInt_size(combJ), 1.0 );
                            char senses[nrows];
                            double rhss[nrows];
                            char **names;
                            ALLOCATE_VECTOR(names, char*, nrows);
                            int cont = 0;

                            for(int c = 0 ; c <  nrows ; c++)
                            {
                                _time =  (double) timelimit - ( (double)omp_get_wtime() - initT );
                                if(_time <=0)
                                {
                                    free(totalusesN);
                                    free(totalusesRR);

                                    free(jobmodeinsubset);
                                    VStr_free(&namesZ);

                                    VInt_free(&combJ);
                                    VInt_free(&combM);
                                    VInt_free(&initsubset);
                                    equals[t]=-1;
                                    //    printf("time is over computing adding row to mip lift\n");
                                    goto TIMEOVER;
                                }

                                ALLOCATE_VECTOR(names[c], char, 81);
                                int sizesb = 0;
                                if(c==VInt_size(initsubset)-1)
                                    sizesb = VInt_size(combJ)-VInt_get(initsubset,c);
                                else
                                    sizesb = VInt_get(initsubset,c+1)-VInt_get(initsubset,c);
                               // int idx[sizesb];
                                //double coef[sizesb];
                                //FILL( coef, 0, sizesb, 1.0 );
                                sprintf( names[c], "ccomb(%d)", c);
                                starts[c] = cont;

                                senses[c] = 'L';
                                rhss[c] = capacity;
                               // sprintf(  names[c], namec);


                                for(int i = VInt_get(initsubset,c) ; i < VInt_get(initsubset,c)+sizesb ; i++ )
                                {
                                    int j = VInt_get(combJ,i);
                                    int m = VInt_get(combM,i);
                                    char name[256];
                                    sprintf( name, "z(%d,%d)", j,m);
                                    int idxZ = lp_col_index(mipp->mipstrengthening[p][t][r],name);
                                    if(idxZ !=-1)
                                    {
                                        idx[cont] = idxZ;
                                        cont++;
                                    }
                                }

                            }
                            starts[nrows] = cont;
                            lp_add_rows(mipp->mipstrengthening[p][t][r], nrows,starts,idx,coef,senses,rhss,names);
                            lp_set_print_messages(mipp->mipstrengthening[p][t][r],0);
                            _time =  (double) timelimit - ( (double)omp_get_wtime() - initT );
                            lp_set_max_seconds(mipp->mipstrengthening[p][t][r], _time);
                            //printf("lp_optimize_as_continuous\n"); fflush(stdout);
                            int status = lp_optimize_as_continuous(mipp->mipstrengthening[p][t][r]);
                            // int status = lp_optimize(mipp->mipstrengthening[p][t][r]);
                            //lp_set_nThreads(mipp->mipstrengthening[p][t][r],1);
                            //lp_set_max_nodes(mipp->mipstrengthening[p][t][r],3);
                            //lp_set_max_seconds(mipp->mipstrengthening[p][t][r],1);
                            VStr_free(&namesZ);
                            for(int c = 0 ; c <  nrows ; c++) free(names[c]);
                            free(names);
                            int oldr = r;
                            contrg++;
                            if(contrg == Inst_nResRGlobal(mipp->inst))
                            {
                                r=Inst_idxResRProj(mipp->inst,p)-1;
                                if(contOneResource==0)
                                {
                                    int idR = contrg;
                                    contOneResource=1;

                                    if(contrg==r+1) idR = end;

                                    mipp->mipstrengthening[p][t][idR] = lp_clone(mipp->mipstrengthening[p][t][oldr]);
                                    int cap = Inst_capResR(mipp->inst, idR);

                                    for ( int i=0 ; (i<lp_rows(mipp->mipstrengthening[p][t][idR])) ; ++i )
                                    {
                                        lp_set_rhs(mipp->mipstrengthening[p][t][idR],i,cap);
                                    }
                                    for ( int i=0 ; (i<lp_cols(mipp->mipstrengthening[p][t][idR])) ; ++i )
                                    {
                                        lp_set_col_bounds(mipp->mipstrengthening[p][t][idR],i,0,cap);
                                    }
                                    _time =  (double) timelimit - ( (double)omp_get_wtime() - initT );
                                    lp_set_max_seconds(mipp->mipstrengthening[p][t][idR], _time);
                                    int status = lp_optimize_as_continuous(mipp->mipstrengthening[p][t][idR]);
                                }
                            }
                            //  printf("--- p %d, t %d, r %d end %d\n", p,t,r,end); fflush(stdout);
                        }
                    }
                    else
                    {
                        equals[t]=-1;
                  //      printf("invalid by it P[%d]: %d\n", t,  equals[t]);
                    }

                    free(totalusesN);
                    free(totalusesRR);

                    free(jobmodeinsubset);

                    VInt_free(&combJ);
                    VInt_free(&combM);
                    VInt_free(&initsubset);
                }
            }


            if(value !=-1 && equals[t] !=-1)
            {
                //  printf("-p %d, t %d, r %d: ", p,t,r); fflush(stdout);

                //for(int r = 0 ; r < Inst_nResR(mipp->inst) ; r++){

                int contrg = 0;
                int end = Inst_nResR(mipp->inst);
                if(p+1<Inst_nProjects(mipp->inst))
                    end = Inst_idxResRProj(mipp->inst,p+1);

                int contOneResource = 0;
                for(int r = 0; r < end ; r++)
                {
                    //   if(mipp->mipstrengthening[p][t][r]==NULL) continue;
                  //  printf("-p %d, t %d, r %d: ", p,t,r);fflush(stdout);

                    //int status = lp_optimize(mipp->mipstrengthening);
                    //int hasSolution = ( ( status == LP_OPTIMAL) || ( status == LP_FEASIBLE ) );
                    // if ( 1 )
                    //{



                    //lp_set_col_bounds(mipp->mipstrengthening[p][t][r], );
                    //lp_set_rhs()


                    const double *z = lp_x(mipp->mipstrengthening[p][t][r]);
                    char prefix[STR_SIZE];
                    int id[3];
                    int idx[lp_cols(mipp->mipstrengthening[p][t][r])];
                    double coef[lp_cols(mipp->mipstrengthening[p][t][r])];
                    int cont = 0 ;
                    for ( int i=0 ; (i<lp_cols(mipp->mipstrengthening[p][t][r])) ; ++i )
                    {
                        char name[STR_SIZE];
                        lp_col_name(mipp->mipstrengthening[p][t][r],i,name);
                        parseName( name, prefix, id );
                        if( z[i]>0)
                        {
//                            if (prefix[0]=='z') {
                            int j = id[0];
                            int m = id[1];
                            if(Par_getTypePSP(mipp->par)==2)
                            {
                                char namev[STR_SIZE];
                                sprintf(namev,"z(%d,%d,%d)",j,m,t);
                                int idz = lp_col_index(mipp->mip,namev);
                                idx[cont] = idz;
                                coef[cont] = z[i];
                                cont++;
                            }
                            mipp->coefR[j][m][r][t] = z[i];
//                          }
                        }
                    }
                    if(Par_getTypePSP(mipp->par)==2)
                    {
                        char namec[STR_SIZE];
                        sprintf(namec,"resRZ(%d,%d,%d)",r,t,p);
                        // printf(" resRZ(%d,%d,%d) \n",r,t,p);
                        lp_add_row( mipp->mip, cont, idx, coef, namec, 'L', Inst_capResR(mipp->inst, r) );
                    }
                    //         if(r==0 && t==13){
                    //  lp_write_lp(mipp->mipstrengthening[p][t][r], "mipstrengthening.lp");
                    //    lp_write_sol(mipp->mipstrengthening[p][t][r],"mipstrengthening.sol");
                    //   lp_write_lp(mipp->mip,"mipstrengtheningoori.lp");
                    //   printf("print lp...\n");
                    //      getchar();
                    // }
                    // } // has solution
                    //  lp_free(&mipstrengthening);
                    contrg++;
                    if(contrg == Inst_nResRGlobal(mipp->inst))
                    {
                        r=Inst_idxResRProj(mipp->inst,p)-1;
                        if(contOneResource==0)
                        {
                            int idR = contrg;
                            contOneResource=1;

                            if(contrg==r+1) idR = end;
                            const double *z = lp_x(mipp->mipstrengthening[p][t][idR]);
                            char prefix[STR_SIZE];
                            int id[3];
                            int idx[lp_cols(mipp->mipstrengthening[p][t][idR])];
                            double coef[lp_cols(mipp->mipstrengthening[p][t][idR])];
                            int cont = 0 ;
                            for ( int i=0 ; (i<lp_cols(mipp->mipstrengthening[p][t][idR])) ; ++i )
                            {
                                char name[STR_SIZE];
                                lp_col_name(mipp->mipstrengthening[p][t][idR],i,name);
                                parseName( name, prefix, id );
                                if( z[i]>0)
                                {
                                    //                            if (prefix[0]=='z') {
                                    int j = id[0];
                                    int m = id[1];
                                    if(Par_getTypePSP(mipp->par)==2)
                                    {
                                        char namev[STR_SIZE];
                                        sprintf(namev,"z(%d,%d,%d)",j,m,t);
                                        int idz = lp_col_index(mipp->mip,namev);
                                        idx[cont] = idz;
                                        coef[cont] = z[i];
                                        cont++;
                                    }
                                    mipp->coefR[j][m][idR][t] = z[i];
                                    //                          }
                                }
                            }
                            if(Par_getTypePSP(mipp->par)==2)
                            {
                                char namec[STR_SIZE];
                                sprintf(namec,"resRZ(%d,%d,%d)",idR,t,p);
                                //     printf(" resRZ(%d,%d,%d) \n",r,t,p);
                                lp_add_row( mipp->mip, cont, idx, coef, namec, 'L', Inst_capResR(mipp->inst, idR) );
                            }
                            //    lp_write_lp(mipp->mipstrengthening[p][t][idR], "mipstrengthening.lp");
                            // lp_write_sol(mipp->mipstrengthening[p][t][idR],"mipstrengthening.sol");
                            //   lp_write_lp(mipp->mip,"mipstrengtheningoori.lp");
                            //   printf("entrou print lp...\n");
                            //  getchar();
                        }
                    }
                }
            }

            VInt_free(&setPJ[t]);
            VInt_free(&setPM[t]);
        }
        free(setPJ);
        free(setPM);

        for(int t = 0 ; t < mipp->maxT; t++)
        {
            if(equals[t]==0)
            {

                int contrg = 0;
                int end = Inst_nResR(mipp->inst);
                if(p+1<Inst_nProjects(mipp->inst))
                    end = Inst_idxResRProj(mipp->inst,p+1);
                int contOneResource = 0;
                for(int r = 0; r < end ; ++r)
                {
                    lp_free(&mipp->mipstrengthening[p][t][r]);
                    contrg++;
                    if(contrg == Inst_nResRGlobal(mipp->inst))
                    {
                        r=Inst_idxResRProj(mipp->inst,p)-1;
                        if(contOneResource==0)
                        {
                            int idR = contrg;
                            contOneResource=1;
                            if(contrg==r+1) idR = end;
                            lp_free(&mipp->mipstrengthening[p][t][idR]);
                        }
                    }
                }
            }
        }
        free(equals);
    }

TIMEOVER:
    printf("time is over computing adding row to mip lift\n");
    fflush(stdout);
}
*/

/*
void MipC_strengthening(MIPCompact *mipp)
{
    for(int p = 0 ; p < Inst_nProjects(mipp->inst); p++)
    {
        const Project *proj = Inst_project(mipp->inst,p);
        VecInt **setPJ;
        VecInt **setPM;
        ALLOCATE_VECTOR(setPJ,VecInt*,mipp->maxT);
        ALLOCATE_VECTOR(setPM,VecInt*,mipp->maxT);
      //  printf("Project %d:\n", p);


        int *equals;
        ALLOCATE_VECTOR_INI(equals,int,mipp->maxT);

        for(int t = 0 ; t < mipp->maxT; t++)
        {
            setPJ[t]  = VInt_create();
            setPM[t]  = VInt_create();
          //  printf("\nsetP[%d]:\n", t);
           int cont = 0;
           equals[0] = 0;
           int equal = 1;
         // for(int j = 0 ; j < Project_nJobs(proj) ; j++)
          for(int j = 0 ; j < Inst_nJobs(mipp->inst) ; j++)
         {

                const Job *job = Inst_job(mipp->inst,j);
                int pp = Job_project(job);
                if(pp == p){
                    for(int m = 0 ; m < Job_nModes(job) ; m++)
                    {
                        char name[256];
                        sprintf( name, "z(%d,%d,%d)", Job_index(job),m,t);
                        int idxZ = lp_col_index(mipp->mip,name);
                        if(idxZ !=-1){
                            VInt_pushBack(setPJ[t],Job_index(job));
                            VInt_pushBack(setPM[t],m);
                       //    printf("(%d,%d) ", Job_index(job), m);
                            if( equal==1 && t>0){
                                if( (VInt_size(setPJ[t-1]) <= cont) )
                                    equal = 0;
                                else if(VInt_get(setPJ[t-1],cont) != VInt_get(setPJ[t],cont) ||  (VInt_get(setPM[t-1],cont) != VInt_get(setPM[t],cont)))
                                    equal = 0;
                            }
                            cont++;

                        }
                    }
                }
            }
            if(equal)
                equals[t] = 1;
         }





     //   getchar();


        for(int t = 0 ; t < mipp->maxT; t++)
        {
            int size = VInt_size(setPJ[t]);

            if(size<=0){
                equals[t]=-1;
                VInt_free(&setPJ[t]);
                VInt_free(&setPM[t]);
                continue;
            }

            int value =0;
            if(equals[t]==1){

               // printf("equal P[%d]: %d ---\n", t , equals[t]); fflush(stdout);
                int contrg = 0;
                int end = Inst_nResR(mipp->inst);
                if(p+1<Inst_nProjects(mipp->inst))
                    end = Inst_idxResRProj(mipp->inst,p+1);

                for(int r = 0 ; r < end ; r++){
                    mipp->mipstrengthening[p][t][r] = mipp->mipstrengthening[p][t-1][r];
                    contrg++;
                    if(contrg == Inst_nResRGlobal(mipp->inst)){
                        r=Inst_idxResRProj(mipp->inst,p)-1;
                    }
                }

            }else{
                //printf("not equal P[%d]: %d\n", t,  equals[t]); fflush(stdout);

               // CutP_quick_sort_vec_int(VInt_getPtr(setPJ[t]),VInt_getPtr(setPM[t]),size);
                int *jobmodeinsubset;
                ALLOCATE_VECTOR(jobmodeinsubset,int,Inst_nJobs(mipp->inst));
                FILL(jobmodeinsubset,0,Inst_nJobs(mipp->inst),-1);

                VecInt *combJ =VInt_create();
                VecInt *combM =VInt_create();
                VecInt *initsubset =VInt_create();

                int *totalusesN;
                int *totalusesRR;

                ALLOCATE_VECTOR_INI(totalusesN,int,Inst_nResN(mipp->inst));
                ALLOCATE_VECTOR_INI(totalusesRR,int,Inst_nResR(mipp->inst));


              //  printf("Starting Backtracking t %d:\n", t);
                if(size>0)
                {
                    double timelimit = 300;
                    int ite = 0;
                    value = MipC_subset_bt(mipp->inst, setPJ[t], setPM[t], combJ, combM, initsubset, size, jobmodeinsubset, totalusesRR, totalusesN, timelimit, ite);
              //      printf("value %d\n", value);
                }
                if(value !=-1){

                    int contrg = 0;
                    int end = Inst_nResR(mipp->inst);
                    if(p+1<Inst_nProjects(mipp->inst))
                        end = Inst_idxResRProj(mipp->inst,p+1);

                    for(int r = 0 ; r < end ; r++){
                        int capacity = Inst_capResR(mipp->inst,r);
                        mipp->mipstrengthening[p][t][r] = lp_create();
                       // printf("p %d, t %d, r %d end %d\n", p,t,r,end);
                        //mipp->mipstrengthening[p][t][r] = lp_create();
                        VecStr *namesZ = VStr_create( STR_SIZE );

                        double valueobjz[size], lb[size], ub[size];
                        char integer[size];
                        FILL( integer, 0, size, 1.0 );

                        char name[STR_SIZE];
                        for(int i = 0 ; i < size ; i++){
                            sprintf(name, "z(%d,%d)", VInt_get(setPJ[t],i), VInt_get(setPM[t],i) );
                            VStr_pushBack( namesZ, name );
                            int j =  VInt_get(setPJ[t],i);
                            int m = VInt_get(setPM[t],i);
                            const Job *jobsb = Inst_job(mipp->inst,j);
                            const Mode *modesb = Job_mode(jobsb,m);
                            int idxR = Mode_idxResROnMode(mipp->inst, modesb,r);
                            lb[i] = 0;
                            if(idxR != -1) lb[i] = Mode_useResR(modesb,idxR);
                            ub[i] = capacity;
                            valueobjz[i]= -1.0;
                        }
                        lp_add_cols( mipp->mipstrengthening[p][t][r], VStr_size(namesZ), valueobjz, lb, ub, integer, VStr_ptr(namesZ) );

                        for(int c = 0 ; c <  VInt_size(initsubset) ; c++){
                                int sizesb = 0;
                                if(c==VInt_size(initsubset)-1)
                                    sizesb = VInt_size(combJ)-VInt_get(initsubset,c);
                                else
                                    sizesb = VInt_get(initsubset,c+1)-VInt_get(initsubset,c);
                                int idx[sizesb];
                                double coef[sizesb];
                                FILL( coef, 0, sizesb, 1.0 );
                                int cont = 0;
                                char namec[256];
                                sprintf( namec, "ccomb(%d)", c);
                                for(int i = VInt_get(initsubset,c) ; i < VInt_get(initsubset,c)+sizesb ; i++ ){
                                    int j = VInt_get(combJ,i);
                                    int m = VInt_get(combM,i);
                                    char name[256];
                                    sprintf( name, "z(%d,%d)", j,m);
                                    int idxZ = lp_col_index(mipp->mipstrengthening[p][t][r],name);
                                    if(idxZ !=-1){
                                        idx[cont] = idxZ;
                                        cont++;
                                    }
                                }
                                lp_add_row(mipp->mipstrengthening[p][t][r], cont, idx,coef,namec,'L', capacity);
                        }
                        lp_set_print_messages(mipp->mipstrengthening[p][t][r],0);
                        int status = lp_optimize_as_continuous(mipp->mipstrengthening[p][t][r]);
                       // int status = lp_optimize(mipp->mipstrengthening[p][t][r]);
                        //lp_set_nThreads(mipp->mipstrengthening[p][t][r],1);
                        //lp_set_max_nodes(mipp->mipstrengthening[p][t][r],3);
                        //lp_set_max_seconds(mipp->mipstrengthening[p][t][r],1);
                        VStr_free(&namesZ);
                        contrg++;
                        if(contrg == Inst_nResRGlobal(mipp->inst)){
                            r=Inst_idxResRProj(mipp->inst,p)-1;
                        }
                     //   printf("--- p %d, t %d, r %d end %d\n", p,t,r,end);
                    }
                }else{
                    equals[t]=-1;
                }
                free(totalusesN);
                free(totalusesRR);

                free(jobmodeinsubset);

                VInt_free(&combJ);
                VInt_free(&combM);
                VInt_free(&initsubset);
            }


           if(value !=-1){

            //for(int r = 0 ; r < Inst_nResR(mipp->inst) ; r++){

                    int contrg = 0;
                    int end = Inst_nResR(mipp->inst);
                    if(p+1<Inst_nProjects(mipp->inst))
                        end = Inst_idxResRProj(mipp->inst,p+1);

                for(int r = 0; r < end ; r++){
             //   if(mipp->mipstrengthening[p][t][r]==NULL) continue;
                  //  printf("-p %d, t %d, r %d: ", p,t,r); fflush(stdout);

                    //int status = lp_optimize(mipp->mipstrengthening);
                //int hasSolution = ( ( status == LP_OPTIMAL) || ( status == LP_FEASIBLE ) );
               // if ( 1 )
                //{



                //lp_set_col_bounds(mipp->mipstrengthening[p][t][r], );
                //lp_set_rhs()


                    const double *z = lp_x(mipp->mipstrengthening[p][t][r]);
                    char prefix[STR_SIZE];
                    int id[3];
                    int idx[lp_cols(mipp->mipstrengthening[p][t][r])];
                    double coef[lp_cols(mipp->mipstrengthening[p][t][r])];
                    int cont = 0 ;
                    for ( int i=0 ; (i<lp_cols(mipp->mipstrengthening[p][t][r])) ; ++i ) {
                        char name[STR_SIZE];
                        lp_col_name(mipp->mipstrengthening[p][t][r],i,name);
                        parseName( name, prefix, id );
                        if( z[i]>0){
//                            if (prefix[0]=='z') {
                            int j = id[0];
                            int m = id[1];
                            if(Par_getTypePSP(mipp->par)==2){
                                char namev[STR_SIZE];
                                sprintf(namev,"z(%d,%d,%d)",j,m,t);
                                int idz = lp_col_index(mipp->mip,namev);
                                idx[cont] = idz;
                                coef[cont] = z[i];
                                cont++;
                            }
                            mipp->coefR[j][m][r][t] = z[i];
//                          }
                        }
                    }
                    if(Par_getTypePSP(mipp->par)==2){
                        char namec[STR_SIZE];
                        sprintf(namec,"resRZ(%d,%d,%d)",r,t,p);
                  //      printf(" resRZ(%d,%d,%d) \n",r,t,p);
                        lp_add_row( mipp->mip, cont, idx, coef, namec, 'L', Inst_capResR(mipp->inst, r) );
                    }
                    //         if(r==0 && t==13){
                    //    lp_write_lp(mipp->mipstrengthening[p][t][r], "mipstrengthening.lp");
                      //  lp_write_sol(mipp->mipstrengthening[p][t][r],"mipstrengthening.sol");
                       // lp_write_lp(mipp->mip,"mipstrengtheningoori.lp");
                      //  printf("print lp...\n");
                      //  getchar();
                   // }
               // } // has solution
                    //  lp_free(&mipstrengthening);
                    contrg++;
                    if(contrg == Inst_nResRGlobal(mipp->inst)){
                       r=Inst_idxResRProj(mipp->inst,p)-1;
                    }
                }
            }

            VInt_free(&setPJ[t]);
            VInt_free(&setPM[t]);
        }
        free(setPJ);
        free(setPM);

        for(int t = 0 ; t < mipp->maxT; t++)
        {
            if(equals[t]==0){

                int contrg = 0;
                int end = Inst_nResR(mipp->inst);
                if(p+1<Inst_nProjects(mipp->inst))
                    end = Inst_idxResRProj(mipp->inst,p+1);
                for(int r = 0; r < end ; ++r){
                    lp_free(&mipp->mipstrengthening[p][t][r]);
                    contrg++;
                    if(contrg == Inst_nResRGlobal(mipp->inst)){
                        r=Inst_idxResRProj(mipp->inst,p)-1;
                    }
                }
            }
        }
        free(equals);
    }

//printf("fim str\n"); fflush(stdout);
}
*/

/*
void MipC_strengthening_allresource(MIPCompact *mipp)
{
    for(int p = 0 ; p < Inst_nProjects(mipp->inst); p++)
    {
        const Project *proj = Inst_project(mipp->inst,p);
        VecInt **setPJ;
        VecInt **setPM;
        ALLOCATE_VECTOR(setPJ,VecInt*,mipp->maxT);
        ALLOCATE_VECTOR(setPM,VecInt*,mipp->maxT);
      //  printf("Project %d:\n", p);


        int *equals;
        ALLOCATE_VECTOR_INI(equals,int,mipp->maxT);

        for(int t = 0 ; t < mipp->maxT; t++)
        {
            setPJ[t]  = VInt_create();
            setPM[t]  = VInt_create();
          //  printf("\nsetP[%d]:\n", t);
           int cont = 0;
           equals[0] = 0;
           int equal = 1;
         // for(int j = 0 ; j < Project_nJobs(proj) ; j++)
          for(int j = 0 ; j < Inst_nJobs(mipp->inst) ; j++)
         {

                const Job *job = Inst_job(mipp->inst,j);
                int pp = Job_project(job);
                if(pp == p){
                    for(int m = 0 ; m < Job_nModes(job) ; m++)
                    {
                        char name[256];
                        sprintf( name, "z(%d,%d,%d)", Job_index(job),m,t);
                        int idxZ = lp_col_index(mipp->mip,name);
                        if(idxZ !=-1){
                            VInt_pushBack(setPJ[t],Job_index(job));
                            VInt_pushBack(setPM[t],m);
                       //    printf("(%d,%d) ", Job_index(job), m);
                            if( equal==1 && t>0){
                                if( (VInt_size(setPJ[t-1]) <= cont) )
                                    equal = 0;
                                else if(VInt_get(setPJ[t-1],cont) != VInt_get(setPJ[t],cont) ||  (VInt_get(setPM[t-1],cont) != VInt_get(setPM[t],cont)))
                                    equal = 0;
                            }
                            cont++;

                        }
                    }
                }
            }
            if(equal)
                equals[t] = 1;
         }





     //   getchar();


        for(int t = 0 ; t < mipp->maxT; t++)
        {
            int size = VInt_size(setPJ[t]);

            if(size<=0){
                equals[t]=-1;
                VInt_free(&setPJ[t]);
                VInt_free(&setPM[t]);
                continue;
            }

            int value =0;
            if(equals[t]==1){

               // printf("equal P[%d]: %d ---\n", t , equals[t]); fflush(stdout);
                for(int r = 0 ; r < Inst_nResR(mipp->inst) ; r++){
                    mipp->mipstrengthening[p][t][r] = mipp->mipstrengthening[p][t-1][r];
                }

            }else{
                //printf("not equal P[%d]: %d\n", t,  equals[t]); fflush(stdout);

               // CutP_quick_sort_vec_int(VInt_getPtr(setPJ[t]),VInt_getPtr(setPM[t]),size);
                int *jobmodeinsubset;
                ALLOCATE_VECTOR(jobmodeinsubset,int,Inst_nJobs(mipp->inst));
                FILL(jobmodeinsubset,0,Inst_nJobs(mipp->inst),-1);

                VecInt *combJ =VInt_create();
                VecInt *combM =VInt_create();
                VecInt *initsubset =VInt_create();

                int *totalusesN;
                int *totalusesRR;

                ALLOCATE_VECTOR_INI(totalusesN,int,Inst_nResN(mipp->inst));
                ALLOCATE_VECTOR_INI(totalusesRR,int,Inst_nResR(mipp->inst));


              //  printf("Starting Backtracking t %d:\n", t);
                if(size>0)
                {
                    double timelimit = 300;
                    int ite = 0;
                    value = MipC_subset_bt(mipp->inst, setPJ[t], setPM[t], combJ, combM, initsubset, size, jobmodeinsubset, totalusesRR, totalusesN, timelimit, ite);
              //      printf("value %d\n", value);
                }
                if(value !=-1){


                    for(int r = 0 ; r < Inst_nResR(mipp->inst) ; r++){
                        int capacity = Inst_capResR(mipp->inst,r);
                        mipp->mipstrengthening[p][t][r] = lp_create();
                       // printf("p %d, t %d, r: %d \n", p,t,r);
                        //mipp->mipstrengthening[p][t][r] = lp_create();
                        VecStr *namesZ = VStr_create( STR_SIZE );

                        double valueobjz[size], lb[size], ub[size];
                        char integer[size];
                        FILL( integer, 0, size, 1.0 );


                        char name[STR_SIZE];
                        for(int i = 0 ; i < size ; i++){
                            sprintf(name, "z(%d,%d)", VInt_get(setPJ[t],i), VInt_get(setPM[t],i) );
                          //  printf("z(%d,%d) ", VInt_get(setPJ[t],i), VInt_get(setPM[t],i) ); fflush(stdout);
                            VStr_pushBack( namesZ, name );
                            int j =  VInt_get(setPJ[t],i);
                            int m = VInt_get(setPM[t],i);
                            const Job *jobsb = Inst_job(mipp->inst,j);
                            const Mode *modesb = Job_mode(jobsb,m);
                            int idxR = Mode_idxResROnMode(mipp->inst, modesb,r);
                            lb[i] = 0;
                            ub[i] = 0;
                            if(idxR != -1) {
                                lb[i] = Mode_useResR(modesb,idxR);
                            }
                            ub[i] = capacity;
                            valueobjz[i]= -1.0;
                        }
                        lp_add_cols( mipp->mipstrengthening[p][t][r], VStr_size(namesZ), valueobjz, lb, ub, integer, VStr_ptr(namesZ) );

                        for(int c = 0 ; c <  VInt_size(initsubset) ; c++){
                                int sizesb = 0;
                                if(c==VInt_size(initsubset)-1)
                                    sizesb = VInt_size(combJ)-VInt_get(initsubset,c);
                                else
                                    sizesb = VInt_get(initsubset,c+1)-VInt_get(initsubset,c);
                                int idx[sizesb];
                                double coef[sizesb];
                                FILL( coef, 0, sizesb, 1.0 );
                                int cont = 0;
                                char namec[256];
                                sprintf( namec, "ccomb(%d)", c);
                              //  printf(  "\nccomb(%d): ", c);
                                for(int i = VInt_get(initsubset,c) ; i < VInt_get(initsubset,c)+sizesb ; i++ ){
                                    int j = VInt_get(combJ,i);
                                    int m = VInt_get(combM,i);
                                    char name[256];
                                    sprintf( name, "z(%d,%d)", j,m);
                                 //   printf( "z(%d,%d)", j,m);fflush(stdout);
                                    int idxZ = lp_col_index(mipp->mipstrengthening[p][t][r],name);
                                    if(idxZ !=-1){
                                        idx[cont] = idxZ;
                                        cont++;
                                    }
                                }
                               // getchar();
                                lp_add_row(mipp->mipstrengthening[p][t][r], cont, idx,coef,namec,'L', capacity);
                        }
                        lp_set_print_messages(mipp->mipstrengthening[p][t][r],0);
                        int status = lp_optimize_as_continuous(mipp->mipstrengthening[p][t][r]);
                       // int status = lp_optimize(mipp->mipstrengthening[p][t][r]);
                        //lp_set_nThreads(mipp->mipstrengthening[p][t][r],1);
                        //lp_set_max_nodes(mipp->mipstrengthening[p][t][r],3);
                        //lp_set_max_seconds(mipp->mipstrengthening[p][t][r],1);
                        VStr_free(&namesZ);
                     //   printf("--- p %d, t %d, r %d end %d\n", p,t,r,end);
                    }
                }else{
                    equals[t]=-1;
                }
                free(totalusesN);
                free(totalusesRR);

                free(jobmodeinsubset);

                VInt_free(&combJ);
                VInt_free(&combM);
                VInt_free(&initsubset);
            }


           if(value !=-1){

            //for(int r = 0 ; r < Inst_nResR(mipp->inst) ; r++){


                for(int r = 0; r < Inst_nResR(mipp->inst) ; r++){
             //   if(mipp->mipstrengthening[p][t][r]==NULL) continue;
                  //  printf("-p %d, t %d, r %d: ", p,t,r); fflush(stdout);

                    //int status = lp_optimize(mipp->mipstrengthening);
                //int hasSolution = ( ( status == LP_OPTIMAL) || ( status == LP_FEASIBLE ) );
               // if ( 1 )
                //{
                    const double *z = lp_x(mipp->mipstrengthening[p][t][r]);
                    char prefix[STR_SIZE];
                    int id[3];
                    int idx[lp_cols(mipp->mipstrengthening[p][t][r])];
                    double coef[lp_cols(mipp->mipstrengthening[p][t][r])];
                    int cont = 0 ;
                    for ( int i=0 ; (i<lp_cols(mipp->mipstrengthening[p][t][r])) ; ++i ) {
                        char name[STR_SIZE];
                        lp_col_name(mipp->mipstrengthening[p][t][r],i,name);
                        parseName( name, prefix, id );
                        if( z[i]>0){
//                            if (prefix[0]=='z') {
                            int j = id[0];
                            int m = id[1];
                            if(Par_getTypePSP(mipp->par)==2){
                                char namev[STR_SIZE];
                                sprintf(namev,"z(%d,%d,%d)",j,m,t);
                                int idz = lp_col_index(mipp->mip,namev);
                                idx[cont] = idz;
                                coef[cont] = z[i];
                                cont++;
                            }
                            mipp->coefR[j][m][r][t] = z[i];
//                          }
                        }
                    }
                    if(Par_getTypePSP(mipp->par)==2){
                        char namec[STR_SIZE];
                        sprintf(namec,"resRZ(%d,%d,%d)",r,t,p);
                  //      printf(" resRZ(%d,%d,%d) \n",r,t,p);
                        lp_add_row( mipp->mip, cont, idx, coef, namec, 'L', Inst_capResR(mipp->inst, r) );
                    }
                    //         if(r==0 && t==13){
                              lp_write_lp(mipp->mipstrengthening[p][t][r], "mipstrengtheningall.lp");
                        lp_write_sol(mipp->mipstrengthening[p][t][r],"mipstrengtheningall.sol");
                        lp_write_lp(mipp->mip,"mipstrengtheningooriall.lp");
                        printf("print lp...\n");
                        getchar();
                   // }
                   // }
               // } // has solution
                    //  lp_free(&mipstrengthening);

                }
            }

            VInt_free(&setPJ[t]);
            VInt_free(&setPM[t]);
        }
        free(setPJ);
        free(setPM);

        for(int t = 0 ; t < mipp->maxT; t++)
        {
            if(equals[t]==0){

                for(int r = 0; r < Inst_nResR(mipp->inst) ; ++r){
                    lp_free(&mipp->mipstrengthening[p][t][r]);

                }
            }
        }
        free(equals);
    }

//printf("fim str\n"); fflush(stdout);
}

*/

/*
void MipC_strengthening_oneresource(MIPCompact *mipp)
{
    for(int p = 0 ; p < Inst_nProjects(mipp->inst); p++)
    {
        const Project *proj = Inst_project(mipp->inst,p);
        VecInt **setPJ;
        VecInt **setPM;
        ALLOCATE_VECTOR(setPJ,VecInt*,mipp->maxT);
        ALLOCATE_VECTOR(setPM,VecInt*,mipp->maxT);
      //  printf("Project %d:\n", p);


        int *equals;
        ALLOCATE_VECTOR_INI(equals,int,mipp->maxT);

        for(int t = 0 ; t < mipp->maxT; t++)
        {
            setPJ[t]  = VInt_create();
            setPM[t]  = VInt_create();
          //  printf("\nsetP[%d]:\n", t);
           int cont = 0;
           equals[0] = 0;
           int equal = 1;
         // for(int j = 0 ; j < Project_nJobs(proj) ; j++)
          for(int j = 0 ; j < Inst_nJobs(mipp->inst) ; j++)
         {

                const Job *job = Inst_job(mipp->inst,j);
                int pp = Job_project(job);
                if(pp == p){
                    for(int m = 0 ; m < Job_nModes(job) ; m++)
                    {
                        char name[256];
                        sprintf( name, "z(%d,%d,%d)", Job_index(job),m,t);
                        int idxZ = lp_col_index(mipp->mip,name);
                        if(idxZ !=-1){
                            VInt_pushBack(setPJ[t],Job_index(job));
                            VInt_pushBack(setPM[t],m);
                       //    printf("(%d,%d) ", Job_index(job), m);
                            if( equal==1 && t>0){
                                if( (VInt_size(setPJ[t-1]) <= cont) )
                                    equal = 0;
                                else if(VInt_get(setPJ[t-1],cont) != VInt_get(setPJ[t],cont) ||  (VInt_get(setPM[t-1],cont) != VInt_get(setPM[t],cont)))
                                    equal = 0;
                            }
                            cont++;

                        }
                    }
                }
            }
            if(equal)
                equals[t] = 1;
         }





     //   getchar();


        for(int t = 0 ; t < mipp->maxT; t++)
        {
            int size = VInt_size(setPJ[t]);

            if(size<=0){
                equals[t]=-1;
                VInt_free(&setPJ[t]);
                VInt_free(&setPM[t]);
                continue;
            }

            int value =0;
            if(equals[t]==1){

               // printf("equal P[%d]: %d ---\n", t , equals[t]); fflush(stdout);
               // for(int r = 0 ; r < Inst_nResR(mipp->inst) ; r++){
                    mipp->mipstrengthening[p][t] = mipp->mipstrengthening[p][t-1];
                //}

            }else{
                //printf("not equal P[%d]: %d\n", t,  equals[t]); fflush(stdout);

               // CutP_quick_sort_vec_int(VInt_getPtr(setPJ[t]),VInt_getPtr(setPM[t]),size);
                int *jobmodeinsubset;
                ALLOCATE_VECTOR(jobmodeinsubset,int,Inst_nJobs(mipp->inst));
                FILL(jobmodeinsubset,0,Inst_nJobs(mipp->inst),-1);

                VecInt *combJ =VInt_create();
                VecInt *combM =VInt_create();
                VecInt *initsubset =VInt_create();

                int *totalusesN;
                int *totalusesRR;

                ALLOCATE_VECTOR_INI(totalusesN,int,Inst_nResN(mipp->inst));
                ALLOCATE_VECTOR_INI(totalusesRR,int,Inst_nResR(mipp->inst));


              //  printf("Starting Backtracking t %d:\n", t);
                if(size>0)
                {
                    double timelimit = 300;
                    int ite = 0;
                    value = MipC_subset_bt(mipp->inst, setPJ[t], setPM[t], combJ, combM, initsubset, size, jobmodeinsubset, totalusesRR, totalusesN, timelimit, ite);
              //      printf("value %d\n", value);
                }
                if(value !=-1){


                        int r = 0;
                  //  for(int r = 0 ; r < Inst_nResR(mipp->inst) ; r++){
                        int capacity = Inst_capResR(mipp->inst,r);
                        mipp->mipstrengthening[p][t] = lp_create();
                       // printf("p %d, t %d, r: %d \n", p,t,r);
                        //mipp->mipstrengthening[p][t][r] = lp_create();
                        VecStr *namesZ = VStr_create( STR_SIZE );

                        double valueobjz[size], lb[size], ub[size];
                        char integer[size];
                        FILL( integer, 0, size, 1.0 );


                        char name[STR_SIZE];
                        for(int i = 0 ; i < size ; i++){
                            sprintf(name, "z(%d,%d)", VInt_get(setPJ[t],i), VInt_get(setPM[t],i) );
                          //  printf("z(%d,%d) ", VInt_get(setPJ[t],i), VInt_get(setPM[t],i) ); fflush(stdout);
                            VStr_pushBack( namesZ, name );
                            int j =  VInt_get(setPJ[t],i);
                            int m = VInt_get(setPM[t],i);
                            const Job *jobsb = Inst_job(mipp->inst,j);
                            const Mode *modesb = Job_mode(jobsb,m);
                            int idxR = Mode_idxResROnMode(mipp->inst, modesb,r);
                            lb[i] = 0;
                            ub[i] = 0;
                            if(idxR != -1) {
                                lb[i] = Mode_useResR(modesb,idxR);
                            }
                            ub[i] = capacity;
                            valueobjz[i]= -1.0;
                        }
                        lp_add_cols( mipp->mipstrengthening[p][t], VStr_size(namesZ), valueobjz, lb, ub, integer, VStr_ptr(namesZ) );

                        for(int c = 0 ; c <  VInt_size(initsubset) ; c++){
                                int sizesb = 0;
                                if(c==VInt_size(initsubset)-1)
                                    sizesb = VInt_size(combJ)-VInt_get(initsubset,c);
                                else
                                    sizesb = VInt_get(initsubset,c+1)-VInt_get(initsubset,c);
                                int idx[sizesb];
                                double coef[sizesb];
                                FILL( coef, 0, sizesb, 1.0 );
                                int cont = 0;
                                char namec[256];
                                sprintf( namec, "ccomb(%d)", c);
                              //  printf(  "\nccomb(%d): ", c);
                                for(int i = VInt_get(initsubset,c) ; i < VInt_get(initsubset,c)+sizesb ; i++ ){
                                    int j = VInt_get(combJ,i);
                                    int m = VInt_get(combM,i);
                                    char name[256];
                                    sprintf( name, "z(%d,%d)", j,m);
                                 //   printf( "z(%d,%d)", j,m);fflush(stdout);
                                    int idxZ = lp_col_index(mipp->mipstrengthening[p][t],name);
                                    if(idxZ !=-1){
                                        idx[cont] = idxZ;
                                        cont++;
                                    }
                                }
                               // getchar();
                                lp_add_row(mipp->mipstrengthening[p][t], cont, idx,coef,namec,'L', capacity);
                        }
                        lp_set_print_messages(mipp->mipstrengthening[p][t],0);
                     //   int status = lp_optimize_as_continuous(mipp->mipstrengthening[p][t]);
                       // int status = lp_optimize(mipp->mipstrengthening[p][t][r]);
                        //lp_set_nThreads(mipp->mipstrengthening[p][t][r],1);
                        //lp_set_max_nodes(mipp->mipstrengthening[p][t][r],3);
                        //lp_set_max_seconds(mipp->mipstrengthening[p][t][r],1);
                        VStr_free(&namesZ);
                     //   printf("--- p %d, t %d, r %d end %d\n", p,t,r,end);
                  //  }
                }else{
                    equals[t]=-1;
                }
                free(totalusesN);
                free(totalusesRR);

                free(jobmodeinsubset);

                VInt_free(&combJ);
                VInt_free(&combM);
                VInt_free(&initsubset);
            }


           if(value !=-1){

            //for(int r = 0 ; r < Inst_nResR(mipp->inst) ; r++){


                for(int r = 0; r < Inst_nResR(mipp->inst) ; r++){
             //   if(mipp->mipstrengthening[p][t][r]==NULL) continue;
                  //  printf("-p %d, t %d, r %d: ", p,t,r); fflush(stdout);


                    int status = lp_optimize_as_continuous(mipp->mipstrengthening[p][t]);
                //int hasSolution = ( ( status == LP_OPTIMAL) || ( status == LP_FEASIBLE ) );
               // if ( 1 )
                //{
                    const double *z = lp_x(mipp->mipstrengthening[p][t]);
                    char prefix[STR_SIZE];
                    int id[3];
                    int idx[lp_cols(mipp->mipstrengthening[p][t])];
                    double coef[lp_cols(mipp->mipstrengthening[p][t])];
                    int cont = 0 ;
                    for ( int i=0 ; (i<lp_cols(mipp->mipstrengthening[p][t])) ; ++i ) {
                        char name[STR_SIZE];
                        lp_col_name(mipp->mipstrengthening[p][t],i,name);
                        parseName( name, prefix, id );
                        if( z[i]>0){
//                            if (prefix[0]=='z') {
                            int j = id[0];
                            int m = id[1];
                            if(Par_getTypePSP(mipp->par)==2){
                                char namev[STR_SIZE];
                                sprintf(namev,"z(%d,%d,%d)",j,m,t);
                                int idz = lp_col_index(mipp->mip,namev);
                                idx[cont] = idz;
                                coef[cont] = z[i];
                                cont++;
                            }
                            mipp->coefR[j][m][r][t] = z[i];
//                          }
                        }
                    }
                    if(Par_getTypePSP(mipp->par)==2){
                        char namec[STR_SIZE];
                        sprintf(namec,"resRZ(%d,%d,%d)",r,t,p);
                  //      printf(" resRZ(%d,%d,%d) \n",r,t,p);
                        lp_add_row( mipp->mip, cont, idx, coef, namec, 'L', Inst_capResR(mipp->inst, r) );
                    }
                    //  if(r==0 && t==13){
                        lp_write_lp(mipp->mipstrengthening[p][t], "mipstrengtheningall.lp");
                        lp_write_sol(mipp->mipstrengthening[p][t],"mipstrengtheningall.sol");
                        lp_write_lp(mipp->mip,"mipstrengtheningooriall.lp");
                        printf("print lp...\n");
                        getchar();
                   // }
                   // }
               // } // has solution
                    //  lp_free(&mipstrengthening);

                }
            }

            VInt_free(&setPJ[t]);
            VInt_free(&setPM[t]);
        }
        free(setPJ);
        free(setPM);

        for(int t = 0 ; t < mipp->maxT; t++)
        {
            if(equals[t]==0){


                    lp_free(&mipp->mipstrengthening[p][t]);


            }
        }
        free(equals);
    }

//printf("fim str\n"); fflush(stdout);
}
*/
int MipC_getSumTPD( MIPCompact *mipp)
{
    return mipp->tpdSum;
}

void createXZ( MIPCompact *mipp, double timeLeft)
{
    assert(mipp!=NULL);

    double startT = omp_get_wtime();
    double _time = 0;
    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int firstJob = mipp->idxFirstJob;
    const int nJobs = mipp->nJobs;
    const int tpdSum = mipp->tpdSum;
    VecInt *coltozero = VInt_create();

    VecStr *names = VStr_create( STR_SIZE );

    for ( int j=firstJob ; (j<firstJob+nJobs) ; ++j )
    {
        const Job *job = Inst_job( inst, j );
        const int nModes = Job_nModes( job );

        mipp->maxTJ[j] = 0; //max tempo by job

        for ( int m=0 ; (m<nModes) ; ++m )
        {
            const Mode *mode = Job_mode( job, m );
            if (!Mode_isFeasible(inst,mode))
                continue;

            const int est = Job_est( job );
            const int duration = Mode_duration( mode );
            int cp1 = Inst_computeCompPathsByJob(inst,j,m,duration);
            int delay = cp1+tpdSum;

            if(nModes==1 && Job_nSucc(job)>0 && Job_nPred(job)<=0)
            {
                delay = est;
            }
            const int maxTJM = mipp->maxTJM[j][m] = delay;
            mipp->maxTJ[j] = MAX( mipp->maxTJ[j], maxTJM );

//             printf("\n duration %d est %d maxTJM %d, maxTJM[%d][%d] %d \n",duration, est, maxTJM, j,m, mipp->maxTJM[j][m]); //getchar();
            for ( int t=est ; (t<=maxTJM) ; ++t )
            {

                char vname[STR_SIZE];
                char zname[STR_SIZE];

                int over = t - cp1 < 0 ? 0 : t - cp1 ;
                if(over > Inst_getSumTPD(inst))
                {
                    printf(" estoura o tpd do projeto x(%d,%d,%d)", j, m, t );
                    continue;
                }

                sprintf( vname, "x(%d,%d,%d)", j, m, t );
               // printf("x(%d,%d,%d) ", j, m, t );
                const int currXIdx = lp_cols(mip) + VStr_size( names );
                VInt_pushBack( mipp->jIdx[j], currXIdx );
                VInt_pushBack( mipp->jtIdx[j][t], currXIdx );
                VInt_pushBack( mipp->jmIdx[j][m], currXIdx );

                VStr_pushBack( names, vname );

                sprintf( zname, "z(%d,%d,%d)", j, m, t );
                //printf( " z(%d,%d,%d) ", j, m, t ); fflush(stdout);
                VStr_pushBack( names, zname );
                VInt_pushBack(coltozero,VStr_size(names)-1);
                VInt_pushBack( mipp->dJ[j], duration );

                //  adding resource constraints
             /*    for ( int ir=0 ; (ir<Mode_nResR(mode)) ; ++ir )
                 {
                     int idxR = Mode_idxResR( mode, ir );
                     int use = Mode_useResR( mode, ir );

                     assert( use>0 );
                     //for ( int tj=t ; ((tj<t+Mode_duration(mode))&&(tj<= mipp->maxT-duration)) ; ++tj ) {
                     for ( int tj=t ; ((tj<t+Mode_duration(mode))) ; ++tj )
                     {
                         VInt_pushBack( mipp->idxResR[idxR][tj], currXIdx );
                         VDbl_pushBack( mipp->coefResR[idxR][tj], use );
                     }
                 }*/

                // non renewable constraints
                for ( int inr=0 ; (inr<Mode_nResN(mode)) ; ++inr )
                {
                    int idxNR = Mode_idxResN( mode, inr );
                    int use = Mode_useResN( mode, inr );

                    VInt_pushBack( mipp->idxResNR[idxNR], currXIdx );
                    VDbl_pushBack( mipp->coefResNR[idxNR], use );
                }
            }
            if(maxTJM>=0 && est<=maxTJM)
            {
                for(int t = maxTJM+1 ; t< maxTJM+Mode_duration(mode) ; t++)
                {


                    int over = maxTJM - cp1 < 0 ? 0 : maxTJM - cp1 ;
                    if(over > Inst_getSumTPD(inst))
                    {
                        printf(" estoura o tpd do projeto x(%d,%d,%d)", j, m, maxTJM ); getchar();
                        continue;
                    }

                    char zname[STR_SIZE];
                    sprintf( zname, "z(%d,%d,%d)", j, m, t );
                   // printf( "-- z(%d,%d,%d) maxTJM %d\n", j, m, t, maxTJM ); fflush(stdout);
                   // getchar();
                    VStr_pushBack( names, zname );
                }
            }
        }

        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreateXZ Time is over %f Time Left %f \n", _time, timeLeft);
            exit(0);
        }

    }
//    getchar();


    const int nCols = VStr_size(names);
    // printf("nObjt nCols %d\n", nCols);
    double *obj;//obj[nCols]
    ALLOCATE_VECTOR_INI(obj,double,nCols);

    FILL( obj, 0, nCols, 0.0 );

    storeVarNames( nCols, (const char**)VStr_ptr(names), mipp );

    lp_add_bin_cols( mip, nCols, obj, VStr_ptr(names) );

    if(Par_getVariablesZ(mipp->par)==2){
        for(int i = 0 ; i < VInt_size(coltozero); i++){
            int col = VInt_get(coltozero,i);
            lp_fix_col(mip,col,0.0);
        }
    }

    VInt_free(&coltozero);
    free(coltozero);
    free(obj);
    VStr_free( &names );
    // VStr_free( &names2 );
}

void createModeSelCons( MIPCompact *mipp, double timeLeft )
{

    double startT = omp_get_wtime();


    LinearProgram *mip = mipp->mip;
    const int firstJob = mipp->idxFirstJob;
    const int nJobsP = mipp->nJobs;

    for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
    {
        double _time = 0;

        // const Job *job = Inst_job(mipp->inst,j);
        // if(Job_minDuration(job)==0) continue;
        const VecInt *jIdx = mipp->jIdx[j];
        char cname[STR_SIZE];
        sprintf( cname, "modeSel(%d)", j );
        //       if(VInt_size(jIdx)<=0) continue;

        double coef[ VInt_size(jIdx) ];
        FILL( coef, 0, VInt_size(jIdx), 1.0 );
        lp_add_row( mip, VInt_size(jIdx), VInt_getPtr((VecInt*)jIdx), coef, cname, 'E', 1.0 );
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreateModeSelCons: Time is over %f Time Left %f \n", _time, timeLeft);
            exit(0);
        }
    }
}

void createResConsR( MIPCompact *mipp, double timeLeft)
{
    double _time;
    double startT = omp_get_wtime();

    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int maxT = mipp->maxT;

    for ( int r=0 ; (r<Inst_nResR(inst)) ; ++r )
    {
        for ( int t=0 ; (t<=maxT) ; ++t )
        {

            char name[STR_SIZE];
            //ALLOCATE_VECTOR(name, char, STR_SIZE)
            sprintf( name, "resR(%d,%d)", r, t );

            const VecInt *vidx = mipp->idxResR[r][t];
            const VecDbl *vcoef = mipp->coefResR[r][t];

            const int nz = VInt_size( vidx );

            if (!nz) continue;
          //  printf( "\n\n resR(%d,%d) nz %d: ", r, t,nz );

           /* for(int n = 0 ; n < nz; n++){
                char namet[STR_SIZE];
                lp_col_name(mipp->mip,VInt_get(vidx,n), namet);
                printf("%f*%d (%s), ", VDbl_get(vcoef,n),VInt_get(vidx,n), namet); fflush(stdout);
            }*/
            int *idx =  VInt_getPtr( (VecInt *) vidx );
            double *coef =  VDbl_getPtr( (VecDbl *) vcoef );

            lp_add_row( mip, nz, idx, coef, name, 'L', Inst_capResR(inst, r) );
            //  printf( "< %d: ",  Inst_capResR(inst, r));//getchar();

            _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
            if(_time <= 0)
            {
                printf( "\ncreateResConsR: Time is over %f Time Left %f \n", _time, timeLeft);
                exit(0);
            }
            // free(name);
        }
    }
}

void createResConsRLiftByZ( MIPCompact *mipp, double timeLeft)
{
    double _time;
    double startT = omp_get_wtime();

    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int maxT = mipp->maxT;

    const int firstJob = mipp->idxFirstJob;
    const int nJobs = mipp->nJobs;
    const int tpdSum = mipp->tpdSum;



    for ( int j=firstJob ; (j<firstJob+nJobs) ; ++j )
    {
        const Job *job = Inst_job( inst, j );
        const int nModes = Job_nModes( job );

        for ( int m=0 ; (m<nModes) ; ++m )
        {
            const Mode *mode = Job_mode( job, m );
            if (!Mode_isFeasible(inst,mode))
                continue;


            const int est = Job_est( job );
            const int duration = Mode_duration( mode );
            int cp1 = Inst_computeCompPathsByJob(inst,j,m,duration);
            int delay = cp1+tpdSum;

            if(nModes==1 && Job_nSucc(job)>0 && Job_nPred(job)<=0)
            {
                delay = est;
            }
            const int maxTJM = mipp->maxTJM[j][m] = delay;
            mipp->maxTJ[j] = MAX( mipp->maxTJ[j], maxTJM );


            //printf("duration %d est %d  maxTJM[%d][%d] %d \n",duration, est,j,m, mipp->maxTJM[j][m]); getchar();
            for ( int t=est ; (t<=maxTJM) ; ++t )
            {


                int over = t - cp1 < 0 ? 0 : t - cp1 ;
                if(over > Inst_getSumTPD(inst))
                {
                    printf(" estoura o tpd do projeto x(%d,%d,%d)", j, m, t );
                    continue;
                }

                char vname[STR_SIZE];
                sprintf( vname, "x(%d,%d,%d)", j, m, t );
                const int currXIdx = lp_col_index(mipp->mip,vname);
                //  adding resource constraints
                for ( int ir=0 ; (ir<Mode_nResR(mode)) ; ++ir )
                {
                    int r = Mode_idxResR( mode, ir );
                    int use = Mode_useResR( mode, ir );
                    //for ( int tj=t ; ((tj<t+Mode_duration(mode))&&(tj<= mipp->maxT-duration)) ; ++tj ) {
                    for ( int tj=t ; ((tj<t+Mode_duration(mode))) ; ++tj )
                    {

                        if(Par_getTypePSP(mipp->par)!=2)
                            use = mipp->coefR[j][m][r][tj];

                        assert( use>0 );
                        VInt_pushBack( mipp->idxResR[r][tj], currXIdx );
                        VDbl_pushBack( mipp->coefResR[r][tj], use );
                    }

                }
            }
        }
    }


    for ( int r=0 ; (r<Inst_nResR(inst)) ; ++r )
    {
        for ( int t=0 ; (t<=maxT) ; ++t )
        {

            char name[STR_SIZE];
            //ALLOCATE_VECTOR(name, char, STR_SIZE)
            sprintf( name, "resR(%d,%d)", r, t );

            const VecInt *vidx = mipp->idxResR[r][t];
            const VecDbl *vcoef = mipp->coefResR[r][t];

            const int nz = VInt_size( vidx );
          //  printf( "\n\n resR(%d,%d) nz %d: ", r, t,nz );

           /*
            for(int n = 0 ; n < nz; n++){
                char namet[STR_SIZE];
                lp_col_name(mipp->mip,VInt_get(vidx,n), namet);
                printf("%f*%d (%s), ", VDbl_get(vcoef,n),VInt_get(vidx,n), namet); fflush(stdout);
            }*/

            if (!nz) continue;

            int *idx =  VInt_getPtr( (VecInt *) vidx );
            double *coef =  VDbl_getPtr( (VecDbl *) vcoef );

            lp_add_row( mip, nz, idx, coef, name, 'L', Inst_capResR(inst, r) );
          //  printf( "< %d: ",  Inst_capResR(inst, r));//getchar();

            _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
            if(_time <= 0)
            {
                printf( "\ncreateResConsR: Time is over %f Time Left %f \n", _time, timeLeft);
                exit(0);
            }
            // free(name);
        }
    }
}


void createResConsRByZ( MIPCompact *mipp, double timeLeft)
{
    double _time;
    double startT = omp_get_wtime();

    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int maxT = mipp->maxT;

    const int firstJob = mipp->idxFirstJob;
    const int nJobs = mipp->nJobs;

    for ( int j=firstJob ; (j<firstJob+nJobs) ; ++j )
    {
        const Job *job = Inst_job( inst, j );
        const int nModes = Job_nModes( job );

        for ( int m=0 ; (m<nModes) ; ++m )
        {
            const Mode *mode = Job_mode( job, m );
            if (!Mode_isFeasible(inst,mode))
                continue;

            const int est = Job_est( job );

            //printf("duration %d est %d  maxTJM[%d][%d] %d \n",duration, est,j,m, mipp->maxTJM[j][m]); getchar();
             if(mipp->maxTJM[j][m]>=0 && est<=mipp->maxTJM[j][m]){
            for ( int t=est ; (t<=mipp->maxTJM[j][m]+Mode_duration(mode)) ; ++t )
            {

                char vname[STR_SIZE];
                sprintf( vname, "z(%d,%d,%d)", j, m, t );
                const int currZIdx = lp_col_index(mipp->mip,vname);

                if(currZIdx==-1) continue;

                 //  adding resource constraints
                for ( int ir=0 ; (ir<Mode_nResR(mode)) ; ++ir )
                {
                    int r = Mode_idxResR( mode, ir );
                    double use = Mode_useResR( mode, ir );

                    if(Par_getTypePSP(mipp->par)!=2)
                        use = mipp->coefR[j][m][r][t];

                    assert( use>0 );
                    VInt_pushBack( mipp->idxResR[r][t], currZIdx );
                    VDbl_pushBack( mipp->coefResR[r][t], use );

                }
            }
            }
        }
    }


    for ( int r=0 ; (r<Inst_nResR(inst)) ; ++r )
    {
        for ( int t=0 ; (t<=maxT) ; ++t )
        {

            char name[STR_SIZE];
            //ALLOCATE_VECTOR(name, char, STR_SIZE)
            sprintf( name, "resR(%d,%d)", r, t );

            const VecInt *vidx = mipp->idxResR[r][t];
            const VecDbl *vcoef = mipp->coefResR[r][t];

            const int nz = VInt_size( vidx );
           // printf( "\n\n resR(%d,%d) nz %d: ", r, t,nz );

           /*  for(int n = 0 ; n < nz; n++){
                char namet[STR_SIZE];
                lp_col_name(mipp->mip,VInt_get(vidx,n), namet);
                printf("%f*%d (%s), ", VDbl_get(vcoef,n),VInt_get(vidx,n), namet); fflush(stdout);
            }*/

            if (!nz) continue;

            int *idx =  VInt_getPtr( (VecInt *) vidx );
            double *coef =  VDbl_getPtr( (VecDbl *) vcoef );

            lp_add_row( mip, nz, idx, coef, name, 'L', Inst_capResR(inst, r) );
          //  printf( "< %d: ",  Inst_capResR(inst, r)); //getchar();
            _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
            if(_time <= 0)
            {
                printf( "\ncreateResConsR: Time is over %f Time Left %f \n", _time, timeLeft);
                exit(0);
            }
            // free(name);
        }
    }
}

void createResConsNR( MIPCompact *mipp, double timeLeft )
{
    double _time = 0;
    double startT = omp_get_wtime();

    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int nJobs = Inst_nJobs( inst );
    const int maxModes = mipp->maxModes;
    const int firstJob = mipp->idxFirstJob;
    const int nJobsP = mipp->nJobs;

    for ( int i=0 ; (i<Inst_nResN(inst)) ; ++i )
    {
        char cname[STR_SIZE];
        sprintf( cname, "resN(%d)", i );

        const int maxNZ = nJobs*maxModes;

        int idx[maxNZ];
        double coef[maxNZ];
        int nz = 0;

        for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
        {
            const Job *job = Inst_job( inst, j );
            const int nModes = Job_nModes( job );
            for ( int m=0 ; (m<nModes) ; ++m )
            {
                const Mode *mode = Job_mode( job, m );
                for ( int k=0 ; (k<Mode_nResN(mode)) ; ++k )
                {
                    if ( (Mode_idxResN(mode,k)==i)&&(Mode_useResN(mode,k)) )
                    {
                        if(mipp->uIdx[j][m]==-1) continue;
                        idx[nz] = mipp->uIdx[j][m];
                        coef[nz] = Mode_useResN(mode,k);
                        ++nz;
                        break;
                    }
                } /* checking resources */
            } /* modes */
        } /* job */

        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreateResConsNR: Time is over %f Time Left %f \n", _time, timeLeft);
            exit(0);
        }


        if (!nz)
            continue;
        int coemdc[nz+1];
        for(int cc = 0; cc < nz ; cc++)
        {
            coemdc[cc] = ROUND(coef[cc]);
            //  printf (" %d * %d ", coemdc[cc], idx[cc]);
        }
        coemdc[nz] = Inst_capResN( inst, i );
        //printf (" RHS %d ", coemdc[c]);
        //printf (": end print original \n");
        int mdc = CutP_maxDivisorCommonVector(coemdc, nz);
        //printf ("\n ------ MDC: %d ------ \n", mdc);
        for(int cc = 0; cc < nz ; cc++)
        {
            coef[cc] = (double) coef[cc]/mdc;
            //  printf ("%f * %d ", coe[cc], idx[cc]);
        }
        double rhs = Inst_capResN( inst, i )/mdc;
        lp_add_row( mip, nz, idx, coef, cname, 'L', rhs );
    }

}

void createY( MIPCompact *mipp, double timeLeft )
{
    double startT = omp_get_wtime();

    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int firstJob = mipp->idxFirstJob;
    const int nJobsP = mipp->nJobs;
    int cont = 0;
    int nVars = firstJob+nJobsP+1;
    double *obj, *lb, *ub;//obj[nVars], lb[nVars], ub[nVars];
    ALLOCATE_VECTOR_INI(obj, double, nVars);
    ALLOCATE_VECTOR_INI(lb, double, nVars);
    ALLOCATE_VECTOR_INI(ub, double, nVars);

    char *integer;//[nVars];
    ALLOCATE_VECTOR_INI(integer, double, nVars);
    FILL( obj, 0,  nVars, 0.0 );

    VecStr *names = VStr_create( STR_SIZE );
    for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
    {
        char cname[STR_SIZE];
        sprintf( cname, "y(%d)", j );
        mipp->yIdx[j] = lp_cols( mip ) + VStr_size( names );
        VStr_pushBack( names, cname );
        double _time = 0;
        const Job *job = Inst_job( inst, j);
        lb[cont] = Job_est( job );

        int maxJTM = 0;
        for ( int k=0 ; (k<Job_nModes(job)) ; ++k )
            maxJTM = MAX( maxJTM, mipp->maxTJM[j][k]);

        ub[cont] = maxJTM;
        integer[cont] = True;
        if (Job_nSucc(job)==0)
            obj[cont] = 1.0;
        cont++;
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreateY: Time is over %f Time Left %f \n", _time, timeLeft);
            exit(0);
        }
    }

    if(Par_getTypePSP(MipC_getPar(mipp))==2)
    {
        char name[STR_SIZE];
        lb[cont] = 0;
        ub[cont] = INT_MAX;
        obj[cont] = 1e-5;
        integer[cont] = True;
        sprintf(name, "yMax");
        VStr_pushBack( names, name );
        cont++;
    }

    lp_add_cols( mip, VStr_size(names), obj, lb, ub, integer, VStr_ptr(names) );
    if(Par_getTypePSP(MipC_getPar(mipp))==2)
    {

        for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
        {
            const Job *job = Inst_job( inst, j);
            if (Job_nSucc(job)==0)
            {
                char consname[STR_SIZE];
                sprintf( consname, "consYMax(%d)", j );
                int *idxy;
                double *coefy;
                ALLOCATE_VECTOR_INI(idxy,int,2);
                ALLOCATE_VECTOR_INI(coefy,double,2);
                idxy[0] = mipp->yIdx[j];
                coefy[0] = -1.0;
                idxy[1] = lp_col_index(mip,"yMax");
                coefy[1] = 1.0;
                lp_add_row(mip,2,idxy,coefy,consname,'G',0);
                free(idxy);
                free(coefy);
            }
        }
    }


    free(obj);
    free(lb);
    free(ub);
    free(integer);
    VStr_free( &names );
}

void createConsLinkXY( MIPCompact *mipp, double timeLeft )
{
    double startT = omp_get_wtime();

    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int firstJob = mipp->idxFirstJob;
    const int nJobsP = mipp->nJobs;

    VecInt *aIdx  = VInt_create();
    VecDbl *aCoef = VDbl_create();

    for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
    {

        double _time = 0;
        const Job *job = Inst_job( inst, j );
        if(Job_nModes(job)==1&&Job_nSucc(job)>0&&Job_minDuration(job)==0) continue;
        const int est = Job_est( job );

        VInt_clear( aIdx );
        VDbl_clear( aCoef );

        for ( int t=est ; (t<=mipp->maxTJ[j]) ; ++t )
        {
            if(t==0) continue; //janniele
            const int nz = VInt_size( mipp->jtIdx[j][t] );
            const int *idx = VInt_getPtr( mipp->jtIdx[j][t] );
            for ( int k=0 ; k<nz ; ++k )
            {
                VInt_pushBack( aIdx, idx[k] );
                VDbl_pushBack( aCoef, t );
            }
        }
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreateConsLinkXY: Time is over %f Time Left %f \n", _time, timeLeft);
            exit(0);
        }

        if (!VInt_size(aIdx))
            continue;

        VInt_pushBack( aIdx, mipp->yIdx[j] );
        VDbl_pushBack( aCoef, -1.0 );
        const int nz = VInt_size( aIdx );
        int *idx = VInt_getPtr( aIdx );
        double *coef = VDbl_getPtr( aCoef );

        char cname[STR_SIZE];
        sprintf( cname, "lnkXY(%d)", j );
        lp_add_row( mip, nz, idx, coef, cname, 'E', 0.0 );

    }

    VInt_free( &aIdx );
    VDbl_free( &aCoef );

}

void createConsLinkXZ( MIPCompact *mipp, double timeLeft )
{
    double startT = omp_get_wtime();

    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int firstJob = mipp->idxFirstJob;
    const int nJobsP = mipp->nJobs;

    VecInt *aIdx  = VInt_create();
    VecDbl *aCoef = VDbl_create();
    VecInt *aStarts = VInt_create();
    VecStr *aNames = VStr_create(20);
    int cont2 = 0;

    for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
    {

        double _time = 0;
        const Job *job = Inst_job( inst, j );

        for ( int m = 0 ; m < Job_nModes(job) ; m++)
        {

            const Mode *mode = Job_mode(job,m);

            const int est = Job_est( job );

            if(mipp->maxTJM[j][m]<0)continue;


            for ( int t=est ; (t<mipp->maxTJM[j][m]+Mode_duration(mode)) ; t++ )
            {

           //     if((t-Mode_duration(mode)+1)<0 || ((t-Mode_duration(mode)+1)<est)) continue;

                char name[STR_SIZE];

                int cont =0;
                int dur = t-Mode_duration(mode)+1;
                if(dur<est) dur=est;
                for ( int tx=(dur) ; (tx<=t) ; ++tx )
                {
                    sprintf( name, "x(%d,%d,%d)", j,m,tx);
                    int idxX = lp_col_index(mip,name);

                    if(idxX!=-1)
                    {
                        VInt_pushBack( aIdx,  idxX );
                        VDbl_pushBack( aCoef,  -1.0 );
                        cont++;
                    }


                }
                if(Mode_duration(mode)==0 && cont==0)
                {
                    sprintf( name, "x(%d,%d,%d)", j,m,t);
                    int idxX = lp_col_index(mip,name);
                    if(idxX!=-1)
                    {
                        VInt_pushBack( aIdx,  idxX );
                        VDbl_pushBack( aCoef,  -1.0 );
                        cont++;
                    }

                }


                if(cont>0)
                {
                    sprintf( name, "z(%d,%d,%d)", j,m,t);
                    int idxZ = lp_col_index(mip,name);
                    VInt_pushBack( aIdx, idxZ );
                    VDbl_pushBack( aCoef,  1.0 );
                    VInt_pushBack( aStarts, cont2);
                    cont++;
                    cont2 += cont;
                    char cname[20];
                    sprintf( cname, "lnkXZ(%d,%d,%d)", j,m,t);
                    VStr_pushBack(aNames,cname);
                  //  printf("cont2 %d\n", cont2); getchar();
                   // lp_add_row( mip, VInt_size(aIdx), VInt_getPtr(aIdx), VDbl_getPtr(aCoef), cname, 'E', 0.0 );
                }
               // VInt_clear( aIdx );
              //  VDbl_clear( aCoef );

                _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
                if(_time <= 0)
                {
                    printf( "\n inside createConsLinkXZ: Time is over %f Time Left %f \n", _time, timeLeft);
                    exit(0);
                }

            }
        }
    }

    int nRows = VStr_size(aNames);
    VInt_pushBack( aStarts, cont2);
    char *aSenses;
    ALLOCATE_VECTOR(aSenses,char,nRows);
    FILL(aSenses,0,nRows,'E');
    double *aRhs;
    ALLOCATE_VECTOR_INI(aRhs,double,nRows);

    if(nRows>0)
    {
        lp_add_rows( mip, nRows, VInt_getPtr(aStarts), VInt_getPtr(aIdx), VDbl_getPtr(aCoef), aSenses, aRhs, VStr_ptr(aNames) );
    }

    free(aSenses);
    free(aRhs);
    VInt_free( &aStarts );
    VInt_free( &aIdx );
    VDbl_free( &aCoef );
    VStr_free( &aNames);

}

void createPrecCons0( MIPCompact *mipp, double timeLeft )
{
    double _time = 0;
    double startT = omp_get_wtime();

    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int firstJob = mipp->idxFirstJob;
    const int nJobsP = mipp->nJobs;

    for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
    {
        const Job *job = Inst_job( inst, j );
        if(Job_nModes(job)==1&&Job_nSucc(job)>0&&Job_minDuration(job)==0) continue;

        for ( int i=0 ; i<Job_nSucc(job) ; ++i )
        {
            const int idxSucc = Job_succ( job, i );
            char cname[STR_SIZE];
            sprintf( cname, "prec(%d,%d)", j, idxSucc );
            int idx[3] = { mipp->yIdx[idxSucc], mipp->yIdx[j], mipp->dIdx[j] };
            double coef[3] = { 1.0, -1.0, -1.0 };
            lp_add_row( mip, 3, idx, coef, cname, 'G', 0.0 );
            _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
            if(_time <= 0)
            {
                printf( "\ncreatePrecCons0: Time is over %f \n", _time);
                exit(0);
            }

        }

    }
}

void createPrecCons1( MIPCompact *mipp, double timeLeft )
{
    double startT = omp_get_wtime();

    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int firstJob = mipp->idxFirstJob;
    const int nJobsP = mipp->nJobs;

    VecInt *vIdx = VInt_create();
    VecDbl *vCoef = VDbl_create();

    for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
    {
        double _time =0;
        const Job *job = Inst_job( inst, j );
        if(Job_nModes(job)==1&&Job_nSucc(job)>0&&Job_minDuration(job)==0) continue;

        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreatePrecCons1: Time is over %f \n", _time);
            exit(0);

        }

        if (!Job_nSucc(job))
            continue;
        const int est = Job_est( job );
        for ( int m=0 ; (m<Job_nModes(job)) ; ++m )
        {
            for ( int t=est ; (t<=mipp->maxTJM[j][m]) ; ++t )
            {
                const int d = Mode_duration( Job_mode(job, m ) );
                int xIdx = getXIdx( mipp, j,m,t);

                _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
                if(_time <= 0)
                {
                    printf( "\ncreatePrecCons1: Time is over %f \n", _time);
                    exit(0);

                }

                if (xIdx==-1)
                    continue;
                for ( int k=0 ; (k<Job_nSucc(job)) ; ++k )
                {
                    int sucIdx = Job_succ( job, k );
                    const Job *jSucc = Inst_job( inst, sucIdx );
                    const int estSuc = Job_est( jSucc );

                    VInt_clear( vIdx );
                    VDbl_clear( vCoef );

                    VInt_pushBack( vIdx, xIdx );
                    VDbl_pushBack( vCoef, 1.0 );

                    for ( int m2=0 ; (m2<Job_nModes(jSucc)) ; ++m2 )
                    {
                        for ( int t2 = t+d-1 ; (t2>=estSuc) ; --t2 )
                        {
                            int xIdxS = getXIdx( mipp, sucIdx, m2, t2 );
                            _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
                            if(_time <= 0)
                            {
                                printf( "\ncreatePrecCons1: Time is over %f \n", _time);
                                exit(0);

                            }

                            if (xIdxS==-1)
                                continue;

                            VInt_pushBack( vIdx, xIdxS );
                            VDbl_pushBack( vCoef, 1.0 );
                        }
                    }

                    int nz = VInt_size( vIdx );
                    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
                    if(_time <= 0)
                    {
                        printf( "\ncreatePrecCons1: Time is over %f \n", _time);
                        exit(0);

                    }

                    if (nz<=1)
                        continue;
                    int *idx = VInt_getPtr( vIdx );
                    double *coef = VDbl_getPtr( vCoef );
                    char rname[STR_SIZE];
                    sprintf( rname, "pred(%d,%d,%d,%d)", j, m, t, sucIdx );
                    lp_add_row( mip, nz, idx, coef, rname, 'L', 1.0 );
                } /* all times */
            } /* all modes */
            _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
            if(_time <= 0)
            {
                printf( "\ncreatePrecCons1: Time is over %f \n", _time);
                exit(0);

            }

        }

    } /* all jobs */

    VInt_free( &vIdx );
    VDbl_free( &vCoef );
}

static void createPrecCons2( MIPCompact *mipp, double timeLeft )
{
    double _time =0;
    double startT = omp_get_wtime();

    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int firstJob = mipp->idxFirstJob;
    const int nJobsP = mipp->nJobs;

    VecInt *vIdx = VInt_create();
    VecDbl *vCoef = VDbl_create();


    for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
    {

        const Job *job = Inst_job( inst, j );
        if(Job_nModes(job)==1&&Job_nSucc(job)>0&&Job_minDuration(job)==0) continue;

        const int est = Job_est( job );
        for ( int k=0 ; (k<Job_nSucc(job)) ; ++k )
        {
            int sucIdx = Job_succ( job, k );
            const Job *jSucc = Inst_job( inst, sucIdx );

            VInt_clear( vIdx );
            VDbl_clear( vCoef );

            /* pred */
            for ( int mp=0 ; (mp<Job_nModes(job)) ; ++mp )
            {
                const int d = Mode_duration( Job_mode(job, mp ) );
                for ( int t=est ; (t<=mipp->maxTJM[j][mp]) ; ++t )
                {
                    int xIdx = getXIdx( mipp, j, mp, t );
                    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
                    if(_time <= 0)
                    {
                        printf( "\ncreatePrecCons2: Time is over %f \n", _time);
                        exit(0);
                    }
                    if (xIdx==-1) continue;
                    VInt_pushBack( vIdx, xIdx );
                    VDbl_pushBack( vCoef, t+d );
                }
                _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
                if(_time <= 0)
                {
                    printf( "\ncreatePrecCons2: Time is over %f \n", _time);
                    exit(0);
                }
            }

            /* succ */
            for ( int ms=0 ; (ms<Job_nModes(jSucc)) ; ++ms )
            {
                for ( int tSucc=Job_est(jSucc) ; (tSucc<=mipp->maxTJM[sucIdx][ms]) ; ++tSucc )
                {
                    //  if(tSucc==0) continue; //janniele
                    int xIdx = getXIdx( mipp, sucIdx, ms, tSucc );
                    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
                    if(_time <= 0)
                    {
                        printf( "\ncreatePrecCons2: Time is over %f \n", _time);
                        exit(0);
                    }
                    if (xIdx==-1) continue;
                    VInt_pushBack( vIdx, xIdx );
                    VDbl_pushBack( vCoef, -tSucc );

                }
                _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
                if(_time <= 0)
                {
                    printf( "\ncreatePrecCons2: Time is over %f \n", _time);
                    exit(0);
                }
            }

            const int nz = VInt_size( vIdx );
            char name[STR_SIZE];
            sprintf( name, "precT(%d,%d)", j, sucIdx );
            lp_add_row( mip, nz, VInt_getPtr(vIdx), VDbl_getPtr(vCoef), name, 'L', 0.0 );
            _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
            if(_time <= 0)
            {
                printf( "\ncreatePrecCons2: Time is over %f \n", _time);
                exit(0);
            }

        }
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreatePrecCons2: Time is over %f \n", _time);
            exit(0);
        }

    }

    VInt_free( &vIdx );
    VDbl_free( &vCoef );
}

void createD( MIPCompact *mipp, double timeLeft )
{

    double startT = omp_get_wtime();
    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int firstJob = mipp->idxFirstJob;
    const int nJobsP = mipp->nJobs;

    VecStr *names = VStr_create( STR_SIZE );

    for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
    {
        double _time=0;
        char cname[STR_SIZE];
        sprintf( cname, "d(%d)", j );
        // printf( "d(%d) ", j );fflush(stdout);
        mipp->dIdx[j] = lp_cols( mip ) + VStr_size( names );
        VStr_pushBack( names, cname );
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreateD: Time is over %f Time Left %f \n", _time, timeLeft);
            exit(0);
        }
    }

    int nVars = VStr_size( names );

    double *obj, *lb, *ub;//obj[nVars], lb[nVars], ub[nVars];
    ALLOCATE_VECTOR_INI(obj, double, nVars);
    ALLOCATE_VECTOR_INI(lb, double, nVars);
    ALLOCATE_VECTOR_INI(ub, double, nVars);

    char *integer;//[nVars];
    ALLOCATE_VECTOR_INI(integer, double, nVars);

    //    double obj[nVars], lb[nVars], ub[nVars];
    //   char integer[nVars];
    FILL( obj, 0, nVars, 0.0 );
    for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
    {

        lb[j-firstJob] = Job_minDuration( Inst_job( inst, j ) );
        ub[j-firstJob] = Job_maxDuration( Inst_job( inst, j ) );
    }

    FILL( integer, 0, nVars, 1 );

    lp_add_cols( mip, VStr_size(names), obj, lb, ub, integer, VStr_ptr(names) );

    VStr_free( &names );

    free(obj);
    free(lb);
    free(ub);
    free(integer);
}

void createConsLinkXD( MIPCompact *mipp, double timeLeft )
{

    double startT = omp_get_wtime();


    LinearProgram *mip = mipp->mip;
    const int firstJob = mipp->idxFirstJob;
    const int nJobsP = mipp->nJobs;

    VecInt *aIdx  = VInt_create();
    VecDbl *aCoef = VDbl_create();

    for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
    {

        const Job*job =Inst_job(mipp->inst,j);
        if(Job_nModes(job)==1&&Job_minDuration(job)==0) continue;

        double _time = 0;

        VInt_clear( aIdx );
        VDbl_clear( aCoef );

        assert( VInt_size( mipp->jIdx[j] ) == VInt_size( mipp->dJ[j] )  );

        for ( int i=0 ; i<VInt_size( mipp->jIdx[j] ) ; ++i )
        {
            VInt_pushBack( aIdx, VInt_get( mipp->jIdx[j],i ) );
            VDbl_pushBack( aCoef, VInt_get( mipp->dJ[j],i )  );
        }

        VInt_pushBack( aIdx, mipp->dIdx[j] );
        VDbl_pushBack( aCoef, -1.0 );
        const int nz = VInt_size( aIdx );
        int *idx = VInt_getPtr( aIdx );
        double *coef = VDbl_getPtr( aCoef );

        char cname[STR_SIZE];
        sprintf( cname, "lnkXD(%d)", j );
        lp_add_row( mip, nz, idx, coef, cname, 'E', 0.0 );
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreateConsLinkXD: Time is over %f Time Left %f \n", _time, timeLeft);
            exit(0);
        }
    }

    VInt_free( &aIdx );
    VDbl_free( &aCoef );
}

void addFOConstants( MIPCompact *mipp, double timeLeft )
{

    double startT = omp_get_wtime();

    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;

    int sumRDCP = 0;
    if ( mipp->idxProj == MIPC_ALL_PROJECTS )
    {
        for ( int p=0 ; (p<Inst_nProjects(inst)) ; ++p )
        {
            double _time = 0;
            const Project *project = Inst_project( inst, p );
            sumRDCP += Project_releaseDate( project ) + Project_criticalPath( project );
            _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
            if(_time <= 0)
            {
                printf( "\ncreateConsLinkXD: Time is over %f Time Left %f \n", _time, timeLeft);
                exit(0);
            }
        }
    }
    else
    {
        const Project *project = Inst_project( inst, mipp->idxProj );
        sumRDCP += Project_releaseDate( project ) + Project_criticalPath( project );
    }

    double lb = 1.0, ub = 1.0, obj = -sumRDCP;
    VecStr *names = VStr_create( STR_SIZE );
    VStr_pushBack( names, "releaseDateAndCP");
    char integer = 1;
    lp_add_cols( mip, 1, &obj, &lb, &ub, &integer, VStr_ptr(names) );


    VStr_free( &names );
}

void createU( MIPCompact *mipp, double timeLeft )
{
    double startT = omp_get_wtime();

    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int firstJob = mipp->idxFirstJob;
    const int nJobsP = mipp->nJobs;

    VecStr *names = VStr_create( STR_SIZE );

    for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
    {
        double _time  =0;
        const Job *job = Inst_job( inst, j );
        const int nModes = Job_nModes( job );
        for ( int m=0 ; (m<nModes) ; ++m )
        {
            char cname[STR_SIZE];
            sprintf( cname, "u(%d,%d)", j, m );
            mipp->uIdx[j][m] = lp_cols( mip ) + VStr_size( names );
            VStr_pushBack( names, cname );
        }
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreateU: Time is over %f Time Left %f \n", _time, timeLeft);
            exit(0);
        }
    }

    const int nVars = VStr_size( names );
    double obj[nVars];
    FILL( obj, 0, nVars, 0.0 );
    lp_add_bin_cols( mip, nVars, obj, VStr_ptr( names ) );

    VStr_free( &names );
}

void createConsLnkXU( MIPCompact *mipp, double timeLeft )
{
    double startT = omp_get_wtime();


    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int firstJob = mipp->idxFirstJob;
    const int nJobsP = mipp->nJobs;

    for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
    {
        double _time = 0;
        const Job *job = Inst_job( inst, j );
        const int nModes = Job_nModes( job );
        for ( int m=0 ; (m<nModes) ; ++m )
        {
            char rname[STR_SIZE];
            sprintf( rname, "lnkXU(%d,%d)", j, m );
            const int nz = VInt_size( mipp->jmIdx[j][m] )+1;
            double coef[nz];
            FILL( coef, 0, nz, 1.0 );
            const int *idxT = VInt_getPtr( mipp->jmIdx[j][m] );
            int idx[nz];
            COPY_VECTOR( idx, idxT, int, nz-1 );
            coef[nz-1] = -1.0;
            idx[nz-1] = mipp->uIdx[j][m];

            lp_add_row( mip, nz, idx, coef, rname, 'E', 0.0 );
        }
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreateConsLnkXU: Time is over %f Time Left %f \n", _time, timeLeft);
            exit(0);
        }
    }
}

void createConsSelU( MIPCompact *mipp, double timeLeft )
{
    double startT = omp_get_wtime();

    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int firstJob = mipp->idxFirstJob;
    const int nJobsP = mipp->nJobs;

    for ( int j=firstJob ; (j<firstJob+nJobsP) ; ++j )
    {

        double _time = 0;
        const Job *job = Inst_job( inst, j );
        const int nModes = Job_nModes( job );

        char rname[STR_SIZE];
        sprintf( rname, "selU(%d)", j );
        int idx[nModes];
        double coef[nModes];
        FILL( coef, 0, nModes, 1.0 );
        for ( int i=0 ; (i<nModes) ; ++i )
            idx[i] = mipp->uIdx[j][i];

        lp_add_row( mip, nModes, idx, coef, rname, 'E', 1.0 );
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreateConsSelU: Time is over %f Time Left %f \n", _time, timeLeft);
            exit(0);
        }
    }

}

double MipC_TPD( MIPCompact *MipP )
{
    return MipP->tpd;
}

void storeVarNames( int n, const char **names, MIPCompact *mipp )
{
    LinearProgram *mip = mipp->mip;
    DictInt *dnames = mipp->dnames;

    for ( int i=0 ; (i<n) ; ++i )
        DInt_set( dnames, names[i], lp_cols(mip)+i );
}

int getXIdx( const MIPCompact *mipp, int j, int m, int t )
{
    char name[STR_SIZE];
    sprintf( name, "x(%d,%d,%d)", j, m, t );

    /*    printf( "x(%d,%d,%d) %d\n", j, m, t, DInt_get( mipp->dnames, name ) );
        const Job* job = Inst_job(mipp->inst,j);
        const Mode* mode = Job_mode(job,m);
        printf( "Infeasible %d: %d\n", m, Mode_isFeasible(mipp->inst, mode));
      */  //lp_write_lp(mipp->mip,"LPXID.lp");
    //getchar();
    return DInt_get( mipp->dnames, name );
}

void MipC_clear( MIPCompact *mipC )
{

    assert(mipC);

    memset(mipC->jobIntegral,0,sizeof(int)*Inst_nJobs(mipC->inst));

    mipC->nIntegral = 0;
    mipC->nFractional = 0;

    CLEAR_VECTOR(mipC->jmtFractional, IntTriple, Inst_nJobs(mipC->inst)*mipC->maxModes*(mipC->maxT+1)  );

    for(int j = 0 ; j < Inst_nJobs(mipC->inst) ; j++)
    {
        mipC->jmtIntegral[j].j = -1;
        mipC->jmtIntegral[j].m = -1;
        mipC->jmtIntegral[j].t = -1;
        mipC->jmtIntegral[j].value = 0;
    }

    //CLEAR_VECTOR(mipC->jmtIntegral, IntTriple, Inst_nJobs(mipC->inst)*mipC->maxModes*(mipC->maxT+1)  );
    //memset(mipC->jmtFractional, NULL, sizeof(IntTriple)* Inst_nJobs(mipC->inst)*mipC->maxModes*(mipC->maxT+1) );
    //memset(mipC->jmtIntegral, NULL, sizeof(IntTriple)*Inst_nJobs(mipC->inst)*mipC->maxModes*(mipC->maxT+1) );
    //ALLOCATE_VECTOR_INI( mipC->jmtFractional, IntTriple, Inst_nJobs(mipC->inst)*mipC->maxModes*(mipC->maxT+1) );
    //ALLOCATE_VECTOR_INI( mipC->jmtIntegral, IntTriple, Inst_nJobs(mipC->inst)*mipC->maxModes*(mipC->maxT+1) );

}

int MipC_linear_relaxation( MIPCompact *mipC, Solution *sol, Results *res,  double timeLeft)
{
    double startT = omp_get_wtime();
    int hasintegersol = 0;


    char filename[256]=" ";
    sprintf( filename, "%s_timeobj.dat", Par_getName(mipC->par));

    //  printf("MipC_solve_as_continuous_analyzing_cut: start\n");
    FILE *fp = fopen( filename, "a" );
    if ( fp == NULL )
    {
        printf( "File name time objective was not opened. : path: %s\n", filename);
        exit( 0 );
    }

    double _time = 0;

    LinearProgram *mip = mipC->mip;
    lp_set_print_messages( mip, 0);
    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    lp_set_max_seconds(mip,_time);
//lp_set_concurrentMIP(mip,1);
    //lp_set_method(mip,4);
    //lp_set_seed(mip,100000);
    int status = lp_optimize_as_continuous( mip );

    if(status == LP_OPTIMAL)
    {
        mipC->tpd = lp_obj_value( mip );
        // if(VERBOSE) printf("%f %g %d \n", (omp_get_wtime()-startT), lp_obj_value( mip ), lp_rows(mip)-originalrows);
        fprintf( fp, "%f %g %d \n", (omp_get_wtime()-startT), lp_obj_value( mip ), lp_rows(mip));
        hasintegersol = MipC_saveSol(mipC);
        // printf(" 1 status %d opt %d lp_obj_value %g\n", status, LP_OPTIMAL, lp_obj_value( mip ) );
    }

    if(hasintegersol) Sol_write( sol, Par_getName(mipC->par) );

    fclose( fp );

    return status;

}

int MipC_cutting_plane_cgraph( MIPCompact *mipC, Solution *sol, Results *res,  double timeLeft)
{
    double startT = omp_get_wtime();
    int hasintegersol = 0;

    int cp = Par_getCutPrec( mipC->par );
    int cr = Par_getCutRR( mipC->par );
    int cc = Par_getCutCLIQUE( mipC->par );
//   int cjs = Par_getCutJS( mipC->par );
    int co = Par_getCutODDHOLES( mipC->par );
    int cg = Par_getCutCG( mipC->par );
    int cgcpu = Par_getCutCGCPU( mipC->par );
    int cggpu = Par_getCutCGGPU( mipC->par );
    int cggpur2= Par_getCutCGGPUR2( mipC->par );

    double slk = Par_getSlack( mipC->par );
    double mc = Par_getMaxCut(mipC->par);
    int mi = Par_getMaxInstant(mipC->par);
    int jmp = Par_getJump(mipC->par);
    char filename[256]=" ";
    sprintf( filename, "%s_cg_%d_cggpu_%d_cggpur2_%d_cp_%d_cr_%d_cc_%d_co_%d_l_%d_mc_%.1f_rc_%.2f_slk_%.2f_mi_%d_jmp_%d_timeobj.dat",   Par_getName(mipC->par), cg,cggpu,cggpur2,cp,cr,cc, co, Par_getLifting(mipC->par),  mc, Par_getMaxReducedCost(mipC->par), slk,mi,jmp);

    //  printf("MipC_solve_as_continuous_analyzing_cut: start\n");
    FILE *fp = fopen( filename, "a" );
    if ( fp == NULL )
    {
        printf( "File name time objective was not opened. : path: %s\n", filename);
        exit( 0 );
    }

    char filenamecuttype[256]=" ";
    sprintf( filenamecuttype, "%s_cg_%d_cggpu_%d_cggpur2_%d_cp_%d_cr_%d_cc_%d_co_%d_l_%d_mc_%.1f_rc_%.2f_slk_%.2f_mi_%d_jmp_%d_timeobj_cuttype.dat",   Par_getName(mipC->par), cg,cggpu,cggpur2,cp,cr,cc,co,Par_getLifting(mipC->par),  mc, Par_getMaxReducedCost(mipC->par), slk,mi,jmp);

    FILE *fp2 = fopen( filenamecuttype, "a" );
    if ( fp2 == NULL )
    {
        printf( "File name cuttype opt as continues was not opened. : path: %s\n", filenamecuttype);
        exit( 0 );
    }
    double _time = 0;

    char filelpsol[256]=" ";
    sprintf( filelpsol, "%s_cg_%d_cggpu_%d_cggpur2_%d_cp_%d_cr_%d_cc_%d_co_%d_l_%d_mc_%f_rc_%f_slk_%f_mi_%d_jmp_%d_lp.sol", Par_getName(mipC->par), cg,cggpu,cggpur2,cp,cr,cc,co, Par_getLifting(mipC->par),  mc, Par_getMaxReducedCost(mipC->par), slk,mi,jmp);
    char filelpsolerr[256]=" ";
    sprintf( filelpsolerr, "%s_cg_%d_cggpu_%d_cggpur2_%d_cp_%d_cr_%d_cc_%d_co_%d_l_%d_mc_%f_rc_%f_slk_%f_mi_%d_jmp_%d_lp_err.sol", Par_getName(mipC->par), cg,cggpu,cggpur2,cp,cr,cc,co, Par_getLifting(mipC->par),  mc, Par_getMaxReducedCost(mipC->par), slk,mi,jmp);




    LinearProgram *mip = mipC->mip;
    CutPool *cutP = CutP_create(mipC->inst, mipC->mip);


    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
//    DataGRBCallback *usrdata = DataGRB_create(mipC,mipC->par,res,cutP,_time-1);
    //  lp_grb_cut_getcallback(mip, usrdata);
    lp_set_print_messages( mip, 0);
//lp_set_concurrentMIP(mip,1);
    lp_set_max_seconds(mip,_time);
    //lp_set_method(mip, 4);
    //lp_set_seed(mip,100000);
    lp_set_nThreads(mip,5);
    int status = lp_optimize_as_continuous( mip );

    int originalrows = lp_rows(mipC->mip);

    // printf(" 0 status %d opt %d lp_obj_value %g\n", status, LP_OPTIMAL, lp_obj_value( mip ) );
    if(status == LP_OPTIMAL)
    {
        mipC->tpd = lp_obj_value( mip );
        MipC_setVariablesZero(mipC);
        if(VERBOSE==2) printf("%f %g %d \n", (omp_get_wtime()-startT), lp_obj_value( mip ), lp_rows(mip)-originalrows);
        fprintf( fp, "%f %g %d \n", (omp_get_wtime()-startT), lp_obj_value( mip ), lp_rows(mip)-originalrows);
        hasintegersol = MipC_saveSol(mipC);
        if(hasintegersol)
        {
            Sol_read_sol_lp(sol, mip);
            Sol_write( sol, Par_getName(mipC->par) );
            return status;
        }
        lp_write_sol(mip,filelpsol);
        // printf(" 1 status %d opt %d lp_obj_value %g\n", status, LP_OPTIMAL, lp_obj_value( mip ) );
    }
    if(VERBOSE==2) printf("info: cols %d, rows %d, nz %d obj %g\n", lp_cols(mip), lp_rows(mip), lp_nz(mip), mipC->tpd);
    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0.01)
    {
        CutP_free(&cutP);
        return status;
    }

    int newCuts = 0;

    //    double lastobj = 0;
    double oldobj = lp_obj_value( mip );
    //    int cont = 0;
    double totalTimeOpt = 0.0;
    double totalTimeRemAdd = 0.0;

    // cr =1;
    // ce =1;
    //  int nround = 0;
    int origCols[lp_cols(mipC->mip )];
    for ( int i=0 ; (i<lp_cols(mipC->mip )) ; ++i )
        origCols[i] = i;


    double createCgraphTime = omp_get_wtime();
    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    CGraph *cgraph;
    if(cc || co || cg || cgcpu || cggpur2)
        cgraph = CutP_compute_conflicts_create_complete_graph(mipC->mip, origCols, mipC->mip, mipC->inst,_time);
    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(VERBOSE==2) printf("time create cgraph %f. \n\n",  (double)(omp_get_wtime()-createCgraphTime));
    createCgraphTime = (double)omp_get_wtime()-createCgraphTime;


    do
    {

        lp_set_nThreads(mip,5);
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0.01)
        {
            if(VERBOSE) printf(" time is over %f \n", _time);
            Res_setTotalTimeOpt(res,totalTimeOpt);
            Res_setTotalTimeSep(res,CutP_getTotalTimeSeparation(cutP));
            Res_setTotalTimeRemAdd(res,totalTimeRemAdd);
            Res_setTotalTimeCGraph(res,createCgraphTime);
            if(hasintegersol)
            {
                Sol_read_sol_lp(sol, mip);
                Sol_write( sol, Par_getName(mipC->par) );
            }
            CutP_free(&cutP);
            if(cc || co || cg || cgcpu || cggpur2)
                cgraph_free( &cgraph );

            fclose( fp );
            fclose( fp2 );
            return 6;
        }

        if(VERBOSE)
        {
            // nround = Res_getRound(res);
            Res_initRound(res);
        }

        newCuts = 0;

        mipC->hasSolution = ( status == LP_OPTIMAL);

        if ( mipC->hasSolution )
        {
            _time = ( (double) timeLeft - (omp_get_wtime()-startT) );

            newCuts = CutP_separationParallelCgraph(cutP, fp2, cgraph, mipC->par, res,   cr,  cp,  cc, co, cg, cggpur2, cgcpu, mipC->maxTJM, mipC->maxTJ, startT, _time);

            if( newCuts > 0)
            {
                double optTime = omp_get_wtime();
                if(VERBOSE == 2) printf("\nlp_rows after find cuts %d ", lp_rows(mip));
                _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
                lp_set_max_seconds(mip,_time);
                //lp_set_concurrentMIP(mip,1);
                //lp_set_method(mip,4);
                //lp_set_seed(mip,100000);
                status = lp_optimize_as_continuous( mip );
                totalTimeOpt +=(double)(omp_get_wtime()-optTime);
//                lp_write_lp(mip,"mipprint.lp"); getchar();
                if(VERBOSE == 2) printf("Time: %g \n Total Cuts: %d \n Obj: %g\n", _time, newCuts,lp_obj_value(mip));
                if(status != LP_OPTIMAL)
                {
                    if(VERBOSE) printf("is not optimal %d\n", status);
                    lp_write_lp(mip,filelpsolerr);
                    Res_setTotalTimeOpt(res,totalTimeOpt);
                    Res_setTotalTimeSep(res,CutP_getTotalTimeSeparation(cutP));
                    Res_setTotalTimeRemAdd(res,totalTimeRemAdd);
                    Res_setTotalTimeCGraph(res,createCgraphTime);
                    if(hasintegersol)
                    {
                        Sol_read_sol_lp(sol, mip);
                        Sol_write( sol, Par_getName(mipC->par) );
                    }
                    CutP_free(&cutP);
                    if(cc || co || cg || cgcpu || cggpur2)
                        cgraph_free( &cgraph );

                    fclose( fp );
                    fclose( fp2 );
                    return status;
                }

#ifdef EXTRA
                fprintf( fp2, "%f %g %d %d %d %d %d %d \n", (omp_get_wtime()-startT), lp_obj_value( cutP->mip ),  cutcr, cutprec,cutclique, cutoddholes,cutcggpur2, cutcggpur3);
#else
                fprintf( fp2, "%f %g \n",  (omp_get_wtime()-startT), lp_obj_value( mip ));
#endif // EXTRA
                //fprintf( fp2, "%f %g %d %d %d %d %d %d %d \n", (omp_get_wtime()-startT), lp_obj_value( cutP->mip ),  cutcr, cutprec,cutclique,cutcg,cutcggpu, cutcggpur2, cutcggpur3);
                mipC->tpd = lp_obj_value( mip );
                hasintegersol = MipC_saveSol(mipC);


                if(lp_obj_value( mip ) > oldobj+0.1 )
                {
                    if(VERBOSE==2)  printf("%f %g %d \n", (omp_get_wtime()-startT), lp_obj_value( mip ),  lp_rows(mip)-originalrows);
                    fprintf( fp, "%f %g %d \n", (omp_get_wtime()-startT), lp_obj_value( mip ), lp_rows(mip)-originalrows);
                    oldobj = lp_obj_value( mip );
                    lp_write_sol(mip,filelpsol);
                }

                _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
                if(_time <= 0.01)
                {
                    if(VERBOSE)  printf("but time is over %f \n", _time);
                    Res_setTotalTimeOpt(res,totalTimeOpt);
                    Res_setTotalTimeSep(res,CutP_getTotalTimeSeparation(cutP));
                    Res_setTotalTimeRemAdd(res,totalTimeRemAdd);
                    Res_setTotalTimeCGraph(res,createCgraphTime);
                    if(hasintegersol)
                    {
                        Sol_read_sol_lp(sol, mip);
                        Sol_write( sol, Par_getName(mipC->par) );
                    }
                    CutP_free(&cutP);
                    if(cc || co || cg || cgcpu || cggpur2)
                        cgraph_free( &cgraph );

                    fclose( fp );
                    fclose( fp2 );
                    return 6;
                }

                double remoaddTime = omp_get_wtime();

                MipC_setVariablesZero(mipC);
                if(VERBOSE == 2)  printf("\nstatus %d opt %d. lp_obj_value %g. time optimization %f. \n", status, LP_OPTIMAL, lp_obj_value( mip ), (omp_get_wtime()-optTime));

                double removecuttime = omp_get_wtime();
                CutP_removeCut(cutP,mip, Par_getSlack(mipC->par));
                if(VERBOSE==3) printf("time removing cuts %f. \n",  (omp_get_wtime()-removecuttime));

                double addcuttime = omp_get_wtime();
                CutP_addCut(cutP,mip,Par_getContinuous(mipC->par),Par_getSlack(mipC->par),Res_getRound(res));
                if(VERBOSE==3) printf("time adding cuts %f. \n\n",  (omp_get_wtime()-addcuttime));
                totalTimeRemAdd += omp_get_wtime()-remoaddTime;
            } /* else {

#ifdef CBC
                newCuts = lp_strengthen_with_cuts(mip, mipC->par->roundCuts);
#endif // CBC
            }*/
        }


        if(VERBOSE) Res_setRound(res,1);

        /* if( mipC->tpd > lastobj+0.00001 ) {
             lastobj = mipC->tpd;
               cont = 0;
           } else cont++; */
        /* if(newCuts<=0 && cr==1){
            ce=1;
            cr=0;
            newCuts = 1;
            getchar();
         }*/
    }
    while( newCuts > 0);  // && cont < mipC->kiterations);

    Res_setTotalTimeSep(res,CutP_getTotalTimeSeparation(cutP));
    Res_setTotalTimeOpt(res,totalTimeOpt);
    Res_setTotalTimeRemAdd(res,totalTimeRemAdd);
    Res_setTotalTimeCGraph(res,createCgraphTime);
    if(hasintegersol)
    {
        Sol_read_sol_lp(sol, mip);
        Sol_write( sol, Par_getName(mipC->par) );
    }

    if(VERBOSE==2) printf("%f %g %d \n", (omp_get_wtime()-startT), lp_obj_value( mip ), lp_rows(mip)-originalrows);
    fprintf( fp, "%f %g %d \n", (omp_get_wtime()-startT), lp_obj_value( mip ), lp_rows(mip)-originalrows);

    fclose( fp );
    fclose( fp2 );
    CutP_free(&cutP);

    if(cc || co || cg || cgcpu || cggpur2)
        cgraph_free( &cgraph );

    return status;

}

int MipC_cutting_plane( MIPCompact *mipC, Solution *sol, Results *res,  double timeLeft)
{
    double startT = omp_get_wtime();
    int hasintegersol = 0;

    int cp = Par_getCutPrec( mipC->par );
    int cr = Par_getCutRR( mipC->par );
    int cc = Par_getCutCLIQUE( mipC->par );
    int co = Par_getCutODDHOLES( mipC->par );
    int cg = Par_getCutCG( mipC->par );
    int cgcpu = Par_getCutCGCPU( mipC->par );
    int cggpu = Par_getCutCGGPU( mipC->par );
    int cggpur2= Par_getCutCGGPUR2( mipC->par );

    double slk = Par_getSlack( mipC->par );
    double mc = Par_getMaxCut(mipC->par);
    int mi = Par_getMaxInstant(mipC->par);
    int jmp = Par_getJump(mipC->par);
    char filename[256]=" ";
    sprintf( filename, "%s_cg_%d_cggpu_%d_cggpur2_%d_cp_%d_cr_%d_cc_%d_co_%d_l_%d_mc_%.1f_rc_%.2f_slk_%.2f_mi_%d_jmp_%d_timeobj.dat",   Par_getName(mipC->par), cg,cggpu,cggpur2,cp,cr,cc, co, Par_getLifting(mipC->par),  mc, Par_getMaxReducedCost(mipC->par), slk,mi,jmp);

    FILE *fp = fopen( filename, "a" );
    if ( fp == NULL )
    {
        printf( "File name time objective was not opened. : path: %s\n", filename);
        exit( 0 );
    }

    char filenamecuttype[256]=" ";
    sprintf( filenamecuttype, "%s_cg_%d_cggpu_%d_cggpur2_%d_cp_%d_cr_%d_cc_%d_co_%d_l_%d_mc_%.1f_rc_%.2f_slk_%.2f_mi_%d_jmp_%d_timeobj_cuttype.dat",   Par_getName(mipC->par), cg,cggpu,cggpur2,cp,cr,cc,co,Par_getLifting(mipC->par),  mc, Par_getMaxReducedCost(mipC->par), slk,mi,jmp);

    FILE *fp2 = fopen( filenamecuttype, "a" );
    if ( fp2 == NULL )
    {
        printf( "File name cuttype opt as continues was not opened. : path: %s\n", filenamecuttype);
        exit( 0 );
    }
    double _time = 0;

    char filelpsol[256]=" ";
    sprintf( filelpsol, "%s_cg_%d_cggpu_%d_cggpur2_%d_cp_%d_cr_%d_cc_%d_co_%d_l_%d_mc_%f_rc_%f_slk_%f_mi_%d_jmp_%d_lp.sol", Par_getName(mipC->par), cg,cggpu,cggpur2,cp,cr,cc,co, Par_getLifting(mipC->par),  mc, Par_getMaxReducedCost(mipC->par), slk,mi,jmp);
    char filelp[256]=" ";
    sprintf( filelp, "%s_cg_%d_cggpu_%d_cggpur2_%d_cp_%d_cr_%d_cc_%d_co_%d_l_%d_mc_%f_rc_%f_slk_%f_mi_%d_jmp_%d_lp_partial.lp", Par_getName(mipC->par), cg,cggpu,cggpur2,cp,cr,cc,co, Par_getLifting(mipC->par),  mc, Par_getMaxReducedCost(mipC->par), slk,mi,jmp);
    char filelpsolerr[256]=" ";
    sprintf( filelpsolerr, "%s_cg_%d_cggpu_%d_cggpur2_%d_cp_%d_cr_%d_cc_%d_co_%d_l_%d_mc_%f_rc_%f_slk_%f_mi_%d_jmp_%d_lp_err.sol", Par_getName(mipC->par), cg,cggpu,cggpur2,cp,cr,cc,co, Par_getLifting(mipC->par),  mc, Par_getMaxReducedCost(mipC->par), slk,mi,jmp);


    LinearProgram *mip = mipC->mip;
    CutPool *cutP = CutP_create(mipC->inst, mipC->mip);

    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    lp_set_nThreads(mip,6);
    lp_set_print_messages( mip, 0);
    lp_set_max_seconds(mip,_time);
    //DataGRBCallback *usrdata = DataGRB_create(mipC,mipC->par,res,cutP,_time-1);
    //lp_grb_cut_getcallback(mip, usrdata);
    //lp_set_concurrentMIP(mip,1);
    //lp_set_method(mip, 4);
    //lp_set_seed(mip,100000);
    int status = lp_optimize_as_continuous( mip );

    int originalrows = lp_rows(mipC->mip);

    if(status == LP_OPTIMAL)
    {
        mipC->tpd = lp_obj_value( mip );
        MipC_setVariablesZero(mipC);
        if(VERBOSE==2) printf("%f %g %d \n", (omp_get_wtime()-startT), lp_obj_value( mip ), lp_rows(mip)-originalrows);
        fprintf( fp, "%f %g %d \n", (omp_get_wtime()-startT), lp_obj_value( mip ), lp_rows(mip)-originalrows);
        fflush(fp);
        hasintegersol = MipC_saveSol(mipC);
        if(hasintegersol)
        {
            Sol_read_sol_lp(sol, mip);
            Sol_write( sol, Par_getName(mipC->par) );
            return status;
        }
        lp_write_sol(mip,filelpsol);
        lp_write_lp(mip,filelp);
    }
    if(VERBOSE==2) printf("info: cols %d, rows %d, nz %d obj %g\n", lp_cols(mip), lp_rows(mip), lp_nz(mip), mipC->tpd);
    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0.01)
    {
        CutP_free(&cutP);
        return status;
    }

    int newCuts = 0;

    double oldobj = lp_obj_value( mip );
    double totalTimeOpt = 0.0;
    double totalTimeRemAdd = 0.0;
    double lasttimelpsaved = omp_get_wtime();

    do
    {

        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0.01)
        {
            if(VERBOSE) printf(" time is over %f \n", _time);
            Res_setTotalTimeOpt(res,totalTimeOpt);
            Res_setTotalTimeSep(res,CutP_getTotalTimeSeparation(cutP));
            Res_setTotalTimeRemAdd(res,totalTimeRemAdd);
            if(hasintegersol)
            {
                Sol_read_sol_lp(sol, mip);
                Sol_write( sol, Par_getName(mipC->par) );
            }
            CutP_free(&cutP);
            fclose( fp );
            fclose( fp2 );
            return 6;
        }

        if(VERBOSE)
        {
            // nround = Res_getRound(res);
            Res_initRound(res);
        }

        newCuts = 0;

        mipC->hasSolution = ( status == LP_OPTIMAL);

        if ( mipC->hasSolution )
        {
            _time = ( (double) timeLeft - (omp_get_wtime()-startT) );

            newCuts = CutP_separationParallel(cutP, fp2, mipC->par, res,   cr,  cp,  cc, co, cg, cggpur2, cgcpu, mipC->maxTJM, mipC->maxTJ, startT, _time);

            if( newCuts > 0)
            {
                double optTime = omp_get_wtime();
                if(VERBOSE == 2) printf("\nlp_rows after find cuts %d ", lp_rows(mip));
                _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
                lp_set_max_seconds(mip,_time);
                //lp_set_concurrentMIP(mip,1);
                //lp_set_method(mip,4);
                //lp_set_seed(mip,100000);
                lp_set_nThreads(mip,6);
                status = lp_optimize_as_continuous( mip );
                totalTimeOpt +=(double)(omp_get_wtime()-optTime);
//                lp_write_lp(mip,"mipprint.lp"); getchar();
                if(VERBOSE == 2) printf("Time: %g \n Total Cuts: %d \n Obj: %g\n", _time, newCuts,lp_obj_value(mip));
                if(status != LP_OPTIMAL)
                {
                    if(VERBOSE) printf("is not optimal %d\n", status);
                    lp_write_lp(mip,filelpsolerr);
                    Res_setTotalTimeOpt(res,totalTimeOpt);
                    Res_setTotalTimeSep(res,CutP_getTotalTimeSeparation(cutP));
                    Res_setTotalTimeRemAdd(res,totalTimeRemAdd);
                    if(hasintegersol)
                    {
                        Sol_read_sol_lp(sol, mip);
                        Sol_write( sol, Par_getName(mipC->par) );
                    }
                    CutP_free(&cutP);
                    fclose( fp );
                    fclose( fp2 );
                    return status;
                }

#ifdef EXTRA
                fprintf( fp2, "%f %g %d %d %d %d %d %d \n", (omp_get_wtime()-startT), lp_obj_value( cutP->mip ),  cutcr, cutprec,cutclique, cutoddholes,cutcggpur2, cutcggpur3);
#else
                fprintf( fp2, "%f %g \n",  (omp_get_wtime()-startT), lp_obj_value( mip ));
                fflush(fp2);
#endif
                mipC->tpd = lp_obj_value( mip );
                hasintegersol = MipC_saveSol(mipC);


                _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
                if(lp_obj_value( mip ) > oldobj+0.1 )
                {
                    if(VERBOSE==2)  printf("%f %g %d \n", (omp_get_wtime()-startT), lp_obj_value( mip ),  lp_rows(mip)-originalrows);
                    fprintf( fp, "%f %g %d \n", (omp_get_wtime()-startT), lp_obj_value( mip ), lp_rows(mip)-originalrows);
                    fflush(fp);
                    oldobj = lp_obj_value( mip );
                    double needsave = omp_get_wtime()-lasttimelpsaved;
                    if(needsave>=3600){
                        lasttimelpsaved =  omp_get_wtime();
                        lp_write_sol(mip,filelpsol);
                        lp_write_lp(mip,filelp);
                    }
                }

                _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
                if(_time <= 0.01)
                {
                    if(VERBOSE)  printf("but time is over %f \n", _time);
                    Res_setTotalTimeOpt(res,totalTimeOpt);
                    Res_setTotalTimeSep(res,CutP_getTotalTimeSeparation(cutP));
                    Res_setTotalTimeRemAdd(res,totalTimeRemAdd);
                    if(hasintegersol)
                    {
                        Sol_read_sol_lp(sol, mip);
                        Sol_write( sol, Par_getName(mipC->par) );
                    }
                    CutP_free(&cutP);
                    fclose( fp );
                    fclose( fp2 );
                    return 6;
                }

                double remoaddTime = omp_get_wtime();

                MipC_setVariablesZero(mipC);

                if(VERBOSE == 2)  printf("\nstatus %d opt %d. lp_obj_value %g. time optimization %f. \n", status, LP_OPTIMAL, lp_obj_value( mip ), (omp_get_wtime()-optTime));

                double removecuttime = omp_get_wtime();
                CutP_removeCut(cutP,mip, Par_getSlack(mipC->par));
                if(VERBOSE==3) printf("time removing cuts %f. \n",  (omp_get_wtime()-removecuttime));

                double addcuttime = omp_get_wtime();
                CutP_addCut(cutP,mip,Par_getContinuous(mipC->par),Par_getSlack(mipC->par),Res_getRound(res));
                if(VERBOSE==3) printf("time adding cuts %f. \n\n",  (omp_get_wtime()-addcuttime));
                totalTimeRemAdd += omp_get_wtime()-remoaddTime;
            }
        }


        if(VERBOSE) Res_setRound(res,1);

    }
    while( newCuts > 0);

    Res_setTotalTimeSep(res,CutP_getTotalTimeSeparation(cutP));
    Res_setTotalTimeOpt(res,totalTimeOpt);
    Res_setTotalTimeRemAdd(res,totalTimeRemAdd);
    if(hasintegersol)
    {
        Sol_read_sol_lp(sol, mip);
        Sol_write( sol, Par_getName(mipC->par) );
    }

    if(VERBOSE==2) printf("%f %g %d \n", (omp_get_wtime()-startT), lp_obj_value( mip ), lp_rows(mip)-originalrows);
    fprintf( fp, "%f %g %d \n", (omp_get_wtime()-startT), lp_obj_value( mip ), lp_rows(mip)-originalrows);
    fflush(fp);
    lp_write_sol(mip,filelpsol);
    lp_write_lp(mip,filelp);

    fclose( fp );
    fclose( fp2 );
    CutP_free(&cutP);

    return status;

}

float MipC_getMinRoundUp(MIPCompact *mipC)
{
    return mipC->minRoundUp;
}

void MipC_allocateSol(MIPCompact *mipC)
{

    ModeSet *ms = Sol_getModeSet( mipC->sol );
    for(int i = 0 ; i < mipC->nIntegral ; i++)
    {
        int idxJob = mipC->jmtIntegral[i].j;
        const Job * job = Inst_job(mipC->inst,mipC->jmtIntegral[i].j);
        int idxMode = Modes_job(ms, idxJob);
        if(idxMode == -1)
        {
            const Mode* mode = Job_mode(job, mipC->jmtIntegral[i].m);
            Modes_modify( ms, i, mipC->jmtIntegral[i].m);
            RRU_allocate(Sol_getRRUsage(mipC->sol),mode,mipC->jmtIntegral[i].t);
            Sol_setStartJob(mipC->sol, mipC->jmtIntegral[i].j, mipC->jmtIntegral[i].t);
            Sol_setMinTimeSucc(mipC->sol, job, mipC->jmtIntegral[i].t+Mode_duration(mode));
        }

    }

    Sol_calcCost(mipC->sol);
#ifdef DEBUG
    printf("\nbuilt sol tpd: %ld tms: %ld\n", Sol_getTPD(mipC->sol), Sol_getTMS(mipC->sol) ) ;
#endif


}

LinearProgram* MipC_mip( MIPCompact *mipC )
{
    return mipC->mip;
}

int MipC_saveSol( MIPCompact *mipC)
{
    if(!MipC_isInteger(mipC)) return 0;

    LinearProgram *mip  = MipC_mip(mipC);
    assert(mip != NULL);
    Tokenizer *tok = Tok_create();
    int nTimes = 0;
    Sol_clearBuildSequence( mipC->sol );
    const double *x = lp_x(mip);
    for ( int i=0 ; (i<lp_cols(mip)) ; ++i )
    {
        if ( x[i] <= 1.0-EPS )  // not an active binary var
            continue;

        char name[STR_SIZE];
        lp_col_name(mip,i,name);
        if ( (strlen(name)>7) && (name[0]=='x') && (name[1]=='(') )
        {
            char *s = strstr(name,"(");
            assert( s );
            ++s;
            char str[STR_SIZE];
            strcpy( str, s );
            s = strstr( str, ")" );
            assert( s );
            *s = '\0';
            Tok_parse( tok, str, ',' );
            const int j = atoi( Tok_token(tok,0) );
            const int m = atoi( Tok_token(tok,1) );
            const int t = atoi( Tok_token(tok,2) );
            const Job *job = Inst_job( mipC->inst, j );
#ifdef DEBUG
            assert( m>=0 && m<Job_nModes(job) );
            assert( t>=Job_est(job) );
#endif // DEBUG
            ModeSet *ms = Sol_getModeSet( mipC->sol );
            Modes_modify( ms, j, m);
            const Mode* mode = Job_mode(job, m);
            RRU_allocate(Sol_getRRUsage(mipC->sol),mode,t);
            Sol_setStartJob(mipC->sol, j, t);
            Sol_setMinTimeSucc(mipC->sol, job, t+Mode_duration(mode));
            ++nTimes;

            assert( nTimes<=Inst_nJobs(mipC->inst) );
        } // x variables
    } // all variables

    assert( nTimes==Inst_nJobs(mipC->inst) );

    Sol_calcCost(mipC->sol);
#ifdef DEBUG
    printf("\nbuilt sol tpd: %ld tms: %ld\n", Sol_getTPD(mipC->sol), Sol_getTMS(mipC->sol) ) ;
    fprintf( stderr, "\nSolution is integer!" );
#endif
    return 1;

}

int MipC_isInteger( MIPCompact *mipC)
{

    LinearProgram *mip  = MipC_mip(mipC);
    assert(mip != NULL);
    int nTimes = 0;
    const double *x = lp_x(mip);
    for ( int i=0 ; (i<lp_cols(mip)) ; ++i )
    {
        if ( x[i] <= 1.0-EPS )  // not an active binary var
            continue;
        char name[STR_SIZE];
        lp_col_name(mip,i,name);
        if ( (strlen(name)>7) && (name[0]=='x') && (name[1]=='(') )
        {
            ++nTimes;
            assert( nTimes<=Inst_nJobs(mipC->inst) );
        } // x variables
    } // all variables

    if (nTimes!=Inst_nJobs(mipC->inst))
    {
        //#ifdef DEBUG
        //                  fprintf( stderr, "\nSolution is not integer!" );
        //#endif
        return 0;
    }
#ifdef DEBUG
    fprintf( stderr, "\nSolution is integer!" );
#endif
    return 1;

}

Solution* MipC_getSol( MIPCompact *mipC )
{
    return mipC->sol;
}

double MipC_getBestObj( MIPCompact *mipC )
{
    return mipC->tpd;
}

Parameters *MipC_getPar(MIPCompact *mipC)
{
    return mipC->par;
}

const Instance* MipC_inst( MIPCompact *mipC )
{
    return mipC->inst;
}

void MipC_solve( MIPCompact *mipp, double timeLeft)
{

    //    double startT = omp_get_wtime();
    //    double _time;

    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    lp_set_print_messages( mip, 1 );
    lp_set_parallel( mip, 0 );

    if (mipCParams)
        lp_parse_options( mip, VStr_size(mipCParams), (const char **) VStr_ptr(mipCParams) );
    /* _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
     if(_time <= 0) {
         printf( "\nMip Solve: Time is over %f Time Left %f \n", _time, timeLeft);
         exit(0);
     }*/

    /* entering initial solution to the model */
    if (mipp->hasIniSol)
    {
        Solution *sol = mipp->bestSol;
        VecStr *cnames = VStr_create( STR_SIZE );
        for ( int j=0 ; (j<Inst_nJobs(inst)) ; ++j )
        {
            int t = Sol_getStartTime( sol, j );
            int m = Sol_getMode( sol, j );
            char cname[STR_SIZE];
            sprintf( cname, "x(%d,%d,%d)", j, m, t );
            VStr_pushBack( cnames, cname );
        }
        double values[Inst_nJobs(inst)];
        FILL( values, 0, Inst_nJobs(inst), 1.0 );

        lp_load_mip_start( mip, Inst_nJobs(inst), (const char **) VStr_ptr(cnames), values );
#ifdef DEBUG
        lp_mipstart_debug( mip );
#endif

        VStr_free( &cnames );
    }

    /*  _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
      if(_time <= 0) {
          printf( "\nMip Solve has Initial Solution: Time is over %f Time Left %f \n", _time, timeLeft);
          exit(0);
      }*/
    //lp_set_concurrentMIP(mip,1);
    //lp_set_method(mip,4);

    //lp_set_seed(mip,100000);
    int status = lp_optimize( mip );

    //   printf("\nO: %d F: %d status: %d \n", LP_OPTIMAL, LP_FEASIBLE, status);
    mipp->hasSolution = ( ( status == LP_OPTIMAL) || ( status == LP_FEASIBLE ) );

    if ( mipp->hasSolution )
    {
        mipp->tpd = (int) lp_obj_value( mip );
#ifdef DEBUG
        //        printf("MIP tpd: %g\n", mipp->tpd );
#endif
        if ( mipp->idxProj==MIPC_ALL_PROJECTS )
        {
            Tokenizer *tok = Tok_create();
            IntPair *times;
            ALLOCATE_VECTOR_INI( times, IntPair, Inst_nJobs(inst) );
            int nTimes = 0;
            Sol_clearBuildSequence( mipp->sol );
            const double *x = lp_x(mip);
            for ( int i=0 ; (i<lp_cols(mip)) ; ++i )
            {
                if ( x[i] <= 1.0-EPS )  // not an active binary var
                    continue;

                char name[STR_SIZE];
                lp_col_name(mip,i,name);
                if ( (strlen(name)>7) && (name[0]=='x') && (name[1]=='(') )
                {
                    char *s = strstr(name,"(");
                    assert( s );
                    ++s;
                    char str[STR_SIZE];
                    strcpy( str, s );
                    s = strstr( str, ")" );
                    assert( s );
                    *s = '\0';
                    Tok_parse( tok, str, ',' );
                    const int j = atoi( Tok_token(tok,0) );
                    const int m = atoi( Tok_token(tok,1) );
                    const int t = atoi( Tok_token(tok,2) );
#ifdef DEBUG
                    const Job *job = Inst_job( inst, j );
                    assert( m>=0 && m<Job_nModes(job) );
                    assert( t>=Job_est(job) );
#endif // DEBUG
                    ModeSet *ms = Sol_getModeSet( mipp->sol );
                    //                    int idxMode = Modes_job(ms,j);
                    Modes_modify( ms, j, m);


                    times[nTimes].a = j;
                    times[nTimes].b = t;
                    ++nTimes;

                    assert( nTimes<=Inst_nJobs(inst) );
                } // x variables
            } // all variables

            if (nTimes!=Inst_nJobs(inst))
            {
#ifdef DEBUG
                fprintf( stderr, "solution not complete!" );
#endif
                abort();
            }

            qsort( times, nTimes, sizeof(IntPair), cmp_int_pair_b );

            int *seq = Sol_sequence( mipp->sol );

#ifdef DEBUG
            printf("job times:\n");
#endif
            for ( int i=0 ; (i<Inst_nJobs(inst)) ; ++i )
            {
#ifdef DEBUG
                printf("%d %d\n", times[i].a, times[i].b );
#endif
                seq[i] = times[i].a;
                //Sol_setSequence( mipp->sol, i, times[i].a );
            }

            Sol_rebuild( mipp->sol );
#ifdef DEBUG
            printf("built sol tpd: %ld tms: %ld\n", Sol_getTPD(mipp->sol), Sol_getTMS(mipp->sol) ) ;
#endif

            Tok_free( &tok );
            free( times );
        } /* complete solution */
    } /* has solution */


}

char MipC_hasSolution( MIPCompact *MipP )
{
    return MipP->hasSolution;
}

void MipC_setMaxSeconds( MIPCompact *MipP, int maxSeconds )
{
    LinearProgram *mip = MipP->mip;

    lp_set_max_seconds( mip, maxSeconds );
}

void MipC_setInitialSolution( MIPCompact *mipp, const Solution *_bestSol )
{
    Sol_cpy( mipp->bestSol, _bestSol );
    mipp->hasIniSol = True;
}

void MipC_parseParameters( int argc, const char **argv )
{
    for ( int i=1 ; (i<argc) ; ++i )
    {
        if (!strlen(argv[i]))
            continue;
        if (argv[i][0]!='-')
            continue;
        if (strstr("prec",argv[i]+1))
        {
            if (i+1<argc)
                precConsType = atoi( argv[i+1] );
            continue;
        }
    }

    /* storing options for lp */
    if (!mipCParams)
        mipCParams = VStr_create( STR_SIZE );
    VStr_clear( mipCParams );
    for ( int i=0 ; (i<argc) ; ++i )
        VStr_pushBack( mipCParams, argv[i] );
}

int cmp_int_dbl_pair_a( const void *v1, const void *v2 )
{
    const IntPair *ip1 = (const IntPair *) v1;
    const IntPair *ip2 = (const IntPair *) v2;

    //if ( ip1->b*ip1->a != ip2->b*ip2->a )
    //  return ip1->b*ip1->a-ip2->b*ip2->a;

    return ip1->a-ip2->a;
}

int cmp_int_dbl_pair_b( const void *v1, const void *v2 )
{
    const IntDblPair *ip1 = (const IntDblPair *) v1;
    const IntDblPair *ip2 = (const IntDblPair *) v2;

    if ( (double) 1000.0*ip2->b < (double)  1000.0*ip1->b ) return -1;
    if ( (double) 1000.0*ip2->b == (double) 1000.0*ip1->b ) return 0;
    if ( (double) 1000.0*ip2->b > (double) 1000.0*ip1->b ) return 1;

    return 0;
}

int cmp_int_triple_t( const void *v1, const void *v2 )
{
    const IntTriple *ip1 = (const IntTriple *) v1;
    const IntTriple *ip2 = (const IntTriple *) v2;

    if ( ip1->t!=ip2->t)
        return ip1->t-ip2->t;

    return ip1->j-ip2->j;
}

int cmp_int_triple_time( const void *v1, const void *v2 )
{
    const IntTriple *ip1 = (const IntTriple *) v1;
    const IntTriple *ip2 = (const IntTriple *) v2;

    return (int)(1000.0*ip1->t - 1000.0*ip2->t);

}

int cmp_int_triple_value( const void *v1, const void *v2 )
{
    const IntTriple *ip1 = (const IntTriple *) v1;
    const IntTriple *ip2 = (const IntTriple *) v2;

    return (int)(1000.0*ip2->value - 1000.0*ip1->value);
}

int cmp_int_triple_j( const void *v1, const void *v2 )
{
    const IntTriple *ip1 = (const IntTriple *) v1;
    const IntTriple *ip2 = (const IntTriple *) v2;

    return (int)(1000.0*ip1->j - 1000.0*ip2->j);
}

void MipC_help( )
{
    printf("\t-prec {0,1,2} : selects constraint type for precedence constraints\n");
}

void MipC_writeLP( const MIPCompact *mipp, const char *fileName )
{
    LinearProgram *mip = mipp->mip;

    lp_write_lp( mip, fileName );
}

void MipC_printConfig( )
{
    printf( "\tprecConsType %d\n", precConsType );
}

void MipC_free( MIPCompact **mipp )
{

    const Instance *inst = (*mipp)->inst;

    lp_free( &((*mipp)->mip) );


    for ( int i=0 ; i<Inst_nResN(inst) ; ++i )
    {
        VInt_free( &(*mipp)->idxResNR[i] );
        VDbl_free( &(*mipp)->coefResNR[i] );
    }
    free( (*mipp)->idxResNR );
    free( (*mipp)->coefResNR );

    free( (*mipp)->yIdx );
    free( (*mipp)->dIdx );
    free( (*mipp)->uIdx[0] );
    free( (*mipp)->uIdx );

    Sol_free( &(*mipp)->bestSol );

    DInt_free( &(*mipp)->dnames );


    for ( int i=0 ; (i<Inst_nResR( inst )*((*mipp)->maxT+1)) ; ++i )
    {
        VInt_free( &(*mipp)->idxResR[0][i] );
        VDbl_free( &(*mipp)->coefResR[0][i] );
    }

    for ( int i=0 ; (i<Inst_nJobs(inst)) ; ++i )
        VInt_free( &(*mipp)->jIdx[i] );

    for ( int i=0 ; (i<Inst_nJobs(inst)) ; ++i )
        VInt_free( &(*mipp)->dJ[i] );

    for ( int j=0 ; j<Inst_nJobs(inst) ; ++j )
        for ( int t=0 ; (t<(*mipp)->maxT+1) ; ++t )
            VInt_free( &((*mipp)->jtIdx[j][t]) );

    for ( int j=0 ; j<Inst_nJobs(inst) ; ++j )
    {
        for ( int m=0 ; (m<(*mipp)->maxModes) ; ++m )
        {
            VInt_free( &((*mipp)->jmIdx[j][m]) );
            for ( int r=0 ; r<Inst_nResR( inst ); ++r )
            {
                free( (*mipp)->coefR[j][m][r] );
            }
            free( (*mipp)->coefR[j][m] );
        }
        free((*mipp)->coefR[j] );
    }
    free( (*mipp)->coefR );

    for(int p = 0 ; p <Inst_nProjects(inst); p++)
    {
        for(int t = 0 ; t <=(*mipp)->maxT; t++)
            free((*mipp)->mipstrengthening[p][t]);
        free((*mipp)->mipstrengthening[p]);
    }
    free((*mipp)->mipstrengthening);

    free( (*mipp)->jtIdx[0] );
    free( (*mipp)->jtIdx );

    free( (*mipp)->jmIdx[0] );
    free( (*mipp)->jmIdx );

    free( (*mipp)->jIdx );
    free( (*mipp)->dJ );
    free( (*mipp)->maxTJM[0] );
    free( (*mipp)->maxTJM );
    free( (*mipp)->maxTJ );


    free( (*mipp)->idxResR[0] );
    free( (*mipp)->coefResR[0] );

    free( (*mipp)->idxResR );
    free( (*mipp)->coefResR );

    free( (*mipp)->jobIntegral );
    free( (*mipp)->jobFixed );
    free( (*mipp)->jmtIntegral);
    free( (*mipp)->jmtFractional);
    free( (*mipp)->jmtFixed);

    MSM_free( &(*mipp)->msm );
    Sol_free( &(*mipp)->sol );
    Par_free( &(*mipp)->par );


    lp_close_env();
    free( ((*mipp)->mip));

    free( *mipp );
    *mipp = NULL;
}



/*if job j on mode m starts at time t*/
//static void createX( MIPCompact *mipp, double timeLeft);


void createX( MIPCompact *mipp, double timeLeft)
{
    assert(mipp!=NULL);

    double startT = omp_get_wtime();
    double _time = 0;
    const Instance *inst = mipp->inst;
    LinearProgram *mip = mipp->mip;
    const int firstJob = mipp->idxFirstJob;
    const int nJobs = mipp->nJobs;
    const int tpdSum = mipp->tpdSum;

    VecStr *names = VStr_create( STR_SIZE );

    //  VecInt *idxRD = VInt_create();
//    VecDbl *coefRD = VDbl_create();

//    int contidx = 0;
    for ( int j=firstJob ; (j<firstJob+nJobs) ; ++j )
    {
        const Job *job = Inst_job( inst, j );
        const int nModes = Job_nModes( job );

        mipp->maxTJ[j] = 0; //max tempo by job

        for ( int m=0 ; (m<nModes) ; ++m )
        {
            const Mode *mode = Job_mode( job, m );
            if (!Mode_isFeasible(inst,mode))
                continue;

            const int est = Job_est( job );
            const int duration = Mode_duration( mode );
            int cp1 = Inst_computeCompPathsByJob(inst,j,m,duration);
            int delay = cp1+tpdSum;

            if(nModes==1 && Job_nSucc(job)>0 && Job_nPred(job)<=0)
            {
                delay = est;
            }
            const int maxTJM = mipp->maxTJM[j][m] = delay;
            mipp->maxTJ[j] = MAX( mipp->maxTJ[j], maxTJM );

            //printf("duration %d est %d  maxTJM[%d][%d] %d \n",duration, est,j,m, mipp->maxTJM[j][m]); getchar();
            for ( int t=est ; (t<=maxTJM) ; ++t )
            {
                char vname[STR_SIZE];

                int over = t - cp1 < 0 ? 0 : t - cp1 ;
                if(over > Inst_getSumTPD(inst))
                {
                    printf(" estoura o tpd do projeto x(%d,%d,%d)", j, m, t ); getchar();
                    continue;
                }

                sprintf( vname, "x(%d,%d,%d)", j, m, t );
                //printf("x(%d,%d,%d)\n", j, m, t );
                const int currXIdx = lp_cols(mip) + VStr_size( names );

                VInt_pushBack( mipp->jIdx[j], currXIdx );
                VInt_pushBack( mipp->dJ[j], duration );

                // adding resource constraints
                for ( int ir=0 ; (ir<Mode_nResR(mode)) ; ++ir )
                {
                    int idxR = Mode_idxResR( mode, ir );
                    int use = Mode_useResR( mode, ir );

                    assert( use>0 );
                    //for ( int tj=t ; ((tj<t+Mode_duration(mode))&&(tj<= mipp->maxT-duration)) ; ++tj ) {
                    for ( int tj=t ; ((tj<t+Mode_duration(mode))) ; ++tj )
                    {
                        VInt_pushBack( mipp->idxResR[idxR][tj], currXIdx );
                        VDbl_pushBack( mipp->coefResR[idxR][tj], use );
                    }
                }

                // non renewable constraints
                for ( int inr=0 ; (inr<Mode_nResN(mode)) ; ++inr )
                {
                    int idxNR = Mode_idxResN( mode, inr );
                    int use = Mode_useResN( mode, inr );

                    VInt_pushBack( mipp->idxResNR[idxNR], currXIdx );
                    VDbl_pushBack( mipp->coefResNR[idxNR], use );
                }

                VInt_pushBack( mipp->jtIdx[j][t], currXIdx );
                VInt_pushBack( mipp->jmIdx[j][m], currXIdx );

                /*Janniele test Release date to reduce the value coef u_p
                  if(Job_nSucc(job)==0){
                      VInt_pushBack( idxRD, currXIdx);
                      VDbl_pushBack( coefRD, -t);
                      contidx++;
                  }//end
                */
                VStr_pushBack( names, vname );
            }
        }
        _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
        if(_time <= 0)
        {
            printf( "\ncreateX Time is over %f Time Left %f \n", _time, timeLeft);
            exit(0);
        }

    }


    const int nCols = VStr_size(names);
    // printf("nObjt nCols %d\n", nCols);
    double *obj;//obj[nCols]
    ALLOCATE_VECTOR_INI(obj,double,nCols);

    FILL( obj, 0, nCols, 0.0 );

    storeVarNames( nCols, (const char**)VStr_ptr(names), mipp );

    lp_add_bin_cols( mip, nCols, obj, VStr_ptr(names) );

    /*Janniele test Release date to reduce the value coef u_p
      char names2[STR_SIZE];
      sprintf( names2,  "releaseDateAndCP" );

      int iRD = lp_col_index(mip,names2);

      VInt_pushBack( idxRD, iRD);
      VDbl_pushBack( coefRD, 1.0);
      contidx++;

      char name[STR_SIZE];
      sprintf( name, "cXRDCP:");
      lp_add_row( mip, contidx, VInt_getPtr(idxRD), VDbl_getPtr(coefRD), name, 'G', 0.0 );
      //end*/

    free(obj);
    VStr_free( &names );
    // VStr_free( &names2 );
}

/*double MipC_getCurrentObj( MIPCompact *mipC )
{
    return  lp_current_obj( mipC->mip);
}*/
/*double MipC_getBestPossibleObj( MIPCompact *mipC)
{
    return lp_best_possible_obj(mipC->mip);
}*/


/*
int parseName( const char *name, char *prefix, int *idx )
{
#define MAX_COMMAS 64
    int nCommas=0;
    int commaPos[MAX_COMMAS];
    // open and close  ()
    int pO=-1, pC=-1;
    int l = strlen(name);
    int i;
    for ( i=0 ; (i<l) ; ++i )
        switch (name[i]) {
            case '(' :
                pO = i;
                break;
            case ')' :
                pC = i;
                break;
            case ',' :
                commaPos[nCommas++] = i;
                break;
        }

    /// not in the propper format
    if ( (pO==-1) || (pC==-1) )
        return 0;

    assert( pO<pC );

    strcpy( prefix, name );

    //printf("%s\n", name );

    prefix[pO] = '\0';

    char str[STR_SIZE];

    for ( i=0 ; (i<nCommas+1) ; ++i ) {
        int pStart = pO;
        if (i>=1)
            pStart = commaPos[i-1];

        int pEnd   = pC;
        if (i<nCommas)
            pEnd = commaPos[i];


        //printf("X%d %d %d\n", pStart, pEnd, i);
        assert( pStart<pEnd );

        strcpy( str, name+pStart+1 );
        str[pEnd-pStart-1] = '\0';
        idx[i] = atoi(str);
    }

    return nCommas+1;
#undef MAX_COMMAS
}
*/

/*
int MipC_solve_as_continuous( MIPCompact *mipC, double timeLeft )
{

    double startT = omp_get_wtime();

    double _time = 0;

    const Instance *inst = mipC->inst;
    LinearProgram *mip = mipC->mip;
    lp_set_print_messages( mip, 1);

    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time < 1) {
        printf( "MipSolve 1 time is over %f \n", _time);
        exit(0);
    }

    lp_set_max_seconds(mip,_time);

    int status = lp_optimize_as_continuous( mip );
    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
    if(_time <= 0) {
        printf( "MipSolve 2 time is over %f \n", _time);
        exit(0);
    }

#ifdef DEBUG
    printf("status: %d (LP_OPTIMAL: %d, LP_FEASIBLE: %d, LP_INFEASIBLE: %d)  \n", status, LP_OPTIMAL, LP_FEASIBLE, LP_INFEASIBLE);
#endif // DEBUG

    mipC->hasSolution = ( status == LP_OPTIMAL);
    if ( mipC->hasSolution ) {

        CutPool *cutP = mipC->cutP;
        LinearProgram *mip = mipC->mip;
        Results *res = mipC->res;
        int startRows = lp_rows( mip );

        int newCuts = 0;
        if(mipC->cutRR) {

            int origCols[lp_cols( mip )];
            for ( int i=0 ; (i<lp_cols( mip )) ; ++i )
                origCols[i] = i;

            lp_add_cuts_cover( mip, origCols, mip, mipC->inst, 0, mipC->continuous, mipC->lifting, cutP, res);

            newCuts = lp_rows( mipC->mip ) - startRows;
            //            nCutsTotal[LPC_RR] += newCuts;
#ifdef DEBUG
            printf("\n%d new cuts Separation where added.\n", newCuts );
#endif
            //status = lp_optimize_as_continuous( mip );
            _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
            if(_time <= 0) {
                printf( "Cut Separation time is over %f \n", _time);
                exit(0);
            }

#ifdef DEBUG
            printf("\n%d new cuts RR where added.\n", newCuts );
#endif

        }

        if(mipC->cutPrec) {

            int origCols[lp_cols( mip )];
            for ( int i=0 ; (i<lp_cols( mip )) ; ++i )
                origCols[i] = i;

            lp_add_cuts_precedence( mip, origCols, mip, mipC->inst ,0, mipC->continuous , mipC->lifting, mipC->maxTJM, mipC->maxTJ, mipC->maxcuts, cutP, res );

            newCuts = lp_rows( mip ) - startRows;
            //          nCutsTotal[LPC_PREC] += newCuts;
            //            _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
            if(_time < 1) {
                printf( "Cut X time is over %f \n", _time);
                exit(0);
            }
#ifdef DEBUG
            printf("\n%d new cuts X where added.\n", newCuts );
#endif
        }

#ifdef CBC
        if (mipC->cutDefaultCBC) {

            //   newCuts += lp_strengthen_with_cuts(mipC->mip, mipC->roundCuts, nCutsTotal);
            //    _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
            if(_time < 1) {
                printf( "Cut CBC time is over %f \n", _time);
                exit(0);
            }
#ifdef DEBUG
            printf("\n%d new cuts where added.\n", newCuts );
#endif
        }
#endif // CBC


        if(mipC->cutCLIQUE) {


            int origCols[lp_cols( mip )];
            for ( int i=0 ; (i<lp_cols( mip )) ; ++i )
                origCols[i] = i;

            lp_add_cuts_conflicts_clique( mip, origCols, mip, mipC->inst , 0, mipC->continuous, mipC->maxTJM, mipC->maxcuts, cutP, res);

            newCuts = lp_rows( mip ) - startRows;
            //           nCutsTotal[LPC_CLIQUE] += newCuts;
#ifdef DEBUG
            printf("\n%d new cuts Separation where added.\n", newCuts );
#endif
            //status = lp_optimize_as_continuous( mip );
            _time = ( (double) timeLeft - (omp_get_wtime()-startT) );
            if(_time <= 0) {
                printf( "cuts RR time is over %f \n", _time);
                exit(0);
            }

#ifdef DEBUG
            printf("\n%d new cuts RR where added.\n", newCuts );
#endif

        }


        mipC->tpd = lp_obj_value( mip );
#ifdef DEBUG
        printf("\nMIP tpd: %g\n\n", mipC->tpd );
#endif
        if ( mipC->idxProj==MIPC_ALL_PROJECTS ) {
            Tokenizer *tok = Tok_create();
            int nIntegral = 0, nFractional = 0;
            //            Sol_clearBuildSequence( mipC->sol );
            const double *x = lp_x(mip);

            for ( int i=0 ; (i<lp_cols(mip)) ; ++i ) {
                char name[STR_SIZE];
                lp_col_name(mip,i,name);

                if ( (strlen(name)>7) && (name[0]=='x') && (name[1]=='(') ) {
                    char *s = strstr(name,"(");
                    assert( s );
                    ++s;
                    char str[STR_SIZE];
                    strcpy( str, s );
                    s = strstr( str, ")" );
                    assert( s );
                    *s = '\0';
                    Tok_parse( tok, str, ',' );
                    const int j = atoi( Tok_token(tok,0) );
                    const int m = atoi( Tok_token(tok,1) );
                    //const Mode *mode = Job_mode(job, m);
                    const int t = atoi( Tok_token(tok,2) );
#ifdef DEBUG
                    const Job *job = Inst_job( inst, j );
                    assert( m>=0 && m<Job_nModes(job) );
                    assert( t>=Job_est(job) );
#endif // DEBUG

                    if( x[i] >= 0.999999999999) { //1-EPS ) {
                        //                      printf("EPS %.15f \n", 1-EPS);

                        mipC->jmtIntegral[j].j = j;
                        mipC->jmtIntegral[j].m = m;
                        mipC->jmtIntegral[j].t = t;
                        mipC->jmtIntegral[j].value = x[i];
                        mipC->jobIntegral[j] = 1;

                        #ifdef DEBUG
                                                printf("I %d %d %d X %.10f ",mipC->jmtIntegral[j].j,mipC->jmtIntegral[j].m,mipC->jmtIntegral[j].t ,  mipC->jmtIntegral[j].value);
                                                printf("Predecessors: " );
                                                const Job * job = Inst_job(inst,j);
                                                for(int p = 0 ; p < Job_nPred(job) ; p++)
                                                    printf("%d ", Job_pred(job,p));
                                                printf("\n" );
                        #endif
                        ++nIntegral;

                    } else  if( x[i] >= 0.001) {

                        mipC->jmtFractional[nFractional].j = j;
                        mipC->jmtFractional[nFractional].m = m;
                        mipC->jmtFractional[nFractional].t = t;
                        mipC->jmtFractional[nFractional].value = x[i];

                        #ifdef DEBUG
                                                printf("F %d %d %d X %.10f ", mipC->jmtFractional[nFractional].j, mipC->jmtFractional[nFractional].m, mipC->jmtFractional[nFractional].t, mipC->jmtFractional[nFractional].value );
                                                printf("Predecessors: " );
                                                const Job * job = Inst_job(inst,j);
                                                for(int p = 0 ; p < Job_nPred(job) ; p++)
                                                    printf("%d ", Job_pred(job,p));
                                                printf("\n" );
                        #endif

                        ++nFractional;

                    }

                    mipC->nIntegral = nIntegral;
                    mipC->nFractional = nFractional;
                    assert( nIntegral<=Inst_nJobs(inst) );
                } // x variables

                //   if ( (strlen(name)<6) && (name[0]=='y') && (name[1]=='(') ) {
                  //     char *s = strstr(name,"(");
                 //      assert( s );
                 //      ++s;
                  //     char str[STR_SIZE];
                 //      strcpy( str, s );
                 //      const int j = atoi(str );
                  //     int valor = ROUND(x[i]);
                  //     if(((x[i]-valor) <= 0.001) ) {
                          // mipC->yIntegral[j] = 1;
                   //        printf("yI y(%d) %.2f \n", j, x[i] );

                   //    } else {
                         //  mipC->yIntegral[j] = 0;
                    //       printf("y(%d) %.6f \n", j, x[i] );
                    //   }
                  // }


            } // all variables

            for ( int i=0 ; (i<Inst_nJobs(inst)) ; ++i ) {

                if(mipC->jobIntegral[i] == 0) continue;

                const Job * job = Inst_job(mipC->inst,mipC->jmtIntegral[i].j);
                for(int pi = 0 ; pi < Inst_nJobs(mipC->inst) ; pi++) {
                    if(Job_hasIndPred(job,pi) && mipC->jobIntegral[pi] == 0  ) {

                        mipC->jmtFractional[mipC->nFractional].j= mipC->jmtIntegral[i].j;
                        mipC->jmtFractional[mipC->nFractional].m = mipC->jmtIntegral[i].m;
                        mipC->jmtFractional[mipC->nFractional].t = mipC->jmtIntegral[i].t;
                        mipC->jmtFractional[mipC->nFractional].value = mipC->jmtIntegral[i].value;
                        ++mipC->nFractional;

                        #ifdef DEBUG
                                                printf("(%d,%d,%d) X %.10f ", mipC->jmtIntegral[i].j, mipC->jmtIntegral[i].m, mipC->jmtIntegral[i].t, mipC->jmtIntegral[i].value );
                                                printf("Predecessors: " );
                                                printf("%d \n", pi);
                                                // getchar();
                        #endif


                    }

                }
            }

            qsort( mipC->jmtFractional, mipC->nFractional, sizeof(IntTriple), cmp_int_triple_value);


#ifdef DEBUG
            //if (nIntegral!=Inst_nJobs(inst))
            //fprintf( stderr, "\nsolution is not complete!\n" );
#endif

            Tok_free( &tok );
        } // complete solution
    } // has solution

    return status;


}
*/
