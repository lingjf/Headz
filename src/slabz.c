#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

//#define DBUG_COMPILE_TIME_OFF
//#define DBUG_COMPILE_TIME_TRACE_OFF
//#define DBUG_COMPILE_TIME_LOG_OFF
//#define DBUG_COMPILE_TIME_LEAK_OFF

#include "listz.h"
#include "jeffz.h"
#include "dbugz.h"
#include "slabz.h"

/*
 *
 *                          ------------------
 *                          |cell| link |cell|
 *                          |head|object|tail|
 *                          ------------------
 *                               \      /
 *                 -----------------------------------------
 *              -> |page~head | cell | cell | ...          |
 *  --------   /   |----------------------------------------
 *  |      |  /    v
 *  | slab | /     -----------------------------------------
 *  |      |<--    |page~head | cell | cell | ...          |
 *  --------   \   |----------------------------------------
 *              \  v
 *               \ -----------------------------------------
 *                \|page~head | cell | cell | ...          |
 *                 -----------------------------------------
 *
 */

#define SLAB_MAGIC_ENABLE 1

#define CELL_MAGIC 0xC0DECAFE

typedef struct slab_head
{
   /* busy list first for better asm code */
   list_head page_busy;
   list_head page_free;
   list_head page_full;

   list_head slabs;

   unsigned long free_cells;
   unsigned long total_cells;
   unsigned long cell_size;
   unsigned long obj_size;

   unsigned long lock;

   /* configured */
   unsigned long size_reqs;
   unsigned long mins_reqs;
   unsigned long maxs_reqs;
   unsigned long flags;
   slab_objector *ctor;
   float growth_factor;
   char name[128];
} slab_head;

typedef struct page_head
{
   list_head pages;
   line_head cell_free;
   unsigned long free_cells;
   unsigned long total_cells;
   unsigned long size;
   unsigned long flags;
   slab_head *slab;
} page_head;

typedef struct cell_head
{
   page_head *page;
} cell_head;

typedef struct cell_tail
{
#if SLAB_MAGIC_ENABLE   
   unsigned long magic;
#endif
} cell_tail;

typedef union object_head
{
   line_head cells;
   unsigned char data[4];
} object_head;

#define PAGE_CELL(page) ((cell_head*)(((unsigned long)(page)) + sizeof(page_head)))
#define CELL_OBJECT(cell) (object_head*)((unsigned long)(cell) + sizeof(cell_head))
#define CELL_CELL(cell,cellsize) (cell_head*)((unsigned long)(cell) + (cellsize))
#define OBJECT_CELL(object) (cell_head*)((unsigned long)(object) - sizeof(cell_head))
#define OBJECT_TAIL(object,objsize) (cell_tail*)((unsigned long)(object) + (objsize))

#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define PAGE_FIXED 0x1
#define PAGE_FUCK  0x2

struct general_size
{
   unsigned long size;
   slab_head *slab;
   void *reserved;
} __builtin_slabs[] = {
	{    32, NULL, NULL},
	{    64, NULL, NULL},
	{   128, NULL, NULL},
	{   256, NULL, NULL},
	{   512, NULL, NULL},
	{  1024, NULL, NULL},
	{  2048, NULL, NULL},
	{  4096, NULL, NULL},
	{  8192, NULL, NULL},
	{ 16384, NULL, NULL},
	{ 32768, NULL, NULL},
	{ 65536, NULL, NULL},
	{131072, NULL, NULL},
	{     0, NULL, NULL}
};

static list_head __slabs_list;
static unsigned long __slabs_lock;

void Mutex_Create(unsigned long x)
{

}
void Mutex_Delete(unsigned long x)
{

}
void Mutex_P(unsigned long x)
{

}
void Mutex_V(unsigned long x)
{

}

void slab_initialize(void)
{
   DBUG_ENTER;
   char name[128];
   struct general_size *gs = __builtin_slabs;
   list_init(&__slabs_list);
   Mutex_Create(__slabs_lock);
   do {
      sprintf(name, "builtin-%lu bytes", gs->size);
      gs->slab = (slab_head*) slab_create(name, gs->size, 0, 0, 0, 0.0, 0);
      gs++;
   } while (gs->size);
   DBUG_RETURN_VOID;
}

void slab_uninitialize(void)
{
   DBUG_ENTER;
   slab_head *p, *t;
   list_for_each_entry_safe(p, t, &__slabs_list, slab_head, slabs) {
      slab_destroy(p);
   }
   Mutex_Delete(__slabs_lock);
   BUILD_BUG_ON(sizeof(slab_head) != __ALIGN4(sizeof(slab_head)));
   BUILD_BUG_ON(sizeof(page_head) != __ALIGN4(sizeof(page_head)));
   BUILD_BUG_ON(sizeof(cell_head) != __ALIGN4(sizeof(cell_head)));
   BUILD_BUG_ON(sizeof(cell_tail) != __ALIGN4(sizeof(cell_tail)));
   BUILD_BUG_ON(sizeof(object_head) != __ALIGN4(sizeof(object_head)));
   DBUG_RETURN_VOID;
}

static void __initial_cell(slab_head *slab, page_head *page, cell_head *cell)
{
   DBUG_ENTER;
   object_head *object;
   cell_tail *tail;
   assert(slab && page && cell);

   memset(cell, 0, slab->cell_size);
   cell->page = page;
   object = CELL_OBJECT(cell);
   tail = OBJECT_TAIL(object, slab->obj_size);

#if SLAB_MAGIC_ENABLE   
   tail->magic = CELL_MAGIC;
#endif   
   DBUG_RETURN_VOID;
}

static void __initial_page(slab_head *slab, page_head *page, unsigned long page_size)
{
   DBUG_ENTER;
   int i, cells;
   assert(slab && page);
   assert(page_size > sizeof(page_head));

   cells = (page_size - sizeof(page_head)) / slab->cell_size;
   page->slab = slab;
   page->size = page_size;
   page->flags = 0;
   list_init(&page->pages);
   line_init(&page->cell_free);
   page->free_cells = 0;
   page->total_cells = 0;
   for (i = 0; i < cells; i++) {
      cell_head *cell = (cell_head*) (((unsigned long) PAGE_CELL(page)) + i * slab->cell_size);
      object_head *object = CELL_OBJECT(cell);
      __initial_cell(slab, page, cell);
      line_push(&object->cells, &page->cell_free);
      page->total_cells++;
      page->free_cells++;
   }
   DBUG_RETURN_VOID;
}

static void __free_page(slab_head *slab, page_head *page)
{
   DBUG_ENTER;
   assert(slab && page);
   list_del(&page->pages);
   slab->free_cells -= page->total_cells;
   slab->total_cells -= page->total_cells;
   free(page);
   DBUG_RETURN_VOID;
}

static void __shrink_page(slab_head *slab, double water_level)
{
   DBUG_ENTER;
   page_head *p, *t;
   assert(slab);
   assert(0 <= water_level && water_level <= 1);

   list_for_each_entry_safe(p, t, &slab->page_free, page_head, pages) {
      int will_frees = slab->free_cells - p->total_cells;
      int will_totals = slab->total_cells - p->total_cells;
      if ((will_totals - will_frees) <= will_totals * water_level)
         __free_page(slab, p);
   }
   DBUG_RETURN_VOID;
}

static page_head * __allocate_page(slab_head *slab, int cells)
{
   DBUG_ENTER;
   page_head *page = NULL;
   unsigned long page_size;

   assert(slab);
   assert(cells);

   page_size = slab->cell_size * cells + sizeof(page_head);
   page = (page_head *) malloc(page_size);
   if (!page) {
      slab_head *s;
      Mutex_P(__slabs_lock);
      list_for_each_entry(s, &__slabs_list, slab_head, slabs) {
         __shrink_page(s, 1);
      }
      Mutex_P(__slabs_lock);
      do {
         page = (page_head *) malloc(page_size);
         if (page)
            break;
         page_size >>= 1;
      } while (page_size > (slab->cell_size + sizeof(page_head)));
   }
   if (!page)
      DBUG_RETURN_DATA((page_head *)NULL);

   __initial_page(slab, page, page_size);
   DBUG_RETURN_DATA(page);
}

void* slab_create(char *name, unsigned long size, unsigned long mins, unsigned long maxs, unsigned long flags,
                  float growth_factor, slab_objector *ctor)
{
   DBUG_ENTER;
   unsigned long initial_objs;
   slab_head *slab;
   assert(name);
   assert(size > 0);
   assert(&mins || &maxs);
   assert(&flags);
   assert(&growth_factor);
   assert(&ctor);

   slab = (slab_head *) malloc(sizeof(slab_head));
   if (!slab)
      DBUG_RETURN_DATA((void*)NULL);
   memset(slab, 0, sizeof(slab_head));
   list_init(&slab->slabs);
   list_init(&slab->page_full);
   list_init(&slab->page_busy);
   list_init(&slab->page_free);
   Mutex_Create(slab->lock);
   /*copy configuration and recalucation*/
   strncpy(slab->name, name, sizeof(slab->name));
   slab->size_reqs = size;
   slab->mins_reqs = mins;
   slab->maxs_reqs = maxs;
   slab->flags = flags;
   slab->ctor = ctor;
   slab->growth_factor = growth_factor;

   slab->obj_size = __ALIGN4(slab->size_reqs);
   slab->cell_size = slab->obj_size + sizeof(cell_head) + sizeof(cell_tail);

   if (slab->maxs_reqs <= 0)
      slab->maxs_reqs = 0x7fffffff;

   /*pre-allocate resources*/
   initial_objs = slab->flags & SLAB_RESERVED ? slab->maxs_reqs : slab->mins_reqs;
   while (initial_objs > 0) {
      page_head * page;
      page = __allocate_page(slab, initial_objs);
      if (page) {
         list_add_tail(&page->pages, &slab->page_busy);
         page->flags |= PAGE_FIXED;
         slab->total_cells += page->total_cells;
         slab->free_cells += page->free_cells;
         initial_objs -= page->total_cells;
      } else {
         DBUG_SETEC(SLAB_OUT_OF_MEMORY_EC);
         DBUG_LOG(L_ERROR_"Out of memory");
         slab_destroy(slab);
         DBUG_RETURN_DATA((void*)NULL);
      }
   }
   Mutex_P(__slabs_lock);
   list_add_tail(&slab->slabs, &__slabs_list);
   Mutex_V(__slabs_lock);
   DBUG_RETURN_DATA(slab);
}

void slab_destroy(void *handle)
{
   DBUG_ENTER;
   page_head *p, *t;
   slab_head *slab = (slab_head*) handle;
   assert(&handle);

   if (!slab)
      DBUG_RETURN_VOID;

   Mutex_P(slab->lock);
   list_for_each_entry_safe(p, t, &slab->page_free, page_head, pages) {
      list_del(&p->pages);
      free(p);
   }
   list_for_each_entry_safe(p, t, &slab->page_busy, page_head, pages) {
      list_del(&p->pages);
      free(p);
   }
   list_for_each_entry_safe(p, t, &slab->page_full, page_head, pages) {
      list_del(&p->pages);
      free(p);
   }
   Mutex_V(slab->lock);

   Mutex_P(__slabs_lock);
   list_del(&slab->slabs);
   Mutex_V(__slabs_lock);

   Mutex_Delete(slab->lock);
   free(slab);
   DBUG_RETURN_VOID;
}

static int __growth_factor(slab_head *slab)
{
   DBUG_ENTER;
   int cells;
   assert(slab);

   if (slab->growth_factor > 1) {
      cells = (int) slab->growth_factor;
   } else if (0 < slab->growth_factor && slab->growth_factor <= 1) {
      cells = (int) (slab->total_cells * slab->growth_factor);
   } else { /* growth_factor <= 0 */
      cells = (int) (slab->total_cells * 0.8);
   }
   cells = (int) MAX(cells, (int)((8192-sizeof(page_head))/slab->cell_size));
   cells = MAX(cells, 1);
   cells = MIN(cells, (int)(slab->maxs_reqs - slab->total_cells));
   DBUG_RETURN_DATA(cells);
}

void* slab_kalloc(void *handle, void *arg)
{
   DBUG_ENTER;
   line_head *line = NULL;
   object_head *object = NULL;
   cell_head *cell = NULL;
   page_head *page = NULL;
   slab_head *slab = (slab_head*) handle;
   assert(&handle);
   assert(&arg);

   if (!slab)
      DBUG_RETURN_DATA((void *)NULL);

   Mutex_P(slab->lock);
   if (slab->free_cells == 0) {
      if (slab->total_cells + 1 > slab->maxs_reqs) {
         Mutex_V(slab->lock);
         DBUG_SETEC(SLAB_OUT_OF_LIMITATION_EC);
         DBUG_LOG(L_WARNING_"Out of limitation");
         DBUG_RETURN_DATA((void *)NULL);
      }
      page = __allocate_page(slab, __growth_factor(slab));
      if (page) {
         list_add_tail(&page->pages, &slab->page_busy);
         slab->total_cells += page->total_cells;
         slab->free_cells += page->free_cells;
      } else {
         Mutex_V(slab->lock);
         DBUG_SETEC(SLAB_OUT_OF_MEMORY_EC);
         DBUG_LOG(L_ERROR_"Out of memory");
         DBUG_RETURN_DATA((void *)NULL);
      }
   }
   if (list_empty(&slab->page_busy))
      list_move_tail(list_get_head(&slab->page_free), &slab->page_busy);

   page = list_entry(list_get_head(&slab->page_busy), page_head, pages);

   line_pop(line, &page->cell_free);
   object = list_entry(line, object_head, cells);
   cell = OBJECT_CELL(object);
   page->free_cells--;
   slab->free_cells--;
   if (page->free_cells == 0)
      list_move_tail(&page->pages, &slab->page_full);

   if (slab->ctor)
      slab->ctor((void*) object, arg);
   Mutex_V(slab->lock);
   DBUG_RETURN_DATA((void*)object);
}

void slab_kfree(void *handle, void *ptr)
{
   DBUG_ENTER;
   cell_head *cell = NULL;
   object_head *object = NULL;
   cell_tail *tail = NULL;
   page_head *page = NULL;
   slab_head *slab = NULL;
   assert(&handle);
   assert(&ptr);

   slab = (slab_head*) handle;
   object = (object_head*) ptr;
   if (!slab || !object)
      DBUG_RETURN_VOID;
   cell = OBJECT_CELL(object);
   tail = OBJECT_TAIL(object, slab->obj_size);
   if (!cell || !tail)
      DBUG_RETURN_VOID;

#if SLAB_MAGIC_ENABLE
   if (tail->magic != CELL_MAGIC) {
      DBUG_SETEC(SLAB_CELL_MAGIC_EC);
      //printf("\nslab_free : object tail magic issue - 0x%08x is corrupted.  Ooooooops \n", (unsigned int)ptr);
      jeff_dump(/*stdout*/0, 0, cell, slab->cell_size, 16, 0);
      tail->magic = CELL_MAGIC; /*Just a reminder, fix it, not return*/
   }
#endif

   page = cell->page;
   if (!page)
      DBUG_RETURN_VOID;

   if (page->slab != slab)
      DBUG_RETURN_VOID;

   Mutex_P(slab->lock);
   line_push(&object->cells, &page->cell_free);
   page->free_cells++;
   slab->free_cells++;
   if (page->free_cells == 1) /*deattach from full list and attach to busy list*/
      list_move_head(&page->pages, &slab->page_busy);

   /*page must be in busy list*/
   if (page->free_cells == page->total_cells)
      if (!(page->flags & PAGE_FIXED))
         list_move_tail(&page->pages, &slab->page_free);

   if (slab->flags & SLAB_RESERVED || slab->flags & SLAB_GREED) {
      /* do not shrink */;
   } else if (slab->flags & SLAB_GENEROUS) {
      __shrink_page(slab, 1);
   } else {
      if (!list_empty(&slab->page_busy))
         __shrink_page(slab, 0.5);
   }

   Mutex_V(slab->lock);
   DBUG_RETURN_VOID;
}

void* slab_malloc(unsigned long size)
{
   DBUG_ENTER;
   struct general_size *gs;
   assert(&size);
   for (gs = __builtin_slabs; gs->size; gs++) {
      if (size > gs->size)
         continue;
      DBUG_RETURN_DATA(slab_kalloc(gs->slab, 0));
   }
   DBUG_RETURN_DATA((void *)NULL);
}

void slab_mfree(void *ptr)
{
   DBUG_ENTER;
   cell_head *cell = NULL;
   page_head *page = NULL;
   slab_head *slab = NULL;
   struct general_size *gs = NULL;
   assert(&ptr);

   if (!ptr)
      DBUG_RETURN_VOID;
   cell = ((cell_head*) ptr) - 1;
   if (!cell)
      DBUG_RETURN_VOID;
   page = cell->page;
   if (!page)
      DBUG_RETURN_VOID;
   slab = page->slab;
   if (!slab)
      DBUG_RETURN_VOID;

   for (gs = __builtin_slabs; gs->size; gs++) {
      if (slab != gs->slab)
         continue;
      slab_kfree(slab, ptr);
      DBUG_RETURN_VOID;
   }
   DBUG_RETURN_VOID;
}

void slab_shrink(void *handle)
{
   DBUG_ENTER;
   slab_head *slab = (slab_head*) handle;
   assert(&handle);

   if (slab) {
      Mutex_P(slab->lock);
      __shrink_page(slab, 1);
      Mutex_V(slab->lock);
      DBUG_RETURN_VOID;
   }
   Mutex_P(__slabs_lock);
   list_for_each_entry(slab, &__slabs_list, slab_head, slabs) {
      __shrink_page(slab, 0);
   }
   Mutex_V(__slabs_lock);
   DBUG_RETURN_VOID;
}
/*-----------------------------------------------------*/

int __slab_inside_cell_size(void *handle)
{
   slab_head *slab = (slab_head*) handle;
   assert(handle);
   return slab->cell_size;
}
int __slab_inside_flags(void *handle)
{
   slab_head *slab = (slab_head*) handle;
   assert(handle);
   return slab->flags;
}
int __slab_inside_object_head_size(void *handle)
{
   assert(&handle);
   return sizeof(object_head);
}
int __slab_inside_cell_head_size(void *handle)
{
   assert(&handle);
   return sizeof(cell_head);
}
int __slab_inside_cell_tail_size(void *handle)
{
   assert(&handle);
   return sizeof(cell_tail);
}
int __slab_inside_page_head_size(void *handle)
{
   assert(&handle);
   return sizeof(page_head);
}
int __slab_inside_slab_head_size(void *handle)
{
   assert(&handle);
   return sizeof(slab_head);
}
int __slab_inside_total_objs(void *handle)
{
   slab_head *slab = (slab_head*) handle;
   assert(handle);
   return slab->total_cells;
}
int __slab_inside_free_objs(void *handle)
{
   slab_head *slab = (slab_head*) handle;
   assert(handle);
   return slab->free_cells;
}

int __slab_inside_free_pages(void *handle)
{
   int c = 0;
   list_head *p;
   slab_head *slab = (slab_head*) handle;
   assert(handle);
   list_for_each(p, &slab->page_free) {
      c++;
   }
   return c;
}
int __slab_inside_busy_pages(void *handle)
{
   int c = 0;
   list_head *p;
   slab_head *slab = (slab_head*) handle;
   assert(handle);
   list_for_each(p, &slab->page_busy) {
      c++;
   }
   return c;
}
int __slab_inside_full_pages(void *handle)
{
   int c = 0;
   list_head *p;
   slab_head *slab = (slab_head*) handle;
   assert(handle);
   list_for_each(p, &slab->page_full) {
      c++;
   }
   return c;
}

int __slab_inside_pages(void *handle)
{
   assert(handle);
   return __slab_inside_full_pages(handle) + __slab_inside_busy_pages(handle)
               + __slab_inside_free_pages(handle);
}

