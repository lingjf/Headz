#ifndef __RBTREET_H_2009_0815__
#define __RBTREET_H_2009_0815__

#ifdef __cplusplus
extern "C" {
#endif

#include "listz.h"

#define RB_BLACK	1
#define RB_RED		0

/**
 *            |
 *          parent
 *            /\
 *           /  \
 *          /    \
 *      current sibling      V(left)<V(current)<V(right)<V(parent)<V(sibling)
 *        /\
 *       /  \
 *    left  right
 */

typedef struct rbtree_head
{
   struct rbtree_head *parent;
   struct rbtree_head *left, *right;
   int color;
} rbtree_head;

typedef unsigned long ( rbtree_key)(rbtree_head *node);
typedef int ( rbtree_cmp)(unsigned long a, unsigned long b);

typedef struct rbtree_tree
{
   rbtree_head *root;
   rbtree_key *key;
   rbtree_cmp *cmp;
   int unique;
} rbtree_tree;

#define RBTREE_HEAD_INIT(obj) { 0,0,0,RB_RED }

void rbtree_create(rbtree_tree *tree, rbtree_key *key, rbtree_cmp *cmp, int unique);
void rbtree_destroy(rbtree_tree *tree);

/**
 1. initialize head structure.
 **/
void rbtree_init(rbtree_head *head);

/**
 1. insert a node into avltree.
 2. orginal avltree can be empty (*root == null)
 3. root may be changed.
 4. return the added node, it should equal node normally, null if failed.
 **/
rbtree_head * rbtree_insert(rbtree_tree *tree, rbtree_head *node);
/**
 1. delete link from avltree.
 2. root may be changed.
 3. return the delete node point, it should equal node normally, null if failed.
 **/
rbtree_head * rbtree_remove(rbtree_tree *tree, rbtree_head *node);

/**
 1. search node by key.
 2. return the found node, null if not found.
 **/
rbtree_head * rbtree_lookup(rbtree_tree *tree, unsigned long key);

rbtree_head * rbtree_min(rbtree_tree *tree);
rbtree_head * rbtree_max(rbtree_tree *tree);
rbtree_head * rbtree_inc(rbtree_head *node);
rbtree_head * rbtree_dec(rbtree_head *node);

/**
 optinal routines
 **/
rbtree_head * rbtree_root(rbtree_head *node);
/*int         rbtree_empty(rbtree_head *root);*/
#define       rbtree_empty(root) ((root) == NULL)
/*int         rbtree_leaf(rbtree_head *node);*/
#define       rbtree_leaf(node) ((node)->left == NULL && (node)->right == NULL)
int rbtree_count(rbtree_head *node);
int rbtree_height(rbtree_head *node);

#define rbtree_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define rbtree_for_each(pos, tree) \
	for ((pos) = rbtree_min(tree); (pos); (pos) = rbtree_inc(pos))

#define rbtree_for_each_prev(pos, tree) \
	for ((pos) = rbtree_max(tree); (pos); (pos) = rbtree_dec(pos))

#define rbtree_for_each_safe(pos, t, tree) \
	for ((pos) = rbtree_min(tree), (t) = rbtree_inc(pos); (pos); (pos) = (t), (t) = rbtree_inc(pos))

#define rbtree_for_each_prev_safe(pos, t, tree) \
	for ((pos) = rbtree_max(tree), (t) = rbtree_dec(pos); (pos); (pos) = (t), (t) = rbtree_dec(pos))   

#ifdef __cplusplus
}
#endif

#endif

