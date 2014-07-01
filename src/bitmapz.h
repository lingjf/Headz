#ifndef __BITMAPZ_H_0f5dda78_7d5c_400b_83d3_9527d5bc80d7__
#define __BITMAPZ_H_0f5dda78_7d5c_400b_83d3_9527d5bc80d7__

#ifdef __cplusplus
extern "C" {
#endif

/*start : [0 - n]*/

void bitmap_set(unsigned long *bmp, int start, int nr);

void bitmap_clear(unsigned long *bmp, int start, int nr);

int bitmap_test_set(unsigned long *bmp, int start, int nr);

int bitmap_test_clear(unsigned long *bmp, int start, int nr);

int bitmap_weight(unsigned long *bmp, int start, int nr);

int bitmap_find_set(unsigned long *bmp, int size, int start);

int bitmap_find_clear(unsigned long *bmp, int size, int start);

int bitmap_find_set_reverse(unsigned long *bmp, int start);

int bitmap_find_clear_reverse(unsigned long *bmp, int start);

#define __bitmap_empty(bmp,bits) bitmap_test_clear(bmp,0,bits)
#define __bitmap_full(bmp,bits) bitmap_test_set(bmp,0,bits)
#define __bitmap_weight(bmp,bits) bitmap_weight(bmp,0,bits)

#ifdef __cplusplus
}
#endif

#endif /* __BITOPZ_H_2009_0815__ */

