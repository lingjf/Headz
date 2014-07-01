
#undef malloc
#undef free
#undef calloc
#undef realloc

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h> /* <winsock2.h> included */
#include <process.h> 
#else
#include <unistd.h> 
#include <sys/time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "ut_stub.h"

typedef struct mblock 
{
   struct mblock *next;
   int     pid;
   void   *data;
   int     len;
   unsigned long rip;
} mblock;

static mblock  *g_block = NULL;
size_t          g_max_ram = 0x7fffffff;


static int __get_thread_id(void) 
{
#ifdef _WIN32
   return (int)GetCurrentThreadId();
#else
   return (int)pthread_self();
#endif
}

void fault_injection_mem_init(int max_size)
{
   if (max_size == -1)
      g_max_ram = 0x7fffffff;

   g_max_ram = max_size;
}

void fault_injection_mem_info(size_t *remain_size)
{
   if (remain_size)
      *remain_size = g_max_ram;
   return;
}

#define RETURN_RIP(v) ((unsigned long*)&(v))[-1]
//extern "C" {


void *stub_malloc(size_t size)
{
   void *data;
   mblock *b;

   if (size > g_max_ram)
      return NULL;
  
   data = malloc(size);
   if (!data)
      return NULL;

   b = (mblock*)malloc(sizeof(mblock));
   if (!b) {
      free(data);
      return NULL;
   }
   
   g_max_ram -= size;
   
   b->data = data;
   b->len  = size;
   b->rip  = RETURN_RIP(size);
   b->pid  = __get_thread_id();
   b->next = g_block;
   g_block = b;
 
   return data;
}

void stub_free(void *ptr)
{
   mblock *b;
   mblock **k;
   for (k = &g_block; *k; k = &(*k)->next)
      if ((*k)->data == ptr) 
         break; 
   if (*k == NULL) 
      return;
   
   b = *k;
   g_max_ram += b->len; 
   *k = b->next; 
   free(b->data);
   free(b);
}

void *stub_calloc(size_t nmemb, size_t size)
{
   void *data;
   mblock *b;
   data = calloc(nmemb, size);
   if (!data)
      return NULL;

   b = (mblock*)malloc(sizeof(mblock));
   if (!b) {
      free(data);
      return NULL;
   }
   b->data = data;
   b->len  = nmemb * size;
   b->rip  = RETURN_RIP(nmemb);
   b->pid  = __get_thread_id();
   b->next = g_block;
   g_block = b;

   g_max_ram -= b->len;
   
   return data;
}

void *stub_realloc(void *ptr, size_t size)
{
   void *data;
   mblock *b;
   if (ptr == NULL)
      return stub_malloc(size);
   if (size == 0) {
      stub_free(ptr);
      return NULL;
   }
   for (b = g_block; b; b = b->next) {
      if (b->data == ptr)
         break;
   }

   if (!b) {
      return NULL;
   }
   data = realloc(ptr, size);
   if (!data)
      return NULL;

   g_max_ram += b->len;
   g_max_ram -= size;
   
   b->data = data;
   b->len  = size;
   b->rip  = RETURN_RIP(ptr); 
   b->pid  = __get_thread_id();  
   
   return data;
}

static char *__find_symbol(unsigned long address)
{
   static char buff[256];
   FILE *fp = NULL;
   fp = fopen("../bin/utest.sym", "r");
   if (fp == NULL)
      return "unknown source where.";
   while (fgets(buff, sizeof(buff), fp)) {
      unsigned long value;
      value = strtoul(buff, 0, 16);
      if (value != 0 && address >= value) {
         fclose(fp);
         return &buff[11];
      }
   }
   fclose(fp);
   return "unknown source where.";
}


long fault_injection_mem_summary()
{
   long leak = 0;
   mblock *b = g_block;
   while(b) {
      leak = 1;
      printf("memory leak: 0x%08lx:%d bytes in %d at %s\n", 
          (unsigned long)b->data, b->len, b->pid, __find_symbol(b->rip));
      b = b->next; 
   }
   return leak;
}

//}

