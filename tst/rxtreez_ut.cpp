#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "listz.h"
#include "rxtreez.h"

extern "C" {
void ____helper__print_rxtree(rxtree_node * p);
}


static int __how_many_1bits(unsigned long x)
{
   int n = 0;
   while (x) {
      x = x & (x - 1);
      n++;
   }
   return n;
}

static int __helper__verify_rxtree(rxtree_node * p)
{
   if (!p)
      return 0;
   // 0 or 2 sons
   int s = 0;
   s += p->son[0] ? 1:0;
   s += p->son[1] ? 1:0;
   if (s == 1)
      return 1;

   //internal or leaf node
   if (s == 0) {
      if (p->mask != 0xFFFFFFFF)
         return 1;
   } else {
      if (p->mask == 0xFFFFFFFF)
         return 1;
   }

   //mask
   if ((31-p->srt)+p->len != __how_many_1bits(p->mask))
      return 1;

   for (int i=0; i < 2; i++) {
      rxtree_node * k = p->son[i];
      if (k) {
         if (!!((1<<k->srt) & k->path) != i)
            return 1;
         if ((k->path&p->mask) != p->path)
            return 1;
         if (__how_many_1bits(k->mask^p->mask) !=k->len)
            return 1;
      }
   }

   //sons
   int r;
   r = __helper__verify_rxtree(p->son[0]);
   if (r)
      return r;
   r = __helper__verify_rxtree(p->son[1]);
   if (r)
      return r;
   
   return 0;
}

H2SUITE(rxtreet)
{ 
	void setup()
  	{ 
  	}

  	void teardown()
  	{  		
  	}  	  	
} ;

H2CASE(rxtreet, RXTREE_HEAD_INIT)
{
   rxtree_node x = RXTREE_HEAD_INIT(x);

   H2EQUAL_INTEGER(0, x.path);
   H2EQUAL_INTEGER(0, x.mask);
   H2EQUAL_INTEGER(0, x.srt); 
   H2EQUAL_INTEGER(0, x.len);
   H2EQUAL_INTEGER(0, x.usr);
   H2EQUAL_INTEGER(0, x.son[0]);
   H2EQUAL_INTEGER(0, x.son[1]);
} 

#if 0
H2CASE(rxtreet, __node_match)
{
   struct test_struct {
   	rxtree_node   node;
	unsigned long index;
	int           expected_return;
	int           expected_diff;
   }x[] =  {
		{{ 0,             0,      31,0 ,0,{0,0} }, 0xff,       1, 31},
		{{ 0,             0,      31,0 ,0,{0,0} }, 0x9f,       1, 31},
		{{ 0xA0000000,0xF0000000, 31,4 ,0,{0,0} }, 0xA1230123, 1, 27},
		{{ 0xA0000000,0xF0000000, 31,4 ,0,{0,0} }, 0xC1230123, 0, 30},
		{{ 0xAAA00000,0xFFF00000, 31,12,0,{0,0} }, 0xAAA30123, 1, 19},
		{{ 0xAAA00000,0xFFF00000, 31,12,0,{0,0} }, 0xABA30123, 0, 24},
		{{ 0xAAA0000A,0xFFFFFFFE, 4, 4 ,0,{0,0} }, 0xAAA0000A, 1, 0 },
		{{ 0xAAA0000A,0xFFFFFFFE, 4, 4 ,0,{0,0} }, 0xAAA0000B, 1, 0 },
		{{ 0xAAA0000A,0xFFFFFFFE, 4, 4 ,0,{0,0} }, 0xAAA00008, 0, 1 },
		{{ 0xAAA0000A,0xFFFFFFFF, 4, 5 ,0,{0,0} }, 0xAAA0000A, 1, -1},
		{{ 0xAAA0000A,0xFFFFFFFF, 4, 5 ,0,{0,0} }, 0xAAA0000B, 0, 0 },
   };
   for (int i=0; i<sizeof(x)/sizeof(x[0]); i++) {
   		int  actual_return;
		int  actual_diff;
          extern int __node_match(rxtree_node *p, unsigned long index, int *diff);

		actual_return = __node_match(&x[i].node, x[i].index, &actual_diff);
		H2EQUAL_INTEGER(x[i].expected_return, actual_return);
		H2EQUAL_INTEGER(x[i].expected_diff, actual_diff);
   }
} 
#endif

H2CASE(rxtreet, rxtree_lookup)
{
	rxtree_node   b = { 0xA0000000,0xFFFFFFFF, 27,28 ,(void*)0x2,{0,0} };
	rxtree_node   c = { 0xA8000001,0xFFFFFFFF, 27,28 ,(void*)0x3,{0,0} };
	rxtree_node   a = { 0xA0000000,0xF0000000, 31,4  ,(void*)0x1,{&b,&c} };

	H2EQUAL_INTEGER((void*)0x2, rxtree_lookup(&a, 0xA0000000));
	H2EQUAL_INTEGER((void*)0x3, rxtree_lookup(&a, 0xA8000001));
}


H2CASE(rxtreet, rxtree_insert)
{
   unsigned int i;
   rxtree_node a;
   rxtree_init(&a);
   unsigned long ip[] =
   {
      0x10000000, 0x20000000, 0x30000000, 0x40000000, 0x50000000, 0x60000000, 0x70000000, 0x80000000, 0x90000000, 0xA0000000, 0xB0000000, 0xC0000000, 0xD0000000, 0xE0000000, 0xC0000000,
      0x01000000, 0x02000000, 0x03000000, 0x04000000, 0x05000000, 0x06000000, 0x07000000, 0x08000000, 0x09000000, 0x0A000000, 0x0B000000, 0x0C000000, 0x0D000000, 0x0E000000, 0x0C000000,   
   };

   for (i=0; i<sizeof(ip)/4; i++) 
      rxtree_insert(&a, ip[i], (void*)ip[i]);

   //____helper__print_rxtree(&a);

   for (i=0; i<sizeof(ip)/4; i++) 
    H2EQUAL_INTEGER((void*)ip[i], rxtree_lookup(&a, ip[i])); 

   H2EQUAL_INTEGER(0, __helper__verify_rxtree(&a));

   for (i=0; i<sizeof(ip)/4; i++) 
      rxtree_remove(&a, ip[i]);
}

#if 0
H2CASE(rxtreet, rxtree_insert_mass)
{
   int i, j;
   rxtree_node a;
   rxtree_init(&a);
   
   for (i=0; i<1000; i+=7) for (j=0; j<3000; j+=13) {
      unsigned long value = (i<<16) + j;
      rxtree_insert(&a, value, (void*)value);
   }
 
   for (i=0; i<1000; i+=7) for (j=0; j<3000; j+=13) {
      unsigned long value = (i<<16) + j;
      H2EQUAL_INTEGER((void*)value, rxtree_lookup(&a, value)); 
      H2EQUAL_INTEGER((void*)0x0, rxtree_lookup(&a, value+1)); 
   }   

   H2EQUAL_INTEGER(0, __helper__verify_rxtree(&a));

   for (i=0; i<1000; i+=7) for (j=0; j<3000; j+=13) {
      unsigned long value = (i<<16) + j;
      rxtree_remove(&a, value);
   }
}
#endif

H2CASE(rxtreet, rxtree_remove)
{
   unsigned int i;
   rxtree_node a;
   rxtree_init(&a);
   unsigned long ip[] =
   {
      0x10000000, 0x20000000, 0x01000000, 0x02000000, 
   };

   for (i=0; i<sizeof(ip)/4; i++) {
      rxtree_insert(&a, ip[i], (void*)ip[i]);
      H2EQUAL_INTEGER(0, __helper__verify_rxtree(&a));
      //____helper__print_rxtree(&a);
   }

   for (i=0; i<sizeof(ip)/4; i++) 
    H2EQUAL_INTEGER((void*)ip[i], rxtree_lookup(&a, ip[i])); 

   //____helper__print_rxtree(&a);
   
   for (i=0; i<sizeof(ip)/4; i++)  {
      rxtree_remove(&a, ip[i]);
      H2EQUAL_INTEGER(0, __helper__verify_rxtree(&a));
      //____helper__print_rxtree(&a);
   }
   
   for (i=0; i<sizeof(ip)/4; i++) 
    H2EQUAL_INTEGER((void*)0, rxtree_lookup(&a, ip[i])); 

   H2EQUAL_INTEGER(0, __helper__verify_rxtree(&a));
   //____helper__print_rxtree(&a);

}


H2CASE(rxtreet, rxtree_delete_0x00000000)
{
   rxtree_node a;
   rxtree_init(&a);
   H2EQUAL_INTEGER(0, __helper__verify_rxtree(&a));
   H2EQUAL_INTEGER((void*)0x00000000, rxtree_lookup(&a, 0x00000000)); 
   
   rxtree_insert(&a, 0x00000000, (void*)0xABCD1234);
   H2EQUAL_INTEGER(0, __helper__verify_rxtree(&a));
   H2EQUAL_INTEGER((void*)0xABCD1234, rxtree_lookup(&a, 0x00000000)); 
   
   rxtree_remove(&a, 0x00000000);
   H2EQUAL_INTEGER(0, __helper__verify_rxtree(&a));
   H2EQUAL_INTEGER((void*)0x00000000, rxtree_lookup(&a, 0x00000000)); 
}

