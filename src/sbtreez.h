#ifndef __SBTREEZ_H_2009_0815__
#define __SBTREEZ_H_2009_0815__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sbtree_head
{
   struct sbtree_head *parent;
   struct sbtree_head *ch[2];
   unsigned long size;
} sbtree_head;

typedef unsigned long ( sbtree_key)(sbtree_head *node);
typedef int ( sbtree_cmp)(unsigned long a, unsigned long b);

typedef struct sbtree_tree
{
   sbtree_head *root;
   sbtree_key *key;
   sbtree_cmp *cmp;
   int unique;
} sbtree_tree;

#define SBTREE_HEAD_INIT(obj) {0,{0,0},1}

void sbtree_create(sbtree_tree *tree, sbtree_key *key, sbtree_cmp *cmp, int unique);
void sbtree_destroy(sbtree_tree *tree);

void sbtree_init(sbtree_head *head);
sbtree_head * sbtree_insert(sbtree_tree *tree, sbtree_head *node);
sbtree_head * sbtree_remove(sbtree_tree *tree, sbtree_head *node);
sbtree_head * sbtree_lookup(sbtree_tree *tree, unsigned long key);
sbtree_head * sbtree_select(sbtree_tree *tree, unsigned long i);
unsigned long sbtree_rank(sbtree_tree *tree, sbtree_head *node);

sbtree_head * sbtree_min(sbtree_tree *tree);
sbtree_head * sbtree_max(sbtree_tree *tree);
sbtree_head * sbtree_inc(sbtree_head *node);
sbtree_head * sbtree_dec(sbtree_head *node);

#ifdef __cplusplus
}
#endif

#endif

