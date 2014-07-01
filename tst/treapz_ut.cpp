#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "listz.h"
#include "treapz.h"

extern "C" {
int  ____helper__verify_treap(treap_tree *tree);
void ____helper__print_treap(treap_tree *tree);
}

typedef struct treap_body_t{
   treap_head tp;int value;
   struct treap_body_t *next;
}treap_body_t;


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

H2SUITE(treapz)
{ public:
   treap_tree tree;
   treap_body_t vals[7];

	void setup()
  	{ 
      treap_create(&tree, &treap_body_key, &treap_body_cmp, 0);
      for (int i=0; i<7; i++) {
         treap_init(&vals[i].tp);
         vals[i].tp.priority = 7-i;
         vals[i].value = i;
      }

  	}

  	void teardown()
  	{  		
  	}  	  	
} ;

H2CASE(treapz, TREAP_HEAD_INIT)
{
   treap_head a = TREAP_HEAD_INIT(a);

   H2EQUAL_INTEGER(0, a.parent);
   H2EQUAL_INTEGER(0, a.left);
   H2EQUAL_INTEGER(0, a.right);
}

H2CASE(treapz, treap_init)
{
   treap_head a;
   treap_init(&a);

   H2EQUAL_INTEGER(0, a.parent);
   H2EQUAL_INTEGER(0, a.left);
   H2EQUAL_INTEGER(0, a.right);
}

H2CASE(treapz, treap_create)
{
   treap_tree a;
   treap_create(&a, &treap_body_key, &treap_body_cmp, 1);

   H2EQUAL_INTEGER(0, a.root);
   //H2EQUAL_INTEGER((treap_key*)&treap_body_key, a.key);
   //H2EQUAL_INTEGER((treap_cmp*)&treap_body_cmp, a.cmp);
   H2EQUAL_INTEGER(1, a.unique);
}

H2CASE(treapz, insert)
{
   treap_insert(&tree, &vals[0].tp);
   H2EQUAL_INTEGER(1,____helper__verify_treap(&tree));
   //____helper__print_treap(&tree);
}

H2CASE(treapz, insert_7)
{
   int i;
   for (i=0; i<7; i++)
      treap_insert(&tree, &vals[i].tp);
   H2EQUAL_INTEGER(1,____helper__verify_treap(&tree));
   //____helper__print_treap(&tree);
}

H2CASE(treapz, delete)
{
   int i;
   for (i=0; i<7; i++)
      treap_insert(&tree, &vals[i].tp);
   
   treap_remove(&tree, &vals[0].tp);
   H2EQUAL_INTEGER(1,____helper__verify_treap(&tree));
   treap_remove(&tree, &vals[5].tp);
   H2EQUAL_INTEGER(1,____helper__verify_treap(&tree));
   //____helper__print_treap(&tree);
}


H2CASE(treapz, lookup)
{
   int i;
   for (i=0; i<7; i++)
      treap_insert(&tree, &vals[i].tp);
   
   H2EQUAL_INTEGER(&vals[0].tp, treap_lookup(&tree, 0));
   H2EQUAL_INTEGER(&vals[1].tp, treap_lookup(&tree, 1));
   H2EQUAL_INTEGER(&vals[2].tp, treap_lookup(&tree, 2));
   H2EQUAL_INTEGER(&vals[3].tp, treap_lookup(&tree, 3));
   H2EQUAL_INTEGER(&vals[4].tp, treap_lookup(&tree, 4));
   H2EQUAL_INTEGER(&vals[5].tp, treap_lookup(&tree, 5));
   H2EQUAL_INTEGER(&vals[6].tp, treap_lookup(&tree, 6));
   //____helper__print_treap(&tree);
}

H2CASE(treapz, min)
{
   int i;
   for (i=0; i<7; i++)
      treap_insert(&tree, &vals[i].tp);
   H2EQUAL_INTEGER(&vals[0].tp, treap_min(&tree));
   //____helper__print_treap(&tree);
}

H2CASE(treapz, max)
{
   int i;
   for (i=0; i<7; i++)
      treap_insert(&tree, &vals[i].tp);
   H2EQUAL_INTEGER(&vals[6].tp, treap_max(&tree));
   //____helper__print_treap(&tree);
}

H2CASE(treapz, inc)
{
   int i;
   for (i=0; i<7; i++)
      treap_insert(&tree, &vals[i].tp);
   H2EQUAL_INTEGER(&vals[6].tp, treap_inc(&vals[5].tp));
   //____helper__print_treap(&tree);
}

H2CASE(treapz, dec)
{
   int i;
   for (i=0; i<7; i++)
      treap_insert(&tree, &vals[i].tp);
   H2EQUAL_INTEGER(&vals[4].tp, treap_dec(&vals[5].tp));
   //____helper__print_treap(&tree);
}

