#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <assert.h>

#include "treapz.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define null_head ((treap_head*)0)

/*
 *
 *            T                            T
 *            |                            |
 *            y       <- left              x
 *          /   \                        /   \
 *        x      C     right ->         A     y
 *       /  \                                / \
 *      A    B                              B   C
 *
 * 
 */

static void __rotate_left(treap_head **pp)
{
   treap_head *x, *y, *B, *T;
   assert(pp);
   if (*pp) {
      x = *pp;
      T = x->parent;
      y = x->right;
      if (y) {
         y->parent = T;
         *pp = y;
         B = y->left;
         y->left = x;
         x->parent = y;
         x->right = B;
         if (B)
            B->parent = x;
      }
   }
}

static void __rotate_right(treap_head **pp)
{
   treap_head *x, *y, *B, *T;
   assert(pp);
   if (*pp) {
      y = *pp;
      T = y->parent;
      x = y->left;
      if (x) {
         x->parent = T;
         *pp = x;
         B = x->right;
         x->right = y;
         y->parent = x;
         y->left = B;
         if (B)
            B->parent = y;
      }
   }
}

void treap_create(treap_tree *tree, treap_key *key, treap_cmp *cmp, int unique)
{
   assert(tree);
   assert(&key || &cmp);
   assert(unique == 0 || unique == !0);

   tree->root = null_head;
   tree->key = key;
   tree->cmp = cmp;
   tree->unique = unique;
   srand(time(0));
}

void treap_destroy(treap_tree *tree)
{
   assert(tree);
}

void treap_init(treap_head *head)
{
   assert(head);
   if (head) {
      head->parent = null_head;
      head->left = head->right = null_head;
      head->priority = rand();
   }
}

#define __TREAP_KEY(tree, node) \
   ((tree)->key?(tree)->key(node):(unsigned long)(node))

#define __TREAP_CMP(tree, key1, key2) \
   ((tree)->cmp?(tree)->cmp(key1,key2):(int)((key1)-(key2)))

treap_head * treap_insert(treap_tree *tree, treap_head *node)
{
   unsigned long key;
   treap_head **pp, *parent;
   assert(tree && node);

   if (!tree || !node)
      return null_head;

   treap_init(node);

   key = __TREAP_KEY(tree, node);
   parent = tree->root;
   pp = &tree->root;
   while (*pp) {
      int __cmp = __TREAP_CMP(tree, key, __TREAP_KEY(tree, *pp));
      if (__cmp == 0 && tree->unique)
         return null_head;
      if (__cmp <= 0) {
         parent = *pp;
         pp = &(*pp)->left;
      } else {
         parent = *pp;
         pp = &(*pp)->right;
      }
   }
   *pp = node;
   node->parent = parent;

   while (1) {
      if (!*pp)
         return node;
      else if ((*pp)->left && (*pp)->left->priority > (*pp)->priority)
         __rotate_right(pp);
      else if ((*pp)->right && (*pp)->right->priority > (*pp)->priority)
         __rotate_left(pp);

      if (!(*pp)->parent)
         return node;
      else if (!(*pp)->parent->parent)
         pp = &tree->root;
      else if ((*pp)->parent == (*pp)->parent->parent->left)
         pp = &(*pp)->parent->parent->left;
      else if ((*pp)->parent == (*pp)->parent->parent->right)
         pp = &(*pp)->parent->parent->right;
   }
   return node;
}

static void __remove(treap_head **pp)
{
   assert(pp);
   while (1) {
      if (!pp || !*pp)
         return;
      else if (!(*pp)->left && !(*pp)->right) {
         *pp = (*pp)->parent = NULL;
      } else if (!(*pp)->left) {
         __rotate_left(pp);
         pp = &(*pp)->left;
      } else if (!(*pp)->right) {
         __rotate_right(pp);
         pp = &(*pp)->right;
      } else if ((*pp)->left->priority > (*pp)->right->priority) {
         __rotate_right(pp);
         pp = &(*pp)->right;
      } else {
         __rotate_left(pp);
         pp = &(*pp)->left;
      }
   }
}

treap_head * treap_remove(treap_tree *tree, treap_head *node)
{
   treap_head **pp;
   assert(tree && node);

   if (!tree || !node)
      return node;

   if (!node->parent)
      if (tree->root == node)
         pp = &tree->root;
      else
         return node;
   else {
      if (node->parent->left == node)
         pp = &node->parent->left;
      else if (node->parent->right == node)
         pp = &node->parent->right;
      else
         return node;
   }
   __remove(pp);
   return node;
}

treap_head * treap_lookup(treap_tree *tree, unsigned long key)
{
   treap_head *p;
   assert(tree);
   assert(&key);

   if (!tree)
      return null_head;

   p = tree->root;
   while (p) {
      int __cmp = __TREAP_CMP(tree, key, __TREAP_KEY(tree, p));

      if (__cmp == 0)
         return p;
      else if (__cmp < 0)
         p = p->left;
      else
         p = p->right;
   }
   return p;
}

static treap_head * __leftmost(treap_head *node)
{
   assert(node);
   if (node)
      while (node->left)
         node = node->left;
   return node;
}

static treap_head * __rightmost(treap_head *node)
{
   assert(node);
   if (node)
      while (node->right)
         node = node->right;
   return node;
}

treap_head * treap_min(treap_tree *tree)
{
   assert(tree);
   return __leftmost(tree->root);
}

treap_head * treap_max(treap_tree *tree)
{
   assert(tree);
   return __rightmost(tree->root);
}

treap_head * treap_inc(treap_head * node)
{
   assert(node);
   if (!node)
      return null_head;

   if (node->right) {
      return __leftmost(node->right);
   } else {
      treap_head * parent = node->parent;
      while (parent && node != parent->left) {
         node = parent;
         parent = parent->parent;
      }
      return parent;
   }
}

treap_head * treap_dec(treap_head * node)
{
   assert(node);
   if (!node)
      return null_head;

   if (node->left) {
      return __rightmost(node->left);
   } else {
      treap_head * parent = node->parent;
      while (parent && node != parent->right) {
         node = parent;
         parent = parent->parent;
      }
      return parent;
   }
}
/*---------------------------------------------------------------*/

treap_head * treap_root(treap_head *node)
{
   assert(node);
   while (node && node->parent)
      node = node->parent;
   return node;
}

int treap_count(treap_head *node)
{
   assert(node);
   if (!node)
      return 0;
   return treap_count(node->left) + treap_count(node->right) + 1;
}
/*---------------------------------------------------------------*/

static int ____verify_treap(treap_tree *tree, treap_head *p)
{
   int result;
   assert(&tree || &p);
   if (p == NULL) {
      return 1;
   }

   if (p->left) {
      int left_cmp = __TREAP_CMP(tree,__TREAP_KEY(tree,p->left), __TREAP_KEY(tree,p));
      if (left_cmp > 0)
         return 0;
      if (tree->unique && left_cmp == 0)
         return 0;
      if (p->left->priority > p->priority)
         return 0;
   }

   if (p->right) {
      int right_cmp = __TREAP_CMP(tree,__TREAP_KEY(tree,p), __TREAP_KEY(tree,p->right));
      if (right_cmp > 0)
         return 0;
      if (tree->unique && right_cmp == 0)
         return 0;
      if (p->right->priority > p->priority)
         return 0;
   }

   result = ____verify_treap(tree, p->left);
   if (!result)
      return result;

   result = ____verify_treap(tree, p->right);
   if (!result)
      return result;

   return 1;
}

int ____helper__verify_treap(treap_tree *tree)
{
   assert(&tree);
   if (tree)
      return ____verify_treap(tree, tree->root);
   return 1;
}

void ____print_treap(treap_tree *tree, treap_head *p)
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
      bklen = printf("%02X:%02X%c ", (int) tree->key(p), p->priority, linke[((!!p->left) << 1) | (!!p->right)]);
      nline = 0;
      ____print_treap(tree, p->right);
      ____print_treap(tree, p->left);
   } else {
      if (!nline)
         printf("\n");
      nline = 1;
   }
   depth--;
}

void ____helper__print_treap(treap_tree * tree)
{
   assert(&tree);
   if (tree) {
      printf("\n");
      ____print_treap(tree, tree->root);
      printf("\n");
   }
}

#ifdef __cplusplus
}
#endif

