#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "listz.h"
#include "priheapz.h"

extern "C" {
int  ____helper__verify_priheap(priheap_tree *tree);
void ____helper__print_priheap(priheap_tree *tree);
}

typedef struct priheap_body_t{
   priheap_head pr;int value;
   struct priheap_body_t *next;
}priheap_body_t;

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

H2SUITE(priheapz)
{
   priheap_tree tree;
   priheap_body_t vals[7];
   void setup()
   { 
      priheap_create(&tree, 10, &priheap_body_key, &priheap_body_cmp);
      for (int i=0; i<7; i++) {
         //priheap_init(&vals[i].pr);
         vals[i].value = i;
      }
   }

   void teardown()
   {  
      priheap_destroy(&tree);
   }  	  	
} ;

H2CASE(priheapz, priheap_create)
{
   priheap_tree x;
   priheap_create(&x, 10, &priheap_body_key, &priheap_body_cmp);

   H2EQUAL_INTEGER(1, x.size);
   H2EQUAL_INTEGER(11, x.avail);
   H2CHECK(0 != x.d);
   H2CHECK((priheap_pri*)&priheap_body_key == x.pri);
   H2CHECK((priheap_cmp*)&priheap_body_cmp == x.cmp);
   priheap_destroy(&x);
}

H2CASE(priheapz, priheap_destroy)
{
}

H2CASE(priheapz, PRIHEAP_HEAD_INIT)
{
   priheap_head x = PRIHEAP_HEAD_INIT(x);

   H2EQUAL_INTEGER(0, x.index);
}

H2CASE(priheapz, insert_1)
{
   priheap_insert(&tree, &vals[0].pr);
   H2EQUAL_INTEGER(1, ____helper__verify_priheap(&tree));
   //____helper__print_priheap(&tree);
}

H2CASE(priheapz, insert_n)
{
   for (int i=0; i<7; i++)
      priheap_insert(&tree, &vals[i].pr);
   H2EQUAL_INTEGER(1, ____helper__verify_priheap(&tree));
   //____helper__print_priheap(&tree);
}

H2CASE(priheapz, update_n)
{
   int i;
   for (i=0; i<7; i++)
      priheap_insert(&tree, &vals[i].pr);

   for (i=6; i>=0; i--) {
      vals[i].value = i*2;
      priheap_update(&tree, &vals[i].pr);
   }
   
   H2EQUAL_INTEGER(1, ____helper__verify_priheap(&tree));
   //____helper__print_priheap(&tree);
}

H2CASE(priheapz, remove)
{
   for (int i=0; i<7; i++)
      priheap_insert(&tree, &vals[i].pr);

   priheap_remove(&tree, &vals[3].pr);
   
   H2EQUAL_INTEGER(1, ____helper__verify_priheap(&tree));
   //____helper__print_priheap(&tree);
}

H2CASE(priheapz, pop)
{
   int i;
   priheap_head * p;
   for (i=0; i<7; i++)
      priheap_insert(&tree, &vals[i].pr);

   for (i=0; i<7; i++) {
      p = priheap_pop(&tree);
      H2CHECK(p!=0);
      H2EQUAL_INTEGER(&vals[6-i].pr, p);
      H2EQUAL_INTEGER(1, ____helper__verify_priheap(&tree));
   }

   p = priheap_pop(&tree);
   H2CHECK(p==0);
  
   //____helper__print_priheap(&tree);
}

H2CASE(priheapz, peek)
{
   int i;
   priheap_head * p;
   for (i=0; i<7; i++)
      priheap_insert(&tree, &vals[i].pr);

   for (i=0; i<7; i++) {
      p = priheap_peek(&tree);
      H2CHECK(p!=0);
      H2EQUAL_INTEGER(&vals[6].pr, p);
      H2EQUAL_INTEGER(1, ____helper__verify_priheap(&tree));
   }

   p = priheap_peek(&tree);
   H2CHECK(p!=0);
   H2EQUAL_INTEGER(&vals[6].pr, p);
  
   //____helper__print_priheap(&tree);
}

H2CASE(priheapz, count)
{
   for (int i=0; i<7; i++)
      priheap_insert(&tree, &vals[i].pr);
   H2EQUAL_INTEGER(1, ____helper__verify_priheap(&tree));
   H2EQUAL_INTEGER(7, priheap_count(&tree));
   //____helper__print_priheap(&tree);
}

H2CASE(priheapz, leaf)
{
   for (int i=0; i<7; i++)
      priheap_insert(&tree, &vals[i].pr);
   H2EQUAL_INTEGER(1, ____helper__verify_priheap(&tree));
   H2EQUAL_INTEGER(1, priheap_leaf(&tree, &vals[0].pr));
   H2EQUAL_INTEGER(1, priheap_leaf(&tree, &vals[1].pr));
   H2EQUAL_INTEGER(1, priheap_leaf(&tree, &vals[2].pr));
   H2EQUAL_INTEGER(1, priheap_leaf(&tree, &vals[4].pr));
   H2EQUAL_INTEGER(0, priheap_leaf(&tree, &vals[3].pr));
   H2EQUAL_INTEGER(0, priheap_leaf(&tree, &vals[5].pr));
   H2EQUAL_INTEGER(0, priheap_leaf(&tree, &vals[6].pr));
   //____helper__print_priheap(&tree);
}
