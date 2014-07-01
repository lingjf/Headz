#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "bintreez.h"

#ifdef __cplusplus
extern "C" {
#endif

#define null_head ((bintree_head*)0)

void bintree_create(bintree_tree *tree, bintree_key *key, bintree_cmp *cmp, int unique)
{
   assert(tree);
   assert(&key || &cmp);
   assert(unique == 0 || unique == !0);

   if (tree) {
      tree->root = null_head;
      tree->key = key;
      tree->cmp = cmp;
      tree->unique = unique;
   }
}

void bintree_destroy(bintree_tree *tree)
{
   assert(&tree);
}

void bintree_init(bintree_head *head)
{
   assert(head);

   if (head) {
      head->parent = null_head;
      head->left = head->right = null_head;
   }
}

#define __BINTREE_KEY(tree, node) \
   ((tree)->key?(tree)->key(node):(unsigned long)(node))

#define __BINTREE_CMP(tree, key1, key2) \
   ((tree)->cmp?(tree)->cmp(key1,key2):(int)((key1)-(key2)))

bintree_head * bintree_insert(bintree_tree *tree, bintree_head *node)
{
   unsigned long key;
   bintree_head **pchild, *parent;
   assert(&tree || &node);

   if (!tree || !node)
      return null_head;

   bintree_init(node);

   key = __BINTREE_KEY(tree, node);
   parent = tree->root;
   pchild = &tree->root;
   while (*pchild) {
      int __cmp = __BINTREE_CMP(tree, key, __BINTREE_KEY(tree, *pchild));
      if (__cmp == 0 && tree->unique)
         return null_head;
      if (__cmp <= 0) {
         parent = *pchild;
         pchild = &(*pchild)->left;
      } else {
         parent = *pchild;
         pchild = &(*pchild)->right;
      }
   }
   *pchild = node;
   node->parent = parent;
   return node;
}

static bintree_head * __leftmost(bintree_head *node)
{
   assert(&node);
   if (node)
      while (node->left)
         node = node->left;
   return node;
}

static bintree_head * __rightmost(bintree_head *node)
{
   assert(&node);
   if (node)
      while (node->right)
         node = node->right;
   return node;
}

bintree_head * bintree_remove(bintree_tree *tree, bintree_head *node)
{
   bintree_head **pchild, *parent;
   assert(&tree || &node);
   if (!tree || !node)
      return null_head;

   parent = node->parent;
   if (parent == NULL) {
      if (tree->root == node)
         pchild = &tree->root;
      else
         return NULL;
   } else {
      if (parent->left == node) {
         pchild = &parent->left;
      } else if (parent->right == node) {
         pchild = &parent->right;
      } else {
         return NULL;
      }
   }
   if (!node->left && !node->right) {
      *pchild = NULL;
   } else if (node->left && !node->right) {
      *pchild = node->left;
      node->left->parent = parent;
   } else if (node->right && !node->left) {
      *pchild = node->right;
      node->right->parent = parent;
   } else {
      bintree_head *left_max = bintree_remove(tree, __rightmost(node->left));
      *pchild = left_max;
      left_max->parent = parent;
      left_max->left = node->left;
      left_max->right = node->right;
   }
   return node;
}

bintree_head * bintree_lookup(bintree_tree *tree, unsigned long key)
{
   bintree_head *p;
   assert(&tree || &key);

   if (!tree)
      return null_head;

   p = tree->root;
   while (p) {
      int __cmp = __BINTREE_CMP(tree, key, __BINTREE_KEY(tree, p));

      if (__cmp == 0)
         return p;
      else if (__cmp < 0)
         p = p->left;
      else
         p = p->right;
   }
   return p;
}

bintree_head * bintree_min(bintree_tree *tree)
{
   assert(tree);
   return __leftmost(tree->root);
}

bintree_head * bintree_max(bintree_tree *tree)
{
   assert(tree);
   return __rightmost(tree->root);
}

bintree_head * bintree_inc(bintree_head *node)
{
   assert(&node);

   if (!node)
      return null_head;

   if (node->right) {
      return __leftmost(node->right);
   } else {
      bintree_head * parent = node->parent;
      while (parent && node != parent->left) {
         node = parent;
         parent = parent->parent;
      }
      return parent;
   }
}

bintree_head * bintree_dec(bintree_head *node)
{
   assert(&node);

   if (!node)
      return null_head;

   if (node->left) {
      return __rightmost(node->left);
   } else {
      bintree_head * parent = node->parent;
      while (parent && node != parent->right) {
         node = parent;
         parent = parent->parent;
      }
      return parent;
   }
}

/*---------------------------------------------------------------*/

bintree_head * bintree_root(bintree_head *node)
{
   assert(&node);

   while (node && node->parent)
      node = node->parent;
   return node;
}

int bintree_empty(bintree_tree *tree)
{
   assert(&tree);

   return (!((tree) && (tree)->root));
}

int bintree_leaf(bintree_head *node)
{
   assert(&node);
   if (!node)
      return 0;
   if (node->left || node->right)
      return 0;
   return 1;
}

int bintree_count(bintree_head *node)
{
   assert(&node);
   if (!node)
      return 0;
   return bintree_count(node->left) + bintree_count(node->right) + 1;
}

/*---------------------------------------------------------------*/

static int ____verify_bintree(bintree_tree *tree, bintree_head *p)
{
   int result;
   assert(&tree || &p);
   if (p == NULL) {
      return 1;
   }

   if (p->left) {
      int left_cmp = __BINTREE_CMP(tree,__BINTREE_KEY(tree,p->left), __BINTREE_KEY(tree,p));
      if (left_cmp > 0)
         return 0;
      if (tree->unique && left_cmp == 0)
         return 0;
   }

   if (p->right) {
      int right_cmp = __BINTREE_CMP(tree,__BINTREE_KEY(tree,p), __BINTREE_KEY(tree,p->right));
      if (right_cmp > 0)
         return 0;
      if (tree->unique && right_cmp == 0)
         return 0;
   }

   result = ____verify_bintree(tree, p->left);
   if (!result)
      return result;

   result = ____verify_bintree(tree, p->right);
   if (!result)
      return result;

   return 1;
}

int ____helper__verify_bintree(bintree_tree *tree)
{
   assert(&tree);
   if (tree)
      return ____verify_bintree(tree, tree->root);
   return 1;
}

void ____print_bintree(bintree_tree *tree, bintree_head *p)
{
   static char linke[] = { '|', '/', '\\', '<' };
   static int depth = 0;
   static int nline = 1;
   static int bklen = 0;
   assert(&tree || &p);
   depth++;
   if (p) {
      if (nline) {
         int i;
         printf("\r");
         for (i = 0; i < (depth - 1) * bklen; i++)
            printf(" ");
      }
      bklen = printf("%02X%c ", (int) tree->key(p), linke[((!!p->left) << 1) | (!!p->right)]);
      nline = 0;
      ____print_bintree(tree, p->right);
      ____print_bintree(tree, p->left);
   } else {
      if (!nline)
         printf("\n");
      nline = 1;
   }
   depth--;
}

void ____helper__print_bintree(bintree_tree *tree)
{
   assert(&tree);
   if (tree) {
      printf("\n");
      ____print_bintree(tree, tree->root);
      printf("\n");
   }
}

#ifdef __cplusplus
}
#endif

