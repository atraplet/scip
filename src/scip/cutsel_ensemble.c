/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*  Copyright 2002-2022 Zuse Institute Berlin                                */
/*                                                                           */
/*  Licensed under the Apache License, Version 2.0 (the "License");          */
/*  you may not use this file except in compliance with the License.         */
/*  You may obtain a copy of the License at                                  */
/*                                                                           */
/*      http://www.apache.org/licenses/LICENSE-2.0                           */
/*                                                                           */
/*  Unless required by applicable law or agreed to in writing, software      */
/*  distributed under the License is distributed on an "AS IS" BASIS,        */
/*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. */
/*  See the License for the specific language governing permissions and      */
/*  limitations under the License.                                           */
/*                                                                           */
/*  You should have received a copy of the Apache-2.0 license                */
/*  along with SCIP; see the file LICENSE. If not visit scipopt.org.         */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   cutsel_ensemble.c
 * @ingroup DEFPLUGINS_CUTSEL
 * @brief  ensemble cut selector
 * @author Mark Turner
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include <assert.h>

#include "scip/scip_cutsel.h"
#include "scip/scip_cut.h"
#include "scip/scip_lp.h"
#include "scip/scip_randnumgen.h"
#include "scip/cutsel_ensemble.h"


#define CUTSEL_NAME              "ensemble"
#define CUTSEL_DESC              "weighted sum of many terms with optional filtering and penalties"
#define CUTSEL_PRIORITY           9000

#define RANDSEED                  0x5EED

#define DEFAULT_MINSCORE                0.0  /**< minimum score s.t. a cut can be selected */
#define DEFAULT_EFFICACYWEIGHT          1.0  /**< weight of normed-efficacy in score calculation */
#define DEFAULT_DIRCUTOFFDISTWEIGHT     0.1  /**< weight of normed-directed cutoff distance in score calculation */
#define DEFAULT_OBJPARALWEIGHT          0.2  /**< weight of objective parallelism in score calculation */
#define DEFAULT_OBJORTHOGWEIGHT         0.1  /**< weight of objective orthogonality in score calculation */
#define DEFAULT_INTSUPPORTWEIGHT        0.1  /**< weight of integral support in cut score calculation */
#define DEFAULT_EXPIMPROVWEIGHT         0.1  /**< weight of normed-expected improvement in cut score calculation */
#define DEFAULT_PSCOSTWEIGHT            0.2  /**< weight of normalised pseudo-costs in cut score calculation */
#define DEFAULT_NLOCKSWEIGHT            0.1  /**< weight of normalised number of locks in cut score calculation */
#define DEFAULT_MAXSPARSITYBONUS        0.6  /**< score given to a cut with complete sparsity */
#define DEFAULT_SPARSITYENDBONUS        0.4  /**< the density at which a cut no longer receives additional score */
#define DEFAULT_GOODNUMERICBONUS        0.1  /**< bonus provided for good numerics */
#define DEFAULT_MAXCOEFRATIOBONUS       10000 /**< maximum coefficient ratio of cut for which numeric bonus is given */
#define DEFAULT_PENALISELOCKS           FALSE /**< whether having less locks should be rewarded instead of more */
#define DEFAULT_FILTERPARALCUTS         TRUE /**< should cuts be filtered so no two parallel cuts are added */
#define DEFAULT_MAXPARAL                0.9  /**< threshold for when two cuts are considered parallel to each other */
#define DEFAULT_PENALISEPARALCUTS       FALSE /**< should two parallel cuts be penalised instead of outright filtered */
#define DEFAULT_PARALPENALTY            0.5  /**< penalty for weaker of two parallel cuts if penalising parallel cuts */
#define DEFAULT_FILTERDENSECUTS         FALSE /**< should cuts over a given density threshold be filtered */
#define DEFAULT_MAXCUTDENSITY           0.4  /**< max allowed cut density if filtering dense cuts */
#define DEFAULT_MAXNONZEROROOTROUND     5    /**< max non-zeros per round applied cuts (root). multiple num LP cols */
#define DEFAULT_MAXNONZEROTREEROUND     3    /**< max non-zeros per round applied cuts (tree). multiple num LP cols */

/*
 * Data structures
 */

/** cut selector data */
struct SCIP_CutselData
{
   SCIP_RANDNUMGEN*      randnumgen;         /**< random generator for tie-breaking */
   SCIP_Real             minscore;           /**< minimum score for a cut to be added to the LP */
   SCIP_Real             objparalweight;     /**< weight of objective parallelism in cut score calculation */
   SCIP_Real             objorthogweight;    /**< weight of objective orthogonality in cut score calculation */
   SCIP_Real             efficacyweight;     /**< weight of normed-efficacy in cut score calculation */
   SCIP_Real             dircutoffdistweight;/**< weight of normed-directed cutoff distance in cut score calculation */
   SCIP_Real             expimprovweight;    /**< weight of normed-expected improvement in cut score calculation */
   SCIP_Real             intsupportweight;   /**< weight of integral support in cut score calculation */
   SCIP_Real             pscostweight;       /**< weight of normalised pseudo-costs in cut score calculation */
   SCIP_Real             locksweight;        /**< weight of normed-number of active locks in cut score calculation */
   SCIP_Real             maxsparsitybonus;   /**< weight of maximum sparsity reward in cut score calculation */
   SCIP_Real             goodnumericsbonus;  /**< weight of good numeric bonus in cut score calculation */
   SCIP_Real             endsparsitybonus;   /**< max sparsity value for which a bonus is applied */
   SCIP_Real             maxparal;           /**< threshold for when two cuts are considered parallel to each other */
   SCIP_Real             paralpenalty;       /**< penalty for weaker of two parallel cuts if penalising parallel cuts */
   SCIP_Real             maxcutdensity;      /**< max allowed cut density if filtering dense cuts */
   SCIP_Real             maxnonzerorootround;/**< max non-zeros per round applied cuts (root). multiple num LP cols */
   SCIP_Real             maxnonzerotreeround;/**< max non-zeros per round applied cuts (tree). multiple num LP cols */
   SCIP_Bool             filterparalcuts;   /**< should cuts be filtered so no two parallel cuts are added */
   SCIP_Bool             penaliseparalcuts;  /**< should two parallel cuts be penalised instead of outright filtered */
   SCIP_Bool             filterdensecuts;    /**< should cuts over a given density threshold be filtered */
   SCIP_Bool             penaliselocks;      /**< whether the number of locks should be penalised instead of rewarded */
   int                   maxcoefratiobonus;  /**< maximum coefficient ratio for which numeric bonus is applied */
};


/*
 * Local methods
 */

/** returns the maximum score of cuts; if scores is not NULL, then stores the individual score of each cut in scores */
static
SCIP_RETCODE scoring(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_ROW**            cuts,               /**< array with cuts to score */
   SCIP_CUTSELDATA*      cutseldata,         /**< cut selector data */
   SCIP_Real*            scores,             /**< array to store the score of cuts or NULL */
   SCIP_Bool             root,               /**< whether we are at the root node or not */
   int                   ncuts               /**< number of cuts in cuts array */
)
{
   SCIP_Real* effs;
   SCIP_Real* dcds;
   SCIP_Real* exps;
   SCIP_Real* cutdensities;
   SCIP_Real* cutlocks;
   SCIP_Real* pscosts;
   SCIP_SOL* sol;
   SCIP_Real maxdcd = 0.0;
   SCIP_Real maxeff = 0.0;
   SCIP_Real maxexp = 0.0;
   SCIP_Real maxpscost = 0.0;
   int i;
   int maxlocks = 0;
   int ncols;
   
   /* Get the solution that we use for directed cutoff distance calculations. Get the number of columns too */
   sol = SCIPgetBestSol(scip);
   ncols = SCIPgetNLPCols(scip);
   
   /* Initialise all array information that we're going to use for scoring */
   SCIP_CALL(SCIPallocBufferArray(scip, &effs, ncuts));
   SCIP_CALL(SCIPallocBufferArray(scip, &dcds, ncuts));
   SCIP_CALL(SCIPallocBufferArray(scip, &exps, ncuts));
   SCIP_CALL(SCIPallocBufferArray(scip, &cutdensities, ncuts));
   SCIP_CALL(SCIPallocBufferArray(scip, &cutlocks, ncuts));
   SCIP_CALL(SCIPallocBufferArray(scip, &pscosts, ncuts));
   
   
   /* Populate the number of cut locks and the cut densities */
   for ( i = 0; i < ncuts; i++ )
   {
      SCIP_COL** cols;
      SCIP_Real* cutvals;
      SCIP_Real cutnorm;
      int ncutcols;
      int j;
      
      cols = SCIProwGetCols(cuts[i]);
      cutvals = SCIProwGetVals(cuts[i]);
      cutnorm = SCIProwGetNorm(cuts[i]);
      ncutcols = SCIProwGetNNonz(cuts[i]);
      cutdensities[i] = ncutcols / ncols;
      cutlocks[i] = 0;
      pscosts[i] = 0;
      
      for ( j = 0; j < ncutcols; j++ )
      {
         SCIP_VAR* colvar;
         colvar = SCIPcolGetVar(cols[j]);
         if( ! SCIPisInfinity(scip, SCIProwGetRhs(cuts[i])) && cutvals[j] > 0.0 )
            cutlocks[i] += SCIPvarGetNLocksUp(colvar);
         if( ! SCIPisInfinity(scip, SCIProwGetLhs(cuts[i])) && cutvals[j] < 0.0 )
            cutlocks[i] += SCIPvarGetNLocksUp(colvar);
         if( ! SCIPisInfinity(scip, SCIProwGetRhs(cuts[i])) && cutvals[j] < 0.0 )
            cutlocks[i] += SCIPvarGetNLocksDown(colvar);
         if( ! SCIPisInfinity(scip, SCIProwGetLhs(cuts[i])) && cutvals[j] > 0.0 )
            cutlocks[i] += SCIPvarGetNLocksDown(colvar);
         
         pscosts[i] += ABS(cutvals[j] / cutnorm) * SCIPgetVarPseudocostScore(scip, colvar, SCIPvarGetLPSol(colvar));
      }
      
      cutlocks[i] = cutlocks[i] / ncutcols;
      
      if( cutlocks[i] > maxlocks )
         maxlocks = cutlocks[i];
      
      if( pscosts[i] > maxpscost )
         maxpscost = pscosts[i];
   }
   
   for ( i = 0; i < ncuts; i++ )
   {
      if( cutseldata->penaliselocks )
         cutlocks[i] = 1 - (cutlocks[i] / maxlocks);
      else
         cutlocks[i] = cutlocks[i] / maxlocks;
      pscosts[i] = pscosts[i] / maxpscost;
   }
   
   
   /* Get the arrays / maximums of directed cutoff distance, efficacy, and expected improvement values. */
   if ( sol != NULL && root )
   {
      for ( i = 0; i < ncuts; i++ )
      {
         dcds[i] = SCIPgetCutLPSolCutoffDistance(scip, sol, cuts[i]);
         maxdcd = MAX(maxdcd, dcds[i]);
      }
   }
   
   for ( i = 0; i < ncuts; i++ )
   {
      effs[i] = SCIPgetCutEfficacy(scip, NULL, cuts[i]);
      exps[i] = effs[i] * SCIPgetRowObjParallelism(scip, cuts[i]);
      maxeff = MAX(maxeff, effs[i]);
      maxexp = MAX(maxexp, exps[i]);
   }
   
   /* Now score the cuts */
   for ( i = 0; i < ncuts; ++i )
   {
      SCIP_Real score;
      SCIP_Real scaleddcd;
      SCIP_Real scaledeff;
      SCIP_Real scaledexp;
      SCIP_Real objparallelism;
      SCIP_Real objorthogonality;
      SCIP_Real intsupport;
      SCIP_Real density;
      SCIP_Real dynamism;
      SCIP_Real mincutval = SCIPinfinity(scip);
      SCIP_Real maxcutval = 0.0;
      SCIP_Real pscost;
      SCIP_Real cutlock;
      
      /* Get the integer support */
      intsupport = SCIPgetRowNumIntCols(scip, cuts[i]) / (SCIP_Real) SCIProwGetNNonz(cuts[i]);
      intsupport *= cutseldata->intsupportweight;
      
      /* Get the objective parallelism and orthogonality */
      objparallelism = SCIPgetRowObjParallelism(scip, cuts[i]);
      objorthogonality = cutseldata->objorthogweight * (1 - objparallelism);
      objparallelism *= cutseldata->objparalweight;
      
      /* Get the density score */
      density = (cutseldata->maxsparsitybonus / cutseldata->endsparsitybonus) * -1 * cutdensities[i];
      density += cutseldata->maxsparsitybonus;
      density = MAX(density, 0.0);
      
      /* Get the normalised pseudo-cost and number of locks score */
      pscost = cutseldata->pscostweight * pscosts[i];
      cutlock = cutseldata->locksweight * cutlocks[i];
      
      /* Get the dynamism (good numerics) score */
      maxcutval = SCIPgetRowMaxCoef(scip, cuts[i]);
      mincutval = SCIPgetRowMinCoef(scip, cuts[i]);
      mincutval = mincutval > 0.0 ? mincutval : 1.0;
      dynamism = cutseldata->maxcoefratiobonus >= maxcutval / mincutval ? cutseldata->goodnumericsbonus : 0.0;
      
      /* Get the dcd / eff / exp score */
      if ( sol != NULL && root )
      {
         if ( SCIPisSumLE(scip, dcds[i], 0.0))
            scaleddcd = 0.0;
         else
            scaleddcd = cutseldata->dircutoffdistweight * SQR(LOG1P(dcds[i]) / LOG1P(maxdcd));
      }
      
      if ( SCIPisSumLE(scip, exps[i], 0.0))
         scaledexp = 0.0;
      else
         scaledexp = cutseldata->expimprovweight * SQR(LOG1P(exps[i]) / LOG1P(maxexp));
      
      if ( SCIPisSumLE(scip, effs[i], 0.0))
      {
         scaledeff = 0.0;
      }
      else
      {
         if ( sol != NULL && root )
            scaledeff = cutseldata->efficacyweight * SQR(LOG1P(effs[i]) / LOG1P(maxeff));
         else
            scaledeff =
               (cutseldata->efficacyweight + cutseldata->dircutoffdistweight) * SQR(LOG1P(effs[i]) / LOG1P(maxeff));
      }
      
      /* Combine all scores and introduce some minor randomness */
      score = scaledeff + scaleddcd + scaledexp + objparallelism + objorthogonality + intsupport + density + dynamism + pscost + cutlock;
      
      score += SCIPrandomGetReal(cutseldata->randnumgen, 0.0, 1e-6);
      
      scores[i] = score;
   }
   
   SCIPfreeBufferArray(scip, &effs);
   SCIPfreeBufferArray(scip, &dcds);
   SCIPfreeBufferArray(scip, &exps);
   SCIPfreeBufferArray(scip, &cutdensities);
   SCIPfreeBufferArray(scip, &cutlocks);
   SCIPfreeBufferArray(scip, &pscosts);
   
   return SCIP_OKAY;
   
}


/** move the cut with the highest score to the first position in the array; there must be at least one cut */
static
void selectBestCut(
   SCIP_ROW**            cuts,               /**< array with cuts to perform selection algorithm */
   SCIP_Real*            scores,             /**< array with scores of cuts to perform selection algorithm */
   int                   ncuts               /**< number of cuts in given array */
)
{
   int i;
   int bestpos;
   SCIP_Real bestscore;
   
   assert(ncuts > 0);
   assert(cuts != NULL);
   assert(scores != NULL);
   
   bestscore = scores[0];
   bestpos = 0;
   
   for( i = 1; i < ncuts; ++i )
   {
      if( scores[i] > bestscore )
      {
         bestpos = i;
         bestscore = scores[i];
      }
   }
   
   SCIPswapPointers((void**) &cuts[bestpos], (void**) &cuts[0]);
   SCIPswapReals(&scores[bestpos], &scores[0]);
}

/** filters the given array of cuts to enforce a maximum parallelism constraint
 *  w.r.t the given cut; moves filtered cuts to the end of the array and returns number of selected cuts */
static
int filterWithParallelism(
   SCIP_ROW*             cut,                /**< cut to filter orthogonality with */
   SCIP_ROW**            cuts,               /**< array with cuts to perform selection algorithm */
   SCIP_Real*            scores,             /**< array with scores of cuts to perform selection algorithm */
   int                   ncuts,              /**< number of cuts in given array */
   SCIP_Real             maxparallel         /**< maximal parallelism for all cuts that are not good */
)
{
   int i;
   
   assert( cut != NULL );
   assert( ncuts == 0 || cuts != NULL );
   assert( ncuts == 0 || scores != NULL );
   
   for( i = ncuts - 1; i >= 0; --i )
   {
      SCIP_Real thisparallel;
      
      thisparallel = SCIProwGetParallelism(cut, cuts[i], 'e');
      
      if( thisparallel > maxparallel )
      {
         --ncuts;
         SCIPswapPointers((void**) &cuts[i], (void**) &cuts[ncuts]);
         SCIPswapReals(&scores[i], &scores[ncuts]);
      }
   }
   
   return ncuts;
}

/** penalises any cut too parallel to cut by reducing the parallel cut's score. */
static
void penaliseWithParallelism(
   SCIP_ROW*             cut,                /**< cut to filter orthogonality with */
   SCIP_ROW**            cuts,               /**< array with cuts to perform selection algorithm */
   SCIP_Real*            scores,             /**< array with scores of cuts to perform selection algorithm */
   int                   ncuts,              /**< number of cuts in given array */
   SCIP_Real             maxparallel,        /**< maximal parallelism for all cuts that are not good */
   SCIP_Real             paralpenalty        /**< penalty for weaker of two parallel cuts if penalising parallel cuts */
)
{
   int i;
   
   assert( cut != NULL );
   assert( ncuts == 0 || cuts != NULL );
   assert( ncuts == 0 || scores != NULL );
   
   for( i = ncuts - 1; i >= 0; --i )
   {
      SCIP_Real thisparallel;
      
      thisparallel = SCIProwGetParallelism(cut, cuts[i], 'e');
      
      if( thisparallel > maxparallel )
      {
         scores[i] -= paralpenalty;
      }
   }
}

/** filters the given array of cuts to enforce a maximum density constraint,
 *  Moves filtered cuts to the end of the array and returns number of selected cuts */
static
int filterWithDensity(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_ROW**            cuts,               /**< array with cuts to perform selection algorithm */
   SCIP_Real             maxdensity,         /**< maximum density s.t. a cut is not filtered */
   int                   ncuts               /**< number of cuts in given array */
)
{
   int i;
   int ncols;
   
   assert( ncuts == 0 || cuts != NULL );
   
   ncols = SCIPgetNLPCols(scip);
   
   for( i = ncuts - 1; i >= 0; --i )
   {
      int nvals;
   
      nvals = SCIProwGetNNonz(cuts[i]);
      
      if( maxdensity < nvals / ncols )
      {
         --ncuts;
         SCIPswapPointers((void**) &cuts[i], (void**) &cuts[ncuts]);
      }
   }
   
   return ncuts;
}


/*
 * Callback methods of cut selector
 */


/** copy method for cut selector plugin (called when SCIP copies plugins) */
static
SCIP_DECL_CUTSELCOPY(cutselCopyEnsemble)
{  /*lint --e{715}*/
   assert(scip != NULL);
   assert(cutsel != NULL);
   assert(strcmp(SCIPcutselGetName(cutsel), CUTSEL_NAME) == 0);
   
   /* call inclusion method of cut selector */
   SCIP_CALL( SCIPincludeCutselEnsemble(scip) );
   
   return SCIP_OKAY;
}

/** destructor of cut selector to free user data (called when SCIP is exiting) */
/**! [SnippetCutselFreeEnsemble] */
static
SCIP_DECL_CUTSELFREE(cutselFreeEnsemble)
{  /*lint --e{715}*/
   SCIP_CUTSELDATA* cutseldata;
   
   cutseldata = SCIPcutselGetData(cutsel);
   
   SCIPfreeBlockMemory(scip, &cutseldata);
   
   SCIPcutselSetData(cutsel, NULL);
   
   return SCIP_OKAY;
}
/**! [SnippetCutselFreeEnsemble] */

/** initialization method of cut selector (called after problem was transformed) */
static
SCIP_DECL_CUTSELINIT(cutselInitEnsemble)
{  /*lint --e{715}*/
   SCIP_CUTSELDATA* cutseldata;
   
   cutseldata = SCIPcutselGetData(cutsel);
   assert(cutseldata != NULL);
   
   SCIP_CALL( SCIPcreateRandom(scip, &(cutseldata)->randnumgen, RANDSEED, TRUE) );
   
   return SCIP_OKAY;
}

/** deinitialization method of cut selector (called before transformed problem is freed) */
static
SCIP_DECL_CUTSELEXIT(cutselExitEnsemble)
{  /*lint --e{715}*/
   SCIP_CUTSELDATA* cutseldata;
   
   cutseldata = SCIPcutselGetData(cutsel);
   assert(cutseldata != NULL);
   assert(cutseldata->randnumgen != NULL);
   
   SCIPfreeRandom(scip, &cutseldata->randnumgen);
   
   return SCIP_OKAY;
}

/** cut selection method of cut selector */
static
SCIP_DECL_CUTSELSELECT(cutselSelectEnsemble)
{  /*lint --e{715}*/
   SCIP_CUTSELDATA* cutseldata;
   
   assert(cutsel != NULL);
   assert(result != NULL);
   
   *result = SCIP_SUCCESS;
   
   cutseldata = SCIPcutselGetData(cutsel);
   assert(cutseldata != NULL);
   
   SCIP_CALL( SCIPselectCutsEnsemble(scip, cuts, forcedcuts, cutseldata, root, ncuts, nforcedcuts,
      maxnselectedcuts, nselectedcuts) );
   
   return SCIP_OKAY;
}


/*
 * cut selector specific interface methods
 */

/** creates the ensemble cut selector and includes it in SCIP */
SCIP_RETCODE SCIPincludeCutselEnsemble(
   SCIP*                 scip                /**< SCIP data structure */
)
{
   SCIP_CUTSELDATA* cutseldata;
   SCIP_CUTSEL* cutsel;
   
   /* create ensemble cut selector data */
   SCIP_CALL( SCIPallocBlockMemory(scip, &cutseldata) );
   BMSclearMemory(cutseldata);
   
   SCIP_CALL( SCIPincludeCutselBasic(scip, &cutsel, CUTSEL_NAME, CUTSEL_DESC, CUTSEL_PRIORITY, cutselSelectEnsemble,
                                     cutseldata) );
   
   assert(cutsel != NULL);
   
   /* set non fundamental callbacks via setter functions */
   SCIP_CALL( SCIPsetCutselCopy(scip, cutsel, cutselCopyEnsemble) );
   
   SCIP_CALL( SCIPsetCutselFree(scip, cutsel, cutselFreeEnsemble) );
   SCIP_CALL( SCIPsetCutselInit(scip, cutsel, cutselInitEnsemble) );
   SCIP_CALL( SCIPsetCutselExit(scip, cutsel, cutselExitEnsemble) );
   
   /* add ensemble cut selector parameters */
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/efficacyweight",
      "weight of normed-efficacy in cut score calculation",
      &cutseldata->efficacyweight, FALSE, DEFAULT_EFFICACYWEIGHT, 0.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/dircutoffdistweight",
      "weight of normed-directed cutoff distance in cut score calculation",
      &cutseldata->dircutoffdistweight, FALSE, DEFAULT_DIRCUTOFFDISTWEIGHT, 0.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/objparalweight",
      "weight of objective parallelism in cut score calculation",
      &cutseldata->objparalweight, FALSE, DEFAULT_OBJPARALWEIGHT, 0.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/intsupportweight",
      "weight of integral support in cut score calculation",
      &cutseldata->intsupportweight, FALSE, DEFAULT_INTSUPPORTWEIGHT, 0.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/expimprovweight",
      "weight of normed-expected obj improvement in cut score calculation",
      &cutseldata->expimprovweight, FALSE, DEFAULT_EXPIMPROVWEIGHT, 0.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/objorthogweight",
      "weight of objective orthogonality in cut score calculation",
      &cutseldata->objorthogweight, TRUE, DEFAULT_OBJORTHOGWEIGHT, 0.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/minscore",
      "minimum score s.t. a cut can be added",
      &cutseldata->minscore, FALSE, DEFAULT_MINSCORE, -SCIP_INVALID/10.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/pscostweight",
      "weight of normed-pseudo-costs in cut score calculation",
      &cutseldata->pscostweight, FALSE, DEFAULT_PSCOSTWEIGHT, 0.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/locksweight",
      "weight of normed-num-locks in cut score calculation",
      &cutseldata->locksweight, FALSE, DEFAULT_NLOCKSWEIGHT, 0.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/maxsparsitybonus",
      "weight of maximum sparsity reward in cut score calculation",
      &cutseldata->maxsparsitybonus, FALSE, DEFAULT_MAXSPARSITYBONUS, 0.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/goodnumericsbonus",
      "weight of good numerics bonus (ratio of coefficients) in cut score calculation",
      &cutseldata->goodnumericsbonus, FALSE, DEFAULT_GOODNUMERICBONUS, 0.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/endsparsitybonus",
      "max sparsity value for which a bonus is applied in cut score calculation",
      &cutseldata->endsparsitybonus, FALSE, DEFAULT_SPARSITYENDBONUS, 0.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/maxparal",
      "threshold for when two cuts are considered parallel to each other",
      &cutseldata->maxparal, FALSE, DEFAULT_MAXPARAL, 0.0, 1.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/paralpenalty",
      "penalty for weaker of two parallel cuts if penalising parallel cuts",
      &cutseldata->paralpenalty, TRUE, DEFAULT_PARALPENALTY, 0.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/maxcutdensity",
      "max allowed cut density if filtering dense cuts",
      &cutseldata->maxcutdensity, TRUE, DEFAULT_MAXCUTDENSITY, 0.0, 1.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/maxnonzerorootround",
      "max non-zeros per round applied cuts (root). multiple num LP cols.",
      &cutseldata->maxnonzerorootround, FALSE, DEFAULT_MAXNONZEROROOTROUND, 0.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddRealParam(scip,
                               "cutselection/" CUTSEL_NAME "/maxnonzerotreeround",
      "max non-zeros per round applied cuts (tree). multiple num LP cols.",
      &cutseldata->maxnonzerotreeround, FALSE, DEFAULT_MAXNONZEROTREEROUND, 0.0, SCIP_INVALID/10.0, NULL, NULL) );
   
   SCIP_CALL( SCIPaddBoolParam(scip,
                               "cutselection/" CUTSEL_NAME "/filterparalcuts",
      "should cuts be filtered so no two parallel cuts are added",
      &cutseldata->filterparalcuts, FALSE, DEFAULT_FILTERPARALCUTS, NULL, NULL) );
   
   SCIP_CALL( SCIPaddBoolParam(scip,
                               "cutselection/" CUTSEL_NAME "/penaliseparalcuts",
      "should two parallel cuts be penalised instead of outright filtered",
      &cutseldata->penaliseparalcuts, TRUE, DEFAULT_PENALISEPARALCUTS, NULL, NULL) );
   
   SCIP_CALL( SCIPaddBoolParam(scip,
                               "cutselection/" CUTSEL_NAME "/filterdensecuts",
      "should cuts over a given density threshold be filtered",
      &cutseldata->filterdensecuts, TRUE, DEFAULT_FILTERDENSECUTS, NULL, NULL) );
   
   SCIP_CALL( SCIPaddBoolParam(scip,
                               "cutselection/" CUTSEL_NAME "/penaliselocks",
      "should the number of locks be penalised instead of rewarded",
      &cutseldata->penaliselocks, TRUE, DEFAULT_PENALISELOCKS, NULL, NULL) );
   
   SCIP_CALL( SCIPaddIntParam(scip,
                               "cutselection/" CUTSEL_NAME "/maxcoefratiobonus",
      "max coefficient ratio for which numeric bonus is applied.",
      &cutseldata->maxcoefratiobonus, TRUE, DEFAULT_MAXCOEFRATIOBONUS, 1, 1000000, NULL, NULL) );
   
   return SCIP_OKAY;

}

/** perform a cut selection algorithm for the given array of cuts
 *
 *  This is the selection method of the ensemble cut selector. It uses a weighted sum of normalised efficacy,
 *  normalised directed cutoff distance, normalised expected improvements, objective parallelism,
 *  objective orthogonality, integer support, sparsity, and dynamism.
 *  As well as the weighted sum scoring there is optional parallelism based filtering, parallelism based penalties,
 *  and density filtering.
 *  There are also additional budget constraints on the amount of cuts that should be added.
 *  The input cuts array gets resorted s.t the selected cuts come first and the remaining ones are the end.
 */
SCIP_RETCODE SCIPselectCutsEnsemble(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_ROW**            cuts,               /**< array with cuts to perform selection algorithm */
   SCIP_ROW**            forcedcuts,         /**< array with forced cuts */
   SCIP_CUTSELDATA*      cutseldata,         /**< cut selector data */
   SCIP_Bool             root,               /**< whether we are at the root node or not */
   int                   ncuts,              /**< number of cuts in cuts array */
   int                   nforcedcuts,        /**< number of forced cuts */
   int                   maxselectedcuts,    /**< maximal number of cuts from cuts array to select */
   int*                  nselectedcuts       /**< pointer to return number of selected cuts from cuts array */
)
{
   SCIP_Real* scores;
   SCIP_Real* origscoresptr;
   SCIP_Real nonzerobudget;
   SCIP_Real budgettaken = 0.0;
   int ncols;
   int i;
   
   assert(cuts != NULL && ncuts > 0);
   assert(forcedcuts != NULL || nforcedcuts == 0);
   assert(nselectedcuts != NULL);
   
   *nselectedcuts = 0;
   ncols = SCIPgetNLPCols(scip);
   
   /* filter dense cuts */
   if( cutseldata->filterdensecuts )
   {
      ncuts = filterWithDensity(scip, cuts, cutseldata->maxcutdensity, ncuts);
   }
   
   /* Initialise the score array */
   SCIP_CALL( SCIPallocBufferArray(scip, &scores, ncuts) );
   origscoresptr = scores;
   
   /* compute scores of cuts */
   scoring(scip, cuts, cutseldata, scores, root, ncuts);
   
   /* perform cut selection algorithm for the cuts */
   
   /* forced cuts are going to be selected so use them to filter cuts */
   for( i = 0; i < nforcedcuts && ncuts > 0; ++i )
   {
      if( cutseldata->filterparalcuts )
         ncuts = filterWithParallelism(forcedcuts[i], cuts, scores, ncuts, cutseldata->maxparal);
      else if( cutseldata->penaliseparalcuts )
         penaliseWithParallelism(forcedcuts[i], cuts, scores, ncuts, cutseldata->maxparal, cutseldata->paralpenalty);
   }
   
   /* Get the budget depending on if we are the root or not */
   nonzerobudget = root ? cutseldata->maxnonzerorootround : cutseldata->maxnonzerotreeround;
   
   /* now greedily select the remaining cuts */
   while( ncuts > 0 )
   {
      SCIP_ROW* selectedcut;
      
      selectBestCut(cuts, scores, ncuts);
      selectedcut = cuts[0];
      
      /* if the best cut of the remaining cuts is considered bad, we discard it and all remaining cuts */
      if( scores[0] < cutseldata->minscore )
         goto TERMINATE;
      
      ++(*nselectedcuts);
      
      /* if the maximal number of cuts was selected, we can stop here */
      if( *nselectedcuts == maxselectedcuts )
         goto TERMINATE;
      
      /* if the maximum (non-zero) budget threshold was hit then we can stop */
      budgettaken += SCIProwGetNNonz(cuts[0]) / ncols;
      if( budgettaken > nonzerobudget )
         goto TERMINATE;
      
      /* move the pointers to the next position and filter the remaining cuts to enforce the maximum parallelism constraint */
      ++cuts;
      ++scores;
      --ncuts;
   
      if( cutseldata->filterparalcuts )
         ncuts = filterWithParallelism(selectedcut, cuts, scores, ncuts, cutseldata->maxparal);
      else if( cutseldata->penaliseparalcuts )
         penaliseWithParallelism(selectedcut, cuts, scores, ncuts, cutseldata->maxparal, cutseldata->paralpenalty);
      
   }
   
   TERMINATE:
   SCIPfreeBufferArray(scip, &origscoresptr);
   
   return SCIP_OKAY;
}
