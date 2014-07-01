#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

/*
 * A red-black tree is a binary search tree with the node color as an
 * extra attribute.  It fulfills a set of conditions:
 *
 *	1. every search path from the root to a leaf consists of the same number of black nodes,
 *	2  each red node must not has a red child,
 *	3. root node is always black.
 *
 * Every operation on a red-black tree is bounded as O(lg n).
 * The maximum height of a red-black tree is 2lg (n+1).
 */

#include "rbtreez.h"

#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define null_head ((rbtree_head*)0)

#define __swap(a,b) do {register int t=(a);(a)=(b);(b)=t;} while(0)

#define __RBTREE_KEY(tree, node) \
   ((tree)->key ? (tree)->key(node) : (unsigned long)(node))

#define __RBTREE_CMP(tree, key1, key2) \
   ((tree)->cmp ? (tree)->cmp(key1, key2) : (int)((key1)-(key2)))

void rbtree_create(rbtree_tree *tree, rbtree_key *key, rbtree_cmp *cmp, int unique)
{
   assert(tree);
   assert(&key || &cmp);
   assert(unique == 0 || unique == !0);

   memset(tree, 0, sizeof(rbtree_tree));
   tree->root = null_head;
   tree->key = key;
   tree->cmp = cmp;
   tree->unique = unique;
}

void rbtree_destroy(rbtree_tree *tree)
{
   assert(tree);
}

static __inline void __left_rotate(rbtree_head* __x, rbtree_head** __root)
{
   assert(__x && __root);
   {
      rbtree_head* __y = __x->right;
      __x->right = __y->left;
      if (__y->left != 0)
         __y->left->parent = __x;
      __y->parent = __x->parent;

      if (__x == *__root)
         *__root = __y;
      else if (__x == __x->parent->left)
         __x->parent->left = __y;
      else
         __x->parent->right = __y;
      __y->left = __x;
      __x->parent = __y;
   }
}

static __inline void __right_rotate(rbtree_head* __x, rbtree_head** __root)
{
   assert(__x && __root);
   {
      rbtree_head* __y = __x->left;
      __x->left = __y->right;
      if (__y->right != 0)
         __y->right->parent = __x;
      __y->parent = __x->parent;

      if (__x == *__root)
         *__root = __y;
      else if (__x == __x->parent->right)
         __x->parent->right = __y;
      else
         __x->parent->left = __y;
      __y->right = __x;
      __x->parent = __y;
   }
}

static __inline void __rebalance_for_add(rbtree_head* __x, rbtree_head** __root)
{
   assert(__x && __root);
   __x->color = RB_RED;
   while (__x != *__root && __x->parent->color == RB_RED) {
      if (__x->parent == __x->parent->parent->left) {
         rbtree_head* __y = __x->parent->parent->right;
         if (__y && __y->color == RB_RED) {
            __x->parent->color = RB_BLACK;
            __y->color = RB_BLACK;
            __x->parent->parent->color = RB_RED;
            __x = __x->parent->parent;
         } else {
            if (__x == __x->parent->right) {
               __x = __x->parent;
               __left_rotate(__x, __root);
            }
            __x->parent->color = RB_BLACK;
            __x->parent->parent->color = RB_RED;
            __right_rotate(__x->parent->parent, __root);
         }
      } else {
         rbtree_head* __y = __x->parent->parent->left;
         if (__y && __y->color == RB_RED) {
            __x->parent->color = RB_BLACK;
            __y->color = RB_BLACK;
            __x->parent->parent->color = RB_RED;
            __x = __x->parent->parent;
         } else {
            if (__x == __x->parent->left) {
               __x = __x->parent;
               __right_rotate(__x, __root);
            }
            __x->parent->color = RB_BLACK;
            __x->parent->parent->color = RB_RED;
            __left_rotate(__x->parent->parent, __root);
         }
      }
   }
   (*__root)->color = RB_BLACK;
}

static __inline rbtree_head*
__rebalance_for_del(rbtree_head* __z, rbtree_head** __root)
{
   rbtree_head* __y = __z;
   rbtree_head* __x = 0;
   rbtree_head* __x_parent = 0;
   assert(__z && __root);
   if (__y->left == 0) /* __z has at most one non-null child. y == z. */
      __x = __y->right; /* __x might be null.*/
   else if (__y->right == 0) /* __z has exactly one non-null child. y == z. */
      __x = __y->left; /* __x is not null.*/
   else { /* __z has two non-null children.  Set __y to */
      __y = __y->right; /*   __z's successor.  __x might be null. */
      while (__y->left != 0)
         __y = __y->left;
      __x = __y->right;
   }
   if (__y != __z) { /* relink y in place of z.  y is z's successor */
      __z->left->parent = __y;
      __y->left = __z->left;
      if (__y != __z->right) {
         __x_parent = __y->parent;
         if (__x)
            __x->parent = __y->parent;
         __y->parent->left = __x; /* __y must be a child of left */
         __y->right = __z->right;
         __z->right->parent = __y;
      } else
         __x_parent = __y;
      if (*__root == __z)
         *__root = __y;
      else if (__z->parent->left == __z)
         __z->parent->left = __y;
      else
         __z->parent->right = __y;
      __y->parent = __z->parent;
      __swap(__y->color, __z->color);
      __y = __z;
      /* __y now points to node to be actually deleted */
   } else { /* __y == __z */
      __x_parent = __y->parent;
      if (__x)
         __x->parent = __y->parent;
      if (*__root == __z)
         *__root = __x;
      else if (__z->parent->left == __z)
         __z->parent->left = __x;
      else
         __z->parent->right = __x;
   }
   if (__y->color != RB_RED) {
      while (__x != *__root && (__x == 0 || __x->color == RB_BLACK))
         if (__x == __x_parent->left) {
            rbtree_head* __w = __x_parent->right;
            if (__w->color == RB_RED) {
               __w->color = RB_BLACK;
               __x_parent->color = RB_RED;
               __left_rotate(__x_parent, __root);
               __w = __x_parent->right;
            }
            if ((__w->left == 0 || __w->left->color == RB_BLACK) && (__w->right == 0 || __w->right->color
                        == RB_BLACK)) {
               __w->color = RB_RED;
               __x = __x_parent;
               __x_parent = __x_parent->parent;
            } else {
               if (__w->right == 0 || __w->right->color == RB_BLACK) {
                  if (__w->left)
                     __w->left->color = RB_BLACK;
                  __w->color = RB_RED;
                  __right_rotate(__w, __root);
                  __w = __x_parent->right;
               }
               __w->color = __x_parent->color;
               __x_parent->color = RB_BLACK;
               if (__w->right)
                  __w->right->color = RB_BLACK;
               __left_rotate(__x_parent, __root);
               break;
            }
         } else { /* same as above, with right <-> left. */
            rbtree_head* __w = __x_parent->left;
            if (__w->color == RB_RED) {
               __w->color = RB_BLACK;
               __x_parent->color = RB_RED;
               __right_rotate(__x_parent, __root);
               __w = __x_parent->left;
            }
            if ((__w->right == 0 || __w->right->color == RB_BLACK) && (__w->left == 0 || __w->left->color
                        == RB_BLACK)) {
               __w->color = RB_RED;
               __x = __x_parent;
               __x_parent = __x_parent->parent;
            } else {
               if (__w->left == 0 || __w->left->color == RB_BLACK) {
                  if (__w->right)
                     __w->right->color = RB_BLACK;
                  __w->color = RB_RED;
                  __left_rotate(__w, __root);
                  __w = __x_parent->left;
               }
               __w->color = __x_parent->color;
               __x_parent->color = RB_BLACK;
               if (__w->left)
                  __w->left->color = RB_BLACK;
               __right_rotate(__x_parent, __root);
               break;
            }
         }
      if (__x)
         __x->color = RB_BLACK;
   }
   return __y;
}

void rbtree_init(rbtree_head *head)
{
   assert(head);
   if (head) {
      head->parent = null_head;
      head->left = head->right = null_head;
      head->color = RB_RED;
   }
}

rbtree_head * rbtree_insert(rbtree_tree *tree, rbtree_head *node)
{
   unsigned long key;
   rbtree_head *p;
   assert(tree && node);
   if (!tree || !node)
      return null_head;
   rbtree_init(node);

   if (tree->root == null_head) { /*first add node*/
      node->color = RB_BLACK;
      tree->root = node;
      return node;
   }
   key = __RBTREE_KEY(tree, node);
   p = tree->root;
   do {
      int __cmp = __RBTREE_CMP(tree, key, __RBTREE_KEY(tree, p));
      if (__cmp == 0 && tree->unique)
         return null_head;

      if (__cmp <= 0) {
         if (p->left == null_head) {
            p->left = node;
            node->parent = p;
            break;
         }
         p = p->left;
      } else {
         if (p->right == null_head) {
            p->right = node;
            node->parent = p;
            break;
         }
         p = p->right;
      }
   } while (p);

   __rebalance_for_add(node, &tree->root);

   return node;
}

static rbtree_head * __leftmost(rbtree_head *node)
{
   assert(node);
   if (node)
      while (node->left)
         node = node->left;
   return node;
}

static rbtree_head * __rightmost(rbtree_head *node)
{
   assert(node);
   if (node)
      while (node->right)
         node = node->right;
   return node;
}

rbtree_head * rbtree_min(rbtree_tree *tree)
{
   assert(tree);
   if (!tree)
      return null_head;

   return __leftmost(tree->root);
}

rbtree_head * rbtree_max(rbtree_tree *tree)
{
   assert(tree);
   if (!tree)
      return null_head;

   return __rightmost(tree->root);
}

rbtree_head * rbtree_inc(rbtree_head *node)
{
   assert(&node);
   if (!node)
      return null_head;

   if (node->right) {
      return __leftmost(node->right);
   } else {
      rbtree_head * parent = node->parent;
      while (parent && node != parent->left) {
         node = parent;
         parent = parent->parent;
      }
      return parent;
   }
}

rbtree_head * rbtree_dec(rbtree_head *node)
{
   assert(&node);
   if (!node)
      return null_head;

   if (node->left) {
      return __rightmost(node->left);
   } else {
      rbtree_head * parent = node->parent;
      while (parent && node != parent->right) {
         node = parent;
         parent = parent->parent;
      }
      return parent;
   }
}

rbtree_head * rbtree_lookup(rbtree_tree *tree, unsigned long key)
{
   int __cmp;
   rbtree_head *p;
   assert(tree);
   assert(&key);
   if (!tree)
      return null_head;

   p = tree->root;
   while (p) {
      __cmp = __RBTREE_CMP(tree, key, __RBTREE_KEY(tree, p));

      if (__cmp == 0)
         return p;
      else if (__cmp < 0)
         p = p->left;
      else
         p = p->right;
   }
   return p;
}

rbtree_head * rbtree_remove(rbtree_tree *tree, rbtree_head *node)
{
   assert(tree && node);
   if (!tree || !node)
      return null_head;
   __rebalance_for_del(node, &tree->root);
   return node;
}

/*---------------------------------------------------------------*/

rbtree_head * rbtree_root(rbtree_head *node)
{
   assert(node);
   while (node && node->parent)
      node = node->parent;
   return node;
}

int rbtree_count(rbtree_head *node)
{
   assert(&node);
   if (!node)
      return 0;
   return rbtree_count(node->left) + rbtree_count(node->right) + 1;
}

int rbtree_height(rbtree_head *node)
{
   assert(&node);
   if (!node)
      return 0;
   return MAX(rbtree_height(node->left), rbtree_height(node->right)) + 1;
}

/*---------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

void ____print_rbtree(rbtree_tree *tree, rbtree_head *p)
{
   static char colors[] = { 'R', 'B' };
   static char linke[] = { '|', '/', '\\', '<' };
   static int depth = 0;
   static int nline = 1;
   static int bklen = 0;
   assert(&tree);
   assert(&p);
   depth++;
   if (p) {
      if (nline) {
         int i;
         printf("\r");
         for (i = 0; i < (depth - 1) * bklen; i++)
            printf(" ");
      }
      bklen = printf("%02X%c%c ", (int) __RBTREE_KEY(tree,p), colors[p->color],
                     linke[((!!p->left) << 1) | (!!p->right)]);
      nline = 0;
      ____print_rbtree(tree, p->right);
      ____print_rbtree(tree, p->left);
   } else {
      if (!nline)
         printf("\n");
      nline = 1;
   }
   depth--;
}

void ____helper__print_rbtree(rbtree_tree *tree)
{
   assert(&tree);
   if (tree) {
      printf("\n");
      ____print_rbtree(tree, tree->root);
      printf("\n");
   }
}

static int ____count_rbtree(rbtree_head *p, int blacks)
{
   int result;
   static int nr_black = 0;
   static int complete = 0;
   assert(&p);
   assert(&blacks);
   if (blacks == 0)
      complete = 0;

   if (p == NULL) {
      if (!complete) {
         complete = 1;
         nr_black = blacks;
         return 0;
      } else {
         if (nr_black != blacks)
            return 1;
         else
            return 0;
      }
   }
   if (p->color == RB_BLACK)
      blacks++;

   result = ____count_rbtree(p->left, blacks);
   if (result)
      return result;
   result = ____count_rbtree(p->right, blacks);
   if (result)
      return result;

   return 0;
}

static int ____verify_rbtree(rbtree_tree *tree, rbtree_head * p)
{
   int result;
   rbtree_head *l, *r;
   assert(&tree);
   assert(&p);
   if (p == NULL)
      return 0;

   l = p->left;
   r = p->right;

   /* left < current < right*/
   if (l && __RBTREE_CMP(tree, __RBTREE_KEY(tree, p), __RBTREE_KEY(tree, l)) < 0)
      return 1;
   if (r && __RBTREE_CMP(tree, __RBTREE_KEY(tree, p), __RBTREE_KEY(tree, r)) > 0)
      return 1;

   /* root color is black*/
   if (p->parent == NULL)
      if (p->color != RB_BLACK)
         return 1;

   /* no red parent and child*/
   if (p->color == RB_RED) {
      if (l && l->color == RB_RED)
         return 1;
      if (r && r ->color == RB_RED)
         return 1;
   }

   /* child legal*/
   result = ____verify_rbtree(tree, l);
   if (result)
      return result;
   result = ____verify_rbtree(tree, r);
   if (result)
      return result;

   return ____count_rbtree(p, 0);
}

int ____helper__verify_rbtree(rbtree_tree *tree)
{
   assert(&tree);
   if (tree)
      return !____verify_rbtree(tree, tree->root);
   return 1;
}

#ifdef __cplusplus
}
#endif
