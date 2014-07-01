#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

#ifdef __GNUC__

unsigned long atomic_read32(volatile unsigned long *mem)
{
   assert(mem);
   return *mem;
}

void atomic_set32(volatile unsigned long *mem, unsigned long val)
{
   assert(mem);
   assert(&val);
   *mem = val;
}

unsigned long atomic_add32(volatile unsigned long *mem, unsigned long val)
{
   assert(mem);
   assert(&val);

   __asm__ volatile ("lock; xaddl %0,%1"
               : "=r" (val), "=m" (*mem)
               : "0" (val), "m" (*mem)
               : "memory", "cc");
   return val;
}

void atomic_sub32(volatile unsigned long *mem, unsigned long val)
{
   assert(mem);
   assert(&val);

   __asm__ volatile ("lock; subl %1, %0"
               : /* no output */
               : "m" (*(mem)), "r" (val)
               : "memory", "cc");
}

unsigned long atomic_inc32(volatile unsigned long *mem)
{
   assert(mem);
   return atomic_add32(mem, 1);
}

int atomic_dec32(volatile unsigned long *mem)
{
   unsigned char prev;
   assert(mem);

   __asm__ volatile ("lock; decl %0; setnz %1"
               : "=m" (*mem), "=qm" (prev)
               : "m" (*mem)
               : "memory");

   return prev;
}

unsigned long atomic_cas32(volatile unsigned long *mem, unsigned long with, unsigned long cmp)
{
   unsigned long prev;
   assert(mem);
   assert(&with || &cmp);

   __asm__ volatile ("lock; cmpxchgl %1, %2"
               : "=a" (prev)
               : "r" (with), "m" (*(mem)), "0"(cmp)
               : "memory", "cc");
   return prev;
}

unsigned long atomic_xchg32(volatile unsigned long *mem, unsigned long val)
{
   unsigned long prev = val;
   assert(mem);
   assert(&val);

   __asm__ volatile ("xchgl %0, %1"
               : "=r" (prev), "+m" (*mem)
               : "0" (prev));
   return prev;
}

#endif /* __GNUC__ */

#if _WIN32_

unsigned long atomic_add32(volatile unsigned long *mem, unsigned long val)
{
   assert(mem);
   assert(&val);

   return InterlockedExchangeAdd(mem, val);
}

/* Of course we want the 2's compliment of the unsigned value, val */
#pragma warning(disable: 4146)

void atomic_sub32(volatile unsigned long *mem, unsigned long val)
{
   assert(mem);
   assert(&val);
   InterlockedExchangeAdd(mem, -val);
}

unsigned long atomic_inc32(volatile unsigned long *mem)
{
   /* we return old value, win32 returns new value :( */
   assert(mem);
   return InterlockedIncrement(mem) - 1;
}

int atomic_dec32(volatile unsigned long *mem)
{
   assert(mem);
   return InterlockedDecrement(mem);
}

void atomic_set32(volatile unsigned long *mem, unsigned long val)
{
   assert(mem);
   assert(&val);
   InterlockedExchange(mem, val);
}

unsigned long atomic_read32(volatile unsigned long *mem)
{
   assert(mem);
   return *mem;
}

unsigned long atomic_cas32(volatile unsigned long *mem, unsigned long with, unsigned long cmp)
{
   assert(mem);
   assert(&with || &cmp);
   return InterlockedCompareExchange(mem, with, cmp);
}

unsigned long atomic_xchg32(volatile unsigned long *mem, unsigned long val)
{
   assert(mem);
   assert(&val);
   return InterlockedExchange(mem, val);
}

#endif /* __WIN32__ */

