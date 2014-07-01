#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "listz.h"
#include "bintreez.h"

extern "C" {
int  ____helper__verify_bintree(bintree_tree *tree);
void ____helper__print_bintree(bintree_tree *tree);
}

typedef struct bintree_body_t{
   bintree_head bn;int value;
   struct bintree_body_t *next;
}bintree_body_t;

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

H2SUITE(bintreez)
{ 
public:
   bintree_tree tree;
   bintree_body_t vals[256];

	void setup()
  	{ 
      bintree_create(&tree, &bintree_body_key, &bintree_body_cmp, 0);
      for (int i=0; i<256; i++) {
         bintree_init(&vals[i].bn);
         vals[i].value = i;
      }

  	}

  	void teardown()
  	{  		
  	}  	  	
} ;

H2CASE(bintreez, BINTREE_HEAD_INIT)
{
   bintree_head a = BINTREE_HEAD_INIT(a);

   H2EQUAL_INTEGER(0, a.parent);
   H2EQUAL_INTEGER(0, a.left);
   H2EQUAL_INTEGER(0, a.right);
}

H2CASE(bintreez, bintree_init)
{
   bintree_head a;
   bintree_init(&a);

   H2EQUAL_INTEGER(0, a.parent);
   H2EQUAL_INTEGER(0, a.left);
   H2EQUAL_INTEGER(0, a.right);
}

H2CASE(bintreez, bintree_create)
{
   bintree_tree a;
   bintree_create(&a, &bintree_body_key, &bintree_body_cmp, 1);

   H2EQUAL_INTEGER(0, a.root);
   //H2EQUAL_INTEGER(bintree_body_key, a.key);
   //H2EQUAL_INTEGER(bintree_body_cmp, a.cmp);
   H2EQUAL_INTEGER(1, a.unique);
}

H2CASE(bintreez, insert)
{
   bintree_insert(&tree, &vals[0].bn);
   H2EQUAL_INTEGER(1,____helper__verify_bintree(&tree));
   //____helper__print_bintree(&tree);
}


H2CASE(bintreez, insert_7)
{
   int i;
   for (i=0; i<7; i++)
      bintree_insert(&tree, &vals[i].bn);
   H2EQUAL_INTEGER(1,____helper__verify_bintree(&tree));
   //____helper__print_bintree(&tree);
}


H2CASE(bintreez, delete)
{
   int i;
   for (i=0; i<7; i++)
      bintree_insert(&tree, &vals[i].bn);
   
   bintree_remove(&tree, &vals[0].bn);
   H2EQUAL_INTEGER(1,____helper__verify_bintree(&tree));
   bintree_remove(&tree, &vals[5].bn);
   H2EQUAL_INTEGER(1,____helper__verify_bintree(&tree));
   //____helper__print_bintree(&tree);
}

H2CASE(bintreez, lookup)
{
   int i;
   for (i=0; i<7; i++)
      bintree_insert(&tree, &vals[i].bn);
   
   H2EQUAL_INTEGER(&vals[0].bn, bintree_lookup(&tree, 0));
   H2EQUAL_INTEGER(&vals[1].bn, bintree_lookup(&tree, 1));
   H2EQUAL_INTEGER(&vals[2].bn, bintree_lookup(&tree, 2));
   H2EQUAL_INTEGER(&vals[3].bn, bintree_lookup(&tree, 3));
   H2EQUAL_INTEGER(&vals[4].bn, bintree_lookup(&tree, 4));
   H2EQUAL_INTEGER(&vals[5].bn, bintree_lookup(&tree, 5));
   H2EQUAL_INTEGER(&vals[6].bn, bintree_lookup(&tree, 6));
   H2EQUAL_INTEGER(0, bintree_lookup(&tree, 7));
   //____helper__print_bintree(&tree);
}

H2CASE(bintreez, min)
{
   int i;
   for (i=0; i<7; i++)
      bintree_insert(&tree, &vals[i].bn);
   H2EQUAL_INTEGER(&vals[0].bn, bintree_min(&tree));
   //____helper__print_bintree(&tree);
}

H2CASE(bintreez, max)
{
   int i;
   for (i=0; i<7; i++)
      bintree_insert(&tree, &vals[i].bn);
   H2EQUAL_INTEGER(&vals[6].bn, bintree_max(&tree));
   //____helper__print_bintree(&tree);
}

H2CASE(bintreez, inc)
{
   int i;
   for (i=0; i<7; i++)
      bintree_insert(&tree, &vals[i].bn);
   H2EQUAL_INTEGER(&vals[6].bn, bintree_inc(&vals[5].bn));
   //____helper__print_bintree(&tree);
}

H2CASE(bintreez, dec)
{
   int i;
   for (i=0; i<7; i++)
      bintree_insert(&tree, &vals[i].bn);
   H2EQUAL_INTEGER(&vals[4].bn, bintree_dec(&vals[5].bn));
   //____helper__print_bintree(&tree);
}

H2CASE(bintreez, root)
{
   int i;
   for (i=0; i<7; i++)
      bintree_insert(&tree, &vals[i].bn);
   H2EQUAL_INTEGER(&vals[0].bn, bintree_root(&vals[5].bn));
   //____helper__print_bintree(&tree);
}

H2CASE(bintreez, empty)
{
   H2CHECK(!!bintree_empty(&tree));
   //____helper__print_bintree(&tree);
}

H2CASE(bintreez, notempty)
{
   int i;
   for (i=0; i<7; i++)
      bintree_insert(&tree, &vals[i].bn);
   H2CHECK(!bintree_empty(&tree));
   //____helper__print_bintree(&tree);
}

H2CASE(bintreez, count)
{
   int i;
   for (i=0; i<7; i++)
      bintree_insert(&tree, &vals[i].bn);
   H2EQUAL_INTEGER(7, bintree_count(&vals[0].bn));
   //____helper__print_bintree(&tree);
}


