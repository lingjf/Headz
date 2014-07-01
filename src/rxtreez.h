#ifndef __RXTREEZ_H_2009_0815__
#define __RXTREEZ_H_2009_0815__

#ifdef __cplusplus
extern "C" {
#endif

/*integer radix tree*/

typedef struct rxtree_node
{
   unsigned long path;
   unsigned long mask;
   int srt, len;
   void *usr;
   struct rxtree_node *son[2];
} rxtree_node;

#define RXTREE_HEAD_INIT(obj) { 0,0,0,0,0,{0,0} }

/**
 1. initialize head structure.
 **/
void rxtree_init(rxtree_node *head);

void rxtree_destroy(rxtree_node *head);

void rxtree_insert(rxtree_node *head, unsigned long idx, void *usr);
void rxtree_remove(rxtree_node *head, unsigned long idx);
void *rxtree_lookup(rxtree_node *head, unsigned long idx);

#ifdef __cplusplus
}
#endif

#endif

