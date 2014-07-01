#ifndef __UTILZ_H_2010_0815__
#define __UTILZ_H_2010_0815__

typedef struct stack_frame
{
   struct stack_frame *next_frame;
   unsigned long return_addr;
} stack_frame;

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))

#define FIELD_OFFSET(t, f) ((unsigned long)&(((t*)0)->f))

/*  asm volatile ("" : : "r" (ngr)); */

#define BUSY_WAIT_NOP	asm ("rep; nop")

#define _RETURN_EIP_                                  \
   ({                                                 \
      unsigned int *_ebp;                             \
      asm volatile ("movl %%ebp, %0" : "=m" (_ebp));  \
      (unsigned int)(*(_ebp+1));                      \
   })

#define _THIS_EIP_                     \
   ({                                  \
      __label__ __here;                \
      __here:                          \
      (unsigned long)&&__here;         \
   })

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

#define clamp(val, min, max)                 \
   ({                                        \
   	typeof(val) __val = (val);		         \
   	typeof(min) __min = (min);		         \
   	typeof(max) __max = (max);		         \
   	(void) (&__val == &__min);		         \
   	(void) (&__val == &__max);		         \
   	__val = __val < __min ? __min: __val;	 \
   	__val > __max ? __max: __val;            \
   })

#define __swap(type, a, b)       \
	do {                         \
      type __tmp = (a);          \
      (a) = (b);                 \
      (b) = __tmp;               \
   } while (0)

#define swap(a, b) __swap(typeof(a), a, b)

#define container_of(ptr, type, member)                     \
   ({                                                       \
	   const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	   (type *)( (char *)__mptr - offsetof(type,member) );  \
   })

#define OVERLOP(x,y, i,j) ((x)<=(j)&&(i)<=(y))
#define CONTAIN(x,y, i,j) ((x)<=(i)&&(j)<=(y))

#define BUG() do { \
	printf("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
	panic("BUG!"); \
} while (0)

#define BUG_ON(condition) do { if (unlikely(condition)) BUG(); } while(0)

#endif

