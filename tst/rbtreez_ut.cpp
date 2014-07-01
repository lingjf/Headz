#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "listz.h"
#include "rbtreez.h"

extern "C" {
int  ____helper__verify_rbtree(rbtree_tree *tree);
void ____helper__print_rbtree(rbtree_tree *tree);
}

typedef struct rbtree_body_t{
   rbtree_head rb;int value;
   struct rbtree_body_t *next;
}rbtree_body_t;

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

H2SUITE(rbtreet)
{ 
   rbtree_tree tree;
   rbtree_body_t vals[9999];

   void setup()
   { 
      rbtree_create(&tree, &rbtree_body_key, &rbtree_body_cmp, 1);
      for (int i=0; i<7; i++) {
         rbtree_init(&vals[i].rb);
         vals[i].value = i;
      }
   }

   void teardown()
   {  		
   }  	  	
} ;

H2CASE(rbtreet, RBTREE_HEAD_INIT)
{
   rbtree_head x = RBTREE_HEAD_INIT(x);

   H2EQUAL_INTEGER((rbtree_head*)NULL, x.parent);
   H2EQUAL_INTEGER((rbtree_head*)NULL, x.left);
   H2EQUAL_INTEGER((rbtree_head*)NULL, x.right);
   H2EQUAL_INTEGER(RB_RED, x.color);
}

H2CASE(rbtreet, rbtree_init)
{
   rbtree_head x;

   rbtree_init(&x);

   H2EQUAL_INTEGER((rbtree_head*)NULL, x.parent);
   H2EQUAL_INTEGER((rbtree_head*)NULL, x.left);
   H2EQUAL_INTEGER((rbtree_head*)NULL, x.right);
   H2EQUAL_INTEGER(RB_RED, x.color);
}

H2CASE(rbtreet, rbtree_insert_first)
{
   rbtree_body_t x;
   rbtree_init(&x.rb);
   x.value = 3;
   H2EQUAL_INTEGER(&x.rb, rbtree_insert(&tree, &x.rb));
   H2EQUAL_INTEGER(1, ____helper__verify_rbtree(&tree));
}

H2CASE(rbtreet, rbtree_insert7)
{
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 5;
   vals[4].value = 4;
   vals[5].value = 3;
   vals[6].value = 7;

   for (int i=0; i<7; i++) {
      H2EQUAL_INTEGER(&vals[i].rb, rbtree_insert(&tree,&vals[i].rb));
   }

   H2EQUAL_INTEGER(1, ____helper__verify_rbtree(&tree));
   //____helper__print_rbtree(r,rbtree_body_key);

}

H2CASE(rbtreet, insert)
{
   rbtree_body_t * body = NULL;
   const int N = 9999;
   for (int i=0; i<N; ++i) {
      rbtree_body_t * t = &vals[i];      
     	rbtree_init(&t->rb);
      t->value = i+1;
      t->next = body;
      body = t;
      H2EQUAL_INTEGER(&t->rb, rbtree_insert(&tree,&t->rb));
   }
   H2EQUAL_INTEGER(1, ____helper__verify_rbtree(&tree));
}

H2CASE(rbtreet, remove)
{
   rbtree_body_t * body = NULL;
   const int N = 999;

   for (int i=0; i<N; ++i) {
      rbtree_body_t * t = &vals[i];      
     	rbtree_init(&t->rb);
      t->value = i+1;
      t->next = body;
      body = t;
      H2EQUAL_INTEGER(&t->rb, rbtree_insert(&tree, &t->rb));
   }
   H2EQUAL_INTEGER(1, ____helper__verify_rbtree(&tree));

   while (body) {
      rbtree_body_t * t = body;
      body = body->next;
      H2EQUAL_INTEGER(&t->rb,  rbtree_remove(&tree, &t->rb));
      H2EQUAL_INTEGER(1, ____helper__verify_rbtree(&tree));
   }
}

H2CASE(rbtreet, rbtree_for_each)
{
   int i;
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 5;
   vals[4].value = 4;
   vals[5].value = 3;
   vals[6].value = 7;

   for (i=0; i<7; i++) H2EQUAL_INTEGER(&vals[i].rb, rbtree_insert(&tree,&vals[i].rb));

   int last;
   rbtree_head *h;
   rbtree_body_t *p;

   /*rbtree_for_each*/
   last = 0;
   rbtree_for_each(h,&tree) {
      p = rbtree_entry(h,rbtree_body_t,rb);
      H2CHECK(p->value > last);
      last = p->value;
   }
}

H2CASE(rbtreet, rbtree_for_each_prev)
{
   int i;
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 5;
   vals[4].value = 4;
   vals[5].value = 3;
   vals[6].value = 7;

   for (i=0; i<7; i++) H2EQUAL_INTEGER(&vals[i].rb, rbtree_insert(&tree,&vals[i].rb));

   int last;
   rbtree_head *h;
   rbtree_body_t *p;

   last = 9;
   rbtree_for_each_prev(h,&tree) {
      p = rbtree_entry(h,rbtree_body_t,rb);
      H2CHECK(p->value < last);
      last = p->value;
   }
}

H2CASE(rbtreet, rbtree_for_each_safe)
{
   int i;
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 5;
   vals[4].value = 4;
   vals[5].value = 3;
   vals[6].value = 7;

   for (i=0; i<7; i++) H2EQUAL_INTEGER(&vals[i].rb, rbtree_insert(&tree,&vals[i].rb));

   int last;
   rbtree_head *h, *ht;
   rbtree_body_t *p;
   
   last = 0;
   rbtree_for_each_safe(h,ht,&tree) {
      p = rbtree_entry(h,rbtree_body_t,rb);
      H2CHECK(p->value > last);
      last = p->value;
      H2EQUAL_INTEGER(h, rbtree_remove(&tree,h));
   }
}

H2CASE(rbtreet, rbtree_for_each_prev_safe)
{
   int i;
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 5;
   vals[4].value = 4;
   vals[5].value = 3;
   vals[6].value = 7;

   for (i=0; i<7; i++) H2EQUAL_INTEGER(&vals[i].rb, rbtree_insert(&tree,&vals[i].rb));

   int last;
   rbtree_head *h, *ht;
   rbtree_body_t *p;
   
   last = 9;
   rbtree_for_each_prev_safe(h,ht,&tree) {
      p = rbtree_entry(h,rbtree_body_t,rb);
      H2CHECK(p->value < last);
      last = p->value;
      H2EQUAL_INTEGER(h, rbtree_remove(&tree,h));
   } 
}

H2CASE(rbtreet, root)
{
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 5;
   vals[4].value = 4;
   vals[5].value = 3;
   vals[6].value = 7;

   for (int i=0; i<7; i++) {
      H2EQUAL_INTEGER(&vals[i].rb, rbtree_insert(&tree, &vals[i].rb));
   }

   H2EQUAL_INTEGER(tree.root, rbtree_root(&vals[0].rb));
   H2EQUAL_INTEGER(tree.root, rbtree_root(&vals[4].rb));
   H2EQUAL_INTEGER(tree.root, rbtree_root(&vals[6].rb));
}

H2CASE(rbtreet, count)
{
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 5;
   vals[4].value = 4;
   vals[5].value = 3;
   vals[6].value = 7;

   for (int i=0; i<7; i++) {
      H2EQUAL_INTEGER(&vals[i].rb, rbtree_insert(&tree, &vals[i].rb));
      H2EQUAL_INTEGER(i+1, rbtree_count(tree.root));
   }
}

H2CASE(rbtreet, height)
{
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 5;
   vals[4].value = 4;
   vals[5].value = 3;
   vals[6].value = 7;

   for (int i=0; i<7; i++) {
      H2EQUAL_INTEGER(&vals[i].rb, rbtree_insert(&tree, &vals[i].rb));
   }

   H2EQUAL_INTEGER(4, rbtree_height(tree.root));
   //____helper__print_rbtree(&root);
}

H2CASE(rbtreet, rbtree_add2_equal)
{
   rbtree_tree r1, r2;
   rbtree_body_t x, y;
   rbtree_init(&x.rb);
   rbtree_init(&y.rb);
   x.value = 3;
   y.value = 3;

   rbtree_create(&r1, &rbtree_body_key, &rbtree_body_cmp, 0);
   rbtree_create(&r2, &rbtree_body_key, &rbtree_body_cmp, 1);
   
   H2EQUAL_INTEGER(&x.rb, rbtree_insert(&r1, &x.rb));
   H2EQUAL_INTEGER(1, ____helper__verify_rbtree(&r1));   
   H2EQUAL_INTEGER(&y.rb, rbtree_insert(&r1, &y.rb)); 
   H2EQUAL_INTEGER(1, ____helper__verify_rbtree(&r1));  

   H2EQUAL_INTEGER(&x.rb, rbtree_insert(&r2, &x.rb));
   H2EQUAL_INTEGER(1, ____helper__verify_rbtree(&r2));   
   H2EQUAL_INTEGER(0, rbtree_insert(&r2, &y.rb)); 
   H2EQUAL_INTEGER(1, ____helper__verify_rbtree(&r2));
   
   //____helper__print_rbtree(&r1);
}



