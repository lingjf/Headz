#ifndef __UT_STUB_H_2011_0305__
#define __UT_STUB_H_2011_0305__

#include <stdlib.h>

extern "C" {
void *stub_calloc(size_t nmemb, size_t size);
void *stub_malloc(size_t size);
void  stub_free(void *ptr);
void *stub_realloc(void *ptr, size_t size);
}

void fault_injection_mem_init(int max_size); 
void fault_injection_mem_info(size_t *remain_size);
long fault_injection_mem_summary();


#endif

