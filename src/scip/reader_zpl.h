/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2012 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   reader_zpl.h
 * @ingroup FILEREADERS
 * @brief  ZIMPL model file reader
 * @author Tobias Achterberg
 *
 * This reader allows to read input in <a href="http://zimpl.zib.de">ZIMPL</a> format. See the web
 * page for a definition and documentation.
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_READER_ZPL_H__
#define __SCIP_READER_ZPL_H__


#include "scip/scip.h"

#ifdef __cplusplus
extern "C" {
#endif

/** includes the zpl file reader into SCIP */
extern
SCIP_RETCODE SCIPincludeReaderZpl(
   SCIP*                 scip                /**< SCIP data structure */
   );

#ifdef __cplusplus
}
#endif

#endif
