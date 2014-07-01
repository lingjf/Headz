#ifndef __MULTREEZ_H_2009_0815__
#define __MULTREEZ_H_2009_0815__

#ifdef __cplusplus
extern "C" {
#endif

#include "listz.h"

typedef struct multree_head
{
   struct multree_head *parent, *child;
   struct list_head sibling;
} multree_head;

#define MULTREE_HEAD_INIT(obj) { 0,0,LIST_HEAD_INIT((obj).sibling) }

void multree_zero(multree_head *node);

/*int   multree_is_root(multree_head * node);*/
#define multree_is_root(node) ((node)->parent == NULL)

/*int   multree_is_first_child(multree_head * node);*/
#define multree_is_first_child(node) ((node)->parent&&(node)->parent->child==(node))

/*int   multree_is_singleton(multree_head * node);*/
#define multree_is_singleton(node) list_empty(&(node)->sibling)

/*int   multree_is_leaf(multree_head * node);*/
#define multree_is_leaf(node) ((node)->child == NULL)

int multree_add_child(multree_head *child, multree_head *parent);

int multree_add_sibling_left(multree_head *node, multree_head *sibling);

int multree_add_sibling_right(multree_head *node, multree_head *sibling);

multree_head * multree_root(multree_head * node);

int multree_depth(multree_head * node);

/*multree_head * multree_left_sibling(multree_head * node);*/
#define multree_left_sibling(node) list_entry((node)->sibling.prev, multree_head, sibling)

/*multree_head * multree_right_sibling(multree_head * node);*/
#define multree_right_sibling(node) list_entry((node)->sibling.next, multree_head, sibling)

void multree_del(multree_head * node);

#ifdef __cplusplus
}
#endif

#endif

