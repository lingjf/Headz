#ifndef __TIMEZ_H__2010_0815__
#define __TIMEZ_H__2010_0815__

#ifdef __cplusplus
extern "C" {
#endif

#include "listz.h"

#define TIMER_PERIODIC 0x01

typedef struct timer_head
{
   struct list_head list;
   unsigned long timeout_ticks;
   unsigned long expired_point;
   unsigned long flags;
   unsigned long data;
   void (*callback)(unsigned long);
} timer_head;

#ifdef __cplusplus
}
#endif

#endif
