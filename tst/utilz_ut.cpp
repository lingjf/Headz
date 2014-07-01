#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "jeffz.h"
#include "utilz.h"


extern "C" {

}


H2SUITE(utilz)
{ 
   void setup()
   { 
   }

   void teardown()
   {  		
   }  	  	
} ;

H2CASE(utilz, FIELD_OFFSET)
{
   struct test_struct {
      int a;
      short b;
      char c;
   };

   test_struct  dd;
   
   H2EQUAL_INTEGER(0, FIELD_OFFSET(struct test_struct, a));
   H2EQUAL_INTEGER(4, FIELD_OFFSET(struct test_struct, b));
   H2EQUAL_INTEGER(6, FIELD_OFFSET(struct test_struct, c));

   H2EQUAL_INTEGER(((int)(&(dd.a)))-((int)(&dd)), FIELD_OFFSET(struct test_struct, a));
   H2EQUAL_INTEGER(((int)(&(dd.b)))-((int)(&dd)), FIELD_OFFSET(struct test_struct, b));
   H2EQUAL_INTEGER(((int)(&(dd.c)))-((int)(&dd)), FIELD_OFFSET(struct test_struct, c));
}

H2CASE(utilz, FIELD_SIZEOF)
{
   struct test_struct {
      int a;
      short b;
      char c;
   };

   H2EQUAL_INTEGER(4, FIELD_SIZEOF(struct test_struct, a));
   H2EQUAL_INTEGER(2, FIELD_SIZEOF(struct test_struct, b));
   H2EQUAL_INTEGER(1, FIELD_SIZEOF(struct test_struct, c));
}

#ifdef __GNUC__

unsigned int return_eip;
unsigned int this_eip;
   
void __stub_this_eip(void)
{
   return_eip = _RETURN_EIP_;
   this_eip = _THIS_EIP_; 
}


void __stub_return_eip(void)
{
   __stub_this_eip();
   this_eip = _THIS_EIP_;
}
/*
H2CASE(utilz, _THIS_EIP_)
{
   __stub_this_eip();
   H2EQUAL_INTEGER(22+(unsigned int)&__stub_this_eip, this_eip);
}

H2CASE(utilz, _RETURN_EIP_)
{
   __stub_return_eip();
   H2CHECK(return_eip == this_eip);
}
*/
H2CASE(utilz, min)
{
	int a, b, x;
	a = 1; b = 2;
	x = min(a,b);
	H2EQUAL_INTEGER(a, x);

	double c, d;
	c = 1.1; d = 2.2;
	H2EQUAL_DOUBLE(c, min(c,d), 0.00001);
}

H2CASE(utilz, max)
{
	int a, b;
	a = 1; b = 2;
	H2EQUAL_INTEGER(b, max(a,b));

	double c, d;
	c = 1.1; d = 2.2;
	H2EQUAL_DOUBLE(d, max(c,d), 0.00001);
}

H2CASE(utilz, swap)
{
	int a, b;
	a = 1; b = 2;
	swap(a,b);
	H2EQUAL_INTEGER(2, a);
	H2EQUAL_INTEGER(1, b);

	double c, d;
	c = 1.1; d = 2.2;
	swap(c,d);
	H2EQUAL_DOUBLE(2.2, c, 0.0001);
	H2EQUAL_DOUBLE(1.1, d, 0.0001);
}
#if 0
#include <unistd.h>

H2CASE(utilz, check)
{
   sleep(1);
   H2CHECK(1==2);
}

H2CASE(utilz, equal_int)
{
   sleep(1);
   H2EQUAL_INTEGER(1, 1);
}

H2CASE(utilz, equal_double)
{
   sleep(1);
   H2EQUAL_DOUBLE(1.1, 2.2, 0.00001);
}

H2CASE(utilz, equal_string)
{
   sleep(1);
   H2EQUAL_STRCMP("abc", "abe");
}
#endif

#endif


