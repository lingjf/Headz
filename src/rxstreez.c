#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "rxstreez.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rxstree_node
{
   char *pattern;
   void *payload;
   struct rxstree_node **subnode;
} rxstree_node;

typedef struct rxstree_tree
{
   int unique;
   struct rxstree_node *root[256];
} rxstree_tree;

void *rxstree_create(int unique)
{
   rxstree_tree *tree;
   assert(unique == 0 || unique == !0);
   tree = (rxstree_tree*) malloc(sizeof(rxstree_tree));
   if (tree) {
      memset(tree, 0, sizeof(rxstree_tree));
      tree->unique = unique;
   }
   return tree;
}

static void __free_node(rxstree_node *node)
{
   assert(node);
   if (node) {
      if (node->subnode)
         free(node->subnode);
      if (node->pattern)
         free(node->pattern);
      free(node);
   }
}

void __destroy_node(rxstree_node *node)
{
   assert(&node);
   if (node) {
      if (node->subnode) {
         int i;
         for (i = 0; i < 256; i++)
            __destroy_node(node->subnode[i]);
      }
      __free_node(node);
   }
}

void rxstree_destroy(void *handle)
{
   int i;
   rxstree_tree *tree = (rxstree_tree *) handle;
   assert(handle);
   if (tree) {
      for (i = 0; i < 256; i++)
         __destroy_node(tree->root[i]);
      free(tree);
   }
}

rxstree_node *__allocate_node(int pattern_len, int subnode_len)
{
   rxstree_node *node;
   assert(&pattern_len);
   assert(&subnode_len);
   node = (rxstree_node*) malloc(sizeof(rxstree_node));
   if (!node)
      goto failure;
   memset(node, 0, sizeof(rxstree_node));

   node->pattern = (char*) malloc(pattern_len);
   if (!node->pattern)
      goto failure;
   memset(node->pattern, 0, pattern_len);

   node->payload = NULL;
   if (subnode_len) {
      node->subnode = (rxstree_node**) malloc(sizeof(rxstree_node*) * 256);
      if (!node->subnode)
         goto failure;
      memset(node->subnode, 0, sizeof(rxstree_node*) * 256);
   } else {
      node->subnode = NULL;

   }
   return node;

   failure: if (node) {
      if (node->subnode)
         free(node->subnode);
      if (node->pattern)
         free(node->pattern);
      free(node);
      node = NULL;
   }
   return node;
}

/**
 case 1 : the first string which start with index[0]
 case 2 : equal node
 case 3 : in the node
 case 4 : fork the node
 case 5 : exceed the node
 */
rxstree_node **__recent_node(rxstree_tree *tree, char *idx)
{
   rxstree_node **pp;
   assert(tree && idx);
   pp = &tree->root[(unsigned char) idx[0]];
   while (*pp) {
      int i;
      rxstree_node *node = *pp;
      for (i = 0; idx[i] && node->pattern[i] == idx[i]; i++)
         ;
      switch (((!!idx[i]) << 1) + (!!node->pattern[i])) {
      case 0x00:
         return pp; /*case2*/
      case 0x01:
         return pp; /*case3*/
      case 0x02:
         if (node->subnode) {
            pp = &node->subnode[(unsigned char) *(idx += i)];
            continue;
         } else {
            return pp;
         }/*case5*/
      case 0x03:
         return pp; /*case4*/
      }
   }
   return pp;
}

void *rxstree_insert(void *handle, char *idx, void *usr)
{
   rxstree_tree *tree;
   rxstree_node **pp;
   assert(handle && idx);
   assert(&usr);
   tree = (rxstree_tree*) handle;
   pp = &tree->root[(unsigned char) idx[0]];
   while (*pp) {
      int i;
      rxstree_node *node = *pp;
      for (i = 0; idx[i] && node->pattern[i] == idx[i]; i++)
         ;
      switch (((!!idx[i]) << 1) + (!!node->pattern[i])) {
      case 0x00: /* abc --> abc */
         return NULL;
      case 0x01: {/* ab --> abc */
         rxstree_node *new_node = __allocate_node(i + 2, 256);
         if (!new_node)
            return NULL;
         strncpy(new_node->pattern, node->pattern, i);
         new_node->payload = usr;
         new_node->subnode[(unsigned char) node->pattern[i]] = node;
         *pp = new_node;
         return new_node->payload;
      }
         break;
      case 0x02: /* abc --> ab */
         if (!node->subnode) {
            node->subnode = (rxstree_node**) malloc(sizeof(rxstree_node*) * 256);
            if (!node->subnode)
               return NULL;
            memset(node->subnode, 0, sizeof(rxstree_node*) * 256);
         }
         pp = &node->subnode[(unsigned char) *(idx += i)];
         break;
      case 0x03: { /* abc --> abd */
         rxstree_node *new_node = __allocate_node(i + 2, 256);
         if (!new_node)
            return NULL;
         strncpy(new_node->pattern, node->pattern, i);
         new_node->subnode[(unsigned char) node->pattern[i]] = node;
         strcpy(node->pattern, node->pattern + i);
         *pp = new_node;
         pp = &new_node->subnode[(unsigned char) *(idx += i)];
      }
         break;
      }
   }

   if (!*pp) {
      rxstree_node *new_node = __allocate_node(strlen(idx) + 2, 0);
      if (!new_node)
         return NULL;
      strcpy(new_node->pattern, idx);
      new_node->payload = usr;
      *pp = new_node;
   }
   return usr;
}

void *__lookup_node(rxstree_tree *tree, char* idx)
{
   rxstree_node **pp;
   assert(tree && idx);
   pp = &tree->root[(unsigned char) idx[0]];
   while (*pp) {
      int i;
      rxstree_node *node = *pp;
      for (i = 0; idx[i] && node->pattern[i] == idx[i]; i++)
         ;
      switch (((!!idx[i]) << 1) + (!!node->pattern[i])) {
      case 0x00:
         return node;
      case 0x01:
         return NULL;
      case 0x02:
         if (node->subnode) {
            pp = &node->subnode[(unsigned char) *(idx += i)];
            continue;
         } else {
            return NULL;
         }
      case 0x03:
         return NULL;
      }
   }
   return NULL;
}

void *rxstree_lookup(void *handle, char *idx)
{
   rxstree_tree *tree;
   rxstree_node *node;

   assert(handle && idx);
   tree = (rxstree_tree *) handle;
   node = (rxstree_node *) __lookup_node(tree, idx);
   if (node)
      return node->payload;
   return NULL;
}

void __reduce_node(rxstree_node **pp)
{
   rxstree_node *node;
   assert(pp);
   node = *pp;
   if (node) {
      int s = 0;
      rxstree_node *sub = NULL;
      if (node->subnode) {
         int i;
         for (i = 0; i < 256; i++)
            if (node->subnode[i])
               __reduce_node(&node->subnode[i]);
         for (i = 0; i < 256; i++)
            if (node->subnode[i]) {
               s++;
               sub = node->subnode[i];
            }
         if (s == 0) {
            free(node->subnode);
            node->subnode = NULL;
         }
      }
      if (node->payload == NULL) {
         if (s == 0) {
            *pp = NULL;
         } else if (s == 1) {
            char *new_pattern = (char*) malloc(strlen(node->pattern) + strlen(sub->pattern) + 2);
            sprintf(new_pattern, "%s%s", node->pattern, sub->pattern);
            free(sub->pattern);
            sub->pattern = new_pattern;
            *pp = sub;
         }
         __free_node(node);
      }

   }
}

void *rxstree_remove(void *handle, char *idx)
{
   void *usr = NULL;
   rxstree_tree *tree;
   rxstree_node *node;
   assert(handle && idx);
   tree = (rxstree_tree *) handle;
   node = (rxstree_node *) __lookup_node(tree, idx);
   if (node) {
      usr = node->payload;
      node->payload = NULL;
      __reduce_node(&tree->root[(unsigned char) idx[0]]);
   }
   return usr;
}

/*---------------------------------------------------------------*/

int ____verify_rxstree(rxstree_node *node, char *prev)
{
   int i;
   char string[246];
   assert(&node || &prev);
   if (!node)
      return 1;
   sprintf(string, "%s%s", prev, node->pattern);
   if (node->payload) {
      if (strcmp(string, (char*) node->payload))
         return 0;
   }

   if (node->subnode) {
      for (i = 0; i < 256; i++) {
         rxstree_node *n2 = node->subnode[i];
         if (n2) {
            if (n2->pattern) {
               int j = (int) (n2->pattern[0]);
               if (j != i)
                  return 0;
            }
            if (!____verify_rxstree(n2, string))
               return 0;
         }
      }
   }
   return 1;
}

int ____helper__verify_rxstree(void *handle)
{
   int i;
   rxstree_tree *tree;
   assert(handle);
   tree = (rxstree_tree*) handle;
   for (i = 0; i < 256; i++) {
      rxstree_node *node = tree->root[i];
      if (node) {
         if (node->pattern) {
            int j = (int) (node->pattern[0]);
            if (j != i)
               return 0;
         }
         if (!____verify_rxstree(node, ""))
            return 0;
      }
   }
   return 1;
}

#ifdef __cplusplus
}
#endif

