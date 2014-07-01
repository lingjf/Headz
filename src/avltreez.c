#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "avltreez.h"

#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define null_head ((avltree_head*)0)

#define __height(node)  ((node)?(node)->height:0)

#define __link_left(node,head) do {(head)->left=(node);if(node)(node)->parent=(head);} while(0)

#define __link_right(node,head) do {(head)->right=(node);if(node)(node)->parent=(head);} while(0)

static __inline void __relink(avltree_head *node, avltree_head *old, avltree_head *parent, avltree_head **root)
{
   assert(&node);
   assert(&old);
   assert(&parent);
   assert(root);

   if (parent == NULL) {
      if (node)
         node->parent = null_head;
      *root = node;
   } else {
      if (parent->left == old)
         __link_left(node, parent);
      else
         __link_right(node, parent);
   }
}

static void __calculate_height(avltree_head *node)
{
   int lh, rh;
   avltree_head *l, *r;
   assert(node);
   l = node->left;
   r = node->right;
   lh = __height(l);
   rh = __height(r);
   node->height = MAX(lh, rh) + 1;
}

static int __calculate_balance(avltree_head *node)
{
   int lh, rh;
   avltree_head *l, *r;
   assert(node);
   l = node->left;
   r = node->right;
   lh = __height(l);
   rh = __height(r);

   return lh - rh;
}

#define __AVLTREE_KEY(tree, node) \
   ((tree)->key ? (tree)->key(node) : (unsigned long)(node))

#define __AVLTREE_CMP(tree, key1, key2)   \
   ((tree)->cmp ? (tree)->cmp(key1, key2) : (int)((key1)-(key2)))

static avltree_head * __append(avltree_head *node, avltree_tree *tree)
{
   int cmp;
   avltree_head *p;

   assert(node);
   assert(tree);

   p = tree->root;
   while (p) {
      cmp = __AVLTREE_CMP(tree, tree->key(node), tree->key(p));

      if (cmp < 0) {
         if (p->left == null_head) {
            __link_left(node, p);
            break;
         }
         p = p->left;
      } else {
         if (p->right == null_head) {
            __link_right(node, p);
            break;
         }
         p = p->right;
      }
   }

   node->height = 1;

   return node;
}

/* {left-left case}
 *
 *         z[h+2]                                            z[h+3]                                             l[h+2]
 *         /    \                                            /    \                                             /    \
 *        /      \                                          /      \                                           /      \
 *      l[h+1]    r[h] (constant)                         l[h+2]    r[h] (constant)                         ll[h+1]   z[h+1]
 *      /   \                          ======>            /   \                          ======>             /         /   \
 *     /     \                                           /     \                                            /         /     \
 *  ll[h]   lr[h]                                   ll[h+1]   lr[h]                                     (new)       lr[h]   r[h} (constant)
 *                                                     /
 *                                                    /
 *                                                (new)
 *
 * just z and l need to recalculate the height.
 */

static avltree_head * __right_rotate(avltree_head *head)
{
   avltree_head *z, *l, *r, *ll, *lr;

   assert(head);
   z = head;
   l = z->left;
   r = z->right;
   ll = l->left;
   lr = l->right;

   __link_left(lr, z);
   __calculate_height(z);
   __link_right(z,l);
   __calculate_height(l);
   return l;
}

static avltree_head * __left_rotate(avltree_head *head)
{
   avltree_head *z, *l, *r, *rl, *rr;

   assert(head);
   z = head;
   l = z->left;
   r = z->right;
   rl = r->left;
   rr = r->right;

   __link_right(rl,z);
   __calculate_height(z);
   __link_left(z,r);
   __calculate_height(r);
   return r;
}

static avltree_head * __left_right_rotate(avltree_head *head)
{
   avltree_head *z, *l;

   assert(head);
   z = head;
   l = z->left;
   l = __left_rotate(l);
   __link_left(l, z);
   __calculate_height(z);
   return __right_rotate(z);
}

static avltree_head * __right_left_rotate(avltree_head *head)
{
   avltree_head *z, *r;

   assert(head);
   z = head;
   r = z->right;
   r = __right_rotate(r);
   __link_right(r, z);
   __calculate_height(z);
   return __left_rotate(z);
}

static avltree_head * __rebalance(avltree_head *head)
{
   int lh, rh;
   avltree_head *h, *l, *r;

   assert(head);
   h = head;
   l = head->left;
   r = head->right;
   lh = __height(l);
   rh = __height(r);
   if (lh > rh + 1) { /*left case -- lh = rh+2*/
      int llh, lrh;
      avltree_head *ll, *lr;
      ll = l->left; /*l must not null*/
      lr = l->right;
      llh = __height(ll);
      lrh = __height(lr);
      if (lrh <= llh) { /* left-left case*/
         return __right_rotate(h);
      } else { /* left-right case*/
         return __left_right_rotate(h);
      }
   } else if (rh > lh + 1) { /*right case -- rh : lh+2 */
      int rlh, rrh;
      avltree_head *rl, *rr;
      rl = r->left; /*r must not null*/
      rr = r->right;
      rlh = __height(rl);
      rrh = __height(rr);
      if (rlh <= rrh) { /* right-right case*/
         return __left_rotate(h);
      } else { /* left-right case*/
         return __right_left_rotate(h);
      }
   }

   else {
      /*should not reach here*/
      return head;
   }
}

static void __rebalance_for_append(avltree_head *head, avltree_head **root)
{
   int balance;
   avltree_head *p, *t, *unbalance;

   assert(head);
   assert(root && *root);

   p = head->parent;
   t = head;
   unbalance = null_head;
   while (p && p->height < t->height + 1) { /*loop when height changing*/
      p->height = t->height + 1;
      balance = __calculate_balance(p);
      if (balance == 2 || balance == -2) {
         unbalance = p; /*unbalance point*/
         break;
      }
      t = p;
      p = p->parent;
   }

   if (unbalance) {
      avltree_head *rebalance, *parent;
      parent = unbalance->parent;
      rebalance = __rebalance(unbalance);
      __relink(rebalance, unbalance, parent, root);
   }
}

void avltree_create(avltree_tree *tree, avltree_key *key, avltree_cmp *cmp, int unique)
{
   assert(tree);
   assert(&key || &cmp);
   assert(unique == 0 || unique == !0);

   if (tree) {
      tree->root = null_head;
      tree->key = key;
      tree->cmp = cmp;
      unique = unique;
   }
}

void avltree_destroy(avltree_tree *tree)
{
   assert(&tree);
}

avltree_head * avltree_insert(avltree_tree *tree, avltree_head *node)
{
   avltree_head *impact;
   assert(tree);
   assert(&node);

   if (node == NULL)
      return null_head;
   avltree_init(node);

   /*first add node*/
   if (tree->root == NULL) {
      tree->root = node;
      return node;
   }

   impact = __append(node, tree);
   if (impact == null_head) /*key is already exist case*/
      return null_head;
   __rebalance_for_append(impact, &tree->root);

   return node;
}

static void __rotate_down(avltree_head *node, avltree_head **root)
{
   avltree_head *rotate, *parent;

   assert(node);
   assert(root && *root);

   while (node->left || node->right) {
      parent = node->parent;

      if (__calculate_balance(node) <= 0)
         rotate = __left_rotate(node);
      else
         rotate = __right_rotate(node);

      __relink(rotate, node, parent, root);
   }
}

static avltree_head * __remove(avltree_head *node, avltree_head **root)
{
   avltree_head *parent;
   assert(node);
   assert(root && *root);

   parent = node->parent;
   __relink(null_head, node, parent, root);
   return parent;
}

static void __rebalance_for_remove(avltree_head *node, avltree_head **root)
{
   int balance;
   avltree_head *p, *unbalance;

   assert(node);
   assert(root && *root);

   p = node;
   while (p) {
      unbalance = p;
      p = p->parent; /*for next loop*/
      __calculate_height(unbalance);
      balance = __calculate_balance(unbalance);
      if (balance >= 2 || balance <= -2) {
         avltree_head *parent, *rebalance;
         parent = unbalance->parent;
         rebalance = __rebalance(unbalance);
         __relink(rebalance, unbalance, parent, root);
      }
   }
}

avltree_head * avltree_remove(avltree_tree *tree, avltree_head *node)
{
   avltree_head *impact;
   assert(tree && tree->root);
   assert(&node);

   if (node == NULL)
      return null_head;
   __rotate_down(node, &tree->root);
   impact = __remove(node, &tree->root);
   if (impact)
      __rebalance_for_remove(impact, &tree->root);
   return node;
}

static avltree_head * __leftmost(avltree_head *node)
{
   assert(node);
   while (node && node->left)
      node = node->left;
   return node;
}

static avltree_head * __rightmost(avltree_head *node)
{
   assert(node);
   while (node && node->right)
      node = node->right;
   return node;
}

void avltree_init(avltree_head *head)
{
   assert(&head);

   if (head == NULL)
      return;

   head->parent = null_head;
   head->left = head->right = null_head;
   head->height = 1;
}

avltree_head * avltree_lookup(avltree_tree *tree, unsigned long key)
{
   int cmp;
   avltree_head *p;
   assert(&tree);
   assert(&key);

   if (tree == NULL)
      return null_head;

   p = tree->root;
   while (p) {
      cmp = __AVLTREE_CMP(tree, key, tree->key(p));

      if (cmp == 0)
         return p;
      else if (cmp < 0)
         p = p->left;
      else
         p = p->right;
   }
   return p;
}

avltree_head * avltree_min(avltree_tree *tree)
{
   assert(&tree);
   if (tree->root == NULL)
      return null_head;

   return __leftmost(tree->root);
}

avltree_head * avltree_max(avltree_tree *tree)
{
   assert(&tree);
   if (tree->root == NULL)
      return null_head;

   return __rightmost(tree->root);
}

avltree_head * avltree_inc(avltree_head *node)
{
   assert(&node);
   if (node == NULL)
      return null_head;

   if (node->right) {
      return __leftmost(node->right);
   } else {
      avltree_head * parent = node->parent;
      while (parent && node != parent->left) {
         node = parent;
         parent = parent->parent;
      }
      return parent;
   }
}

avltree_head * avltree_dec(avltree_head *node)
{
   assert(&node);
   if (node == NULL)
      return null_head;

   if (node->left) {
      return __rightmost(node->left);
   } else {
      avltree_head * parent = node->parent;
      while (parent && node != parent->right) {
         node = parent;
         parent = parent->parent;
      }
      return parent;
   }
}

/*---------------------------------------------------------------*/

avltree_head * avltree_root(avltree_head *node)
{
   assert(&node);
   while (node && node->parent)
      node = node->parent;
   return node;
}

int avltree_count(avltree_head *node)
{
   assert(&node);
   if (node == NULL)
      return 0;
   return avltree_count(node->left) + avltree_count(node->right) + 1;
}

/*---------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

void ____print_avltree(avltree_tree * tree, avltree_head * p)
{
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
      bklen = printf("%02X:%02d%c ", (int) __AVLTREE_KEY(tree,p), p->height,
                     linke[((!!p->left) << 1) | (!!p->right)]);
      nline = 0;
      ____print_avltree(tree, p->right);
      ____print_avltree(tree, p->left);
   } else {
      if (!nline)
         printf("\n");
      nline = 1;
   }
   depth--;
}

void ____helper__print_avltree(avltree_tree *tree)
{
   assert(&tree);

   if (tree) {
      printf("\n");
      ____print_avltree(tree, tree->root);
      printf("\n");
   }
}

int ____verify_avltree(avltree_tree *tree, avltree_head *p, int *height)
{
   int result;
   int left_height, right_height;
   assert(&tree);
   assert(&p);
   assert(&height);

   if (p == NULL) {
      *height = 0;
      return 0;
   }

   result = ____verify_avltree(tree, p->left, &left_height);
   if (result)
      return result;

   result = ____verify_avltree(tree, p->right, &right_height);
   if (result)
      return result;

   result = left_height - right_height;
   if (result >= 2 || result <= -2)
      return 1;

   if (p->left && __AVLTREE_CMP(tree,__AVLTREE_KEY(tree,p), __AVLTREE_KEY(tree,p->left)) < 0)
      return 1;

   if (p->right && __AVLTREE_CMP(tree,__AVLTREE_KEY(tree,p), __AVLTREE_KEY(tree,p->right)) > 0)
      return 1;

   *height = left_height >= right_height ? left_height + 1 : right_height + 1;
   return 0;
}

int ____helper__verify_avltree(avltree_tree *tree)
{
   int height;
   assert(&tree);
   if (tree)
      return !____verify_avltree(tree, tree->root, &height);
   return 1;
}

#ifdef __cplusplus
}
#endif
