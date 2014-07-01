#ifndef __BINTREEZ_H_a2910d55_63a7_4eef_a0cc_be5475528533__
#define __BINTREEZ_H_a2910d55_63a7_4eef_a0cc_be5475528533__

#ifdef __cplusplus
extern "C" {
#endif

/**
 http://en.wikipedia.org/wiki/Binary_search_tree
 */

typedef struct bintree_head
{
   struct bintree_head *parent;
   struct bintree_head *left, *right;
} bintree_head;

#define BINTREE_HEAD_INIT(obj) { 0,0,0 }

typedef unsigned long ( bintree_key)(bintree_head *node);
typedef int ( bintree_cmp)(unsigned long a, unsigned long b);

typedef struct bintree_tree
{
   bintree_head *root;
   bintree_key *key;
   bintree_cmp *cmp;
   int unique;
} bintree_tree;

void bintree_create(bintree_tree *tree, bintree_key *key, bintree_cmp *cmp, int unique);
void bintree_destroy(bintree_tree *tree);

void bintree_init(bintree_head *head);

bintree_head * bintree_insert(bintree_tree *tree, bintree_head *node);
bintree_head * bintree_remove(bintree_tree *tree, bintree_head *node);
bintree_head * bintree_lookup(bintree_tree *tree, unsigned long key);

bintree_head * bintree_min(bintree_tree *tree);
bintree_head * bintree_max(bintree_tree *tree);
bintree_head * bintree_inc(bintree_head *node);
bintree_head * bintree_dec(bintree_head *node);

bintree_head * bintree_root(bintree_head *node);
int bintree_empty(bintree_tree *tree);
int bintree_leaf(bintree_head *node);
int bintree_count(bintree_head *node);

#if 0

/**
 *            |                       |
 *            p                       p
 *            /\                      /\
 *           /  \                    /  \
 *          /    \                  /    \
 *         x      z                x      z
 *        / \                     / \
 *       /   \                   /   \
 *      xl    y                 y    xr
 *           / \               / \
 *          /   \             /   \
 *         yl   yr           yl   yr
 */
static void __swap_node(pritree_tree *tree, pritree_head *x, pritree_head *y)
{
   pritree_head *p, *xl, *xr, *yl, *yr;
   pritree_head **px;
   p = x->parent;
   xl = x->left;
   xr = x->right;
   yl = y->left;
   yr = y->right;
   if (p == NULL) {
      px = &tree->root;
   } else {
      px = p->left == x ? &p->left : &p->right;
   }
   y->parent = p;
   *px = y;
   x->parent = y;
   if (xr == y) {
      y->right = x;
      y->left = xl;
   } else {
      y->left = x;
      y->right = xr;
   }
   x->left = yl;
   x->right = yr;
}

#endif

#ifdef __cplusplus
}
#endif

#endif

