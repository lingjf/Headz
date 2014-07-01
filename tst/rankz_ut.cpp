#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "jeffz.h"
#include "rankz.h"


extern "C" {

}


H2SUITE(rankz)
{ 
   void setup()
   { 
   }

   void teardown()
   {  		
   }  	  	
} ;

H2CASE(rankz, Pnm)
{
   H2EQUAL_INTEGER(1, Pnm(0,0));
   H2EQUAL_INTEGER(1, Pnm(3,0));
	H2EQUAL_INTEGER(3, Pnm(3,1));
   H2EQUAL_INTEGER(6, Pnm(3,2));
   H2EQUAL_INTEGER(6, Pnm(3,3));
   H2EQUAL_INTEGER(0, Pnm(3,4));
}

H2CASE(rankz, Ann)
{
   H2EQUAL_INTEGER(1, Ann(0));
   H2EQUAL_INTEGER(1, Ann(1));
   H2EQUAL_INTEGER(2, Ann(2));
   H2EQUAL_INTEGER(6, Ann(3));
}

H2CASE(rankz, Cnm)
{
   H2EQUAL_INTEGER(1, Cnm(0,0));
   H2EQUAL_INTEGER(1, Cnm(3,0));
	H2EQUAL_INTEGER(3, Cnm(3,1));
   H2EQUAL_INTEGER(3, Cnm(3,2));
   H2EQUAL_INTEGER(1, Cnm(3,3));
   H2EQUAL_INTEGER(0, Cnm(3,4));
}

void __callback(int a[], int n, int idx)
{
   int b[12];
   idx_2_seq(b, n, idx);
   H2EQUAL_INTEGER(0, memcmp(a,b,n));
}


H2CASE(rankz, seq_idx_gen)
{
	int i, a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
   
	for (i=3; i<10; i++) {
		seq_gen(a, i, 0, __callback);
   }
}


