#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "listz.h"
#include "sptreez.h"

extern "C" {
void ____helper__print_sptree(sptree_tree *tree);
}

typedef struct sptree_body_t{
   sptree_head sp;int value;
   struct sptree_body_t *next;
}sptree_body_t;

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

H2SUITE(sptreez)
{ public:
   sptree_tree tree;
   sptree_body_t vals[7];

   void setup()
   { 
      sptree_create(&tree, &sptree_body_key, &sptree_body_cmp, 0);
      for (int i=0; i<7; i++) {
         sptree_init(&vals[i].sp);
         vals[i].value = i;
      }
   }

   void teardown()
   {  		
   }  	  	
} ;

H2CASE(sptreez, sptree_create)
{
   sptree_tree x;
   sptree_create(&x, &sptree_body_key, &sptree_body_cmp, 0);

   H2EQUAL_INTEGER(0, x.root);
   H2CHECK((sptree_key*)&sptree_body_key == x.key);
   H2CHECK((sptree_cmp*)&sptree_body_cmp == x.cmp);
}

H2CASE(sptreez, sptree_destroy)
{
}

H2CASE(sptreez, SPTREE_HEAD_INIT)
{
   sptree_head x = SPTREE_HEAD_INIT(x);

   H2EQUAL_INTEGER(0, x.left);
   H2EQUAL_INTEGER(0, x.right);
}


H2CASE(sptreez, sptree_insert)
{   
   for (int i=0; i<7; i++) {
      sptree_insert(&tree,&vals[i].sp); 
      H2EQUAL_INTEGER(&vals[i].sp, tree.root);
   }

   //____helper__print_sptree(&tree);
}

H2CASE(sptreez, sptree_insert2)
{   
   for (int i=6; i>=0; i--) {
      sptree_insert(&tree,&vals[i].sp);
      H2EQUAL_INTEGER(&vals[i].sp, tree.root);
   }

   //____helper__print_sptree(&tree);
}

H2CASE(sptreez, sptree_remove)
{
   sptree_insert(&tree,&vals[0].sp); 
   H2EQUAL_INTEGER(&vals[0].sp, tree.root);
   
   sptree_remove(&tree,&vals[0].sp);
   H2EQUAL_INTEGER(0, tree.root);
}

H2CASE(sptreez, sptree_lookup)
{
   sptree_insert(&tree,&vals[0].sp);  
   H2EQUAL_INTEGER(&vals[0].sp, tree.root);
   
   H2EQUAL_INTEGER(&vals[0].sp, sptree_lookup(&tree,vals[0].value));
   H2EQUAL_INTEGER(&vals[0].sp, tree.root);
}

H2CASE(sptreez, sptree_min_max)
{
   sptree_insert(&tree,&vals[4].sp);  
   sptree_insert(&tree,&vals[2].sp); 
   sptree_insert(&tree,&vals[5].sp);  
   sptree_insert(&tree,&vals[1].sp);  
   sptree_insert(&tree,&vals[0].sp);  
   sptree_insert(&tree,&vals[3].sp);  
   sptree_insert(&tree,&vals[6].sp); 
   
   H2EQUAL_INTEGER(vals[0].value, tree.key(sptree_min(&tree)));
   H2EQUAL_INTEGER(vals[6].value, tree.key(sptree_max(&tree)));
   //____helper__print_sptree(&tree);
}


H2CASE(sptreez, sptree_inc_dec)
{
   sptree_insert(&tree,&vals[4].sp);  
   sptree_insert(&tree,&vals[2].sp); 
   sptree_insert(&tree,&vals[5].sp);  
   sptree_insert(&tree,&vals[1].sp);  
   sptree_insert(&tree,&vals[0].sp);  
   sptree_insert(&tree,&vals[3].sp);  
   sptree_insert(&tree,&vals[6].sp); 
   
   H2EQUAL_INTEGER(vals[3].value, tree.key(sptree_inc(&tree, &vals[2].sp)));
   H2EQUAL_INTEGER(vals[4].value, tree.key(sptree_dec(&tree, &vals[5].sp)));
   //____helper__print_sptree(&tree);
}

