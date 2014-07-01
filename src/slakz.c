#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

#include "listz.h"
#include "jeffz.h"
#include "dbugz.h"
#include "slakz.h"

#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define SLAK_MAGIC_ENABLE 1

#define DEFAULT_PAGE_SIZE 255*1024

#define CELL_HEAD_MAGIC 0xFEEDBEEF

typedef struct slak_head
{
   list_head page_stack;
   list_head page_free;
   line_head cell_stack;
   unsigned long flags;
} slak_head;

typedef struct page_head
{
   list_head pages;
   unsigned long size;
} page_head;

typedef struct cell_head
{
#if SLAK_MAGIC_ENABLE
   unsigned long magic;
#endif
   line_head cells;
   unsigned long objsize;
} cell_head;

typedef struct object_head
{
   unsigned long reserved;
} object_head;

#define PAGE_CELL(page) ((cell_head*)(((unsigned long)(page)) + sizeof(page_head)))
#define CELL_SIZE(cell) ((cell)->objsize + sizeof(cell_head))
#define CELL_OBJECT(cell) (object_head*)((unsigned long)(cell) + sizeof(cell_head))
#define CELL_CELL(cell) (cell_head*)((unsigned long)(cell) + CELL_SIZE(cell))
#define OBJECT_CELL(object) (cell_head*)((unsigned long)(object) - sizeof(cell_head))

void slak_initialize(void)
{
}

void slak_uninitialize(void)
{
   BUILD_BUG_ON(sizeof(slak_head) != __ALIGN4(sizeof(slak_head)));
   BUILD_BUG_ON(sizeof(page_head) != __ALIGN4(sizeof(page_head)));
   BUILD_BUG_ON(sizeof(cell_head) != __ALIGN4(sizeof(cell_head)));
   BUILD_BUG_ON(sizeof(object_head) != __ALIGN4(sizeof(object_head)));
}

void* slak_create(int size, unsigned long flags)
{
   slak_head *slak;
   page_head *page;
   cell_head *cell;
   int page_len;
   assert(&size);
   assert(&flags);

   slak = (slak_head*) malloc(sizeof(slak_head));
   if (!slak)
      return NULL;
   list_init(&slak->page_stack);
   list_init(&slak->page_free);
   line_init(&slak->cell_stack);
   slak->flags = flags;
   if (slak->flags & SLAK_GREED && slak->flags & SLAK_GENEROUS)
      goto error_up;

   page_len = MAX(DEFAULT_PAGE_SIZE, __ALIGN4(size)) + sizeof(page_head);
   page = (page_head*) malloc(page_len);
   if (!page)
      goto error_up;
   list_push(&page->pages, &slak->page_stack);
   page->size = page_len;

   cell = PAGE_CELL(page);
   cell->magic = CELL_HEAD_MAGIC;
   cell->objsize = 0;
   line_push(&cell->cells, &slak->cell_stack);

   return slak;

   error_up: if (slak) {
      free(slak);
   }
   return NULL;
}

void slak_destroy(void *handle)
{
   list_head *p, *t;
   slak_head *slak = (slak_head*) handle;
   assert(&handle);
   if (slak) {
      list_for_each_safe(p, t, &slak->page_stack) {
         list_del_init(p);
         free(list_entry(p, page_head, pages));
      }
      list_for_each_safe(p, t, &slak->page_free) {
         list_del_init(p);
         free(list_entry(p, page_head, pages));
      }
      free(slak);
   }
}

void* slak_malloc(void *handle, int size)
{
   slak_head *slak;
   page_head *page;
   cell_head *cell;
   object_head *object;
   slak = (slak_head*) handle;
   assert(handle);
   assert(size > 0);

   if (!slak)
      return NULL;

   cell = list_entry(line_get_top(&slak->cell_stack), cell_head, cells);
   page = list_entry(list_get_top(&slak->page_stack), page_head, pages);

   if (page->size - ((int) cell - (int) page) - CELL_SIZE(cell) >= sizeof(cell_head) + __ALIGN4(size)) {
      cell = CELL_CELL(cell);
   } else {
      list_head *p = list_get_top(&slak->page_free);
      if (p) {
         list_move_head(p, &slak->page_stack);
         page = list_entry(p, page_head, pages);
      } else {
         int page_len = MAX(DEFAULT_PAGE_SIZE, __ALIGN4(size)) + sizeof(page_head);
         page = (page_head*) malloc(page_len);
         if (!page)
            return NULL;
         list_push(&page->pages, &slak->page_stack);
         page->size = page_len;
      }
      cell = PAGE_CELL(page);
   }

#if SLAK_MAGIC_ENABLE   
   cell->magic = CELL_HEAD_MAGIC;
#endif
   cell->objsize = __ALIGN4(size);

   object = CELL_OBJECT(cell);
   line_push(&cell->cells, &slak->cell_stack);

   if (slak->flags & SLAK_BZERO)
      memset((void*) object, 0, cell->objsize);

   return (void*) object;
}

static void __shrink_page(slak_head *slak, list_head *p)
{
   list_head *t;
   assert(slak && p);
   list_for_each_continue_safe(p, t, &slak->page_free) {
      list_del_init(p);
      free(list_entry(p, page_head, pages));
   }
}

void slak_mfree(void *handle, void *ptr)
{
   slak_head *slak;
   page_head *page, *page0;
   cell_head *cell, *cell1;
   object_head *object;
   slak = (slak_head*) handle;
   assert(&handle);
   assert(ptr);
   if (!slak)
      return;
   object = (object_head *) ptr;
   cell1 = OBJECT_CELL(object);
   while (line_get_top(&slak->cell_stack)) {
      line_head *temp;
      line_pop(temp, &slak->cell_stack);
      cell = list_entry(temp, cell_head, cells);
#if SLAK_MAGIC_ENABLE
      if (cell->magic != CELL_HEAD_MAGIC) {
         /*BUG*/assert(0);
      }
#endif
      if (cell == cell1)
         break;
   }
   cell = list_entry(line_get_top(&slak->cell_stack), cell_head, cells);

   page0 = list_entry(list_get_top(&slak->page_stack), page_head, pages);
   while (list_get_top(&slak->page_stack)) {
      list_head *temp = list_get_top(&slak->page_stack);
      page = list_entry(temp, page_head, pages);
      if ((unsigned long) page <= (unsigned long) cell && (unsigned long) cell < (unsigned long) page
                  + page->size)
         break;
      list_move(temp, &slak->page_free);
   }
   page = list_entry(list_get_top(&slak->page_stack), page_head, pages);

   if (page != page0) {
      if (slak->flags & SLAK_GREED)
         /*do nothing*/;
      else if (slak->flags & SLAK_GENEROUS)
         __shrink_page(slak, &slak->page_free);
      else
         __shrink_page(slak, list_get_top(&slak->page_free));
   }
}

void slak_shrink(void *handle)
{
   slak_head *slak = (slak_head*) handle;
   assert(handle);
   if (!slak)
      return;

   __shrink_page(slak, &slak->page_free);
}

/*-----------------------------------------------------*/

int __slak_inside_cell_head_size(void *handle)
{
   assert(handle);
   return sizeof(cell_head);
}

int __slak_inside_flags(void *handle)
{
   slak_head * slak = (slak_head*) handle;
   assert(handle);
   return slak->flags;
}

int __slak_inside_pages(void *handle)
{
   int s = 0;
   list_head *p;
   slak_head * slak = (slak_head*) handle;
   assert(handle);

   list_for_each(p, &slak->page_stack) {
      s++;
   }
   list_for_each(p, &slak->page_free) {
      s++;
   }
   return s;
}

int __slak_inside_cells(void *handle)
{
   int s = 0;
   line_head *p;
   slak_head * slak = (slak_head*) handle;
   assert(handle);

   list_for_each(p, &slak->cell_stack) {
      s++;
   }
   return s;
}

int __slak_inside_last_obj_size(void *handle)
{
   slak_head * slak = (slak_head*) handle;
   assert(handle);
   return list_entry(line_get_top(&slak->cell_stack), cell_head, cells)->objsize;
}

