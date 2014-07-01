#ifndef __PRIHEAPZ_H_2009_0815__
#define __PRIHEAPZ_H_2009_0815__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct priheap_head
{
   int index;
} priheap_head;

typedef unsigned long ( priheap_pri)(priheap_head *node);
typedef int ( priheap_cmp)(unsigned long a, unsigned long b);

typedef struct priheap_tree
{
   int size, avail;
   priheap_pri *pri;
   priheap_cmp *cmp;
   priheap_head **d;
} priheap_tree;

#define PRIHEAP_HEAD_INIT(obj) {0}

void priheap_create(priheap_tree *tree, int n, priheap_pri *pri, priheap_cmp *cmp);
void priheap_destroy(priheap_tree *tree);

int priheap_insert(priheap_tree *tree, priheap_head *node);
void priheap_update(priheap_tree *tree, priheap_head *node);
int priheap_remove(priheap_tree *tree, priheap_head *node);

priheap_head *priheap_pop(priheap_tree *tree);
priheap_head *priheap_peek(priheap_tree *tree);

int priheap_leaf(priheap_tree *tree, priheap_head *node);
int priheap_count(priheap_tree *tree);

#ifdef __cplusplus
}
#endif 

#endif 

