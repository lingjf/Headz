#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "bitopz.h"
#include "bitmapz.h"

/**  |--------==========|==================|===========-------|
 **  |         head word|     normal       |tail word         |**/

#define BITMAP_HEAD_WORD_MASK(idx) (~0UL << ((idx) % BITS_PER_LONG))
#define BITMAP_TAIL_WORD_MASK(idx) ((((idx)+1) % BITS_PER_LONG) ? (1UL<<(((idx)+1) % BITS_PER_LONG))-1 : ~0UL)

/*
 int __bitmap_empty(unsigned long *bmp, int bits)
 {
 int k, lim = bits/BITS_PER_LONG;
 for (k = 0; k < lim; ++k)
 if (bmp[k])
 return 0;

 if (bits % BITS_PER_LONG)
 if (bmp[k] & BITMAP_TAIL_WORD_MASK(bits-1))
 return 0;

 return 1;
 }

 int __bitmap_full(unsigned long *bmp, int bits)
 {
 int k, lim = bits/BITS_PER_LONG;
 for (k = 0; k < lim; ++k)
 if (~bmp[k])
 return 0;

 if (bits % BITS_PER_LONG)
 if (~bmp[k] & BITMAP_TAIL_WORD_MASK(bits-1))
 return 0;

 return 1;
 }

 int __bitmap_weight(unsigned long *bmp, int bits)
 {
 int k, w = 0, lim = bits/BITS_PER_LONG;

 for (k = 0; k < lim; k++)
 w += bitweight32(bmp[k]);

 if (bits % BITS_PER_LONG)
 w += bitweight32(bmp[k] & BITMAP_TAIL_WORD_MASK(bits-1));

 return w;
 }
 */

void bitmap_set(unsigned long *bmp, int start, int nr)
{
   unsigned long *p = bmp + start / BITS_PER_LONG;
   const int size = start + nr;
   int bits_to_set = BITS_PER_LONG - (start % BITS_PER_LONG);
   unsigned long mask_to_set = BITMAP_HEAD_WORD_MASK(start);
   assert(&bmp);
   assert(&start);
   assert(&nr);

   while (nr - bits_to_set >= 0) {
      *p |= mask_to_set;
      nr -= bits_to_set;
      bits_to_set = BITS_PER_LONG;
      mask_to_set = ~0UL;
      p++;
   }
   if (nr) {
      mask_to_set &= BITMAP_TAIL_WORD_MASK(size-1);
      *p |= mask_to_set;
   }
}

void bitmap_clear(unsigned long *bmp, int start, int nr)
{
   unsigned long *p = bmp + start / BITS_PER_LONG;
   const int size = start + nr;
   int bits_to_clear = BITS_PER_LONG - (start % BITS_PER_LONG);
   unsigned long mask_to_clear = BITMAP_HEAD_WORD_MASK(start);
   assert(&bmp);
   assert(&start);
   assert(&nr);

   while (nr - bits_to_clear >= 0) {
      *p &= ~mask_to_clear;
      nr -= bits_to_clear;
      bits_to_clear = BITS_PER_LONG;
      mask_to_clear = ~0UL;
      p++;
   }
   if (nr) {
      mask_to_clear &= BITMAP_TAIL_WORD_MASK(size-1);
      *p &= ~mask_to_clear;
   }
}

int bitmap_test_set(unsigned long *bmp, int start, int nr)
{
   unsigned long *p = bmp + start / BITS_PER_LONG;
   const int size = start + nr;
   int bits_to_test = BITS_PER_LONG - (start % BITS_PER_LONG);
   unsigned long mask_to_test = BITMAP_HEAD_WORD_MASK(start);
   assert(&bmp);
   assert(&start);
   assert(&nr);

   while (nr - bits_to_test >= 0) {
      if ((*p & mask_to_test) != mask_to_test)
         return 0;
      nr -= bits_to_test;
      bits_to_test = BITS_PER_LONG;
      mask_to_test = ~0UL;
      p++;
   }
   if (nr) {
      mask_to_test &= BITMAP_TAIL_WORD_MASK(size-1);
      if ((*p & mask_to_test) != mask_to_test)
         return 0;
   }
   return 1;
}

int bitmap_test_clear(unsigned long *bmp, int start, int nr)
{
   unsigned long *p = bmp + start / BITS_PER_LONG;
   const int size = start + nr;
   int bits_to_test = BITS_PER_LONG - (start % BITS_PER_LONG);
   unsigned long mask_to_test = BITMAP_HEAD_WORD_MASK(start);
   assert(&bmp);
   assert(&start);
   assert(&nr);

   while (nr - bits_to_test >= 0) {
      if (*p & mask_to_test)
         return 0;
      nr -= bits_to_test;
      bits_to_test = BITS_PER_LONG;
      mask_to_test = ~0UL;
      p++;
   }
   if (nr) {
      mask_to_test &= BITMAP_TAIL_WORD_MASK(size-1);
      if (*p & mask_to_test)
         return 0;
   }
   return 1;
}

int bitmap_weight(unsigned long *bmp, int start, int nr)
{
   unsigned long *p = bmp + start / BITS_PER_LONG;
   const int size = start + nr;
   int bits_to_weight = BITS_PER_LONG - (start % BITS_PER_LONG);
   unsigned long mask_to_weight = BITMAP_HEAD_WORD_MASK(start);
   int weight = 0;
   assert(&bmp);
   assert(&start);
   assert(&nr);

   while (nr - bits_to_weight >= 0) {
      weight += bitweight32(*p & mask_to_weight);
      nr -= bits_to_weight;
      bits_to_weight = BITS_PER_LONG;
      mask_to_weight = ~0UL;
      p++;
   }
   if (nr) {
      mask_to_weight &= BITMAP_TAIL_WORD_MASK(size-1);
      weight += bitweight32(*p & mask_to_weight);
   }
   return weight;
}

int bitmap_find_set(unsigned long *bmp, int size, int start)
{
   unsigned long *p = bmp + start / BITS_PER_LONG;
   int nr = size - start;
   int bits_to_find = BITS_PER_LONG - (start % BITS_PER_LONG);
   unsigned long mask_to_find = BITMAP_HEAD_WORD_MASK(start);
   int d, pos = start;
   assert(&bmp);
   assert(&size);
   assert(&start);

   while (nr - bits_to_find >= 0) {
      if ((d = __ffs(*p & mask_to_find)))
         return pos + d - (BITS_PER_LONG - bits_to_find) - 1;
      pos += bits_to_find;
      nr -= bits_to_find;
      bits_to_find = BITS_PER_LONG;
      mask_to_find = ~0UL;
      p++;
   }
   if (nr) {
      mask_to_find &= BITMAP_TAIL_WORD_MASK(size-1);
      if ((d = __ffs(*p & mask_to_find)))
         return pos + d - 1;
   }
   return -1;
}

int bitmap_find_clear(unsigned long *bmp, int size, int start)
{
   unsigned long *p = bmp + start / BITS_PER_LONG;
   int nr = size - start;
   int bits_to_find = BITS_PER_LONG - (start % BITS_PER_LONG);
   unsigned long mask_to_find = BITMAP_HEAD_WORD_MASK(start);
   int d, pos = start;
   assert(&bmp);
   assert(&size);
   assert(&start);

   while (nr - bits_to_find >= 0) {
      if ((d = __ffs((~*p) & mask_to_find)))
         return pos + d - (BITS_PER_LONG - bits_to_find) - 1;
      pos += bits_to_find;
      nr -= bits_to_find;
      bits_to_find = BITS_PER_LONG;
      mask_to_find = ~0UL;
      p++;
   }
   if (nr) {
      mask_to_find &= BITMAP_TAIL_WORD_MASK(size-1);
      if ((d = __ffs((~*p) & mask_to_find)))
         return pos + d - 1;
   }
   return -1;
}

int bitmap_find_set_reverse(unsigned long *bmp, int start)
{
   unsigned long *p = bmp + start / BITS_PER_LONG;
   int nr = start + 1;
   int bits_to_find = start % BITS_PER_LONG + 1;
   unsigned long mask_to_find = BITMAP_TAIL_WORD_MASK(start);
   int d, pos = start;
   assert(&bmp);
   assert(&start);

   while (nr > 0) {
      if ((d = __fls(*p & mask_to_find)))
         return pos - bits_to_find + d;
      pos -= bits_to_find;
      nr -= bits_to_find;
      bits_to_find = BITS_PER_LONG;
      mask_to_find = ~0UL;
      p--;
   }
   return -1;
}

int bitmap_find_clear_reverse(unsigned long *bmp, int start)
{
   unsigned long *p = bmp + start / BITS_PER_LONG;
   int nr = start + 1;
   int bits_to_find = start % BITS_PER_LONG + 1;
   unsigned long mask_to_find = BITMAP_TAIL_WORD_MASK(start);
   int d, pos = start;
   assert(&bmp);
   assert(&start);

   while (nr > 0) {
      if ((d = __fls((~*p) & mask_to_find)))
         return pos - bits_to_find + d;
      pos -= bits_to_find;
      nr -= bits_to_find;
      bits_to_find = BITS_PER_LONG;
      mask_to_find = ~0UL;
      p--;
   }
   return -1;
}

