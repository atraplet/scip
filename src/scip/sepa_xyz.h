/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2018 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   sepa_xyz.h
 * @ingroup SEPARATORS
 * @brief  xyz separator
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_SEPA_XYZ_H__
#define __SCIP_SEPA_XYZ_H__


#include "scip/def.h"
#include "scip/type_retcode.h"
#include "scip/def.h"
#include "scip/type_retcode.h"
#include "scip/type_scip.h"

#ifdef __cplusplus
extern "C" {
#endif

/** creates the xyz separator and includes it in SCIP
 *
 * @ingroup SeparatorIncludes
 */
EXTERN
SCIP_RETCODE SCIPincludeSepaXyz(
   SCIP*                 scip                /**< SCIP data structure */
   );

/**@addtogroup SEPARATORS
 *
 * @{
 */

/* TODO place other public methods in this group to facilitate navigation through the documentation */

/* @} */

#ifdef __cplusplus
}
#endif

#endif
