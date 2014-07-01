#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "jeffz.h"
#include "utilz.h"
#include "bitopz.h"
#include "bitmapz.h"

extern "C" {

}


H2SUITE(bitmapz)
{ 
   void setup()
   { 
   }

   void teardown()
   {  		
   }  	  	
} ;

H2CASE(bitmapz, __bitmap_empty0)
{ 
   unsigned char buff[4];
   
   memset(buff, 0, sizeof(buff)); 
   H2EQUAL_INTEGER(1, __bitmap_empty((unsigned long*)buff, 0));
   memset(buff, 0xff, sizeof(buff));
   H2EQUAL_INTEGER(1, __bitmap_empty((unsigned long*)buff, 0)); 
}

H2CASE(bitmapz, __bitmap_empty1)
{ 
   unsigned char buff[4];
   memset(buff, 0, sizeof(buff));

   for (unsigned int i=1; i<=sizeof(buff)*8; i++)
      H2EQUAL_INTEGER(1, __bitmap_empty((unsigned long*)buff, i));
}

H2CASE(bitmapz, __bitmap_empty2)
{ 
   unsigned char buff[4];
   memset(buff, 0xff, sizeof(buff));

   for (unsigned int i=1; i<=sizeof(buff)*8; i++)
      H2EQUAL_INTEGER(0, __bitmap_empty((unsigned long*)buff, i));
}

H2CASE(bitmapz, __bitmap_empty3)
{ 
   unsigned char buff[4];
   memset(&buff[0], 0x00, 2);
   memset(&buff[2], 0xff, 2);

   for (unsigned int i=1; i<=2*8; i++)
      H2EQUAL_INTEGER(1, __bitmap_empty((unsigned long*)buff, i));

   for (unsigned int j=17; j<=4*8; j++)
      H2EQUAL_INTEGER(0, __bitmap_empty((unsigned long*)buff, j));
}

H2CASE(bitmapz, __bitmap_full0)
{ 
   unsigned char buff[4];
   
   memset(buff, 0, sizeof(buff)); 
   H2EQUAL_INTEGER(1, __bitmap_full((unsigned long*)buff, 0));
   memset(buff, 0xff, sizeof(buff));
   H2EQUAL_INTEGER(1, __bitmap_full((unsigned long*)buff, 0)); 
}

H2CASE(bitmapz, __bitmap_full1)
{ 
   unsigned char buff[4];
   memset(buff, 0, sizeof(buff));

   for (unsigned int i=1; i<=sizeof(buff)*8; i++)
      H2EQUAL_INTEGER(0, __bitmap_full((unsigned long*)buff, i));
}

H2CASE(bitmapz, __bitmap_full2)
{ 
   unsigned char buff[4];
   memset(buff, 0xff, sizeof(buff));

   for (unsigned int i=1; i<=sizeof(buff)*8; i++)
      H2EQUAL_INTEGER(1, __bitmap_full((unsigned long*)buff, i));
}

H2CASE(bitmapz, __bitmap_full3)
{ 
   unsigned char buff[4];
   buff[0] = 0xFF;
   buff[1] = 0x00;
   buff[2] = 0xFF;
   buff[3] = 0x00;

   for (unsigned int i=1; i<=8; i++)
      H2EQUAL_INTEGER(1, __bitmap_full((unsigned long*)buff, i));

   for (unsigned int j=9; j<=32; j++)
      H2EQUAL_INTEGER(0, __bitmap_full((unsigned long*)buff, j));
}

H2CASE(bitmapz, __bitmap_weight0)
{ 
   unsigned long x[2];
   x[0] = 0;
   x[1] = 0;
   for (unsigned int i=0; i<=64; i++)
      H2EQUAL_INTEGER(0, __bitmap_weight((unsigned long*)&x, i));
}

H2CASE(bitmapz, __bitmap_weight1)
{ 
   unsigned long x[2];
   x[0] = 0xFFFFFFFF;
   x[1] = 0xFFFFFFFF;
   for (unsigned int i=0; i<=64; i++)
      H2EQUAL_INTEGER(i, __bitmap_weight((unsigned long*)&x, i));
}

H2CASE(bitmapz, __bitmap_weight2)
{ 
   unsigned long x[2];
   x[0] = 0x01020408;
   x[1] = 0xA5A5A5A5;
   H2EQUAL_INTEGER(4, __bitmap_weight((unsigned long*)&x, 32));
   H2EQUAL_INTEGER(20, __bitmap_weight((unsigned long*)&x, 64));
}

H2CASE(bitmapz, bitmap_set0)
{ 
   unsigned long x[3];
   x[0] = 0;
   x[1] = 0;
   x[2] = 0;
   bitmap_set((unsigned long*)&x, 0, 0);
   H2EQUAL_INTEGER(0, x[0]);
   H2EQUAL_INTEGER(0, x[1]);
   H2EQUAL_INTEGER(0, x[2]);
}

H2CASE(bitmapz, bitmap_set1)
{ 
   unsigned long x = 0;
   for (unsigned int i=0; i<32; i++){
      unsigned long y = x;
      bitmap_set((unsigned long*)&x, i, 1);
      H2EQUAL_INTEGER(y+(1<<i), x);
   }
}

H2CASE(bitmapz, bitmap_set_align)
{ 
   unsigned long x[3];
   x[0] = 0;
   x[1] = 0;
   x[2] = 0;
   bitmap_set((unsigned long*)&x, 32, 32);
   H2EQUAL_INTEGER(0, x[0]);
   H2EQUAL_INTEGER(0xffffffff, x[1]);
   H2EQUAL_INTEGER(0, x[2]);
}

H2CASE(bitmapz, bitmap_set_first)
{ 
   unsigned long x[3];
   x[0] = 0;
   x[1] = 0;
   x[2] = 0;
   bitmap_set((unsigned long*)&x, 30, 34);
   H2EQUAL_INTEGER(0xC0000000, x[0]);
   H2EQUAL_INTEGER(0xffffffff, x[1]);
   H2EQUAL_INTEGER(0, x[2]);
}

H2CASE(bitmapz, bitmap_set_last)
{ 
   unsigned long x[3];
   x[0] = 0;
   x[1] = 0;
   x[2] = 0;
   bitmap_set((unsigned long*)&x, 32, 33);
   H2EQUAL_INTEGER(0, x[0]);
   H2EQUAL_INTEGER(0xffffffff, x[1]);
   H2EQUAL_INTEGER(0x00000001, x[2]);
}

H2CASE(bitmapz, bitmap_set_first_last)
{ 
   unsigned long x[4];
   x[0] = 0;
   x[1] = 0;
   x[2] = 0;
   x[3] = 0;
   bitmap_set((unsigned long*)&x, 30, 67);
   H2EQUAL_INTEGER(0xC0000000, x[0]);
   H2EQUAL_INTEGER(0xffffffff, x[1]);
   H2EQUAL_INTEGER(0xffffffff, x[2]);
   H2EQUAL_INTEGER(0x00000001, x[3]);
}


H2CASE(bitmapz, bitmap_clear0)
{ 
   unsigned long x[3];
   x[0] = 0xffffffff;
   x[1] = 0xffffffff;
   x[2] = 0xffffffff;
   bitmap_clear((unsigned long*)&x, 0, 0);
   H2EQUAL_INTEGER(0xffffffff, x[0]);
   H2EQUAL_INTEGER(0xffffffff, x[1]);
   H2EQUAL_INTEGER(0xffffffff, x[2]);
}

H2CASE(bitmapz, bitmap_clear1)
{ 
   unsigned long x = 0xffffffff;
   for (unsigned int i=0; i<32; i++){
      unsigned long y = x;
      bitmap_clear((unsigned long*)&x, i, 1);
      H2EQUAL_INTEGER(y-(1<<i), x);
   }
}

H2CASE(bitmapz, bitmap_clear_align)
{ 
   unsigned long x[3];
   x[0] = 0xffffffff;
   x[1] = 0xffffffff;
   x[2] = 0xffffffff;
   bitmap_clear((unsigned long*)&x, 32, 32);
   H2EQUAL_INTEGER(0xffffffff, x[0]);
   H2EQUAL_INTEGER(0x00000000, x[1]);
   H2EQUAL_INTEGER(0xffffffff, x[2]);
}

H2CASE(bitmapz, bitmap_clear_first)
{ 
   unsigned long x[3];
   x[0] = 0xffffffff;
   x[1] = 0xffffffff;
   x[2] = 0xffffffff;
   bitmap_clear((unsigned long*)&x, 30, 34);
   H2EQUAL_INTEGER(0x3fffffff, x[0]);
   H2EQUAL_INTEGER(0x00000000, x[1]);
   H2EQUAL_INTEGER(0xffffffff, x[2]);
}

H2CASE(bitmapz, bitmap_clear_last)
{ 
   unsigned long x[3];
   x[0] = 0xffffffff;
   x[1] = 0xffffffff;
   x[2] = 0xffffffff;
   bitmap_clear((unsigned long*)&x, 32, 33);
   H2EQUAL_INTEGER(0xffffffff, x[0]);
   H2EQUAL_INTEGER(0x00000000, x[1]);
   H2EQUAL_INTEGER(0xfffffffE, x[2]);
}

H2CASE(bitmapz, bitmap_clear_first_last)
{ 
   unsigned long x[4];
   x[0] = 0xffffffff;
   x[1] = 0xffffffff;
   x[2] = 0xffffffff;
   x[3] = 0xffffffff;
   bitmap_clear((unsigned long*)&x, 30, 67);
   H2EQUAL_INTEGER(0x3fffffff, x[0]);
   H2EQUAL_INTEGER(0x00000000, x[1]);
   H2EQUAL_INTEGER(0x00000000, x[2]);
   H2EQUAL_INTEGER(0xfffffffE, x[3]);
}



H2CASE(bitmapz, bitmap_test_set0)
{ 
   unsigned long x[3];
   x[0] = 0;
   x[1] = 0;
   x[2] = 0;
   H2EQUAL_INTEGER(1, bitmap_test_set((unsigned long*)&x, 0, 0));
}

H2CASE(bitmapz, bitmap_test_set1)
{ 
   unsigned long x = 0, y = 0xffffffff;
   for (unsigned int i=0; i<32; i++){
      H2EQUAL_INTEGER(0, bitmap_test_set((unsigned long*)&x, i, 1));
      H2EQUAL_INTEGER(1, bitmap_test_set((unsigned long*)&y, i, 1));
   }
}

H2CASE(bitmapz, bitmap_test_set_align)
{ 
   unsigned long x[3];
   x[0] = 0x00000000;
   x[1] = 0xffffffff;
   x[2] = 0xff0fff0f;

   H2EQUAL_INTEGER(0, bitmap_test_set((unsigned long*)&x, 0, 32));
   H2EQUAL_INTEGER(1, bitmap_test_set((unsigned long*)&x, 32, 32));
   H2EQUAL_INTEGER(0, bitmap_test_set((unsigned long*)&x, 64, 32));
}

H2CASE(bitmapz, bitmap_test_set_first)
{ 
   unsigned long x[3];
   x[0] = 0xC0000000;
   x[1] = 0xffffffff;
   x[2] = 0x00000000;

   H2EQUAL_INTEGER(0, bitmap_test_set((unsigned long*)&x, 29, 35));
   H2EQUAL_INTEGER(1, bitmap_test_set((unsigned long*)&x, 30, 34));
}

H2CASE(bitmapz, bitmap_test_set_last)
{ 
   unsigned long x[3];
   x[0] = 0x00000000;
   x[1] = 0xffffffff;
   x[2] = 0x00000001;

   H2EQUAL_INTEGER(1, bitmap_test_set((unsigned long*)&x, 32, 33));
   H2EQUAL_INTEGER(0, bitmap_test_set((unsigned long*)&x, 32, 34));
}

H2CASE(bitmapz, bitmap_test_set_first_last)
{ 
   unsigned long x[4];
   x[0] = 0xC0000000;
   x[1] = 0xffffffff;
   x[2] = 0xffffffff;
   x[3] = 0x00000001;

   H2EQUAL_INTEGER(0, bitmap_test_set((unsigned long*)&x, 29, 67));
   H2EQUAL_INTEGER(1, bitmap_test_set((unsigned long*)&x, 30, 67));
   H2EQUAL_INTEGER(0, bitmap_test_set((unsigned long*)&x, 30, 68));
   H2EQUAL_INTEGER(0, bitmap_test_set((unsigned long*)&x, 29, 68));
}


H2CASE(bitmapz, bitmap_test_clear0)
{ 
   unsigned long x[3];
   x[0] = 0;
   x[1] = 0;
   x[2] = 0;
   H2EQUAL_INTEGER(1, bitmap_test_clear((unsigned long*)&x, 0, 0));
}

H2CASE(bitmapz, bitmap_test_clear1)
{ 
   unsigned long x = 0, y = 0xffffffff;
   for (unsigned int i=0; i<32; i++){
      H2EQUAL_INTEGER(1, bitmap_test_clear((unsigned long*)&x, i, 1));
      H2EQUAL_INTEGER(0, bitmap_test_clear((unsigned long*)&y, i, 1));
   }
}

H2CASE(bitmapz, bitmap_test_clear_align)
{ 
   unsigned long x[3];
   x[0] = 0x00000000;
   x[1] = 0xffffffff;
   x[2] = 0xff0fff0f;

   H2EQUAL_INTEGER(1, bitmap_test_clear((unsigned long*)&x, 0, 32));
   H2EQUAL_INTEGER(0, bitmap_test_clear((unsigned long*)&x, 32, 32));
   H2EQUAL_INTEGER(0, bitmap_test_clear((unsigned long*)&x, 64, 32));
}

H2CASE(bitmapz, bitmap_test_clear_first)
{
   unsigned long x[3];
   x[0] = 0x3fffffff;
   x[1] = 0x00000000;
   x[2] = 0xffffffff;

   H2EQUAL_INTEGER(0, bitmap_test_clear((unsigned long*)&x, 29, 35));
   H2EQUAL_INTEGER(1, bitmap_test_clear((unsigned long*)&x, 30, 34));
}

H2CASE(bitmapz, bitmap_test_clear_last)
{ 
   unsigned long x[3];
   x[0] = 0xffffffff;
   x[1] = 0x00000000;
   x[2] = 0xfffffffE;

   H2EQUAL_INTEGER(0, bitmap_test_clear((unsigned long*)&x, 32, 35));
   H2EQUAL_INTEGER(1, bitmap_test_clear((unsigned long*)&x, 32, 33));
}


H2CASE(bitmapz, bitmap_test_clear_first_last)
{ 
   unsigned long x[4];
   x[0] = 0x3fffffff;
   x[1] = 0x00000000;
   x[2] = 0x00000000;
   x[3] = 0xfffffffE;

   H2EQUAL_INTEGER(0, bitmap_test_clear((unsigned long*)&x, 29, 67));
   H2EQUAL_INTEGER(1, bitmap_test_clear((unsigned long*)&x, 30, 67));
   H2EQUAL_INTEGER(0, bitmap_test_clear((unsigned long*)&x, 30, 68));
   H2EQUAL_INTEGER(0, bitmap_test_clear((unsigned long*)&x, 29, 68));
}


H2CASE(bitmapz, bitmap_weight0)
{ 
   unsigned long x[3];
   x[0] = 0;
   x[1] = 0;
   x[2] = 0;
   H2EQUAL_INTEGER(0, bitmap_weight((unsigned long*)&x, 0, 0));
}

H2CASE(bitmapz, bitmap_weight1)
{ 
   unsigned long x = 0, y = 0xffffffff;
   for (unsigned int i=0; i<32; i++){
      H2EQUAL_INTEGER(0, bitmap_weight((unsigned long*)&x, i, 1));
      H2EQUAL_INTEGER(1, bitmap_weight((unsigned long*)&y, i, 1));
   }
}

H2CASE(bitmapz, bitmap_weight_align)
{ 
   unsigned long x[3];
   x[0] = 0x00000000;
   x[1] = 0xffffffff;
   x[2] = 0xff0fff0f;

   H2EQUAL_INTEGER(0, bitmap_weight((unsigned long*)&x, 0, 32));
   H2EQUAL_INTEGER(32, bitmap_weight((unsigned long*)&x, 32, 32));
   H2EQUAL_INTEGER(24, bitmap_weight((unsigned long*)&x, 64, 32));
}

H2CASE(bitmapz, bitmap_weight_first)
{ 
   unsigned long x[3];
   x[0] = 0xC0000000;
   x[1] = 0xffffffff;
   x[2] = 0x00000000;

   H2EQUAL_INTEGER(34, bitmap_weight((unsigned long*)&x, 29, 35));
   H2EQUAL_INTEGER(34, bitmap_weight((unsigned long*)&x, 30, 34));
}

H2CASE(bitmapz, bitmap_weight_last)
{ 
   unsigned long x[3];
   x[0] = 0x00000000;
   x[1] = 0xffffffff;
   x[2] = 0x00000001;

   H2EQUAL_INTEGER(33, bitmap_weight((unsigned long*)&x, 32, 33));
   H2EQUAL_INTEGER(33, bitmap_weight((unsigned long*)&x, 32, 34));
}

H2CASE(bitmapz, bitmap_weight_first_last)
{ 
   unsigned long x[4];
   x[0] = 0xC0000000;
   x[1] = 0xffffffff;
   x[2] = 0xffffffff;
   x[3] = 0x00000001;

   H2EQUAL_INTEGER(66, bitmap_weight((unsigned long*)&x, 29, 67));
   H2EQUAL_INTEGER(67, bitmap_weight((unsigned long*)&x, 30, 67));
   H2EQUAL_INTEGER(67, bitmap_weight((unsigned long*)&x, 30, 68));
   H2EQUAL_INTEGER(67, bitmap_weight((unsigned long*)&x, 29, 68));
}

int __stub_bitmap_find(unsigned long * bmp, int size, int start, int set_or_clear)
{
   int s = 0;
   for (int i=0; i<(size+31)/32; i++) {
      unsigned long x = bmp[i];
      for (int j=0; j<32; j++) {
         if (s >= start && s < size) {
            if (set_or_clear) {
               if((x>>j)&1)
                  return s; 
            } else {
               if(!((x>>j)&1))
                  return s;
            }
         }
         s++;
      }
   }
   return -1;
}

int __stub_bitmap_find_reverse(unsigned long * bmp, int size, int start, int set_or_clear)
{
   int words = (size+31)/32;
   int s = words * 32 - 1;
   for (int i=words; i>0; i--) {
      unsigned long x = bmp[i-1];
      for (int j=32; j>0; j--) {
         if (s <= start) {
            unsigned long y = (x >> (j-1)) & 1;
            if (set_or_clear) {
               if(y)
                  return s; 
            } else {
               if(!y)
                  return s;
            }
         }
         s--;
      }
   }
   return -1;
}

H2CASE(bitmapz, bitmap_find_set_clear)
{ 
   unsigned long x[4];
   x[0] = 0xC0000000;
   x[1] = 0xffffff0F;
   x[2] = 0xffffffff;
   x[3] = 0x00000001;

   for (int k=0; k<2000; k++) { 
      for (int i=0; i<32*4; i++) {
         //printf("0x%08x, 0x%08x, 0x%08x, 0x%08x, i=%d\n", (int)x[0],(int)x[1],(int)x[2],(int)x[3],i);
         H2EQUAL_INTEGER(__stub_bitmap_find((unsigned long*)&x, 32*4, i, 1), 
                     bitmap_find_set((unsigned long*)&x, 32*4, i));
         H2EQUAL_INTEGER(__stub_bitmap_find((unsigned long*)&x, 32*4, i, 0),
                     bitmap_find_clear((unsigned long*)&x, 32*4, i));
         H2EQUAL_INTEGER(__stub_bitmap_find_reverse((unsigned long*)&x, 32*4, i, 1), 
                     bitmap_find_set_reverse((unsigned long*)&x, i));
         H2EQUAL_INTEGER(__stub_bitmap_find_reverse((unsigned long*)&x, 32*4, i, 0),
                     bitmap_find_clear_reverse((unsigned long*)&x, i));
      } 
      
      x[0] = rand();
      x[1] = rand();
      x[2] = rand();
      x[3] = rand();
   }
}


