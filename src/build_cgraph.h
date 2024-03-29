#ifndef BUILDCGRAPH_H_INCLUDED
#define BUILDCGRAPH_H_INCLUDED

#include "cgraph.h"

CGraph *build_cgraph_osi( const void *_solver );
CGraph *build_cgraph_lp( const void *_mip );
CGraph* build_cgraph_conflicts(int **conf, int *nconf, int nVertices, double runtime);
#endif
