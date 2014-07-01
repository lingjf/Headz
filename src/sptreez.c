#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "sptreez.h"

/* based on gnu/gcc/libiberty */

#define null_head ((sptree_head*)0)

void sptree_create(sptree_tree *tree, sptree_key *key, sptree_cmp *cmp, int unique)
{
   assert(tree);
   assert(&key || &cmp);
   assert(unique == 0 || unique == !0);

   tree->root = null_head;
   tree->key = key;
   tree->cmp = cmp;
   tree->unique = unique;
}

void sptree_destroy(sptree_tree *tree)
{
   assert(tree);
}

void sptree_init(sptree_head *head)
{
   assert(head);
   if (head) {
      head->left = head->right = null_head;
   }
}

/* Rotate the edge joining the left child N with its parent P.  
 PP is the grandparents' pointer to P.  */

static __inline void __rotate_left(sptree_head **pp, sptree_head *p, sptree_head *n)
{
   sptree_head * tmp;
   assert(pp && p && n);

   tmp = n->right;
   n->right = p;
   p->left = tmp;
   *pp = n;
}

/* Rotate the edge joining the right child N with its parent P.  
 PP is the grandparents' pointer to P.  */

static __inline void __rotate_right(sptree_head **pp, sptree_head *p, sptree_head *n)
{
   sptree_head * tmp;
   assert(pp && p && n);

   tmp = n->left;
   n->left = p;
   p->right = tmp;
   *pp = n;
}

#define __SPTREE_KEY(tree, node) \
   ((tree)->key ? (tree)->key(node) : (unsigned long)(node))

#define __SPTREE_CMP(tree, key1, key2)   \
   ((tree)->cmp ? (tree)->cmp(key1, key2) : (int)((key1)-(key2)))

/* Bottom up splay of key.  */

static void __sptree_splay(sptree_tree *tree, unsigned long key)
{
   assert(tree);
   assert(&key);

   if (tree->root == 0)
      return;

   do {
      int cmp1, cmp2;
      sptree_head *n, *c;

      n = tree->root;
      cmp1 = __SPTREE_CMP(tree, key, __SPTREE_KEY(tree, n));

      /* Found.  */
      if (cmp1 == 0)
         return;

      /* Left or right?  If no child, then we're done.  */
      if (cmp1 < 0)
         c = n->left;
      else
         c = n->right;
      if (!c)
         return;

      /* Next one left or right?  If found or no child, we're done after one rotation.  */
      cmp2 = __SPTREE_CMP(tree, key, __SPTREE_KEY(tree, c));
      if (cmp2 == 0 || (cmp2 < 0 && !c->left) || (cmp2 > 0 && !c->right)) {
         if (cmp1 < 0)
            __rotate_left(&tree->root, n, c);
         else
            __rotate_right(&tree->root, n, c);
         return;
      }

      /* Now we have the four cases of double-rotation.  */
      if (cmp1 < 0 && cmp2 < 0) {
         __rotate_left(&n->left, c, c->left);
         __rotate_left(&tree->root, n, n->left);
      } else if (cmp1 > 0 && cmp2 > 0) {
         __rotate_right(&n->right, c, c->right);
         __rotate_right(&tree->root, n, n->right);
      } else if (cmp1 < 0 && cmp2 > 0) {
         __rotate_right(&n->left, c, c->right);
         __rotate_left(&tree->root, n, n->left);
      } else if (cmp1 > 0 && cmp2 < 0) {
         __rotate_left(&n->right, c, c->left);
         __rotate_right(&tree->root, n, n->right);
      }
   } while (1);
}

/* Insert a new node (associating KEY with DATA) into SP.  If a
 previous node with the indicated KEY exists, its data is replaced
 with the new value.  Returns the new node.  */

sptree_head * sptree_insert(sptree_tree *tree, sptree_head *node)
{
   unsigned long key;
   int comparison = 0;
   assert(tree && node);
   key = __SPTREE_KEY(tree, node);

   __sptree_splay(tree, key);

   if (tree->root) {
      comparison = __SPTREE_CMP(tree, __SPTREE_KEY(tree, tree->root), key);
      if (tree->unique && comparison == 0)
         return null_head;
   }

   if (!tree->root) {
      node->left = node->right = 0;
   } else if (comparison < 0) {
      node->left = tree->root;
      node->right = node->left->right;
      node->left->right = 0;
   } else {
      node->right = tree->root;
      node->left = node->right->left;
      node->right->left = 0;
   }
   tree->root = node;
   return tree->root;
}

/* Remove KEY from SP.  It is not an error if it did not exist.  */

void sptree_remove(sptree_tree *tree, sptree_head *node)
{
   assert(tree && node);

   __sptree_splay(tree, __SPTREE_KEY(tree, node));

   if (tree->root && __SPTREE_CMP(tree, __SPTREE_KEY(tree, tree->root), __SPTREE_KEY(tree, node)) == 0) {
      sptree_head *left, *right;

      left = tree->root->left;
      right = tree->root->right;

      /* One of the children is now the root.  Doesn't matter much which, so long as we preserve the properties of the tree.  */
      if (left) {
         tree->root = left;
         /* If there was a right child as well, hang it off the right-most leaf of the left child.  */
         if (right) {
            while (left->right)
               left = left->right;
            left->right = right;
         }
      } else {
         tree->root = right;
      }
   }
}

/* Lookup KEY in SP, returning VALUE if present, and NULL otherwise.  */

sptree_head * sptree_lookup(sptree_tree *tree, unsigned long key)
{
   assert(tree);
   assert(&key);
   __sptree_splay(tree, key);

   if (tree->root && __SPTREE_CMP(tree, __SPTREE_KEY(tree, tree->root), key) == 0)
      return tree->root;
   else
      return null_head;
}

static sptree_head * __leftmost(sptree_head *node)
{
   assert(node);
   if (node)
      while (node->left)
         node = node->left;
   return node;
}

static sptree_head * __rightmost(sptree_head *node)
{
   assert(node);
   if (node)
      while (node->right)
         node = node->right;
   return node;
}

sptree_head * sptree_min(sptree_tree *tree)
{
   assert(tree);
   if (tree->root == NULL)
      return null_head;

   return __leftmost(tree->root);
}

sptree_head * sptree_max(sptree_tree *tree)
{
   assert(tree);
   if (tree->root == NULL)
      return null_head;

   return __rightmost(tree->root);
}

/* Return the immediate predecessor KEY, or NULL if there is no
 predecessor.  KEY need not be present in the tree.  */

sptree_head * __predecessor(sptree_tree *tree, unsigned long key)
{
   int comparison;
   assert(tree);
   assert(&key);

   /* If the tree is empty, there is certainly no predecessor.  */
   if (!tree->root)
      return null_head;

   /* Splay the tree around KEY.  That will leave either the KEY
    itself, its predecessor, or its successor at the root.  */
   __sptree_splay(tree, key);
   comparison = __SPTREE_CMP(tree, __SPTREE_KEY(tree, tree->root), key);

   /* If the predecessor is at the root, just return it.  */
   if (comparison < 0)
      return tree->root;

   /* Otherwise, find the rightmost element of the left subtree.  */
   return __rightmost(tree->root->left);
}

/* Return the immediate successor KEY, or NULL if there is no
 successor.  KEY need not be present in the tree.  */

sptree_head * __successor(sptree_tree *tree, unsigned long key)
{
   int comparison;
   assert(tree);
   assert(&key);

   /* If the tree is empty, there is certainly no successor.  */
   if (!tree->root)
      return null_head;

   /* Splay the tree around KEY.  That will leave either the KEY
    itself, its predecessor, or its successor at the root.  */
   __sptree_splay(tree, key);
   comparison = __SPTREE_CMP(tree, __SPTREE_KEY(tree, tree->root), key);

   /* If the successor is at the root, just return it.  */
   if (comparison > 0)
      return tree->root;

   /* Otherwise, find the leftmost element of the right subtree.  */
   return __leftmost(tree->root->right);
}

sptree_head * sptree_inc(sptree_tree *tree, sptree_head *node)
{
   assert(tree);
   assert(node);

   if (node == NULL)
      return null_head;
   return __successor(tree, __SPTREE_KEY(tree, node));
}

sptree_head * sptree_dec(sptree_tree *tree, sptree_head *node)
{
   assert(tree);
   assert(node);

   if (node == NULL)
      return null_head;

   return __predecessor(tree, __SPTREE_KEY(tree, node));
}

/*---------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

void ____print_sptree(sptree_tree *tree, sptree_head *p)
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
      bklen = printf("%02X%c ", (int) __SPTREE_KEY(tree, p), linke[((!!p->left) << 1) | (!!p->right)]);
      nline = 0;
      ____print_sptree(tree, p->right);
      ____print_sptree(tree, p->left);
   } else {
      if (!nline)
         printf("\n");
      nline = 1;
   }
   depth--;
}

void ____helper__print_sptree(sptree_tree *tree)
{
   assert(&tree);

   if (tree) {
      printf("\n");
      ____print_sptree(tree, tree->root);
      printf("\n");
   }
}

#ifdef __cplusplus
}
#endif

