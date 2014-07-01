#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "listz.h"
#include "slakz.h"

extern "C" {

}


typedef struct my_struct{
   int a; 
   long b;
   unsigned int c;
   float d;
   double e;
   unsigned char f;
   short g;
   char * x;
   char y[10];
   int * z;
}my_struct;

H2SUITE(slakz)
{ 
	void setup()
  	{ 
  	   slak_initialize();
  	}

  	void teardown()
  	{  	
  	   slak_uninitialize();
  	}  	  	
} ;


H2CASE(slakz, create)
{
   void * slak = NULL;
   slak = slak_create(0, 0);
   H2CHECK(slak != 0);
   
   H2EQUAL_INTEGER(0, __slak_inside_flags(slak));
   H2EQUAL_INTEGER(1, __slak_inside_pages(slak));
   H2EQUAL_INTEGER(1, __slak_inside_cells(slak));
   H2EQUAL_INTEGER(0, __slak_inside_last_obj_size(slak));
   
   slak_destroy(slak);
} 

H2CASE(slakz, create_conflict_flags)
{
   void * slak = NULL;
   slak = slak_create(100, SLAK_GREED|SLAK_GENEROUS);
   H2EQUAL_INTEGER(0, slak);
   slak_destroy(slak);
} 
H2CASE(slakz, malloc_free_basic)
{
   my_struct * my;
   void * slak = NULL;
   slak = slak_create(0, 0);
   H2CHECK(slak != 0);
   my = (my_struct*) slak_malloc(slak, sizeof(my_struct)); 
   H2CHECK(my != 0); 
   H2EQUAL_INTEGER(0, __slak_inside_flags(slak));
   H2EQUAL_INTEGER(1, __slak_inside_pages(slak));
   H2EQUAL_INTEGER(2, __slak_inside_cells(slak));
   H2EQUAL_INTEGER(sizeof(my_struct), __slak_inside_last_obj_size(slak));
   slak_mfree(slak, my); 
   H2EQUAL_INTEGER(1, __slak_inside_pages(slak));
   H2EQUAL_INTEGER(1, __slak_inside_cells(slak));
   H2EQUAL_INTEGER(0, __slak_inside_last_obj_size(slak));
   slak_destroy(slak);
} 

H2CASE(slakz, malloc_frees)
{
   int i;
   const int M = 256;
   const int C = 1024;
   void * my[M];
   void * slak = NULL;
   slak = slak_create(0, 0);
   H2CHECK(slak != 0);

   for (i=0; i<M; i++) {
      my[i] = slak_malloc(slak, C);
      H2CHECK(my[i] != 0);
      H2EQUAL_INTEGER(i+2, __slak_inside_cells(slak));
      H2EQUAL_INTEGER(C, __slak_inside_last_obj_size(slak));
   }

   H2EQUAL_INTEGER(2, __slak_inside_pages(slak));
   
   for (i=M-1; i>=0; i--) {
      H2EQUAL_INTEGER(C, __slak_inside_last_obj_size(slak));
      slak_mfree(slak, my[i]);
      H2EQUAL_INTEGER(i+1, __slak_inside_cells(slak)); 
   }
   
   slak_destroy(slak);
} 

H2CASE(slakz, malloc_free_more)
{
   int i;
   const int M = 256;
   const int C = 1024;
   void * my[M];
   void * slak = NULL;
   slak = slak_create(0, 0);
   H2CHECK(slak != 0);

   for (i=0; i<M; i++) {
      my[i] = slak_malloc(slak, C);
      H2CHECK(my[i] != 0);
      H2EQUAL_INTEGER(i+2, __slak_inside_cells(slak));
      H2EQUAL_INTEGER(C, __slak_inside_last_obj_size(slak));
   }

   H2EQUAL_INTEGER(2, __slak_inside_pages(slak));

   slak_mfree(slak, my[100]);
   H2EQUAL_INTEGER(101, __slak_inside_cells(slak)); 
   
   slak_destroy(slak);
}
H2CASE(slakz, malloc_free_lack)
{
   void * my;
   void * slak = NULL;
   slak = slak_create(0, 0);
   H2CHECK(slak != 0);
   my = slak_malloc(slak, 300*1024); 
   H2CHECK(my != 0); 
   H2EQUAL_INTEGER(0, __slak_inside_flags(slak));
   H2EQUAL_INTEGER(2, __slak_inside_pages(slak));
   H2EQUAL_INTEGER(2, __slak_inside_cells(slak));
   H2EQUAL_INTEGER(300*1024, __slak_inside_last_obj_size(slak));
   slak_mfree(slak, my); 
   slak_destroy(slak);
} 

H2CASE(slakz, malloc_free_flags_with_greed)
{
   int i;
   const int M = 999;
   const int C = 1024-16;
   void * my[M];
   void * slak = NULL;
   slak = slak_create(0, SLAK_GREED);
   H2CHECK(slak != 0);

   for (i=0; i<M; i++) {
      my[i] = slak_malloc(slak, C);
      H2CHECK(my[i] != 0);
      H2EQUAL_INTEGER(i+2, __slak_inside_cells(slak));
      H2EQUAL_INTEGER(C, __slak_inside_last_obj_size(slak));
   }

   for (i=M-1; i>=0; i--) {
      H2EQUAL_INTEGER(C, __slak_inside_last_obj_size(slak));
      slak_mfree(slak, my[i]);
      H2EQUAL_INTEGER(i+1, __slak_inside_cells(slak)); 
   }
  
   double xn = (C+__slak_inside_cell_head_size(slak))*M/(255.0*1024.0);
   H2EQUAL_INTEGER((int)ceil(xn), __slak_inside_pages(slak)); 
   
   slak_destroy(slak);
}

H2CASE(slakz, malloc_free_flags_with_generous)
{
   int i;
   const int M = 999;
   const int C = 1024-16;
   void * my[M];
   void * slak = NULL;
   slak = slak_create(0, SLAK_GENEROUS);
   H2CHECK(slak != 0);

   for (i=0; i<M; i++) {
      my[i] = slak_malloc(slak, C);
      H2CHECK(my[i] != 0);
      H2EQUAL_INTEGER(i+2, __slak_inside_cells(slak));
      H2EQUAL_INTEGER(C, __slak_inside_last_obj_size(slak));
   }
   
   for (i=M-1; i>=0; i--) {
      H2EQUAL_INTEGER(C, __slak_inside_last_obj_size(slak));
      slak_mfree(slak, my[i]);
      H2EQUAL_INTEGER(i+1, __slak_inside_cells(slak)); 
   }
  
   H2EQUAL_INTEGER(1, __slak_inside_pages(slak)); 
   
   slak_destroy(slak);
}

H2CASE(slakz, malloc_free_flags_with_buffer1)
{
   int i;
   const int M = 999;
   const int C = 1024-16;
   void * my[M];
   void * slak = NULL;
   slak = slak_create(0, 0);
   H2CHECK(slak != 0);

   for (i=0; i<M; i++) {
      my[i] = slak_malloc(slak, C);
      H2CHECK(my[i] != 0);
      H2EQUAL_INTEGER(i+2, __slak_inside_cells(slak));
      H2EQUAL_INTEGER(C, __slak_inside_last_obj_size(slak));
   }

   for (i=M-1; i>=0; i--) {
      H2EQUAL_INTEGER(C, __slak_inside_last_obj_size(slak));
      slak_mfree(slak, my[i]);
      H2EQUAL_INTEGER(i+1, __slak_inside_cells(slak)); 
   }
  
   H2EQUAL_INTEGER(2, __slak_inside_pages(slak)); 
   
   slak_destroy(slak);
}


H2CASE(slakz, shrink)
{
   int i;
   const int M = 999;
   
   void * my[M];
   void * slak = NULL;
   slak = slak_create(0, SLAK_GREED);
   H2CHECK(slak != 0);

   int C = 1024-__slak_inside_cell_head_size(slak);
   for (i=0; i<M; i++) {
      my[i] = slak_malloc(slak, C);
      H2CHECK(my[i] != 0);
      H2EQUAL_INTEGER(i+2, __slak_inside_cells(slak));
      H2EQUAL_INTEGER(C, __slak_inside_last_obj_size(slak));
   }

   for (i=M-1; i>=0; i--) {
      H2EQUAL_INTEGER(C, __slak_inside_last_obj_size(slak));
      slak_mfree(slak, my[i]);
      H2EQUAL_INTEGER(i+1, __slak_inside_cells(slak)); 
   }
  
   double xn = (C+__slak_inside_cell_head_size(slak))*M/(255.0*1024.0);
   H2EQUAL_INTEGER((int)ceil(xn), __slak_inside_pages(slak)); 

   slak_shrink(slak);
   H2EQUAL_INTEGER(1, __slak_inside_pages(slak));
   slak_destroy(slak);
}


