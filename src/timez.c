#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

#include "listz.h"
#include "jeffz.h"
#include "timez.h"

#define semP(x)
#define semV(x)

#define MSS_PER_TICK       (50)    
#define TICKS_PER_CIRCLE   (1<<8) 
#define MASK_CIRCLE        (TICKS_PER_CIRCLE-1)

static struct
{
   unsigned long point;
   list_head circle[TICKS_PER_CIRCLE];
} spiral;

void gtimer_initialize(void)
{
   int i;
   for (i = 0; i < TICKS_PER_CIRCLE; i++) {
      list_init(&spiral.circle[i]);
   }
   spiral.point = 0;
}

void gtimer_create(timer_head *th, int periodic)
{
   assert(th);
   assert(&periodic);
   list_init(&th->list);
   if (periodic)
      th->flags |= TIMER_PERIODIC;
}

void gtimer_delete(timer_head *th)
{
   assert(th);
   semP(spiral);
   list_del_init(&th->list); semV(spiral);
}

void gtimer_set(timer_head *th, unsigned long timeout)
{
   assert(th);
   assert(&timeout);
   semP(spiral);
   list_del_init(&th->list);

   th->timeout_ticks = timeout > MSS_PER_TICK ? timeout / MSS_PER_TICK : 1;
   th->expired_point = th->timeout_ticks + spiral.point;
   list_add_tail(&th->list, &spiral.circle[th->expired_point & MASK_CIRCLE]);

   semV(spiral);
}

void gtimer_reset(timer_head *th)
{
   assert(th);
   semP(spiral);
   list_del_init(&th->list); semV(spiral);

   th->timeout_ticks = 0;
   th->expired_point = 0;
}

void gtimer_in(timer_head *th, unsigned long timeout)
{
   unsigned long timeout_ticks;
   assert(th);
   assert(&timeout);
   semP(spiral);
   list_del_init(&th->list); semV(spiral);

   timeout_ticks = timeout > MSS_PER_TICK ? timeout / MSS_PER_TICK : 1;
   th->expired_point = timeout_ticks + spiral.point;

   semP(spiral);
   list_add_tail(&th->list, &spiral.circle[th->expired_point & MASK_CIRCLE]); semV(spiral);
}

void gtimer_drive()
{
   timer_head *th, *tp;
   spiral.point++;
   list_for_each_entry_safe(th, tp, &spiral.circle[spiral.point & MASK_CIRCLE], timer_head, list) {
      if (th->expired_point != spiral.point)
         continue;

      list_del_init(&th->list);

      th->callback(th->data);

      if (th->flags & TIMER_PERIODIC) {
         th->expired_point = th->timeout_ticks + spiral.point;
         list_add_tail(&th->list, &spiral.circle[th->expired_point & MASK_CIRCLE]);
      }
   }
}

