/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2003 Tobias Achterberg                              */
/*                                                                           */
/*                  2002-2003 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the SCIP Academic Licence.        */
/*                                                                           */
/*  You should have received a copy of the SCIP Academic License             */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: lp.h,v 1.53 2003/11/21 10:35:36 bzfpfend Exp $"

/**@file   lp.h
 * @brief  LP management methods and datastructures
 * @author Tobias Achterberg
 *
 *  In SCIP, the LP is defined as follows:
 *
 *   min       obj * x
 *      lhs <=   A * x + const <= rhs
 *      lb  <=       x         <= ub
 *
 *  The row activities are defined as 
 *     activity = A * x + const
 *  and must therefore be in the range of [lhs,rhs].
 *
 *  The reduced costs are defined as
 *     redcost = obj - A^T * y
 *  and must be   nonnegative, if the corresponding lb is nonnegative,
 *                zero,        if the corresponging lb is negative.
 *
 *  The main datastructures for storing an LP are the rows and the columns.
 *  A row can live on its own (if it was created by a separator), or as LP
 *  relaxation of a constraint. Thus, it has a nuses-counter, and is
 *  deleted, if not needed any more.
 *  A column cannot live on its own. It is always connected to a problem
 *  variable. Because pricing is always problem specific, it cannot create
 *  LP columns without introducing new variables. Thus, each column is
 *  connected to exactly one variable, and is deleted, if the variable
 *  is deleted.
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __LP_H__
#define __LP_H__


#include <stdio.h>


/** solution status after solving LP */
enum LPSolStat
{
   SCIP_LPSOLSTAT_NOTSOLVED  = 0,       /**< LP was not solved, no solution exists */
   SCIP_LPSOLSTAT_OPTIMAL    = 1,       /**< LP was solved to optimality */
   SCIP_LPSOLSTAT_INFEASIBLE = 2,       /**< LP is primal infeasible */
   SCIP_LPSOLSTAT_UNBOUNDED  = 3,       /**< LP is primal unbounded */
   SCIP_LPSOLSTAT_OBJLIMIT   = 4,       /**< objective limit was reached during optimization */
   SCIP_LPSOLSTAT_ITERLIMIT  = 5,       /**< iteration limit was reached during optimization */
   SCIP_LPSOLSTAT_TIMELIMIT  = 6,       /**< time limit was reached during optimization */
   SCIP_LPSOLSTAT_ERROR      = 7        /**< an error occured during optimization */
};
typedef enum LPSolStat LPSOLSTAT;

/** type of variable bound: lower or upper bound */
enum BoundType
{
   SCIP_BOUNDTYPE_LOWER = 0,            /**< lower bound */
   SCIP_BOUNDTYPE_UPPER = 1             /**< upper bound */
};
typedef enum BoundType BOUNDTYPE;

/** type of row side: left hand or right hand side */
enum SideType
{
   SCIP_SIDETYPE_LEFT  = 0,             /**< left hand side */
   SCIP_SIDETYPE_RIGHT = 1              /**< right hand side */
};
typedef enum SideType SIDETYPE;

typedef struct Col COL;                 /**< column of an LP */
typedef struct Row ROW;                 /**< row of an LP */
typedef struct Lp LP;                   /**< actual LP data */



#include "def.h"
#include "mem.h"
#include "set.h"
#include "stat.h"
#include "lpi.h"
#include "var.h"
#include "cons.h"
#include "sol.h"
#include "clock.h"



/** variable of the problem and corresponding LP column */
struct Col
{
   VAR*             var;                /**< variable, this column represents; there cannot be a column without variable */
   ROW**            rows;               /**< rows of column entries, that may have a nonzero dual solution value */
   Real*            vals;               /**< coefficients of column entries */
   int*             linkpos;            /**< position of col in col vector of the row, or -1 if not yet linked */
   Real             obj;                /**< current objective value of column in LP */
   Real             lb;                 /**< current lower bound of column in LP */
   Real             ub;                 /**< current upper bound of column in LP */
   Real             primsol;            /**< primal solution value in LP, is 0 if col is not in LP */
   Real             redcost;            /**< reduced cost value in LP, or SCIP_INVALID if not yet calculated */
   Real             farkas;             /**< value in dual farkas infeasibility proof */
   Real             strongdown;         /**< strong branching information for downwards branching */
   Real             strongup;           /**< strong branching information for upwards branching */
   int              index;              /**< consecutively numbered column identifier */
   int              size;               /**< size of the row- and val-arrays */
   int              len;                /**< number of nonzeros in column */
   int              nunlinked;          /**< number of column entries, where the rows don't know about the column */
   int              lppos;              /**< column position number in actual LP, or -1 if not in actual LP */
   int              lpipos;             /**< column position number in LP solver, or -1 if not in LP solver */
   int              validredcostlp;     /**< lp number for which reduced cost value is valid */
   int              validfarkaslp;      /**< lp number for which farkas value is valid */
   int              validstronglp;      /**< lp number for which strong branching values are valid */
   int              strongitlim;        /**< strong branching iteration limit used to get strongdown and strongup, or -1 */
   int              age;                /**< number of successive times this variable was in LP and was 0.0 in solution */
   Longint          obsoletenode;       /**< last node where this column was removed due to aging */
   int              var_probindex;      /**< copy of var->probindex for avoiding expensive dereferencing */
   unsigned int     sorted:1;           /**< TRUE iff row indices are sorted in increasing order */
   unsigned int     objchanged:1;       /**< TRUE iff objective value changed, and data of LP solver has to be updated */
   unsigned int     lbchanged:1;        /**< TRUE iff lower bound changed, and data of LP solver has to be updated */
   unsigned int     ubchanged:1;        /**< TRUE iff upper bound changed, and data of LP solver has to be updated */
   unsigned int     coefchanged:1;      /**< TRUE iff the coefficient vector changed, and LP solver has to be updated */
   unsigned int     removeable:1;       /**< TRUE iff column is removeable from the LP (due to aging or cleanup) */
};

/** row of the LP */
struct Row
{
   char*            name;               /**< name of the row */
   COL**            cols;               /**< columns of row entries, that may have a nonzero primal solution value */
   int*             cols_probindex;     /**< copy of cols[i]->var->probindex for avoiding expensive dereferencing */
   Real*            vals;               /**< coefficients of row entries */
   int*             linkpos;            /**< position of row in row vector of the column, or -1 if not yet linked */
   Real             constant;           /**< constant shift c in row lhs <= ax + c <= rhs */
   Real             lhs;                /**< left hand side of row */
   Real             rhs;                /**< right hand side of row */
   Real             sqrnorm;            /**< squared euclidean norm of row vector */
   Real             maxval;             /**< maximal absolute value of row vector, only valid if nummaxval > 0 */
   Real             minval;             /**< minimal absolute non-zero value of row vector, only valid if numminval > 0 */
   Real             dualsol;            /**< dual solution value in LP, is 0 if row is not in LP */
   Real             activity;           /**< row activity value in LP, or SCIP_INVALID if not yet calculated */
   Real             dualfarkas;         /**< multiplier value in dual farkas infeasibility proof */
   Real             pseudoactivity;     /**< row activity value in pseudo solution, or SCIP_INVALID if not yet calculated */
   Real             minactivity;        /**< minimal activity value w.r.t. the column's bounds, or SCIP_INVALID */
   Real             maxactivity;        /**< maximal activity value w.r.t. the column's bounds, or SCIP_INVALID */
   int              index;              /**< consecutively numbered row identifier */
   int              size;               /**< size of the col- and val-arrays */
   int              len;                /**< number of nonzeros in row */
   int              nunlinked;          /**< number of row entries, where the columns don't know about the row */
   int              nuses;              /**< number of times, this row is referenced */
   int              lppos;              /**< row position number in actual LP, or -1 if not in actual LP */
   int              lpipos;             /**< row position number in LP solver, or -1 if not in LP solver */
   int              minidx;             /**< minimal column index of row entries */
   int              maxidx;             /**< maximal column index of row entries */
   int              nummaxval;          /**< number of coefs with absolute value equal to maxval, zero if maxval invalid */
   int              numminval;          /**< number of coefs with absolute value equal to minval, zero if minval invalid */
   int              validactivitylp;    /**< lp number for which activity value is valid */
   Longint          validpsactivitybdchg; /**< bound change number for which pseudo activity value is valid */
   Longint          validactivitybdsbdchg;/**< bound change number for which activity bound values are valid */
   int              age;                /**< number of successive times this row was in LP and was not sharp in solution */
   Longint          obsoletenode;       /**< last node where this row was removed due to aging */
   unsigned int     sorted:1;           /**< are column indices sorted in increasing order? */
   unsigned int     delaysort:1;        /**< should the row sorting be delayed and done in a lazy fashion? */
   unsigned int     validminmaxidx:1;   /**< are minimal and maximal column index valid? */
   unsigned int     lhschanged:1;       /**< was left hand side or constant changed, and has LP solver to be updated? */
   unsigned int     rhschanged:1;       /**< was right hand side or constant changed, and has LP solver to be updated? */
   unsigned int     coefchanged:1;      /**< was the coefficient vector changed, and has LP solver to be updated? */
   unsigned int     local:1;            /**< is row only valid locally? */
   unsigned int     modifiable:1;       /**< is row modifiable during node processing (subject to column generation)? */
   unsigned int     removeable:1;       /**< TRUE iff row is removeable from the LP (due to aging or cleanup) */
   unsigned int     nlocks:24;          /**< number of sealed locks of an unmodifiable row */
};

/** actual LP data */
struct Lp
{
   LPI*             lpi;                /**< LP solver interface */
   LPISTATE*        divelpistate;       /**< stores LPI state (basis information) before diving starts */
   COL**            lpicols;            /**< array with columns actually stored in the LP solver */
   ROW**            lpirows;            /**< array with rows actually stored in the LP solver */
   COL**            chgcols;            /**< array of changed columns not yet applied to the LP solver */
   ROW**            chgrows;            /**< array of changed rows not yet applied to the LP solver */
   COL**            cols;               /**< array with actual LP columns in correct order */
   ROW**            rows;               /**< array with actual LP rows in correct order */
   LPSOLSTAT        lpsolstat;          /**< solution status of last LP solution */
   Real             objval;             /**< objective value of LP, or SCIP_INVALID */
   Real             upperbound;         /**< upper objective limit of LP solver (copy of primal->upperbound) */
   int              lpicolssize;        /**< available slots in lpicols vector */
   int              nlpicols;           /**< number of columns in the LP solver */
   int              lpirowssize;        /**< available slots in lpirows vector */
   int              nlpirows;           /**< number of rows in the LP solver */
   int              lpifirstchgcol;     /**< first column of the LP which differs from the column in the LP solver */
   int              lpifirstchgrow;     /**< first row of the LP which differs from the row in the LP solver */
   int              chgcolssize;        /**< available slots in chgcols vector */
   int              nchgcols;           /**< actual number of chgcols (number of used slots in chgcols vector) */
   int              chgrowssize;        /**< available slots in chgrows vector */
   int              nchgrows;           /**< actual number of chgrows (number of used slots in chgrows vector) */
   int              colssize;           /**< available slots in cols vector */
   int              ncols;              /**< actual number of LP columns (number of used slots in cols vector) */
   int              rowssize;           /**< available slots in rows vector */
   int              nrows;              /**< actual number of LP rows (number of used slots in rows vector) */
   int              firstnewcol;        /**< first column added at the active node */
   int              firstnewrow;        /**< first row added at the active node */
   int              nremoveablecols;    /**< number of removeable columns in the LP */
   int              nremoveablerows;    /**< number of removeable rows in the LP */
   unsigned int     flushed:1;          /**< are all cached changes applied to the LP solver? */
   unsigned int     solved:1;           /**< is current LP solved? */
   unsigned int     primalfeasible:1;   /**< is actual LP basis primal feasible? */
   unsigned int     dualfeasible:1;     /**< is actual LP basis dual feasible? */
   unsigned int     diving:1;           /**< LP is used for diving: col bounds and obj don't corresond to variables */
};


/*
 * Column methods
 */

/** creates an LP column */
extern
RETCODE SCIPcolCreate(
   COL**            col,                /**< pointer to column data */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat,               /**< problem statistics */
   VAR*             var,                /**< variable, this column represents */
   int              len,                /**< number of nonzeros in the column */
   ROW**            row,                /**< array with rows of column entries */
   Real*            val,                /**< array with coefficients of column entries */
   Bool             removeable          /**< should the column be removed from the LP due to aging or cleanup? */
   );

/** frees an LP column */
extern
RETCODE SCIPcolFree(
   COL**            col,                /**< pointer to LP column */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp                  /**< actual LP data */
   );

/** sorts column entries by row index */
extern
void SCIPcolSort(
   COL*             col                 /**< column to be sorted */
   );

/** adds a previously non existing coefficient to an LP column */
extern
RETCODE SCIPcolAddCoeff(
   COL*             col,                /**< LP column */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp,                 /**< actual LP data */
   ROW*             row,                /**< LP row */
   Real             val                 /**< value of coefficient */
   );

/** deletes coefficient from column */
extern
RETCODE SCIPcolDelCoeff(
   COL*             col,                /**< column to be changed */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp,                 /**< actual LP data */
   ROW*             row                 /**< coefficient to be deleted */
   );

/** changes or adds a coefficient to an LP column */
extern
RETCODE SCIPcolChgCoeff(
   COL*             col,                /**< LP column */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp,                 /**< actual LP data */
   ROW*             row,                /**< LP row */
   Real             val                 /**< value of coefficient */
   );

/** increases value of an existing or nonexisting coefficient in an LP column */
extern
RETCODE SCIPcolIncCoeff(
   COL*             col,                /**< LP column */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp,                 /**< actual LP data */
   ROW*             row,                /**< LP row */
   Real             incval              /**< value to add to the coefficient */
   );

/** changes objective value of column */
extern
RETCODE SCIPcolChgObj(
   COL*             col,                /**< LP column to change */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp,                 /**< actual LP data */
   Real             newobj              /**< new objective value */
   );

/** changes lower bound of column */
extern
RETCODE SCIPcolChgLb(
   COL*             col,                /**< LP column to change */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp,                 /**< actual LP data */
   Real             newlb               /**< new lower bound value */
   );

/** changes upper bound of column */
extern
RETCODE SCIPcolChgUb(
   COL*             col,                /**< LP column to change */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp,                 /**< actual LP data */
   Real             newub               /**< new upper bound value */
   );

/** gets lower bound of column */
extern
Real SCIPcolGetLb(
   COL*             col                 /**< LP column */
   );

/** gets upper bound of column */
extern
Real SCIPcolGetUb(
   COL*             col                 /**< LP column */
   );

/** gets variable this column represents */
extern
VAR* SCIPcolGetVar(
   COL*             col                 /**< LP column */
   );

/** gets position of column in actual LP, or -1 if it is not in LP */
extern
int SCIPcolGetLPPos(
   COL*             col                 /**< LP column */
   );

/** returns TRUE iff column is member of actual LP */
extern
Bool SCIPcolIsInLP(
   COL*             col                 /**< LP column */
   );

/** gets best bound of column with respect to the objective function */
extern
Real SCIPcolGetBestBound(
   COL*             col                 /**< LP column */
   );

/** gets the primal LP solution of a column */
extern
Real SCIPcolGetPrimsol(
   COL*             col                 /**< LP column */
   );

/** gets the reduced costs of a column in last LP or after recalculation */
extern
Real SCIPcolGetRedcost(
   COL*             col,                /**< LP column */
   STAT*            stat                /**< problem statistics */
   );

/** gets the feasibility of a column in last LP or after recalculation */
extern
Real SCIPcolGetFeasibility(
   COL*             col,                /**< LP column */
   STAT*            stat                /**< problem statistics */
   );

/** gets the farkas value of a column in last LP (which must be infeasible) */
extern
Real SCIPcolGetFarkas(
   COL*             col,                /**< LP column */
   STAT*            stat                /**< problem statistics */
   );

/** gets strong branching information on a column variable */
extern
RETCODE SCIPcolGetStrongbranch(
   COL*             col,                /**< LP column */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat,               /**< problem statistics */
   LP*              lp,                 /**< actual LP data */
   Real             upperbound,         /**< actual global upper bound */
   int              itlim,              /**< iteration limit for strong branchings */
   Real*            down,               /**< stores dual bound after branching column down */
   Real*            up                  /**< stores dual bound after branching column up */
   );

/** get number of nonzero entries in column vector */
extern
int SCIPcolGetNNonz(
   COL*             col                 /**< LP column */
   );

/** gets array with rows of nonzero entries */
extern
ROW** SCIPcolGetRows(
   COL*             col                 /**< LP column */
   );

/** gets array with coefficients of nonzero entries */
extern
Real* SCIPcolGetVals(
   COL*             col                 /**< LP column */
   );

/** output column to file stream */
extern
void SCIPcolPrint(
   COL*             col,                /**< LP column */
   FILE*            file                /**< output file (or NULL for standard output) */
   );


/*
 * Row methods
 */

/** creates and captures an LP row */
extern
RETCODE SCIProwCreate(
   ROW**            row,                /**< pointer to LP row data */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat,               /**< problem statistics */
   const char*      name,               /**< name of row */
   int              len,                /**< number of nonzeros in the row */
   COL**            col,                /**< array with columns of row entries */
   Real*            val,                /**< array with coefficients of row entries */
   Real             lhs,                /**< left hand side of row */
   Real             rhs,                /**< right hand side of row */
   Bool             local,              /**< is row only valid locally? */
   Bool             modifiable,         /**< is row modifiable during node processing (subject to column generation)? */
   Bool             removeable          /**< should the row be removed from the LP due to aging or cleanup? */
   );

/** frees an LP row */
extern
RETCODE SCIProwFree(
   ROW**            row,                /**< pointer to LP row */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp                  /**< actual LP data */
   );

/** ensures, that column array of row can store at least num entries */
extern
RETCODE SCIProwEnsureSize(
   ROW*             row,                /**< LP row */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   int              num                 /**< minimum number of entries to store */
   );

/** increases usage counter of LP row */
extern
void SCIProwCapture(
   ROW*             row                 /**< LP row */
   );

/** decreases usage counter of LP row, and frees memory if necessary */
extern
RETCODE SCIProwRelease(
   ROW**            row,                /**< pointer to LP row */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp                  /**< actual LP data */
   );

/** locks an unmodifiable row, which forbids further changes */
extern
RETCODE SCIProwLock(
   ROW*             row                 /**< LP row */
   );

/** unlocks a lock of a row; a row with no sealed lock may be modified */
extern
RETCODE SCIProwUnlock(
   ROW*             row                 /**< LP row */
   );

/** sorts row entries by column index */
extern
void SCIProwSort(
   ROW*             row                 /**< row to be sorted */
   );

/** enables delaying of row sorting */
extern
void SCIProwDelaySort(
   ROW*             row                 /**< LP row */
   );

/** disables delaying of row sorting, sorts row and merges coefficients with equal columns */
extern
void SCIProwForceSort(
   ROW*             row,                /**< LP row */
   const SET*       set                 /**< global SCIP settings */
   );

/** adds a previously non existing coefficient to an LP row */
extern
RETCODE SCIProwAddCoeff(
   ROW*             row,                /**< LP row */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp,                 /**< actual LP data */
   COL*             col,                /**< LP column */
   Real             val                 /**< value of coefficient */
   );

/** deletes coefficient from row */
extern
RETCODE SCIProwDelCoeff(
   ROW*             row,                /**< row to be changed */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp,                 /**< actual LP data */
   COL*             col                 /**< coefficient to be deleted */
   );

/** changes or adds a coefficient to an LP row */
extern
RETCODE SCIProwChgCoeff(
   ROW*             row,                /**< LP row */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp,                 /**< actual LP data */
   COL*             col,                /**< LP column */
   Real             val                 /**< value of coefficient */
   );

/** increases value of an existing or nonexisting coefficient in an LP column */
extern
RETCODE SCIProwIncCoeff(
   ROW*             row,                /**< LP row */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp,                 /**< actual LP data */
   COL*             col,                /**< LP column */
   Real             incval              /**< value to add to the coefficient */
   );

/** changes constant value of a row */
extern
RETCODE SCIProwChgConstant(
   ROW*             row,                /**< LP row */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat,               /**< problem statistics */
   LP*              lp,                 /**< actual LP data */
   Real             constant            /**< new constant value */
   );

/** add constant value to a row */
extern
RETCODE SCIProwAddConstant(
   ROW*             row,                /**< LP row */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat,               /**< problem statistics */
   LP*              lp,                 /**< actual LP data */
   Real             addval              /**< constant value to add to the row */
   );

/** changes left hand side of LP row */
extern
RETCODE SCIProwChgLhs(
   ROW*             row,                /**< LP row */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp,                 /**< actual LP data */
   Real             lhs                 /**< new left hand side */
   );

/** changes right hand side of LP row */
extern
RETCODE SCIProwChgRhs(
   ROW*             row,                /**< LP row */
   const SET*       set,                /**< global SCIP settings */
   LP*              lp,                 /**< actual LP data */
   Real             rhs                 /**< new right hand side */
   );

/** tries to find a rational representation of the row and multiplies coefficients with common denominator */
extern
RETCODE SCIProwMakeRational(
   ROW*             row,                /**< LP row */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat,               /**< problem statistics */
   LP*              lp,                 /**< actual LP data */
   Longint          maxdnom,            /**< maximal denominator allowed in rational numbers */
   Bool*            success             /**< stores whether row could be made rational */
   );

/** returns the activity of a row in the actual LP solution */
extern
Real SCIProwGetLPActivity(
   ROW*             row,                /**< LP row */
   STAT*            stat                /**< problem statistics */
   );

/** returns the feasibility of a row in the actual LP solution */
extern
Real SCIProwGetLPFeasibility(
   ROW*             row,                /**< LP row */
   STAT*            stat                /**< problem statistics */
   );

/** returns the pseudo activity of a row in the actual pseudo solution */
extern
Real SCIProwGetPseudoActivity(
   ROW*             row,                /**< LP row */
   STAT*            stat                /**< problem statistics */
   );

/** returns the pseudo feasibility of a row in the actual pseudo solution */
extern
Real SCIProwGetPseudoFeasibility(
   ROW*             row,                /**< LP row */
   STAT*            stat                /**< problem statistics */
   );

/** returns the activity of a row for a given solution */
extern
RETCODE SCIProwGetSolActivity(
   ROW*             row,                /**< LP row */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat,               /**< problem statistics data */
   SOL*             sol,                /**< primal CIP solution */
   Real*            solactivity         /**< pointer to store the row's activity for the solution */
   );

/** returns the feasibility of a row for the given solution */
extern
RETCODE SCIProwGetSolFeasibility(
   ROW*             row,                /**< LP row */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat,               /**< problem statistics data */
   SOL*             sol,                /**< primal CIP solution */
   Real*            solfeasibility      /**< pointer to store the row's feasibility for the solution */
   );

/** returns the minimal activity of a row w.r.t. the column's bounds */
extern
Real SCIProwGetMinActivity(
   ROW*             row,                /**< LP row */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat                /**< problem statistics data */
   );

/** returns the maximal activity of a row w.r.t. the column's bounds */
extern
Real SCIProwGetMaxActivity(
   ROW*             row,                /**< LP row */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat                /**< problem statistics data */
   );

/** gets maximal absolute value of row vector coefficients */
extern
Real SCIProwGetMaxval(
   ROW*             row,                /**< LP row */
   const SET*       set                 /**< global SCIP settings */
   );

/** gets minimal absolute value of row vector's non-zero coefficients */
extern
Real SCIProwGetMinval(
   ROW*             row,                /**< LP row */
   const SET*       set                 /**< global SCIP settings */
   );

/** output row to file stream */
extern
void SCIProwPrint(
   ROW*             row,                /**< LP row */
   FILE*            file                /**< output file (or NULL for standard output) */
   );

#ifndef NDEBUG

/* In debug mode, the following methods are implemented as function calls to ensure
 * type validity.
 */

/** get number of nonzero entries in row vector */
extern
int SCIProwGetNNonz(
   ROW*             row                 /**< LP row */
   );

/** gets array with columns of nonzero entries */
extern
COL** SCIProwGetCols(
   ROW*             row                 /**< LP row */
   );

/** gets array with coefficients of nonzero entries */
extern
Real* SCIProwGetVals(
   ROW*             row                 /**< LP row */
   );

/** gets constant shift of row */
extern
Real SCIProwGetConstant(
   ROW*             row                 /**< LP row */
   );

/** get euclidean norm of row vector */
extern
Real SCIProwGetNorm(
   ROW*             row                 /**< LP row */
   );

/** returns the left hand side of the row */
extern
Real SCIProwGetLhs(
   ROW*             row                 /**< LP row */
   );

/** returns the right hand side of the row */
extern
Real SCIProwGetRhs(
   ROW*             row                 /**< LP row */
   );

/** returns the name of the row */
extern
const char* SCIProwGetName(
   ROW*             row                 /**< LP row */
   );

/** gets unique index of row */
extern
int SCIProwGetIndex(
   ROW*             row                 /**< LP row */
   );

/** returns TRUE iff row is only valid locally */
extern
Bool SCIProwIsLocal(
   ROW*             row                 /**< LP row */
   );

/** gets position of row in actual LP, or -1 if it is not in LP */
extern
int SCIProwGetLPPos(
   ROW*             row                 /**< LP row */
   );

/** returns TRUE iff row is member of actual LP */
extern
Bool SCIProwIsInLP(
   ROW*             row                 /**< LP row */
   );

#else

/* In optimized mode, the methods are implemented as defines to reduce the number of function calls and
 * speed up the algorithms.
 */

#define SCIProwGetNNonz(row)            (row->len)
#define SCIProwGetCols(row)             (row->cols)
#define SCIProwGetVals(row)             (row->vals)
#define SCIProwGetConstant(row)         (row->constant)
#define SCIProwGetNorm(row)             (sqrt(row->sqrnorm))
#define SCIProwGetLhs(row)              (row->lhs)
#define SCIProwGetRhs(row)              (row->rhs)
#define SCIProwGetName(row)             (row->name)
#define SCIProwGetIndex(row)            (row->index)
#define SCIProwIsLocal(row)             (row->local)
#define SCIProwGetLPPos(row)            (row->lppos)
#define SCIProwIsInLP(row)              (row->lppos >= 0)

#endif




/*
 * LP methods
 */

/** creates empty LP data object */
extern
RETCODE SCIPlpCreate(
   LP**             lp,                 /**< pointer to LP data object */
   const SET*       set,                /**< global SCIP settings */
   const char*      name                /**< problem name */
   );

/** frees LP data object */
extern
RETCODE SCIPlpFree(
   LP**             lp,                 /**< pointer to LP data object */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set                 /**< global SCIP settings */
   );

/** adds a column to the LP and captures the variable */
extern
RETCODE SCIPlpAddCol(
   LP*              lp,                 /**< LP data */
   const SET*       set,                /**< global SCIP settings */
   COL*             col                 /**< LP column */
   );

/** adds a row to the LP and captures it */
extern
RETCODE SCIPlpAddRow(
   LP*              lp,                 /**< LP data */
   const SET*       set,                /**< global SCIP settings */
   ROW*             row                 /**< LP row */
   );

/** removes all columns after the given number of columns from the LP */
extern
RETCODE SCIPlpShrinkCols(
   LP*              lp,                 /**< LP data */
   int              newncols            /**< new number of columns in the LP */
   );

/** removes and releases all rows after the given number of rows from the LP */
extern
RETCODE SCIPlpShrinkRows(
   LP*              lp,                 /**< LP data */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   int              newnrows            /**< new number of rows in the LP */
   );

/** removes all columns and rows from LP, releases all rows */
extern
RETCODE SCIPlpClear(
   LP*              lp,                 /**< LP data */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set                 /**< global SCIP settings */
   );

/** remembers number of columns and rows to track the newly added ones */
extern
void SCIPlpMarkSize(
   LP*              lp                  /**< actual LP data */
   );

/** get array with newly added columns after the last mark */
extern
COL** SCIPlpGetNewcols(
   LP*              lp                  /**< actual LP data */
   );

/** get number of newly added columns after the last mark */
extern
int SCIPlpGetNumNewcols(
   LP*              lp                  /**< actual LP data */
   );

/** get array with newly added rows after the last mark */
extern
ROW** SCIPlpGetNewrows(
   LP*              lp                  /**< actual LP data */
   );

/** get number of newly added rows after the last mark */
extern
int SCIPlpGetNumNewrows(
   LP*              lp                  /**< actual LP data */
   );

/** gets all indices of basic columns and rows: index i >= 0 corresponds to column i, index i < 0 to row -i-1 */
extern
RETCODE SCIPlpGetBasisInd(
   LP*              lp,                 /**< LP data */
   int*             basisind            /**< pointer to store the basis indices */
   );

/** gets actual basis status for columns and rows; arrays must be large enough to store the basis status */
extern
RETCODE SCIPlpGetBase(
   LP*              lp,                 /**< LP data */
   int*             cstat,              /**< array to store column basis status, or NULL */
   int*             rstat               /**< array to store row basis status, or NULL */
   );

/** gets a row from the inverse basis matrix B^-1 */
extern
RETCODE SCIPlpGetBInvRow(
   LP*              lp,                 /**< LP data */
   int              r,                  /**< row number */
   Real*            coef                /**< pointer to store the coefficients of the row */
   );

/** gets a row from the product of inverse basis matrix B^-1 and coefficient matrix A (i.e. from B^-1 * A) */
extern
RETCODE SCIPlpGetBInvARow(
   LP*              lp,                 /**< LP data */
   int              r,                  /**< row number */
   Real*            binvrow,            /**< row in B^-1 from prior call to SCIPlpGetBInvRow(), or NULL */
   Real*            coef                /**< pointer to store the coefficients of the row */
   );

/** calculates a weighted sum of all LP rows; for negative weights, the left and right hand side of the corresponding
 *  LP row are swapped in the summation
 */
extern
RETCODE SCIPlpSumRows(
   LP*              lp,                 /**< LP data */
   const SET*       set,                /**< global SCIP settings */
   int              nvars,              /**< number of active variables in the problem */
   Real*            weights,            /**< row weights in row summation */
   REALARRAY*       sumcoef,            /**< array to store sum coefficients indexed by variables' probindex */
   Real*            sumlhs,             /**< pointer to store the left hand side of the row summation */
   Real*            sumrhs              /**< pointer to store the right hand side of the row summation */
   );

/* calculates a MIR cut out of the weighted sum of LP rows; The weights of modifiable rows are set to 0.0, because these
 * rows cannot participate in a MIR cut.
 */
extern
RETCODE SCIPlpCalcMIR(
   LP*              lp,                 /**< LP data */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat,               /**< problem statistics */
   int              nvars,              /**< number of active variables in the problem */
   VAR**            vars,               /**< active variables in the problem */
   Real             minfrac,            /**< minimal fractionality of rhs to produce MIR cut for */
   Real*            weights,            /**< row weights in row summation */
   Real*            mircoef,            /**< array to store MIR coefficients: must be of size nvars */
   Real*            mirrhs,             /**< pointer to store the right hand side of the MIR row */
   Bool*            success             /**< pointer to store whether the returned coefficients are a valid MIR cut */
   );

/** stores LP state (like basis information) into LP state object */
extern
RETCODE SCIPlpGetState(
   LP*              lp,                 /**< LP data */
   MEMHDR*          memhdr,             /**< block memory */
   LPISTATE**       lpistate            /**< pointer to LP state information (like basis information) */
   );

/** loads LP state (like basis information) into solver */
extern
RETCODE SCIPlpSetState(
   LP*              lp,                 /**< LP data */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   LPISTATE*        lpistate            /**< LP state information (like basis information) */
   );

/** sets the feasibility tolerance of the LP solver */
extern
RETCODE SCIPlpSetFeastol(
   LP*              lp,                 /**< actual LP data */
   Real             feastol             /**< new feasibility tolerance */
   );

/** sets the upper objective limit of the LP solver */
extern
RETCODE SCIPlpSetUpperbound(
   LP*              lp,                 /**< actual LP data */
   Real             upperbound          /**< new upper objective limit */
   );

/** solves the LP with the primal or dual simplex algorithm, depending on the current basis feasibility */
extern
RETCODE SCIPlpSolve(
   LP*              lp,                 /**< actual LP data */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat                /**< problem statistics */
   );

/** solves the LP with simplex algorithm, and copy the solution into the column's data */
extern
RETCODE SCIPlpSolveAndEval(
   LP*              lp,                 /**< LP data */
   MEMHDR*          memhdr,             /**< block memory buffers */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat,               /**< problem statistics */
   PROB*            prob,               /**< problem data */
   Bool             aging               /**< should aging and removal of obsolete cols/rows be applied? */
   );

/** gets solution status of last solve call */
extern
LPSOLSTAT SCIPlpGetSolstat(
   LP*              lp                  /**< actual LP data */
   );

/** gets objective value of last solution */
extern
Real SCIPlpGetObjval(
   LP*              lp                  /**< actual LP data */
   );

/** stores the LP solution in the columns and rows */
extern
RETCODE SCIPlpGetSol(
   LP*              lp,                 /**< actual LP data */
   MEMHDR*          memhdr,             /**< block memory buffers */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat,               /**< problem statistics */
   Bool*            infeasible          /**< pointer to store whether the solution is primal infeasible, or NULL */
   );

/** stores LP solution with infinite objective value in the columns and rows */
extern
RETCODE SCIPlpGetUnboundedSol(
   LP*              lp,                 /**< actual LP data */
   MEMHDR*          memhdr,             /**< block memory buffers */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat                /**< problem statistics */
   );

/** stores the dual farkas multipliers for infeasibility proof in rows */
extern
RETCODE SCIPlpGetDualfarkas(
   LP*              lp,                 /**< actual LP data */
   MEMHDR*          memhdr,             /**< block memory buffers */
   const SET*       set                 /**< global SCIP settings */
   );

/** get number of iterations used in last LP solve */
extern
RETCODE SCIPlpGetIterations(
   LP*              lp,                 /**< actual LP data */
   int*             iterations          /**< pointer to store the iteration count */
   );

/** increases age of columns with solution value 0.0 and rows with activity not at its bounds,
 *  resets age of non-zero columns and sharp rows
 */
extern
RETCODE SCIPlpUpdateAges(
   LP*              lp,                 /**< actual LP data */
   const SET*       set                 /**< global SCIP settings */
   );

/** removes all columns and rows in the part of the LP created at the current node, that are too old */
extern
RETCODE SCIPlpRemoveNewObsoletes(
   LP*              lp,                 /**< actual LP data */
   MEMHDR*          memhdr,             /**< block memory buffers */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat                /**< problem statistics */
   );

/** removes all columns and rows in whole LP, that are too old */
extern
RETCODE SCIPlpRemoveAllObsoletes(
   LP*              lp,                 /**< actual LP data */
   MEMHDR*          memhdr,             /**< block memory buffers */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat                /**< problem statistics */
   );

/** removes all columns at 0.0 and rows not at their bound in the part of the LP created at the current node */
extern
RETCODE SCIPlpCleanupNew(
   LP*              lp,                 /**< actual LP data */
   MEMHDR*          memhdr,             /**< block memory buffers */
   const SET*       set                 /**< global SCIP settings */
   );

/** removes all columns at 0.0 and rows not at their bound in the whole LP */
extern
RETCODE SCIPlpCleanupAll(
   LP*              lp,                 /**< actual LP data */
   MEMHDR*          memhdr,             /**< block memory buffers */
   const SET*       set                 /**< global SCIP settings */
   );

/** initiates LP diving */
extern
RETCODE SCIPlpStartDive(
   LP*              lp,                 /**< actual LP data */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set                 /**< global SCIP settings */
   );

/** quits LP diving and resets bounds and objective values of columns to the actual node's values */
extern
RETCODE SCIPlpEndDive(
   LP*              lp,                 /**< actual LP data */
   MEMHDR*          memhdr,             /**< block memory */
   const SET*       set,                /**< global SCIP settings */
   STAT*            stat,               /**< problem statistics */
   PROB*            prob,               /**< problem data */
   VAR**            vars,               /**< array with all active variables */
   int              nvars               /**< number of active variables */
   );

/** writes LP to a file */
extern 
RETCODE SCIPlpWrite(
   LP*              lp,                 /**< actual LP data */
   const char*      fname               /**< file name */
   );

#endif
