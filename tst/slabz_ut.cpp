#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "listz.h"
#include "slabz.h"
#include "ut_stub.h"
extern "C" {

}

typedef struct my_struct
{
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
} my_struct;

static void my_ctor(void*a, void*b)
{
   my_struct * my = (my_struct*) a;
   my->a = 1;
   my->b = 2;
   my->c = 3;
   my->d = 4.4f;
   my->e = 5.5f;
   my->f = 6;
   my->g = 7;
   my->x = (char*) 8;
   my->y[0] = 'a';
   my->y[1] = 'b';
   my->y[2] = 'c';
   my->y[3] = 'd';
   my->y[4] = '\0';
   my->z = (int*) 9;
}

H2SUITE(slabz)
{
   void setup()
   {
      fault_injection_mem_init(-1);
      slab_initialize();
   }

   void teardown()
   {
      slab_uninitialize();
      fault_injection_mem_init(-1);
      fault_injection_mem_summary();
   }
};

H2CASE(slabz, create_normal)
{
   void * slab = NULL;
   slab = slab_create("my_struct", sizeof(my_struct), 1, 10000, 0, 0, 0);
   H2CHECK(slab != 0);
   H2EQUAL_INTEGER(1,__slab_inside_total_objs(slab));
   H2EQUAL_INTEGER(1,__slab_inside_free_objs(slab));
   H2EQUAL_INTEGER(1,__slab_inside_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_pages(slab));
   H2EQUAL_INTEGER(1,__slab_inside_busy_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_full_pages(slab));
   slab_destroy(slab);
}

H2CASE(slabz, create_lack_memory)
{
   void * slab = NULL;
   /*lack memory of slab head sturcture*/
   fault_injection_mem_init(10);
   slab = slab_create("my_struct", sizeof(my_struct), 1, 10, 0, 0, 0);
   H2EQUAL_INTEGER(0, slab);
   slab_destroy(slab);

   /*lack memory of objects*/
   fault_injection_mem_init(__slab_inside_slab_head_size(0) + 100);
   slab = slab_create("my_struct", sizeof(my_struct), 10, 100, 0, 0, 0);
   H2EQUAL_INTEGER(0, slab);
   slab_destroy(slab);
   fault_injection_mem_init(-1);
}

H2CASE(slabz, kalloc_normal)
{
   my_struct * my;
   void * slab = NULL;
   slab = slab_create("my_struct", sizeof(my_struct), 10, 10000, 0, 0, my_ctor);
   H2CHECK(slab != 0);
   my = (my_struct*) slab_kalloc(slab, 0);

   H2EQUAL_INTEGER(1, my->a);
   H2EQUAL_INTEGER(2, my->b);
   H2EQUAL_INTEGER(3, my->c);
   H2EQUAL_DOUBLE(4.4, my->d, 0.000001);
   H2EQUAL_DOUBLE(5.5, my->e, 0.000001);
   H2EQUAL_INTEGER(6, my->f);
   H2EQUAL_INTEGER(7, my->g);
   H2EQUAL_INTEGER((char*)8, my->x);
   H2EQUAL_STRCMP("abcd", my->y);
   H2EQUAL_INTEGER((int*)9, my->z);

   slab_kfree(slab, my);

   slab_destroy(slab);
}

H2CASE(slabz, kalloc_reuse)
{
   int i;
   my_struct *my[100], *your;
   void * slab = NULL;
   slab = slab_create("my_struct", sizeof(my_struct), 0, 10000, 0, 0, my_ctor);
   H2CHECK(slab != 0);
   for (i = 0; i < 100; i++) {
      my[i] = (my_struct*) slab_kalloc(slab, 0);
      H2CHECK(my[i] != 0);
   }
   for (i = 0; i < 100; i++) {
      slab_kfree(slab, my[i]);
   }

   for (i = 0; i < 1000; i++) {
      your = (my_struct*) slab_kalloc(slab, 0);
      H2CHECK(your != 0);
   }

   slab_destroy(slab);
}

H2CASE(slabz, kalloc_invalid_argument)
{
   my_struct * my;
   void * slab = NULL;
   slab = slab_create("my_struct", sizeof(my_struct), 10, 10000, 0, 0, my_ctor);
   H2CHECK(slab != 0);
   my = (my_struct*) slab_kalloc(0, 0);
   H2EQUAL_INTEGER(0, my);
   slab_kfree(0, 0);
}

H2CASE(slabz, kalloc_lack_memory)
{
   my_struct * my;
   void * slab = NULL;
   slab = slab_create("my_struct", sizeof(my_struct), 0, 10000, 0, 0, my_ctor);
   H2CHECK(slab != 0);
   fault_injection_mem_init(10);

   my = (my_struct*) slab_kalloc(slab, 0);
   H2EQUAL_INTEGER(0, my);
   fault_injection_mem_init(-1);
}

H2CASE(slabz, kalloc_align)
{
   char * my;
   void * slab = NULL;
   slab = slab_create("", 1, 10, 10000, 0, 0, 0);
   H2CHECK(slab != 0);
   H2EQUAL_INTEGER(4+__slab_inside_cell_head_size(slab)+__slab_inside_cell_tail_size(slab),
               __slab_inside_cell_size(slab));
   my = (char*) slab_kalloc(slab, 0);
   slab_destroy(slab);
}

H2CASE(slabz, kalloc_max_limit)
{
   int i;
   char * my;
   void * slab = NULL;
   slab = slab_create("", 1, 10, 1000, 0, 0, 0);
   H2CHECK(slab != 0);
   H2EQUAL_INTEGER(10,__slab_inside_total_objs(slab));
   H2EQUAL_INTEGER(10,__slab_inside_free_objs(slab));
   for (i = 0; i < 1000; i++) {
      my = (char*) slab_kalloc(slab, 0);
      H2CHECK(my != 0);
      H2EQUAL_INTEGER(i+1, __slab_inside_total_objs(slab) - __slab_inside_free_objs(slab));
   }
   my = (char*) slab_kalloc(slab, 0);
   H2CHECK(my == 0);
   slab_destroy(slab);
}

H2CASE(slabz, kalloc_page_flags_with_fixed)
{
   char * my;
   void * slab = NULL;
   slab = slab_create("", 1, 10, 1000, 0, 0, 0);
   H2CHECK(slab != 0);
   H2EQUAL_INTEGER(10,__slab_inside_total_objs(slab));
   H2EQUAL_INTEGER(10,__slab_inside_free_objs(slab));
   my = (char*) slab_kalloc(slab, 0);
   H2CHECK(my != 0);
   H2EQUAL_INTEGER(10,__slab_inside_total_objs(slab));
   H2EQUAL_INTEGER(9,__slab_inside_free_objs(slab));

   H2EQUAL_INTEGER(1,__slab_inside_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_pages(slab));
   H2EQUAL_INTEGER(1,__slab_inside_busy_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_full_pages(slab));

   slab_kfree(slab, my);
   H2EQUAL_INTEGER(10,__slab_inside_total_objs(slab));
   H2EQUAL_INTEGER(10,__slab_inside_free_objs(slab));

   H2EQUAL_INTEGER(1,__slab_inside_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_pages(slab));
   H2EQUAL_INTEGER(1,__slab_inside_busy_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_full_pages(slab));

   slab_destroy(slab);
}

H2CASE(slabz, kalloc_page_flags_without_fixed)
{
   char * my;
   void * slab = NULL;
   slab = slab_create("", 1, 0, 1000, 0, 0, 0);
   H2CHECK(slab != 0);
   H2EQUAL_INTEGER(0,__slab_inside_total_objs(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_objs(slab));

   my = (char*) slab_kalloc(slab, 0);
   H2CHECK(my != 0);
   H2EQUAL_INTEGER(1,__slab_inside_total_objs(slab) - __slab_inside_free_objs(slab));
   H2EQUAL_INTEGER(1,__slab_inside_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_pages(slab));
   H2EQUAL_INTEGER(1,__slab_inside_busy_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_full_pages(slab));

   slab_kfree(slab, my);
   H2EQUAL_INTEGER(0,__slab_inside_total_objs(slab) - __slab_inside_free_objs(slab));
   H2EQUAL_INTEGER(1,__slab_inside_pages(slab));
   H2EQUAL_INTEGER(1,__slab_inside_free_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_busy_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_full_pages(slab));

   slab_destroy(slab);
}

H2CASE(slabz, kalloc_slab_flags_with_reserved)
{
   char * my;
   void * slab = NULL;
   slab = slab_create("", 1, 0, 1000, SLAB_RESERVED, 0, 0);
   H2CHECK(slab != 0);
   H2EQUAL_INTEGER(1000,__slab_inside_total_objs(slab));
   H2EQUAL_INTEGER(1000,__slab_inside_free_objs(slab));

   my = (char*) slab_kalloc(slab, 0);
   H2CHECK(my != 0);
   H2EQUAL_INTEGER(1,__slab_inside_total_objs(slab) - __slab_inside_free_objs(slab));
   H2EQUAL_INTEGER(1,__slab_inside_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_pages(slab));
   H2EQUAL_INTEGER(1,__slab_inside_busy_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_full_pages(slab));

   slab_kfree(slab, my);
   H2EQUAL_INTEGER(0,__slab_inside_total_objs(slab) - __slab_inside_free_objs(slab));
   H2EQUAL_INTEGER(1,__slab_inside_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_pages(slab));
   H2EQUAL_INTEGER(1,__slab_inside_busy_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_full_pages(slab));

   slab_destroy(slab);
}

H2CASE(slabz, kalloc_slab_flags_with_greed)
{
   char * my;
   void * slab = NULL;
   slab = slab_create("", 1, 0, 1000, SLAB_GREED, 0, 0);
   H2CHECK(slab != 0);
   H2EQUAL_INTEGER(0,__slab_inside_total_objs(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_objs(slab));

   my = (char*) slab_kalloc(slab, 0);
   H2CHECK(my != 0);
   H2EQUAL_INTEGER(1,__slab_inside_total_objs(slab) - __slab_inside_free_objs(slab));
   H2EQUAL_INTEGER(1,__slab_inside_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_pages(slab));
   H2EQUAL_INTEGER(1,__slab_inside_busy_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_full_pages(slab));

   slab_kfree(slab, my);
   H2EQUAL_INTEGER(0,__slab_inside_total_objs(slab) - __slab_inside_free_objs(slab));
   H2EQUAL_INTEGER(1,__slab_inside_pages(slab));
   H2EQUAL_INTEGER(1,__slab_inside_free_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_busy_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_full_pages(slab));

   slab_destroy(slab);
}

H2CASE(slabz, kalloc_slab_flags_with_generous)
{
   char * my;
   void * slab = NULL;
   slab = slab_create("", 1, 0, 1000, SLAB_GENEROUS, 0, 0);
   H2CHECK(slab != 0);
   H2EQUAL_INTEGER(0,__slab_inside_total_objs(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_objs(slab));

   my = (char*) slab_kalloc(slab, 0);
   H2CHECK(my != 0);
   H2EQUAL_INTEGER(1,__slab_inside_total_objs(slab) - __slab_inside_free_objs(slab));
   H2EQUAL_INTEGER(1,__slab_inside_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_pages(slab));
   H2EQUAL_INTEGER(1,__slab_inside_busy_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_full_pages(slab));

   slab_kfree(slab, my);
   H2EQUAL_INTEGER(0,__slab_inside_total_objs(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_objs(slab));
   H2EQUAL_INTEGER(0,__slab_inside_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_busy_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_full_pages(slab));

   slab_destroy(slab);
}

H2CASE(slabz, malloc_mfree)
{
   char * my;
   my = (char*) slab_malloc(188);
   H2CHECK(my != 0);
   slab_mfree(my);
}

H2CASE(slabz, shrink_interface)
{
   char * my;
   void * slab = NULL;
   slab = slab_create("", 1, 0, 1000, 0, 0, 0);
   H2CHECK(slab != 0);
   H2EQUAL_INTEGER(0,__slab_inside_total_objs(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_objs(slab));

   my = (char*) slab_kalloc(slab, 0);
   H2CHECK(my != 0);
   H2EQUAL_INTEGER(1,__slab_inside_total_objs(slab) - __slab_inside_free_objs(slab));
   H2EQUAL_INTEGER(1,__slab_inside_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_pages(slab));
   H2EQUAL_INTEGER(1,__slab_inside_busy_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_full_pages(slab));

   slab_kfree(slab, my);
   H2EQUAL_INTEGER(0,__slab_inside_total_objs(slab) - __slab_inside_free_objs(slab));
   H2EQUAL_INTEGER(1,__slab_inside_pages(slab));
   H2EQUAL_INTEGER(1,__slab_inside_free_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_busy_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_full_pages(slab));

   slab_shrink(slab);
   H2EQUAL_INTEGER(0,__slab_inside_total_objs(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_objs(slab));
   H2EQUAL_INTEGER(0,__slab_inside_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_free_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_busy_pages(slab));
   H2EQUAL_INTEGER(0,__slab_inside_full_pages(slab));

   my = (char*) slab_kalloc(slab, 0);
   H2CHECK(my != 0);
   slab_kfree(slab, my);
   slab_shrink(0);
   H2EQUAL_INTEGER(0,__slab_inside_total_objs(slab));

   slab_destroy(slab);
}

H2CASE(slabz, shrink_lack_memory)
{
   int i;
   void *my_obj[101], *your_obj[101];
   void *my_slab, *your_slab;
   fault_injection_mem_init(10000);
   my_slab = slab_create("my_slab", 100, 0, 10000, SLAB_GREED, 0, 0);
   H2CHECK(my_slab != 0);

   your_slab = slab_create("your_slab", 100, 0, 10000, SLAB_GREED, 0, 0);
   H2CHECK(your_slab != 0);

   memset(my_obj, 0, sizeof(my_obj));
   memset(your_obj, 0, sizeof(your_obj));

   my_obj[0] = slab_kalloc(my_slab, 0);
   H2CHECK(my_obj[0] != 0);
   for (i = 1; i < 100; i++) {
      my_obj[i] = slab_kalloc(my_slab, 0);
   }
   my_obj[i] = slab_kalloc(my_slab, 0);
   H2CHECK(my_obj[i] == 0);

   your_obj[0] = slab_kalloc(your_slab, 0);
   H2CHECK(your_obj[0] == 0);

   for (i = 0; i < 100; i++) {
      if (my_obj[i])
         slab_kfree(my_slab, my_obj[i]);
   }

   for (i = 0; i < 50; i++) {
      your_obj[i] = slab_kalloc(your_slab, 0);
      H2CHECK(your_obj[i] != 0);
   }

   slab_destroy(my_slab);
   slab_destroy(your_slab);
}

H2CASE(slabz, aggress)
{
   char * my;
   void * slab = NULL;
   slab = slab_create("", 4, 10, 10000, 0, 0, 0);
   H2CHECK(slab != 0);
   my = (char*) slab_kalloc(slab, 0);
   H2CHECK(my != 0);
   *(my + 0) = 'a';
   *(my + 1) = 'b';
   *(my + 2) = 'c';
   *(my + 3) = 'd';
   *(my + 4) = 'e';
   slab_kfree(slab, my);
   slab_destroy(slab);
}

