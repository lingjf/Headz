#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "listz.h"
#include "sbtreez.h"

extern "C" {
int  ____helper__verify_sbtree(sbtree_tree *tree);
void ____helper__print_sbtree(sbtree_tree *tree);
}

typedef struct sbtree_body_t{
   sbtree_head sb;int value;
   struct sbtree_body_t *next;
}sbtree_body_t;

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


H2SUITE(sbtreez)
{
   sbtree_tree tree;
   sbtree_body_t vals[256];

   void setup()
   { 
      sbtree_create(&tree, &sbtree_body_key, &sbtree_body_cmp, 1);
      for (int i=0; i<256; i++) {
         sbtree_init(&vals[i].sb);
         vals[i].value = i;
      }
   }

   void teardown()
   {  		
   }  	  	
} ;

H2CASE(sbtreez, sbtree_create)
{
   sbtree_tree x;
   sbtree_create(&x, &sbtree_body_key, &sbtree_body_cmp, 1);

   H2EQUAL_INTEGER(0, x.root);
   H2CHECK((sbtree_key*)&sbtree_body_key == x.key);
   H2CHECK((sbtree_cmp*)&sbtree_body_cmp == x.cmp);
   H2EQUAL_INTEGER(1, x.unique);
}

H2CASE(sbtreez, sbtree_destroy)
{
}

H2CASE(sbtreez, SBTREE_HEAD_INIT)
{
   sbtree_head x = SBTREE_HEAD_INIT(x);

   H2EQUAL_INTEGER(1, x.size);
   H2EQUAL_INTEGER(0, x.parent);
   H2EQUAL_INTEGER(0, x.ch[0]);
   H2EQUAL_INTEGER(0, x.ch[1]);
}

H2CASE(sbtreez, sbtree_insert)
{   
   for (int i=0; i<256; i++) {
      sbtree_insert(&tree, &vals[i].sb); 
   }
   H2EQUAL_INTEGER(1, ____helper__verify_sbtree(&tree));
   //____helper__print_sbtree(&tree);
}

H2CASE(sbtreez, sbtree_insert2)
{   
   for (int i=255; i>=0; i--) {
      sbtree_insert(&tree, &vals[i].sb); 
   }

   H2EQUAL_INTEGER(1, ____helper__verify_sbtree(&tree));
   //____helper__print_sbtree(&tree);
}

H2CASE(sbtreez, select3)
{
   sbtree_insert(&tree,&vals[4].sb); 
   sbtree_insert(&tree,&vals[2].sb); 
   sbtree_insert(&tree,&vals[5].sb); 
   sbtree_insert(&tree,&vals[1].sb); 
   sbtree_insert(&tree,&vals[0].sb); 
   sbtree_insert(&tree,&vals[3].sb); 
   sbtree_insert(&tree,&vals[6].sb); 

   H2EQUAL_INTEGER(1, ____helper__verify_sbtree(&tree));
   //____helper__print_sbtree(&tree);
}
H2CASE(sbtreez, lookup)
{
   int i;
   for (i=0; i<7; i++)
      sbtree_insert(&tree, &vals[i].sb);
   
   H2EQUAL_INTEGER(&vals[0].sb, sbtree_lookup(&tree, 0));
   H2EQUAL_INTEGER(&vals[1].sb, sbtree_lookup(&tree, 1));
   H2EQUAL_INTEGER(&vals[2].sb, sbtree_lookup(&tree, 2));
   H2EQUAL_INTEGER(&vals[3].sb, sbtree_lookup(&tree, 3));
   H2EQUAL_INTEGER(&vals[4].sb, sbtree_lookup(&tree, 4));
   H2EQUAL_INTEGER(&vals[5].sb, sbtree_lookup(&tree, 5));
   H2EQUAL_INTEGER(&vals[6].sb, sbtree_lookup(&tree, 6));
   H2EQUAL_INTEGER(0, sbtree_lookup(&tree, 7));
   //____helper__print_sbtree(&tree);
}

H2CASE(sbtreez, remove)
{
   int i;
   for (i=0; i<7; i++)
      sbtree_insert(&tree, &vals[i].sb);
   sbtree_remove(&tree, &vals[0].sb);
   sbtree_remove(&tree, &vals[1].sb);
   sbtree_remove(&tree, &vals[2].sb);

   vals[1].value = 9;
   sbtree_insert(&tree, &vals[1].sb);
   //____helper__print_sbtree(&tree); 
   H2EQUAL_INTEGER(1, ____helper__verify_sbtree(&tree));
} 

H2CASE(sbtreez, sbtree_min_max)
{
   sbtree_insert(&tree,&vals[4].sb); 
   sbtree_insert(&tree,&vals[2].sb); 
   sbtree_insert(&tree,&vals[5].sb); 
   sbtree_insert(&tree,&vals[1].sb); 
   sbtree_insert(&tree,&vals[0].sb); 
   sbtree_insert(&tree,&vals[3].sb); 
   sbtree_insert(&tree,&vals[6].sb); 
   
   H2EQUAL_INTEGER(vals[0].value, tree.key(sbtree_min(&tree)));
   H2EQUAL_INTEGER(vals[6].value, tree.key(sbtree_max(&tree)));
   //____helper__print_sbtree(&tree);
}

H2CASE(sbtreez, sbtree_inc_dec)
{
   sbtree_insert(&tree,&vals[4].sb); 
   sbtree_insert(&tree,&vals[2].sb); 
   sbtree_insert(&tree,&vals[5].sb); 
   sbtree_insert(&tree,&vals[1].sb); 
   sbtree_insert(&tree,&vals[0].sb); 
   sbtree_insert(&tree,&vals[3].sb); 
   sbtree_insert(&tree,&vals[6].sb); 
   
   H2EQUAL_INTEGER(vals[3].value, tree.key(sbtree_inc( &vals[2].sb)));
   H2EQUAL_INTEGER(vals[4].value, tree.key(sbtree_dec( &vals[5].sb)));
   //____helper__print_sbtree(&tree);
}

H2CASE(sbtreez, inc)
{
   int i;
   for (i=0; i<256; i++)
      sbtree_insert(&tree, &vals[i].sb);

   H2EQUAL_INTEGER(1, ____helper__verify_sbtree(&tree));
   //____helper__print_sbtree(&tree);

   for (i=0; i<255; i++)
      H2EQUAL_INTEGER(vals[i+1].value, tree.key(sbtree_inc( &vals[i].sb)));

   H2EQUAL_INTEGER(0, sbtree_inc(&vals[255].sb));
}

H2CASE(sbtreez, dec)
{
   int i;
   for (i=0; i<256; i++)
      sbtree_insert(&tree, &vals[i].sb);

   H2EQUAL_INTEGER(1, ____helper__verify_sbtree(&tree));
   //____helper__print_sbtree(&tree);

   for (i=1; i<256; i++)
      H2EQUAL_INTEGER(vals[i-1].value, tree.key(sbtree_dec( &vals[i].sb)));

   H2EQUAL_INTEGER(0, sbtree_dec(&vals[0].sb));
}

H2CASE(sbtreez, select)
{
   int i;
   for (i=0; i<256; i++)
      sbtree_insert(&tree, &vals[i].sb);

   H2EQUAL_INTEGER(1, ____helper__verify_sbtree(&tree));
   //____helper__print_sbtree(&tree);

   for (i=0; i<256; i++)
      H2EQUAL_INTEGER(vals[i].value, sbtree_body_key(sbtree_select(&tree, i)));
}

H2CASE(sbtreez, rank)
{
   int i;
   for (i=0; i<15; i++)
      sbtree_insert(&tree, &vals[i].sb);

   H2EQUAL_INTEGER(1, ____helper__verify_sbtree(&tree));
   //____helper__print_sbtree(&tree);

   for (i=0; i<15; i++)
      H2EQUAL_INTEGER(i, sbtree_rank(&tree, &vals[i].sb)); 
}   

