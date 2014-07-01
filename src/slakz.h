#ifndef __SLAKZ_H_7e80eeb0_315d_498b_8d1b_6c7b53e78d28__
#define __SLAKZ_H_7e80eeb0_315d_498b_8d1b_6c7b53e78d28__

#ifdef __cplusplus
extern "C" {
#endif

/*stack memory allocation system*/

#define SLAK_GREED        0x1
#define SLAK_GENEROUS     0x2
#define SLAK_BZERO        0x4

void slak_initialize(void);
void slak_uninitialize(void);

void* slak_create(int size, unsigned long flags);

void slak_destroy(void *handle);

void slak_shrink(void *handle);

void* slak_malloc(void *handle, int size);
void slak_mfree(void *handle, void *ptr);

int __slak_inside_cell_head_size(void *handle);
int __slak_inside_flags(void *handle);
int __slak_inside_pages(void *handle);
int __slak_inside_cells(void *handle);
int __slak_inside_last_obj_size(void *handle);

#ifdef __cplusplus
}
#endif

#endif

