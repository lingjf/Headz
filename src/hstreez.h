#ifndef __BINTREEZ_H_a2910d55_63a7_4eef_a0cc_be5475528533__
#define __BINTREEZ_H_a2910d55_63a7_4eef_a0cc_be5475528533__

#ifdef __cplusplus
extern "C" {
#endif

#include "listz.h"

typedef unsigned long (*hstree_hash_key)(list_head *);
typedef unsigned long (*hstree_hash_value)(list_head *);
typedef int (*hstree_cmp_key)(list_head *, list_head *);
typedef int (*hstree_visitor)(void *args, unsigned long startkey, unsigned long endkey, unsigned long hash,
                              list_head *items);

typedef struct hstree_node
{
   unsigned long count;
   unsigned long hash;
   union
   {
      struct hstree_node **childs;
      list_head heads;
   };
} hstree_node;

typedef struct hstree_tree
{
   int width;
   int height;
   hstree_hash_key hashkey;
   hstree_hash_value hashvalue;
   hstree_cmp_key cmpkey;
   hstree_node root;
   int nodes;
   int items;
} hstree_tree;

hstree_tree *hstree_create(hstree_tree *tree, int width, hstree_hash_key hashkey, hstree_hash_value hashvalue,
                           hstree_cmp_key cmpkey);
void hstree_destroy(hstree_tree *tree);
void hstree_insert(hstree_tree *tree, list_head *item);
void hstree_remove(hstree_tree *tree, list_head *item);
unsigned long hstree_gethash(hstree_tree *tree, unsigned long startkey, unsigned long endkey);
void hstree_synchronize(hstree_tree *tree, hstree_visitor visitor, void *args);

char *hstree_tostring(hstree_tree *tree, int detail);

#ifdef __cplusplus
}
#endif

#endif

