#ifndef __TREAPZ_H_2009_0815__
#define __TREAPZ_H_2009_0815__

#ifdef __cplusplus
extern "C" {
#endif

/**
 http://en.wikipedia.org/wiki/Treap
 */

typedef struct treap_head
{
   struct treap_head *parent;
   struct treap_head *left, *right;
   int priority;
} treap_head;

#define TREAP_HEAD_INIT(obj) { 0,0,0,0 }

typedef unsigned long ( treap_key)(treap_head *node);
typedef int ( treap_cmp)(unsigned long a, unsigned long b);

typedef struct treap_tree
{
   treap_head *root;
   treap_key *key;
   treap_cmp *cmp;
   int unique;
} treap_tree;

void treap_create(treap_tree *tree, treap_key *key, treap_cmp *cmp, int unique);
void treap_destroy(treap_tree *tree);

void treap_init(treap_head *head);

treap_head * treap_insert(treap_tree *tree, treap_head *node);
treap_head * treap_remove(treap_tree *tree, treap_head *node);
treap_head * treap_lookup(treap_tree *tree, unsigned long key);

treap_head * treap_min(treap_tree *tree);
treap_head * treap_max(treap_tree *tree);
treap_head * treap_inc(treap_head *node);
treap_head * treap_dec(treap_head *node);

/**
 optinal routines
 **/
treap_head * treap_root(treap_head *node);
/*int        treap_empty(treap_tree *tree);*/
#define      treap_empty(tree) ((tree->root) == NULL)
/*int        treap_leaf(treap_head *node);*/
#define      treap_leaf(node) ((node)->left == NULL && (node)->right == NULL)
int treap_count(treap_head *node);

#ifdef __cplusplus
}
#endif

#endif

