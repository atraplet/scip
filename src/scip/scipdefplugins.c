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
#pragma ident "@(#) $Id: scipdefplugins.c,v 1.5 2003/11/21 10:35:40 bzfpfend Exp $"

/**@file   scipdefplugins.c
 * @brief  default SCIP plugins
 * @author Tobias Achterberg
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include "scipdefplugins.h"
#include "reader_cnf.h"
#include "reader_mps.h"
#include "cons_and.h"
#include "cons_binpack.h"
#include "cons_eqknapsack.h"
#include "cons_integral.h"
#include "cons_invarknapsack.h"
#include "cons_knapsack.h"
#include "cons_linear.h"
#include "cons_logicor.h"
#include "cons_setppc.h"
#include "cons_varlb.h"
#include "cons_varub.h"
#include "presol_dualfix.h"
#include "presol_trivial.h"
#include "nodesel_bfs.h"
#include "nodesel_dfs.h"
#include "nodesel_restartdfs.h"
#include "branch_fullstrong.h"
#include "branch_mostinf.h"
#include "branch_leastinf.h"
#include "heur_diving.h"
#include "heur_rounding.h"
#include "sepa_gomory.h"
#include "disp_default.h"
#include "dialog_default.h"




/** includes default SCIP plugins into SCIP */
RETCODE SCIPincludeDefaultPlugins(
   SCIP*            scip                /**< SCIP data structure */
   )
{
   CHECK_OKAY( SCIPincludeConshdlrAnd(scip) );
   CHECK_OKAY( SCIPincludeConshdlrIntegral(scip) );
   CHECK_OKAY( SCIPincludeConshdlrLinear(scip) );
   CHECK_OKAY( SCIPincludeConshdlrLogicor(scip) );
   CHECK_OKAY( SCIPincludeConshdlrSetppc(scip) );

#if 0
   CHECK_OKAY( SCIPincludeConshdlrKnapsack(scip) );
   CHECK_OKAY( SCIPincludeConshdlrEqknapsack(scip) );
   CHECK_OKAY( SCIPincludeConshdlrInvarknapsack(scip) );
   CHECK_OKAY( SCIPincludeConshdlrBinpack(scip) );
   CHECK_OKAY( SCIPincludeConshdlrVarlb(scip) );
   CHECK_OKAY( SCIPincludeConshdlrVarub(scip) );
#endif

   CHECK_OKAY( SCIPincludeReaderCnf(scip) );
   CHECK_OKAY( SCIPincludeReaderMps(scip) );
   CHECK_OKAY( SCIPincludePresolDualfix(scip) );
   CHECK_OKAY( SCIPincludePresolTrivial(scip) );
   CHECK_OKAY( SCIPincludeNodeselBfs(scip) );
   CHECK_OKAY( SCIPincludeNodeselDfs(scip) );
   CHECK_OKAY( SCIPincludeNodeselRestartdfs(scip) );
   CHECK_OKAY( SCIPincludeBranchruleFullstrong(scip) );
   CHECK_OKAY( SCIPincludeBranchruleMostinf(scip) );
   CHECK_OKAY( SCIPincludeBranchruleLeastinf(scip) );
   CHECK_OKAY( SCIPincludeHeurDiving(scip) );
   CHECK_OKAY( SCIPincludeHeurRounding(scip) );
   CHECK_OKAY( SCIPincludeSepaGomory(scip) );
   CHECK_OKAY( SCIPincludeDispDefault(scip) );
   CHECK_OKAY( SCIPincludeDialogDefault(scip) );

   return SCIP_OKAY;
}
