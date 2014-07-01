#ifndef __SPTREEZ_H_4019f808_5ab9_4a8b_98d8_d198abd29d88__
#define __SPTREEZ_H_4019f808_5ab9_4a8b_98d8_d198abd29d88__

#ifdef __cplusplus
extern "C" {
#endif

/**
 http://en.wikipedia.org/wiki/Splay_tree
 */

typedef struct sptree_head
{
   struct sptree_head *left, *right;
} sptree_head;

typedef unsigned long ( sptree_key)(sptree_head *node);
typedef int ( sptree_cmp)(unsigned long a, unsigned long b);

typedef struct sptree_tree
{
   sptree_head *root;
   sptree_key *key;
   sptree_cmp *cmp;
   int unique;
} sptree_tree;

#define SPTREE_HEAD_INIT(obj) {0,0}

void sptree_create(sptree_tree *tree, sptree_key *key, sptree_cmp *cmp, int unique);
void sptree_destroy(sptree_tree *tree);

void sptree_init(sptree_head *head);
sptree_head *sptree_insert(sptree_tree *tree, sptree_head *node);
void sptree_remove(sptree_tree *tree, sptree_head *node);
sptree_head *sptree_lookup(sptree_tree *tree, unsigned long key);

sptree_head *sptree_min(sptree_tree *tree);
sptree_head *sptree_max(sptree_tree *tree);
sptree_head *sptree_inc(sptree_tree *tree, sptree_head *node);
sptree_head *sptree_dec(sptree_tree *tree, sptree_head *node);

#ifdef __cplusplus
}
#endif

#endif

