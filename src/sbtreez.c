#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "sbtreez.h"

#ifdef __cplusplus
extern "C" {
#endif

#define null_head ((sbtree_head*)0)

#define left  ch[0]
#define right ch[1]

void sbtree_create(sbtree_tree *tree, sbtree_key *key, sbtree_cmp *cmp, int unique)
{
   assert(tree);
   assert(&key || &cmp);
   assert(unique == 0 || unique == !0);

   memset(tree, 0, sizeof(sbtree_tree));
   tree->root = null_head;
   tree->key = key;
   tree->cmp = cmp;
   tree->unique = unique;
}

void sbtree_destroy(sbtree_tree *tree)
{
   assert(tree);
}

void sbtree_init(sbtree_head *head)
{
   assert(head);
   if (head) {
      head->parent = null_head;
      head->ch[0] = head->ch[1] = null_head;
      head->size = 1;
   }
}

#define __SBTREE_SIZE(node) ((node)?(node)->size:0)

#define __SBTREE_KEY(tree, node) \
   ((tree)->key ? (tree)->key(node) : (unsigned long)(node))

#define __SBTREE_CMP(tree, key1, key2) \
   ((tree)->cmp ? (tree)->cmp(key1, key2) : (int)((key1)-(key2)))

/*
 *
 *            p                    p             p                        p    
 *            |                    |             |                        |    
 *            x                    y             x                        y    
 *          /   \                /   \         /   \                    /   \  
 *        y      c   right ->   a     x       a     y     left ->     x      c 
 *       /  \                        / \           / \               /  \      
 *      a    b                      b   c         b   c             a    b     
 *                                                                               
 */
static __inline void __rotate(sbtree_head **xx, int flag)
{
   sbtree_head *x, *y, **yy, *b, **bb, *p;
   assert(xx);
   assert(&flag);
   if (*xx) {
      x = *xx;
      p = x->parent;
      yy = &x->ch[flag];
      y = x->ch[flag];
      if (y) {
         y->parent = p;
         *xx = y;
         bb = &y->ch[!flag];
         b = y->ch[!flag];
         *bb = x;
         x->parent = y;
         *yy = b;
         if (b)
            b->parent = x;
         y->size = x->size;
         x->size = __SBTREE_SIZE(x->ch[0]) + __SBTREE_SIZE(x->ch[1]) + 1;
      }
   }
}
/*
 Maintain (t,flag)
 01     If flag=false then
 02          If s[left[left[t]]>s[right[t]] then         //case1
 03               Right-Rotate(t)
 04          Else
 05               If s[right[left[t]]>s[right[t]] then   //case2
 06                    Left-Rotate(left[t])
 07                    Right-Rotate(t)
 08          Else                                        //needn¡¯t repair
 09               Exit
 10     Else
 11          If s[right[right[t]]>s[left[t]] then        //case1'
 12               Left-Rotate(t)
 13          Else
 14               If s[left[right[t]]>s[left[t]] then    //case2'
 15                    Right-Rotate(right[t])
 16                    Left-Rotate(t)
 17          Else                                        //needn¡¯t repair
 18               Exit
 19     Maintain(left[t],false)                     //repair the left subtree
 20     Maintain(right[t],true)                     //repair the right subtree
 21     Maintain(t,false)                           //repair the whole tree
 22     Maintain(t,true)                            //repair the whole tree
 */

/*
 *
 *            p                      p         
 *            |                      |         
 *            t                      t         
 *          /   \                  /   \       
 *        x      y               y      x      
 *       /  \   /  \            /  \   /  \    
 *      a    b c    d          d    c b    a   
 *
 */
static void __maintain(sbtree_head **tt, int flag)
{
   sbtree_head *t, *x, *y, *a, *b;
   assert(tt);
   assert(&flag);
   t = *tt;
   if (!t)
      return;

   x = t->ch[flag];
   y = t->ch[!flag];
   a = x ? x->ch[flag] : null_head;
   b = x ? x->ch[!flag] : null_head;

   if (__SBTREE_SIZE(a) > __SBTREE_SIZE(y)) {
      __rotate(tt, flag);
   } else if (__SBTREE_SIZE(b) > __SBTREE_SIZE(y)) {
      __rotate(&t->ch[flag], !flag);
      __rotate(tt, flag);
   } else {
      return;
   }
   __maintain(&t->ch[0], 0);
   __maintain(&t->ch[1], 1);
   __maintain(tt, 0);
   __maintain(tt, 1);
}

static void __insert(sbtree_tree *tree, sbtree_head **pp, sbtree_head *node)
{
   int flag;
   sbtree_head *t;
   assert(tree && pp && node);

   t = *pp;
   if (!t) {
      *pp = node;
      return;
   }
   t->size++;
   node->parent = t;
   flag = __SBTREE_CMP(tree, __SBTREE_KEY(tree, node), __SBTREE_KEY(tree, t)) >= 0;
   __insert(tree, &t->ch[flag], node);
   __maintain(pp, flag);
}

sbtree_head * sbtree_insert(sbtree_tree *tree, sbtree_head *node)
{
   assert(tree && node);
   if (!tree || !node)
      return null_head;

   sbtree_init(node);
   __insert(tree, &tree->root, node);
   return node;
}

static sbtree_head * __leftmost(sbtree_head *node)
{
   assert(node);
   if (node)
      while (node->left)
         node = node->left;
   return node;
}

static sbtree_head * __rightmost(sbtree_head *node)
{
   assert(node);
   if (node)
      while (node->right)
         node = node->right;
   return node;
}

sbtree_head * sbtree_remove(sbtree_tree *tree, sbtree_head *node)
{
   sbtree_head **pchild, *parent;
   assert(tree && node);
   if (!tree || !node)
      return null_head;

   parent = node->parent;
   if (parent == NULL) {
      if (tree->root == node)
         pchild = &tree->root;
      else
         return NULL; /*BUG*/
   } else {
      if (parent->left == node) {
         pchild = &parent->left;
      } else if (parent->right == node) {
         pchild = &parent->right;
      } else {
         return NULL; /*BUG*/
      }
   }
   if (!node->left || !node->right) {
      if (!node->left && !node->right) {
         *pchild = NULL;
      } else if (node->left && !node->right) {
         *pchild = node->left;
         node->left->parent = parent;
      } else if (node->right && !node->left) {
         *pchild = node->right;
         node->right->parent = parent;
      }
      for (; parent; parent = parent->parent)
         parent->size--;
   } else {
      sbtree_head *left_max = sbtree_remove(tree, __rightmost(node->left));
      *pchild = left_max;
      left_max->parent = parent;
      left_max->left = node->left;
      left_max->right = node->right;
      left_max->size = node->size;
   }
   return node;
}

sbtree_head * sbtree_lookup(sbtree_tree *tree, unsigned long key)
{
   int __cmp;
   sbtree_head *p;
   assert(tree);
   assert(&key);
   if (!tree)
      return null_head;

   p = tree->root;
   while (p) {
      __cmp = __SBTREE_CMP(tree, key, __SBTREE_KEY(tree, p));

      if (__cmp == 0)
         return p;
      else if (__cmp < 0)
         p = p->left;
      else
         p = p->right;
   }
   return p;
}

static sbtree_head * __select(sbtree_head *node, unsigned long i)
{
   unsigned long left_size, less_size = i;
   assert(node);
   assert(&i);

   if (less_size >= __SBTREE_SIZE(node))
      return null_head;
   left_size = __SBTREE_SIZE(node->left);

   if (left_size == less_size)
      return node;
   else if (left_size < less_size)
      return __select(node->right, i - left_size - 1);
   else
      return __select(node->left, i);
}

sbtree_head *sbtree_select(sbtree_tree *tree, unsigned long i)
{
   assert(tree);
   assert(&i);

   if (!tree)
      return null_head;
   return __select(tree->root, i);
}

unsigned long sbtree_rank(sbtree_tree *tree, sbtree_head *node)
{
   unsigned long i;
   sbtree_head * parent;
   assert(tree && node);

   if (!tree || !node)
      return 0;
   i = __SBTREE_SIZE(node->left);
   parent = node->parent;
   while (parent) {
      if (parent->right == node)
         i += __SBTREE_SIZE(parent->left) + 1;
      node = parent;
      parent = node->parent;
   }
   return i;
}

sbtree_head * sbtree_min(sbtree_tree *tree)
{
   assert(tree);
   if (!tree)
      return null_head;

   return __leftmost(tree->root);
}

sbtree_head * sbtree_max(sbtree_tree *tree)
{
   assert(tree);
   if (!tree)
      return null_head;

   return __rightmost(tree->root);
}

sbtree_head * sbtree_inc(sbtree_head * node)
{
   assert(node);
   if (!node)
      return null_head;

   if (node->right) {
      return __leftmost(node->right);
   } else {
      sbtree_head * parent = node->parent;
      while (parent && node != parent->left) {
         node = parent;
         parent = parent->parent;
      }
      return parent;
   }
}

sbtree_head * sbtree_dec(sbtree_head * node)
{
   assert(node);
   if (!node)
      return null_head;

   if (node->left) {
      return __rightmost(node->left);
   } else {
      sbtree_head * parent = node->parent;
      while (parent && node != parent->right) {
         node = parent;
         parent = parent->parent;
      }
      return parent;
   }
}
/*---------------------------------------------------------------*/

static void ____print_sbtree(sbtree_tree *tree, sbtree_head *p)
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
      bklen = printf("%02X:%02X%c ", (int) __SBTREE_KEY(tree,p), (unsigned int) p->size,
                     linke[((!!p->left) << 1) | (!!p->right)]);
      nline = 0;
      ____print_sbtree(tree, p->right);
      ____print_sbtree(tree, p->left);
   } else {
      if (!nline)
         printf("\n");
      nline = 1;
   }
   depth--;
}

void ____helper__print_sbtree(sbtree_tree *tree)
{
   assert(&tree);
   if (tree) {
      printf("\n");
      ____print_sbtree(tree, tree->root);
      printf("\n");
   }
}

static int ____verify_sbtree(sbtree_tree *tree, sbtree_head *p)
{
   int result;
   assert(&tree);
   assert(&p);
   if (p == NULL) {
      return 1;
   }

   /*verify size field*/
   {
      unsigned long s, ls, rs, lls, lrs, rrs, rls;
      s = __SBTREE_SIZE(p);
      ls = __SBTREE_SIZE(p->left);
      rs = __SBTREE_SIZE(p->right);
      lls = p->left ? __SBTREE_SIZE(p->left->left) : 0;
      lrs = p->left ? __SBTREE_SIZE(p->left->right) : 0;
      rls = p->right ? __SBTREE_SIZE(p->right->left) : 0;
      rrs = p->right ? __SBTREE_SIZE(p->right->right) : 0;

      if (s != ls + rs + 1)
         return 0;
      if (lls > rs)
         return 0;
      if (lrs > rs)
         return 0;
      if (rls > ls)
         return 0;
      if (rrs > ls)
         return 0;
   }
   if (p->left) {
      int left_cmp = __SBTREE_CMP(tree,__SBTREE_KEY(tree,p->left), __SBTREE_KEY(tree,p));
      if (left_cmp > 0)
         return 0;
      if (tree->unique && left_cmp == 0)
         return 0;
   }

   if (p->right) {
      int right_cmp = __SBTREE_CMP(tree,__SBTREE_KEY(tree,p), __SBTREE_KEY(tree,p->right));
      if (right_cmp > 0)
         return 0;
      if (tree->unique && right_cmp == 0)
         return 0;
   }

   result = ____verify_sbtree(tree, p->left);
   if (!result)
      return result;

   result = ____verify_sbtree(tree, p->right);
   if (!result)
      return result;

   return 1;
}

int ____helper__verify_sbtree(sbtree_tree *tree)
{
   assert(&tree);
   if (tree)
      return ____verify_sbtree(tree, tree->root);
   return 1;
}

#ifdef __cplusplus
}
#endif

