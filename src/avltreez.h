#ifndef __AVLTREET_H_2009_0815__
#define __AVLTREET_H_2009_0815__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct avltree_head
{
   struct avltree_head *parent;
   struct avltree_head *left, *right;
   int height;
} avltree_head;

typedef unsigned long ( avltree_key)(avltree_head *node);
typedef int ( avltree_cmp)(unsigned long a, unsigned long b);

typedef struct avltree_tree
{
   avltree_head *root;
   avltree_key *key;
   avltree_cmp *cmp;
   int unique;
} avltree_tree;

#define AVLTREE_HEAD_INIT(obj) { 0,0,0,1 }

void avltree_create(avltree_tree *tree, avltree_key *key, avltree_cmp *cmp, int unique);
void avltree_destroy(avltree_tree *tree);

/**
 1. initialize head structure.
 **/
void avltree_init(avltree_head *head);

/**
 1. insert a node into avltree.
 2. orginal avltree can be empty (*root == null)
 3. root may be changed.
 4. return the added node, it should equal node normally, null if failed.
 **/
avltree_head * avltree_insert(avltree_tree *tree, avltree_head *node);

/**
 1. delete link from avltree.
 2. root may be changed.
 3. return the deleted node, it should equal node normally, null if failed.
 */
avltree_head * avltree_remove(avltree_tree *tree, avltree_head *node);

/**
 1. search node by key.
 2. return the found node, null if not found.
 **/
avltree_head * avltree_lookup(avltree_tree *tree, unsigned long key);

avltree_head * avltree_min(avltree_tree *tree);
avltree_head * avltree_max(avltree_tree *tree);
avltree_head * avltree_inc(avltree_head *node);
avltree_head * avltree_dec(avltree_head *node);

/**
 optinal routines
 **/
avltree_head * avltree_root(avltree_head *node);
/*int          avltree_empty(avltree_tree *tree);*/
#define        avltree_empty(tree) ((tree->root) == NULL)
/*int          avltree_leaf(avltree_head *node);*/
#define        avltree_leaf(node) ((node)->left == NULL && (node)->right == NULL)
int avltree_count(avltree_head *node);
/*int          avltree_height(avltree_head *node);*/
#define        avltree_height(node) ((node)->height)

#define avltree_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define avltree_for_each(pos, tree) \
	for ((pos) = avltree_min(tree); (pos); (pos) = avltree_inc(pos))

#define avltree_for_each_prev(pos, tree) \
	for ((pos) = avltree_max(tree); (pos); (pos) = avltree_dec(pos))

#define avltree_for_each_safe(pos, t, tree) \
	for ((pos) = avltree_min(tree), (t) = avltree_inc(pos); (pos); (pos) = (t), (t) = avltree_inc(pos))

#define avltree_for_each_prev_safe(pos, t, tree) \
	for ((pos) = avltree_max(tree), (t) = avltree_dec(pos); (pos); (pos) = (t), (t) = avltree_dec(pos))   

#ifdef __cplusplus
}
#endif

#endif

