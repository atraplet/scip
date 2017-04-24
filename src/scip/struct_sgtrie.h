/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2016 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   struct_sgtrie.h
 * @brief  struct definitions for signature trie
 * @author Robert Lion Gottwald
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_STRUCT_SGTRIE_H__
#define __SCIP_STRUCT_SGTRIE_H__

#include "scip/type_sgtrie.h"
#include "scip/def.h"
#include "blockmemshell/memory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct InnerNodeData INNERNODEDATA;
typedef struct LeafNodeData LEAFNODEDATA;
typedef union NodeData NODEDATA;

struct SCIP_SgTrie
{
   int                   nelements;          /**< number of elements stored in the trie */
   SCIP_SGTRIENODE*      root;               /**< root node of the trie */
   BMS_BLKMEM*           blkmem;             /**< blockmemory used for trie nodes */
   BMS_BUFMEM*           bufmem;             /**< buffer memory for maintaing a stack of nodes in internal functions */
   SCIP_DECL_ISSETEQ     ((*seteq));         /**< call back for comparing sets for equality */
   SCIP_DECL_ISSUBSET    ((*subset));        /**< call back for checking if a set is a subset of another set */
   SCIP_DECL_GETSIGNATURE ((*getsignature)); /**< call back for computing signature of set */
};

struct InnerNodeData
{
   SCIP_SGTRIENODE*      left;
   SCIP_SGTRIENODE*      right;
};

struct LeafNodeData
{
   void*                 element;
   LEAFNODEDATA*         next;
};

union NodeData
{
   INNERNODEDATA         inner;
   LEAFNODEDATA          leaf;
};

struct SCIP_SgTrieNode
{
   NODEDATA              data;
   uint64_t              prefix;
   uint64_t              mask;
};

#ifdef __cplusplus
}
#endif

#endif
