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
#pragma ident "@(#) $Id: heur_fracdiving.h,v 1.16 2011/01/02 11:10:47 bzfheinz Exp $"

/**@file   heur_fracdiving.h
 * @brief  LP diving heuristic that chooses fixings w.r.t. the fractionalities
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_HEUR_FRACDIVING_H__
#define __SCIP_HEUR_FRACDIVING_H__


#include "scip/scip.h"

#ifdef __cplusplus
extern "C" {
#endif

/** creates the fracdiving heuristic and includes it in SCIP */
extern
SCIP_RETCODE SCIPincludeHeurFracdiving(
   SCIP*                 scip                /**< SCIP data structure */
   );

#ifdef __cplusplus
}
#endif

#endif
