#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <signal.h>

#ifdef _WIN32
#include <windows.h> 
#else
#include <unistd.h>
#endif

#include "listz.h"
#include "bintreez.h"
#include "multreez.h"
#include "avltreez.h"
#include "rbtreez.h"
#include "rxtreez.h"
#include "sptreez.h"
#include "sbtreez.h"
#include "priheapz.h"
#include "treapz.h"
#include "hashz.h"
#include "slabz.h"
#include "slakz.h"
#include "jeffz.h"
#include "dbugz.h"


typedef struct bintree_body_t{
   bintree_head bn;int value;
   struct bintree_body_t *next;
}bintree_body_t;

typedef struct avltree_body_t{
   avltree_head avl;int value;
   struct avltree_body_t *next;
}avltree_body_t;

typedef struct rbtree_body_t{
   rbtree_head rb;int value;
   struct rbtree_body_t *next;
}rbtree_body_t;

typedef struct sptree_body_t{
   sptree_head sp;int value;
   struct sptree_body_t *next;
}sptree_body_t;

typedef struct priheap_body_t{
   priheap_head pr;int value;
   struct priheap_body_t *next;
}priheap_body_t;

typedef struct treap_body_t{
   treap_head tp;int value;
   struct treap_body_t *next;
}treap_body_t;

typedef struct sbtree_body_t{
   sbtree_head sb;int value;
   struct sbtree_body_t *next;
}sbtree_body_t;


unsigned long avltree_body_key(avltree_head * x); 
int avltree_body_cmp(unsigned long x, unsigned long y);
unsigned long rbtree_body_key(rbtree_head * x); 
int rbtree_body_cmp(unsigned long x, unsigned long y);
unsigned long sptree_body_key(sptree_head * x);
int sptree_body_cmp(unsigned long x, unsigned long y); 
unsigned long priheap_body_key(priheap_head * x);
int priheap_body_cmp(unsigned long x, unsigned long y);
unsigned long bintree_body_key(bintree_head * x);
int bintree_body_cmp(unsigned long x, unsigned long y);
unsigned long treap_body_key(treap_head * x);
int treap_body_cmp(unsigned long x, unsigned long y);
unsigned long sbtree_body_key(sbtree_head * x);
int sbtree_body_cmp(unsigned long x, unsigned long y);

#ifdef __cplusplus
extern "C" {
#endif
int  ____helper__verify_bintree(bintree_tree *tree);
void ____helper__print_bintree(bintree_tree *tree);
int  ____helper__verify_treap(treap_tree *tree);
void ____helper__print_treap(treap_tree *tree);
int  ____helper__verify_avltree(avltree_tree * tree);
void ____helper__print_avltree(avltree_tree * tree); 
int  ____helper__verify_rbtree(rbtree_tree *tree);
void ____helper__print_rbtree(rbtree_tree *tree);
void ____helper__print_sptree(sptree_tree *tree);
void ____helper__print_rxtree(rxtree_node * p);
int  ____helper__verify_priheap(priheap_tree *tree);
void ____helper__print_priheap(priheap_tree *tree);
int  ____helper__verify_sbtree(sbtree_tree *tree);
void ____helper__print_sbtree(sbtree_tree *tree);
int  ____helper__verify_rxstree(void* tree);
#ifdef __cplusplus
}
#endif

unsigned long avltree_body_key(avltree_head * x) 
{
   avltree_body_t * body;
   body = list_entry(x, avltree_body_t, avl);
   return body->value;
}

int avltree_body_cmp(unsigned long x, unsigned long y)
{
   return (int)x-(int)y;
}

unsigned long rbtree_body_key(rbtree_head * x)
{
   rbtree_body_t * body;
   body = list_entry(x, rbtree_body_t, rb);
   return body->value;
}

int rbtree_body_cmp(unsigned long x, unsigned long y)
{
   return (int)x-(int)y;
}

unsigned long sptree_body_key(sptree_head * x)
{
   sptree_body_t * body;
   body = list_entry(x, sptree_body_t, sp);
   return body->value;
}

int sptree_body_cmp(unsigned long x, unsigned long y)
{
   return (int)x-(int)y;
}


unsigned long priheap_body_key(priheap_head * x)
{
   priheap_body_t * body;
   body = list_entry(x, priheap_body_t, pr);
   return body->value;
}

int priheap_body_cmp(unsigned long x, unsigned long y)
{
   return (int)x-(int)y;
}

unsigned long bintree_body_key(bintree_head * x)
{
   bintree_body_t * body;
   body = list_entry(x, bintree_body_t, bn);
   return body->value;
}

int bintree_body_cmp(unsigned long x, unsigned long y)
{
   return (int)x-(int)y;
}

unsigned long treap_body_key(treap_head * x)
{
   treap_body_t * body;
   body = list_entry(x, treap_body_t, tp);
   return body->value;
}

int treap_body_cmp(unsigned long x, unsigned long y)
{
   return (int)x-(int)y;
}

unsigned long sbtree_body_key(sbtree_head * x)
{
   sbtree_body_t * body;
   body = list_entry(x, sbtree_body_t, sb);
   return body->value;
}

int sbtree_body_cmp(unsigned long x, unsigned long y)
{
   return (int)x-(int)y;
}

void headz()
{
  printf("sizeof(list_head) = %d \n", sizeof(list_head));
  printf("sizeof(line_head) = %d \n", sizeof(line_head));
  printf("sizeof(avltree_head) = %d \n", sizeof(avltree_head));
  printf("sizeof(bintree_head) = %d \n", sizeof(bintree_head));
  printf("sizeof(multree_head) = %d \n", sizeof(multree_head));
  printf("sizeof(rbtree_head) = %d \n", sizeof(rbtree_head));
  printf("sizeof(rxtree_node) = %d \n", sizeof(rxtree_node));
  printf("sizeof(object_head) = %d \n", __slab_inside_cell_head_size(0));
  printf("sizeof(object_tail) = %d \n", __slab_inside_cell_tail_size(0));
  printf("sizeof(page_head) = %d \n", __slab_inside_page_head_size(0));
  printf("sizeof(slab_head) = %d \n", __slab_inside_slab_head_size(0));
}

void listz()
{
   typedef struct list_body {
      int value;
      list_head link;
      int data;
   }list_body;
   
   list_head r, l;
   list_head a = LIST_HEAD_NULL;
   list_head b = LIST_HEAD_INIT(b);
   list_head c;
   list_head x, y, z;
   list_head *p, *t;
   list_body u, *q;
   int m;

   list_init(&r);
   list_init(&l);
   list_null(&c);

   m = list_empty(&r);
   
   list_add_next(&a, &r);
   list_add_prev(&b, &r);
   list_add_head(&c, &r);
   list_add_tail(&x, &l);
   list_add(&y, &l);

   list_del(&a);
   list_del_null(&b);
   list_del_init(&c);

   p = list_get_head(&l);
   t = list_get_tail(&l);

   list_enqueue(&c, &r);
   list_dequeue(p, &r);

   p = list_get_top(&l);
   t = list_get_bottom(&l);
   
   list_push(&b, &r);
   list_pop(p, &r);

   list_replace(&z, &y);
   
   list_move_head(&x, &l);
   list_move_tail(&y, &l);
   list_move(&z, &l);

   list_splice_head(&l, &r);
   list_splice_tail(&r, &l);

   list_lr(&l);
   list_rr(&r);

   q = list_entry(&u.link, list_body, link);
   
   list_for_each(p, &r){}
   list_for_each_safe(p, t, &r){}
   list_for_each_continue(p, &r){}
   list_for_each_continue_safe(p, t, &r){}
   list_for_each_from(p, &r){}
   list_for_each_from_safe(p, t, &r){}

   list_for_each_prev(p, &r){}
   list_for_each_prev_safe(p, t, &r){}
   list_for_each_prev_continue(p, &r){}
   list_for_each_prev_continue_safe(p, t, &r){}
   list_for_each_prev_from(p, &r){}
   list_for_each_prev_from_safe(p, t, &r){}
}

void avltreez()
{
   int i;
   avltree_tree r;
   avltree_body_t a[10];
   avltree_create(&r, &avltree_body_key, &avltree_body_cmp,1);
   
   for (i=0; i<10; i++) {
      avltree_init(&a[i].avl);
      a[i].value = i;
      avltree_insert( &r,&a[i].avl);
   }
   
   ____helper__print_avltree(&r);
}

void rbtreez()
{
   int i;
   rbtree_tree r;
   rbtree_body_t a[10];

   rbtree_create(&r, &rbtree_body_key, &rbtree_body_cmp, 1);
   
   for (i=0; i<10; i++) {
      rbtree_init(&a[i].rb);
      a[i].value = i;
      rbtree_insert(&r, &a[i].rb);
   }

   ____helper__print_rbtree(&r);
}

void sptreez()
{
   int i;
   sptree_tree r;
   sptree_body_t a[10];
   sptree_create(&r, &sptree_body_key, &sptree_body_cmp, 0);
   
   for (i=0; i<10; i++) {
      sptree_init(&a[i].sp);
      a[i].value = i;
      sptree_insert( &r,&a[i].sp); 
   }
   
   ____helper__print_sptree(&r);
}

void slab_cp()
{
   int i, j;
   void * slab = NULL;
   void * rest = NULL;
   slab_initialize();

   slab = slab_create("my_struct", 6000, 10000, 0, 0, 0.0, 0);
   for (i=1; i<0xffffff; i++) {
      for (j=0; j<100; j++)
         rest = slab_kalloc(slab, 0);
      printf("%d = %p  %g MB\n", i, rest, (6000.0 / 1024.0 *i) / 10.24 );
   }
   slab_uninitialize();
}

int jeffz_eg_main();
int dbugz_eg_main();

int main(int argc, char *argv[])
{
   headz();
   listz();
   
   avltreez();
   rbtreez();
   sptreez();
   jeffz_eg_main();  
   dbugz_eg_main();
   //slab_cp();
   return 0;
}



