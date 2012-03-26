/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2011 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*#define SCIP_DEBUG*/

/**@file    presol_domcol.c
 * @ingroup PRESOLVERS
 * @brief   dominated column presolver
 * @author  Dieter Weninger
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "matrix/matrix.h" /* TODO: maybe it is better to put the matrix inside this plugin ? */

#include "presol_domcol.h"

#define PRESOL_NAME            "domcol"
#define PRESOL_DESC            "dominated column presolver"
#define PRESOL_PRIORITY         -5000000     /**< priority of the presolver (>= 0: before, < 0: after constraint handlers) */
#define PRESOL_MAXROUNDS              -1     /**< maximal number of presolving rounds the presolver participates in (-1: no limit) */
#define PRESOL_DELAY               FALSE     /**< should presolver be delayed, if other presolvers found reductions? */

/*
 * Data structures
 */

/** type of fixing direction */
enum Fixingdirection
   {
      FIXATLB = -1,
      NOFIX   =  0,
      FIXATUB =  1
   };
typedef enum Fixingdirection FIXINGDIRECTION;


/*
 * Local methods
 */

/** get minimum/maximum residual activity without the specified variable */
static
void getActivityResiduals(
   SCIP*                 scip,               /**< SCIP main data structure */
   CONSTRAINTMATRIX*     matrix,             /**< matrix containing the constraints */
   SCIP_VAR*             var,                /**< variable for activity residual calculation */
   SCIP_Real             val,                /**< coefficient of this variable in this row */
   int                   row,                /**< row index */
   SCIP_Real*            minresactivity,     /**< minimum residual activity of this row */
   SCIP_Real*            maxresactivity      /**< maximum residual activity of this row */
   )
{
   SCIP_Real lb;
   SCIP_Real ub;

   assert(scip != NULL);
   assert(matrix != NULL);
   assert(var != NULL);
   assert(row < matrix->nrows);
   assert(minresactivity != NULL);
   assert(maxresactivity != NULL);

   lb = SCIPvarGetLbLocal(var);
   ub = SCIPvarGetUbLocal(var);

   if( val > 0.0 )
   {
      if( SCIPisInfinity(scip, lb) )
      {
         assert(matrix->minactivityposinf[row] >= 1);
         if( matrix->minactivityposinf[row] >= 2 )
            *minresactivity = SCIPinfinity(scip);
         else if( matrix->minactivityneginf[row] >= 1 )
            *minresactivity = -SCIPinfinity(scip);
         else
            *minresactivity = matrix->minactivity[row];
      }
      else if( SCIPisInfinity(scip, -lb) )
      {
         assert(matrix->minactivityneginf[row] >= 1);
         if( matrix->minactivityposinf[row] >= 1 )
            *minresactivity = SCIPinfinity(scip);
         else if( matrix->minactivityneginf[row] >= 2 )
            *minresactivity = -SCIPinfinity(scip);
         else
            *minresactivity = matrix->minactivity[row];
      }
      else
      {
         if( matrix->minactivityposinf[row] >= 1 )
            *minresactivity = SCIPinfinity(scip);
         else if( matrix->minactivityneginf[row] >= 1 )
            *minresactivity = -SCIPinfinity(scip);
         else
            *minresactivity = matrix->minactivity[row] - val * lb;
      }

      if( SCIPisInfinity(scip, -ub) )
      {
         assert(matrix->maxactivityneginf[row] >= 1);
         if( matrix->maxactivityneginf[row] >= 2 )
            *maxresactivity = -SCIPinfinity(scip);
         else if( matrix->maxactivityposinf[row] >= 1 )
            *maxresactivity = SCIPinfinity(scip);
         else
            *maxresactivity = matrix->maxactivity[row];
      }
      else if( SCIPisInfinity(scip, ub) )
      {
         assert(matrix->maxactivityposinf[row] >= 1);
         if( matrix->maxactivityneginf[row] >= 1 )
            *maxresactivity = -SCIPinfinity(scip);
         else if( matrix->maxactivityposinf[row] >= 2 )
            *maxresactivity = SCIPinfinity(scip);
         else
            *maxresactivity = matrix->maxactivity[row];
      }
      else
      {
         if( matrix->maxactivityneginf[row] >= 1 )
            *maxresactivity = -SCIPinfinity(scip);
         else if( matrix->maxactivityposinf[row] >= 1 )
            *maxresactivity = SCIPinfinity(scip);
         else
            *maxresactivity = matrix->maxactivity[row] - val * ub;
      }
   }
   else
   {
      if( SCIPisInfinity(scip, -ub) )
      {
         assert(matrix->minactivityposinf[row] >= 1);
         if( matrix->minactivityposinf[row] >= 2 )
            *minresactivity = SCIPinfinity(scip);
         else if( matrix->minactivityneginf[row] >= 1 )
            *minresactivity = -SCIPinfinity(scip);
         else
            *minresactivity = matrix->minactivity[row];
      }
      else if( SCIPisInfinity(scip, ub) )
      {
         assert(matrix->minactivityneginf[row] >= 1);
         if( matrix->minactivityposinf[row] >= 1 )
            *minresactivity = SCIPinfinity(scip);
         else if( matrix->minactivityneginf[row] >= 2 )
            *minresactivity = -SCIPinfinity(scip);
         else
            *minresactivity = matrix->minactivity[row];
      }
      else
      {
         if( matrix->minactivityposinf[row] >= 1 )
            *minresactivity = SCIPinfinity(scip);
         else if( matrix->minactivityneginf[row] >= 1 )
            *minresactivity = -SCIPinfinity(scip);
         else
            *minresactivity = matrix->minactivity[row] - val * ub;
      }

      if( SCIPisInfinity(scip, lb) )
      {
         assert(matrix->maxactivityneginf[row] >= 1);
         if( matrix->maxactivityneginf[row] >= 2 )
            *maxresactivity = -SCIPinfinity(scip);
         else if( matrix->maxactivityposinf[row] >= 1 )
            *maxresactivity = SCIPinfinity(scip);
         else
            *maxresactivity = matrix->maxactivity[row];
      }
      else if( SCIPisInfinity(scip, -lb) )
      {
         assert(matrix->maxactivityposinf[row] >= 1);
         if( matrix->maxactivityneginf[row] >= 1 )
            *maxresactivity = -SCIPinfinity(scip);
         else if( matrix->maxactivityposinf[row] >= 2 )
            *maxresactivity = SCIPinfinity(scip);
         else
            *maxresactivity = matrix->maxactivity[row];
      }
      else
      {
         if( matrix->maxactivityneginf[row] >= 1 )
            *maxresactivity = -SCIPinfinity(scip);
         else if( matrix->maxactivityposinf[row] >= 1 )
            *maxresactivity = SCIPinfinity(scip);
         else
            *maxresactivity = matrix->maxactivity[row] - val * lb;
      }
   }
}

/** calculate bounds of the dominating variable by rowbound analysis.
 *  we use a special kind of predictive rowbound analysis by first setting the dominated
 *  variable to their lower bound.
 */
static
SCIP_RETCODE calcVarBounds(
   SCIP*                 scip,               /**< SCIP main data structure */
   CONSTRAINTMATRIX*     matrix,             /**< matrix containing the constraints */
   int                   row,                /**< current row index */
   int                   coldominating,      /**< column index of dominating variable */
   SCIP_Real             valdominating,      /**< row coefficient of dominating variable */
   int                   coldominated,       /**< column index of dominated variable */
   SCIP_Real             valdominated,       /**< row coefficient of dominated variable */
   SCIP_Bool*            lbCalculated,       /**< was a lower bound calculated? */
   SCIP_Real*            calculatedLb,       /**< actual calculated lower bound */
   SCIP_Bool*            ubCalculated,       /**< was a upper bound calculated? */
   SCIP_Real*            calculatedUb,       /**< actual calculated upper bound */
   SCIP_Bool*            lb2Calculated,      /**< was a lower worst case bound calculated? */
   SCIP_Real*            calculatedLb2,      /**< actual calculated worst case lower bound */
   SCIP_Bool*            ub2Calculated,      /**< was a worst case upper bound calculated? */
   SCIP_Real*            calculatedUb2       /**< actual calculated worst case upper bound */
   )
{
   SCIP_VAR* vardominating;
   SCIP_Real lbdominating;
   SCIP_Real ubdominating;

   SCIP_VAR* vardominated;
   SCIP_Real lbdominated;
   SCIP_Real ubdominated;

   SCIP_Real minresactivity;
   SCIP_Real maxresactivity;
   SCIP_Real lhs;
   SCIP_Real rhs;

   assert(scip != NULL);
   assert(matrix != NULL);
   assert(0 <= row && row < matrix->nrows);
   assert(0 <= coldominating && coldominating < matrix->ncols);
   assert(0 <= coldominated && coldominated < matrix->ncols);

   assert(lbCalculated != NULL);
   assert(calculatedLb != NULL);
   assert(ubCalculated != NULL);
   assert(calculatedUb != NULL);
   assert(lb2Calculated != NULL);
   assert(calculatedLb2 != NULL);
   assert(ub2Calculated != NULL);
   assert(calculatedUb2 != NULL);

   assert(!SCIPisZero(scip, valdominating));
   assert(matrix->vars[coldominating] != NULL);

   vardominating = matrix->vars[coldominating];
   vardominated = matrix->vars[coldominated];

   *lbCalculated = FALSE;
   *ubCalculated = FALSE;
   *lb2Calculated = FALSE;
   *ub2Calculated = FALSE;

   /* no rowbound analysis for multiaggregated variables */
   if( SCIPvarGetStatus(vardominating) == SCIP_VARSTATUS_MULTAGGR ||
      SCIPvarGetStatus(vardominated) == SCIP_VARSTATUS_MULTAGGR )
   {
      return SCIP_OKAY;
   }

   lhs = matrix->lhs[row];
   rhs = matrix->rhs[row];
   assert(!SCIPisInfinity(scip, lhs));
   assert(!SCIPisInfinity(scip, -rhs));

   /* get minimum/maximum activity of this row without the dominating variable */
   getActivityResiduals(scip, matrix, vardominating, valdominating, row, &minresactivity, &maxresactivity);

   assert(!SCIPisInfinity(scip, minresactivity));
   assert(!SCIPisInfinity(scip, -maxresactivity));

   lbdominating = SCIPvarGetLbLocal(vardominating);
   ubdominating = SCIPvarGetUbLocal(vardominating);

   lbdominated = SCIPvarGetLbLocal(vardominated);
   ubdominated = SCIPvarGetUbLocal(vardominated);

   *calculatedLb = -SCIPinfinity(scip);
   *calculatedUb = SCIPinfinity(scip);
   *calculatedLb2 = -SCIPinfinity(scip);
   *calculatedUb2 = SCIPinfinity(scip);

   /* predictive rowbound analysis */

   if( valdominating > 0.0 )
   {
      /* upper bound calculation */
      if( !SCIPisInfinity(scip, -minresactivity) && !SCIPisInfinity(scip, rhs) )
      {
         /* <= */
         *ubCalculated = TRUE;
         if( SCIPisZero(scip, valdominated) )
            *calculatedUb = (rhs - minresactivity)/valdominating;
         else
            if( valdominated < 0.0 )
               *calculatedUb = (rhs - (minresactivity - (valdominated * ubdominated) + (valdominated * lbdominated)))/valdominating;
            else
               *calculatedUb = (rhs - (minresactivity /* - (valdominated * lbdominated) + (valdominated * lbdominated)*/))/valdominating;
      }

      /* worst case calculation for upper bound */
      if( !SCIPisInfinity(scip, maxresactivity) && !SCIPisInfinity(scip, rhs) )
      {
         /* <= */
         *ub2Calculated = TRUE;
         if( SCIPisZero(scip, valdominated) )
            *calculatedUb2 = (rhs - maxresactivity)/valdominating;
         else
            if( valdominating < 0.0 )
               *calculatedUb2 = (rhs - maxresactivity)/valdominating;
            else
               *calculatedUb2 = (rhs - (maxresactivity - (valdominated * ubdominated) + (valdominated * lbdominated)))/valdominating;
      }

      /* lower bound calculation */
      if( !SCIPisInfinity(scip, maxresactivity) && !SCIPisInfinity(scip, -lhs) )
      {
         /* >= */
         *lbCalculated = TRUE;
         if( SCIPisZero(scip, valdominated) )
            *calculatedLb = (lhs - maxresactivity)/valdominating;
         else
            if( valdominated < 0.0 )
               *calculatedLb = (lhs - (maxresactivity /* - (valdominated * lbdominated) + (valdominated * lbdominated)*/ ))/valdominating;
            else
               *calculatedLb = (lhs - (maxresactivity - (valdominated * ubdominated) + (valdominated * lbdominated)))/valdominating;
      }

      /* worst case calculation for lower bound */
      if( !SCIPisInfinity(scip, -minresactivity) && !SCIPisInfinity(scip, -lhs) )
      {
         /* >= */
         *lb2Calculated = TRUE;
         if( SCIPisZero(scip, valdominated) )
            *calculatedLb2 = (lhs - minresactivity)/valdominating;
         else
            if( valdominated < 0.0 )
               *calculatedLb2 = (lhs - (minresactivity - (valdominated * ubdominated) + (valdominated * lbdominated)))/valdominating;
            else
               *calculatedLb2 = (lhs - minresactivity)/valdominating;
      }
   }
   else
   {
      /* normal case for lower bound calculation */
      if( !SCIPisInfinity(scip, -minresactivity) && !SCIPisInfinity(scip, rhs) )
      {
         /* <= */
         *lbCalculated = TRUE;
         if( SCIPisZero(scip, valdominated) )
            *calculatedLb = (rhs - minresactivity)/valdominating;
         else
            if( valdominated < 0.0 )
               *calculatedLb = (rhs - (minresactivity - (valdominated * ubdominated) + (valdominated * lbdominated)))/valdominating;
            else
               *calculatedLb = (rhs - (minresactivity /* - (valdominated * lbdominated) + (valdominated * lbdominated)*/))/valdominating;
      }

      /* worst case calculation for lower bound */
      if( !SCIPisInfinity(scip, maxresactivity) && !SCIPisInfinity(scip, rhs) )
      {
         /* <= */
         *lb2Calculated = TRUE;
         if( SCIPisZero(scip, valdominated) )
            *calculatedLb2 = (rhs - maxresactivity)/valdominating;
         else
            if( valdominated < 0.0 )
               *calculatedLb2 = (rhs - maxresactivity)/valdominating;
            else
               *calculatedLb2 = (rhs - (maxresactivity - (valdominated * ubdominated) + (valdominated * lbdominated)))/valdominating;
      }

      /* normal case for upper bound calculation */
      if( !SCIPisInfinity(scip, maxresactivity) && !SCIPisInfinity(scip, -lhs) )
      {
         /* >= */
         *ubCalculated = TRUE;
         if( SCIPisZero(scip, valdominated) )
            *calculatedUb = (lhs - maxresactivity)/valdominating;
         else
            if( valdominated < 0.0 )
               *calculatedUb = (lhs - (maxresactivity /* - (valdominated * lbdominated) + (valdominated * lbdominated)*/))/valdominating;
            else
               *calculatedUb = (lhs - (maxresactivity - (valdominated * ubdominated) + (valdominated * lbdominated)))/valdominating;
      }

      /* worst case calculation for upper bound */
      if( !SCIPisInfinity(scip, -minresactivity) && !SCIPisInfinity(scip, -lhs) )
      {
         /* >= */
         *ub2Calculated = TRUE;
         if( SCIPisZero(scip, valdominated) )
            *calculatedUb2 = (lhs - minresactivity)/valdominating;
         else
            if( valdominated < 0.0 )
               *calculatedUb2 = (lhs - (minresactivity - (valdominated * ubdominated) + (valdominated * lbdominated)))/valdominating;
            else
               *calculatedUb2 = (lhs - minresactivity)/valdominating;
      }
   }

   return SCIP_OKAY;
}

/** try to find new variable bounds and update them whether they are better then the old bounds */
static
SCIP_RETCODE updateBounds(
   SCIP*                 scip,               /**< SCIP main data structure */
   CONSTRAINTMATRIX*     matrix,             /**< matrix containing the constraints */
   int                   row,                /**< row index */
   int                   col1,               /**< dominating variable index */
   SCIP_Real             val1,               /**< dominating variable coefficient */
   int                   col2,               /**< dominated variable index */
   SCIP_Real             val2,               /**< dominated variable coefficient */
   SCIP_Real*            ubbound,            /**< current upper bound */
   SCIP_Real*            lbbound,            /**< current lower bound */
   SCIP_Real*            ub2bound,           /**< worstcase upper bound */
   SCIP_Real*            lb2bound            /**< worstcase lower bound */
   )
{
   SCIP_Bool lbcalculated;
   SCIP_Bool ubcalculated;
   SCIP_Bool lb2calculated;
   SCIP_Bool ub2calculated;

   SCIP_Real newlb;
   SCIP_Real newub;
   SCIP_Real newlb2;
   SCIP_Real newub2;

   assert(scip != NULL);
   assert(matrix != NULL);
   assert(row < matrix->nrows);
   assert(col1 < matrix->ncols);
   assert(col2 < matrix->ncols);

   /* do predictive rowbound analysis */
   SCIP_CALL( calcVarBounds(scip,matrix,row,
         col1,val1,col2,val2,
         &lbcalculated,&newlb,
         &ubcalculated,&newub,
         &lb2calculated,&newlb2,
         &ub2calculated,&newub2) );

   /* update bounds in case if they are better */
   if( ubcalculated )
   {
      if( newub < *ubbound )
         *ubbound = newub;
   }
   if( lbcalculated )
   {
      if( newlb > *lbbound )
         *lbbound = newlb;
   }
   if( ub2calculated )
   {
      if( newub2 < *ub2bound )
         *ub2bound = newub2;
   }
   if( lb2calculated )
   {
      if( newlb2 > *lb2bound )
         *lb2bound = newlb2;
   }

   return SCIP_OKAY;
}

/** try to find possible variable fixings */
static
void findFixings(
   SCIP*                 scip,               /**< SCIP main data structure */
   SCIP_VAR*             dominatingvar,      /**< dominating variable */
   int                   dominatingidx,      /**< column index of the dominating variable */
   SCIP_Real             dominatingubbound,  /**< current upper bound of the dominating variable */
   SCIP_Real             dominatinglbbound,  /**< current lower bound of the dominating variable */
   SCIP_Real             dominatingub2bound, /**< current worst case upper bound of the dominating variable */
   SCIP_Real             dominatinglb2bound, /**< current worst case lower bound of the dominating variable */
   SCIP_VAR*             dominatedvar,       /**< dominated variable */
   int                   dominatedidx,       /**< column index of the dominated variable */
   int*                  varstofix,          /**< array holding fixing information */
   SCIP_Bool             onlybinvars,        /**< flag indicating only binary variables are present */
   int*                  npossiblefixings,   /**< counter for possible fixings */
   int*                  ncliquepreventions, /**< counter for clique preventions */
   int*                  nboundpreventions   /**> counter for bound preventions */
   )
{
   if( onlybinvars )
   {
      if( SCIPvarsHaveCommonClique(dominatingvar,TRUE,dominatedvar,TRUE,TRUE) &&
         (!SCIPvarsHaveCommonClique(dominatingvar,TRUE,dominatedvar,FALSE,TRUE) ||
            !SCIPvarsHaveCommonClique(dominatingvar,FALSE,dominatedvar,FALSE,TRUE) ) )
      {
         /* we have a (1->1)-clique with dominance relation (x->y) (x dominates y)
          * from dominance relation we know (1->0) is better then (0->1)
          * it follows, only (1->0) or (0->0) are possible => y=0
          */
         if( varstofix[dominatedidx] == NOFIX )
         {
            varstofix[dominatedidx] = FIXATLB;
            (*npossiblefixings)++;
         }
      }
      else if( SCIPvarsHaveCommonClique(dominatingvar,FALSE,dominatedvar,FALSE,TRUE) &&
         (!SCIPvarsHaveCommonClique(dominatingvar,TRUE,dominatedvar,TRUE,TRUE) ||
            !SCIPvarsHaveCommonClique(dominatingvar,TRUE,dominatedvar,FALSE,TRUE) ) )
      {
         /* we have a (0->0)-clique with dominance relation x->y (x dominates y)
          * from dominance relation we know (1->0) is better then (0->1)
          * it follows only (1->0) or (1->1) are possible => x=1
          */
         if( varstofix[dominatingidx] == NOFIX )
         {
            varstofix[dominatingidx] = FIXATUB;
            (*npossiblefixings)++;
         }
      }
      else
      {
         (*ncliquepreventions)++;
      }
   }
   else
   {
      if( SCIPvarGetObj(dominatingvar) > 0.0 )
      {
         assert(SCIPvarGetObj(dominatedvar) > 0.0);
         if( !SCIPisInfinity(scip,-dominatinglb2bound) &&
            SCIPisLE(scip,dominatinglb2bound,SCIPvarGetUbLocal(dominatingvar)) )
         {
            /* we have a x->y dominance relation with a positive obj coefficient
             * of the dominating variable x, thus the obj coefficient of the
             * dominated variable y is positive too. we need to secure feasibility
             * by testing if the lower worst case bound is less equal the current upper bound.
             */
            if( varstofix[dominatedidx] == NOFIX )
            {
               varstofix[dominatedidx] = FIXATLB;
               (*npossiblefixings)++;
            }
         }
         else
         {
            (*nboundpreventions)++;
         }
      }
      else if( SCIPvarGetObj(dominatingvar) < 0.0 /*&& SCIPvarGetObj(dominatedvar) > 0.0*/ )
      {
         if( !SCIPisInfinity(scip,dominatingubbound) &&
            SCIPisLE(scip,dominatingubbound,SCIPvarGetUbLocal(dominatingvar)) )
         {
            /* we have a x->y dominance relation with a negative obj coefficient
             * of the dominating variable x, thus the obj coefficient of the
             * dominated variable y is positive or negative. in both cases we have to look
             * if the upper bound of the dominating variable is great enough.
             */
            if( varstofix[dominatedidx] == NOFIX )
            {
               varstofix[dominatedidx] = FIXATLB;
               (*npossiblefixings)++;
            }
         }
         else
         {
            (*nboundpreventions)++;
         }
      }
      else
      {
         if( !SCIPisInfinity(scip,-dominatinglb2bound) &&
            SCIPisLE(scip,dominatinglb2bound,SCIPvarGetUbLocal(dominatingvar)) &&
            !SCIPisInfinity(scip,dominatingubbound) &&
            SCIPisLE(scip,dominatingubbound,SCIPvarGetUbLocal(dominatingvar)) )
         {
            /* we claim both cases from above */
            if( varstofix[dominatedidx] == NOFIX )
            {
               varstofix[dominatedidx] = FIXATLB;
               (*npossiblefixings)++;
            }
         }
         else
         {
            (*nboundpreventions)++;
         }
      }
   }
}

/** find dominance relation between variable pairs */
static
SCIP_RETCODE findDominancePairs(
   SCIP*                 scip,               /**< SCIP main data structure */
   CONSTRAINTMATRIX*     matrix,             /**< matrix containing the constraints */
   int*                  searchcols,         /**< indexes of variables for pair comparisons */
   int                   searchsize,         /**< number of variables for pair comparisons */
   SCIP_Bool             onlybinvars,        /**< flag indicating searchcols has only binary variable indexes */
   int*                  varstofix,          /**< array holding information for later upper/lower bound fixing */
   int*                  npossiblefixings,   /**< found number of possible fixings */
   int*                  ndomrelations,      /**< found number of dominance relations */
   int*                  ncliquepreventions, /**< number of clique preventions for doing a variable fixing */
   int*                  nboundpreventions   /**< number of bound preventions for doing a variable fixing */
   )
{
   int cnt1;
   int cnt2;
   int col1;
   int col2;
   int* rows1;
   int* rows2;
   int nrows1;
   int nrows2;
   SCIP_Real* vals1;
   SCIP_Real* vals2;
   int r1;
   int r2;
   SCIP_Bool col1domcol2;
   SCIP_Bool col2domcol1;
   SCIP_Real tmpUbBoundCol1;
   SCIP_Real tmpLbBoundCol1;
   SCIP_Real tmpUbBoundCol2;
   SCIP_Real tmpLbBoundCol2;
   SCIP_Real tmpUb2BoundCol1;
   SCIP_Real tmpLb2BoundCol1;
   SCIP_Real tmpUb2BoundCol2;
   SCIP_Real tmpLb2BoundCol2;

   assert(scip != NULL);
   assert(matrix != NULL);
   assert(searchcols != NULL);
   assert(varstofix != NULL);
   assert(npossiblefixings != NULL);
   assert(ndomrelations != NULL);
   assert(ncliquepreventions != NULL);
   assert(nboundpreventions != NULL);

   /* pair comparisons */
   for( cnt1 = 0; cnt1 < searchsize; cnt1++ )
   {
      for( cnt2 = cnt1+1; cnt2 < searchsize; cnt2++ )
      {
         /* get indexes of this variable pair */
         col1 = searchcols[cnt1];
         col2 = searchcols[cnt2];

         /* we always have minimize as obj sense */

         /* column 1 dominating column 2 */
         col1domcol2 = (SCIPvarGetObj(matrix->vars[col1]) <= SCIPvarGetObj(matrix->vars[col2]));

         /* column 2 dominating column 1 */
         col2domcol1 = (SCIPvarGetObj(matrix->vars[col2]) <= SCIPvarGetObj(matrix->vars[col1]));

         /* search only if nothing was found yet */
         col1domcol2 = col1domcol2 && (varstofix[col2]==NOFIX);
         col2domcol1 = col2domcol1 && (varstofix[col1]==NOFIX);

         if( !col1domcol2 && !col2domcol1 )
            continue;

         /* get the data for both columns */
         vals1 = matrix->colmatval + matrix->colmatbeg[col1];
         rows1 = matrix->colmatind + matrix->colmatbeg[col1];
         nrows1 = matrix->colmatcnt[col1];
         r1 = 0;
         vals2 = matrix->colmatval + matrix->colmatbeg[col2];
         rows2 = matrix->colmatind + matrix->colmatbeg[col2];
         nrows2 = matrix->colmatcnt[col2];
         r2 = 0;

         /* do we have a obj constant? */
         if( nrows1 == 0 || nrows2 == 0 )
         {
            col1domcol2 = FALSE;
            col2domcol1 = FALSE;
            continue;
         }

         /* initialize temporary bounds */
         tmpUbBoundCol1 = SCIPinfinity(scip);
         tmpLbBoundCol1 = -tmpUbBoundCol1;
         tmpUbBoundCol2 = tmpUbBoundCol1;
         tmpLbBoundCol2 = tmpLbBoundCol1;
         tmpUb2BoundCol1 = SCIPinfinity(scip);
         tmpLb2BoundCol1 = -tmpUb2BoundCol1;
         tmpUb2BoundCol2 = tmpUb2BoundCol1;
         tmpLb2BoundCol2 = tmpLb2BoundCol1;

         /* compare rows of this column pair */
         while( (col1domcol2 || col2domcol1) && (r1 < nrows1 || r2 < nrows2))
         {
            assert( (r1 >= nrows1-1) || (rows1[r1] < rows1[r1+1]) );
            assert( (r2 >= nrows2-1) || (rows2[r2] < rows2[r2+1]) );

            /* there is a nonredundant row containing column 1 but not column 2 */
            if( r1 < nrows1 && (r2 == nrows2 || rows1[r1] < rows2[r2]) )
            {
               /* dominance depends on the type of relation */
               if( !SCIPisInfinity(scip,-matrix->lhs[rows1[r1]]) &&
                  !SCIPisInfinity(scip, matrix->rhs[rows1[r1]]) )
               {
                  /* no dominance relation for equations or ranged rows */
                  col2domcol1 = FALSE;
                  col1domcol2 = FALSE;
               }
               else if( SCIPisInfinity(scip,-matrix->lhs[rows1[r1]]) &&
                  !SCIPisInfinity(scip, matrix->rhs[rows1[r1]]) )
               {
                  /* <= relation, smaller coefficients dominate larger ones */
                  if( vals1[r1] < 0.0 )
                     col2domcol1 = FALSE;
                  else if( vals1[r1] > 0.0 )
                     col1domcol2 = FALSE;
               }
               else if( !SCIPisInfinity(scip,-matrix->lhs[rows1[r1]]) &&
                  SCIPisInfinity(scip, matrix->rhs[rows1[r1]]) )
               {
                  /* >= relation, larger coefficients dominate smaller ones */
                  if( vals1[r1] > 0.0 )
                     col2domcol1 = FALSE;
                  else if( vals1[r1] < 0.0 )
                     col1domcol2 = FALSE;
               }
               else
               {
                  col2domcol1 = FALSE;
                  col1domcol2 = FALSE;
               }

               /* update bounds only for dominance relation of non binary variables */
               if( col1domcol2 && !onlybinvars )
               {
                  SCIP_CALL( updateBounds(scip,matrix,rows1[r1],col1,vals1[r1],col2,0.0,
                        &tmpUbBoundCol1,&tmpLbBoundCol1,&tmpUb2BoundCol1,&tmpLb2BoundCol1) );
               }

               r1++;
            }
            /* there is a nonredundant row containing column 2, but not column 1 */
            else if( r2 < nrows2 && (r1 == nrows1 || rows1[r1] > rows2[r2]) )
            {
               /* dominance depends on the type of relation */
               if( !SCIPisInfinity(scip,-matrix->lhs[rows2[r2]]) &&
                  !SCIPisInfinity(scip, matrix->rhs[rows2[r2]]) )
               {
                  /* no dominance relation for equations or ranged rows */
                  col2domcol1 = FALSE;
                  col1domcol2 = FALSE;
               }
               else if( SCIPisInfinity(scip,-matrix->lhs[rows2[r2]]) &&
                  !SCIPisInfinity(scip, matrix->rhs[rows2[r2]]) )
               {
                  /* <= relation, smaller coefficients dominate larger ones */
                  if( vals2[r2] > 0.0 )
                     col2domcol1 = FALSE;
                  else if( vals2[r2] < 0.0 )
                     col1domcol2 = FALSE;
               }
               else if( !SCIPisInfinity(scip,-matrix->lhs[rows2[r2]]) &&
                  SCIPisInfinity(scip, matrix->rhs[rows2[r2]]) )
               {
                  /* >= relation, larger coefficients dominate smaller ones */
                  if( vals2[r2] < 0.0 )
                     col2domcol1 = FALSE;
                  else if( vals2[r2] > 0.0 )
                     col1domcol2 = FALSE;
               }
               else
               {
                  col2domcol1 = FALSE;
                  col1domcol2 = FALSE;
               }

               /* update bounds only for dominance relation of non binary variables */
               if( col2domcol1 && !onlybinvars )
               {
                  SCIP_CALL( updateBounds(scip,matrix,rows2[r2],col2,vals2[r2],col1,0.0,
                        &tmpUbBoundCol2,&tmpLbBoundCol2,&tmpUb2BoundCol2,&tmpLb2BoundCol2) );
               }
               r2++;
            }
            /* if both columns appear in a common row, compare the coefficients */
            else
            {
               assert(r1 < nrows1 && r2 < nrows2);
               assert(rows1[r1] == rows2[r2]);

               /* dominance depends on the type of inequality */
               if( !SCIPisInfinity(scip,-matrix->lhs[rows1[r1]]) &&
                  !SCIPisInfinity(scip, matrix->rhs[rows1[r1]]) )
               {
                  /* no dominance relation if coefficients differ for equations or ranged rows */
                  if( vals1[r1] != vals2[r2] )
                  {
                     col2domcol1 = FALSE;
                     col1domcol2 = FALSE;
                  }
               }
               else if(  SCIPisInfinity(scip,-matrix->lhs[rows1[r1]]) &&
                  !SCIPisInfinity(scip,matrix->rhs[rows1[r1]]) )
               {
                  /* <= relation, smaller coefficients dominate larger ones */
                  if( vals1[r1] < vals2[r2] )
                     col2domcol1 = FALSE;
                  else if( vals1[r1] > vals2[r2] )
                     col1domcol2 = FALSE;
               }
               else if( !SCIPisInfinity(scip,-matrix->lhs[rows1[r1]]) &&
                  SCIPisInfinity(scip, matrix->rhs[rows1[r1]]) )
               {
                  /* >= relation, larger coefficients dominate smaller ones */
                  if( vals1[r1] > vals2[r2] )
                     col2domcol1 = FALSE;
                  else if( vals1[r1] < vals2[r2] )
                     col1domcol2 = FALSE;
               }
               else
               {
                  col1domcol2 = FALSE;
                  col2domcol1 = FALSE;
               }

               /* update bounds only for dominance relation of non binary variables */
               if( col1domcol2 && !onlybinvars )
               {
                  SCIP_CALL( updateBounds(scip,matrix,rows1[r1],col1,vals1[r1],col2,vals2[r2],
                        &tmpUbBoundCol1,&tmpLbBoundCol1,&tmpUb2BoundCol1,&tmpLb2BoundCol1) );
               }
               else if( col2domcol1 && !onlybinvars )
               {
                  SCIP_CALL( updateBounds(scip,matrix,rows2[r2],col2,vals2[r2],col1,vals1[r1],
                        &tmpUbBoundCol2,&tmpLbBoundCol2,&tmpUb2BoundCol2,&tmpLb2BoundCol2) );
               }

               r1++;
               r2++;
            }
         }

         /* a column is only dominated, if there are no more rows in which it is contained */
         col1domcol2 = col1domcol2 && r2 == nrows2;
         col2domcol1 = col2domcol1 && r1 == nrows1;

         if( !col1domcol2 && !col2domcol1 )
            continue;

         /* no dominance relation for left equations or ranged rows */
         while( r1 < nrows1 )
         {
            if( !SCIPisInfinity(scip,-matrix->lhs[rows1[r1]]) &&
               !SCIPisInfinity(scip, matrix->rhs[rows1[r1]]) )
            {
               col2domcol1 = FALSE;
               col1domcol2 = FALSE;
               break;
            }
            r1++;
         }
         while( r2 < nrows2 )
         {
            if( !SCIPisInfinity(scip,-matrix->lhs[rows2[r2]]) &&
               !SCIPisInfinity(scip, matrix->rhs[rows2[r2]]) )
            {
               col2domcol1 = FALSE;
               col1domcol2 = FALSE;
               break;
            }
            r2++;
         }

         if( col1domcol2 || col2domcol1 )
            (*ndomrelations)++;

         /* use dominance relation and cliquen/bound-information to find variable fixings */
         if( col1domcol2 )
         {
            findFixings(scip, matrix->vars[col1], col1, tmpUbBoundCol1, tmpLbBoundCol1, tmpUb2BoundCol1, tmpLb2BoundCol1,
               matrix->vars[col2], col2, varstofix, onlybinvars, npossiblefixings, ncliquepreventions, nboundpreventions);
         }
         else if( col2domcol1 )
         {
            findFixings(scip,matrix->vars[col2],col2,tmpUbBoundCol2,tmpLbBoundCol2,tmpUb2BoundCol2,tmpLb2BoundCol2,
               matrix->vars[col1], col1, varstofix, onlybinvars, npossiblefixings, ncliquepreventions, nboundpreventions);
         }
      }
   }

   return SCIP_OKAY;
}


/*
 * Callback methods of presolver
 */

#define presolCopyDomcol NULL
#define presolFreeDomcol NULL
#define presolInitDomcol NULL
#define presolExitDomcol NULL
#define presolInitpreDomcol NULL
#define presolExitpreDomcol NULL


/** execution method of presolver */
static
SCIP_DECL_PRESOLEXEC(presolExecDomcol)
{  /*lint --e{715}*/
   CONSTRAINTMATRIX* matrix;
   SCIP_Bool initialized;

   assert(result != NULL);
   *result = SCIP_DIDNOTRUN;

   /* do no dominated column presolving in case of probing an nonlinear processing */
   if( (SCIPgetStage(scip) != SCIP_STAGE_PRESOLVING) || SCIPinProbing(scip) || SCIPhasNonlinearitiesPresent(scip) )
      return SCIP_OKAY;

   *result = SCIP_DIDNOTFIND;

   /* initialize constraint matrix */
   matrix = NULL;
   initialized = FALSE;
   SCIP_CALL( SCIPallocBuffer(scip, &matrix) );
   initMatrix(scip, matrix, &initialized);

   if ( initialized )
   {
      int npossiblefixings;
      int ncliquepreventions;
      int nboundpreventions;
      int ndomrelations;
      int v;
      int r;
      int* varstofix;
      int* varsprocessed;
      int nconvarsfixed;
      int nintvarsfixed;
      int nbinvarsfixed;
      int nvars;
      int nrows;
      int* rowidxsorted;
      int* rowsparsity;
      int varcount;
      int* consearchcols;
      int* intsearchcols;
      int* binsearchcols;
      int nconfill;
      int nintfill;
      int nbinfill;
      SCIP_Bool onlybinvars;

      assert(SCIPgetNVars(scip) == matrix->ncols);

      npossiblefixings = 0;
      ncliquepreventions = 0;
      nboundpreventions = 0;
      ndomrelations = 0;
      nconvarsfixed = 0;
      nintvarsfixed = 0;
      nbinvarsfixed = 0;
      nvars = matrix->ncols;
      nrows = matrix->nrows;

      SCIP_CALL( SCIPallocBufferArray(scip, &varstofix, nvars) );
      SCIP_CALL( SCIPallocBufferArray(scip, &varsprocessed, nvars) );
      for( v = 0; v < nvars; ++v )
      {
         varstofix[v] = NOFIX;
         varsprocessed[v] = FALSE;
      }
      SCIP_CALL( SCIPallocBufferArray(scip, &consearchcols, nvars) );
      SCIP_CALL( SCIPallocBufferArray(scip, &intsearchcols, nvars) );
      SCIP_CALL( SCIPallocBufferArray(scip, &binsearchcols, nvars) );

      SCIP_CALL( SCIPallocBufferArray(scip, &rowidxsorted, nrows) );
      SCIP_CALL( SCIPallocBufferArray(scip, &rowsparsity, nrows) );
      for( r = 0; r < nrows; ++r )
      {
         rowidxsorted[r] = r;
         rowsparsity[r] = matrix->rowmatcnt[r];
      }

      /* sort rows per sparsity */
      SCIPsortIntInt(rowsparsity, rowidxsorted, nrows);

      /* verify if we only have binary variables */
      onlybinvars = (SCIPgetNIntVars(scip) + SCIPgetNImplVars(scip) + SCIPgetNContVars(scip)) == 0;

      /* search for dominance relations by row-sparsity order */
      varcount = 0;
      for( r = 0; r < nrows; ++r )
      {
         int rowidx;
         int* rowpnt;
         int* rowend;

         rowidx = rowidxsorted[r];
         rowpnt = matrix->rowmatind + matrix->rowmatbeg[rowidx];
         rowend = rowpnt + matrix->rowmatcnt[rowidx];

         nconfill = 0;
         nintfill = 0;
         nbinfill = 0;

         if( !onlybinvars )
         {
            for(; rowpnt < rowend; rowpnt++ )
            {
               if( varsprocessed[*rowpnt] == FALSE )
               {
                  int varidx;
                  varidx = *rowpnt;

                  /* higher variable types dominate smaller ones: bin <- int <- impl <- cont
                   * we search only for dominance relations between the same variable type
                   */
                  if(SCIPvarGetType(matrix->vars[varidx]) == SCIP_VARTYPE_CONTINUOUS)
                  {
                     consearchcols[nconfill++] = varidx;
                  }
                  else if( SCIPvarGetType(matrix->vars[varidx]) == SCIP_VARTYPE_INTEGER ||
                     SCIPvarGetType(matrix->vars[varidx]) == SCIP_VARTYPE_IMPLINT )
                  {
                     intsearchcols[nintfill++] = varidx;
                  }
                  else if(SCIPvarGetType(matrix->vars[varidx]) == SCIP_VARTYPE_BINARY)
                  {
                     binsearchcols[nbinfill++] = varidx;
                  }
               }
            }

            /* search for dominance relations between continuous variables */
            if( nconfill > 1 )
            {
               SCIP_CALL( findDominancePairs(scip,matrix,consearchcols,nconfill,FALSE,
                     varstofix,&npossiblefixings,&ndomrelations,&ncliquepreventions,&nboundpreventions) );
            }
            for( v = 0; v < nconfill; ++v )
            {
               varsprocessed[consearchcols[v]] = TRUE;
            }
            varcount += nconfill;

            /* search for dominance relations between integer and impl-integer variables */
            if( nintfill > 1 )
            {
               SCIP_CALL( findDominancePairs(scip,matrix,intsearchcols,nintfill,FALSE,
                     varstofix,&npossiblefixings,&ndomrelations,&ncliquepreventions,&nboundpreventions) );
            }
            for( v = 0; v < nintfill; ++v )
            {
               varsprocessed[intsearchcols[v]] = TRUE;
            }
            varcount += nintfill;

            /* search for dominance relations between binary variables */
            if( nbinfill > 1 )
            {
               SCIP_CALL( findDominancePairs(scip,matrix,binsearchcols,nbinfill,TRUE,
                     varstofix,&npossiblefixings,&ndomrelations,&ncliquepreventions,&nboundpreventions) );
            }
            for( v = 0; v < nbinfill; ++v )
            {
               varsprocessed[binsearchcols[v]] = TRUE;
            }
            varcount += nbinfill;
         }
         else
         {
            /* we only have binary variables */
            for(; rowpnt < rowend; rowpnt++ )
            {
               if( varsprocessed[*rowpnt] == FALSE )
               {
                  binsearchcols[nbinfill++] = *rowpnt;
               }
            }

            /* search for dominance relations between binary variables */
            if( nbinfill > 1 )
            {
               SCIP_CALL( findDominancePairs(scip,matrix,binsearchcols,nbinfill,TRUE,
                     varstofix,&npossiblefixings,&ndomrelations,&ncliquepreventions,&nboundpreventions) );
            }
            for( v = 0; v < nbinfill; ++v )
            {
               varsprocessed[binsearchcols[v]] = TRUE;
            }
            varcount += nbinfill;
         }

         if( varcount >= nvars )
         {
            break;
         }
      }

      if( npossiblefixings > 0 )
      {
         /* look for fixable variables
          * loop backwards, since a variable fixing can change the current and
          * the subsequent slots in the vars array
          */
         for( v = matrix->ncols-1; v >= 0; --v )
         {
            SCIP_Bool infeasible;
            SCIP_Bool fixed;
            SCIP_VAR* var;

            if( varstofix[v] == FIXATLB )
            {
               SCIP_Real lb;

               var = matrix->vars[v];
               lb = SCIPvarGetLbLocal(var);

               if( SCIPvarGetType(var) != SCIP_VARTYPE_CONTINUOUS )
               {
                  lb = SCIPfeasCeil(scip, lb);
               }

               /* fix at lower bound */
               SCIP_CALL( SCIPfixVar(scip, var, lb, &infeasible, &fixed) );
               if( infeasible )
               {
                  SCIPdebugMessage(" -> infeasible fixing\n");
                  *result = SCIP_CUTOFF;
                  return SCIP_OKAY;
               }
               assert(fixed);
               (*nfixedvars)++;
               *result = SCIP_SUCCESS;

               if( SCIPvarGetType(var) == SCIP_VARTYPE_CONTINUOUS )
               {
                  nconvarsfixed++;
               }
               else if( SCIPvarGetType(var) == SCIP_VARTYPE_INTEGER || SCIPvarGetType(var) == SCIP_VARTYPE_IMPLINT )
               {
                  nintvarsfixed++;
               }
               else
               {
                  nbinvarsfixed++;
               }
            }
            else if( varstofix[v] == FIXATUB )
            {
               SCIP_Real ub;

               var = matrix->vars[v];
               ub = SCIPvarGetUbLocal(var);

               if( SCIPvarGetType(var) != SCIP_VARTYPE_CONTINUOUS )
               {
                  ub = SCIPfeasFloor(scip, ub);
               }

               /* fix at upper bound */
               SCIP_CALL( SCIPfixVar(scip, var, ub, &infeasible, &fixed) );
               if( infeasible )
               {
                  SCIPdebugMessage(" -> infeasible fixing\n");
                  *result = SCIP_CUTOFF;
                  return SCIP_OKAY;
               }
               assert(fixed);
               (*nfixedvars)++;
               *result = SCIP_SUCCESS;

               if( SCIPvarGetType(var) == SCIP_VARTYPE_CONTINUOUS )
               {
                  nconvarsfixed++;
               }
               else if( SCIPvarGetType(var) == SCIP_VARTYPE_INTEGER || SCIPvarGetType(var) == SCIP_VARTYPE_IMPLINT )
               {
                  nintvarsfixed++;
               }
               else
               {
                  nbinvarsfixed++;
               }
            }
         }
      }

      SCIPfreeBufferArray(scip, &rowsparsity);
      SCIPfreeBufferArray(scip, &rowidxsorted);
      SCIPfreeBufferArray(scip, &binsearchcols);
      SCIPfreeBufferArray(scip, &intsearchcols);
      SCIPfreeBufferArray(scip, &consearchcols);
      SCIPfreeBufferArray(scip, &varsprocessed);
      SCIPfreeBufferArray(scip, &varstofix);

      if( (nconvarsfixed + nintvarsfixed + nbinvarsfixed) > 0 )
      {
         SCIPdebugMessage("### %d vars [%d dom] (%d clqprev, %d bndprev) ===>>> fixed [c=%d, z=%d, b=%d]\n",
            matrix->ncols, ndomrelations, ncliquepreventions, nboundpreventions, nconvarsfixed, nintvarsfixed, nbinvarsfixed);
      }
      else
      {
          SCIPdebugMessage("### %d vars [%d dom] (%d clqprev, %d bndprev)\n",
            matrix->ncols, ndomrelations, ncliquepreventions, nboundpreventions);
      }
   }

   freeMatrix(scip, &matrix);

   return SCIP_OKAY;
}


/*
 * presolver specific interface methods
 */

/** creates the domcol presolver and includes it in SCIP */
SCIP_RETCODE SCIPincludePresolDomcol(
   SCIP*                 scip                /**< SCIP data structure */
   )
{
   SCIP_PRESOLDATA* presoldata;

   presoldata = NULL;

   /* include presolver */
   SCIP_CALL( SCIPincludePresol(scip,
         PRESOL_NAME,
         PRESOL_DESC,
         PRESOL_PRIORITY,
         PRESOL_MAXROUNDS,
         PRESOL_DELAY,
         presolCopyDomcol,
         presolFreeDomcol,
         presolInitDomcol,
         presolExitDomcol,
         presolInitDomcol,
         presolExitDomcol,
         presolExecDomcol,
         presoldata) );

   return SCIP_OKAY;
}
