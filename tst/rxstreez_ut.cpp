#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "listz.h"
#include "rxstreez.h"

extern "C" {
int  ____helper__verify_rxstree(void* tree);
}


H2SUITE(rxstreez)
{ 
   void *tree;
	void setup()
  	{ 
  	   tree = rxstree_create(1);
  	}

  	void teardown()
  	{  
  	   rxstree_destroy(tree);
  	}  	  	
} ;

H2CASE(rxstreez, create_destroy)
{
   void *x = rxstree_create(1);
   H2CHECK(x!=NULL);
   rxstree_destroy(x);
} 

H2CASE(rxstreez, insert_lookup)
{
   unsigned int i;
   char * src[] = 
   {
      "romane",
      "romanus",
      "romulus",
      "rubens",
      "ruber",
      "rubicon",
      "rubicundus"
   };

   for (i=0; i<sizeof(src)/sizeof(src[0]); i++)
      H2EQUAL_INTEGER((int)src[i], (int)rxstree_insert(tree, src[i], src[i]));

   H2EQUAL_INTEGER(1, ____helper__verify_rxstree(tree));
   
   for (i=0; i<sizeof(src)/sizeof(src[0]); i++)
      H2EQUAL_STRCMP(src[i], (char*)rxstree_lookup(tree, src[i]));
      
}


H2CASE(rxstreez, remove_lookup)
{
   unsigned int i,j;
   char * src[] = 
   {
      "a",
      "ab",
      "abc",
      "abcd",
      "abcde",
      "abcdef",
      "abcdefg"
   };

   for (i=0; i<sizeof(src)/sizeof(src[0]); i++)
      H2EQUAL_INTEGER((int)src[i], (int)rxstree_insert(tree, src[i], src[i]));

   H2EQUAL_INTEGER(1, ____helper__verify_rxstree(tree));
   
   for (i=0; i<sizeof(src)/sizeof(src[0]); i++)
      H2EQUAL_STRCMP(src[i], (char*)rxstree_lookup(tree, src[i]));

   for (i=0; i<sizeof(src)/sizeof(src[0]); i++) { 
      H2EQUAL_STRCMP(src[i], (char*)rxstree_remove(tree, src[i]));
      for (j=i+1; j<sizeof(src)/sizeof(src[0]); j++) { 
         H2EQUAL_STRCMP(src[j], (char*)rxstree_lookup(tree, src[j]));
      }
   }
}

