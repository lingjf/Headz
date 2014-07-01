#ifndef __BITOPZ_H_1e57fa9b_c2e9_451e_833e_98c18b2508d8__
#define __BITOPZ_H_1e57fa9b_c2e9_451e_833e_98c18b2508d8__

#ifdef __cplusplus
extern "C" {
#endif
/*
 * These have to be done with inline assembly: that way the bit-setting
 * is guaranteed to be atomic. All bit operations return 0 if the bit
 * was cleared before the operation and != 0 if it was not.
 *
 * bit 0 is the LSB of addr; bit 32 is the LSB of (addr+1).
 */

#define BITS_PER_BYTE (8)
#define BITS_PER_LONG (sizeof(unsigned long)*BITS_PER_BYTE)

#ifdef __GNUC__

static inline void set_bit(int nr, volatile unsigned long *addr)
{
   __asm__ volatile("bts %1,%0"
               : "=m" (*(volatile long *)(addr))
               : "Ir" (nr)
               : "memory");
}

static inline void clear_bit(int nr, volatile unsigned long *addr)
{
   __asm__ volatile("btr %1,%0"
               : "=m" (*(volatile long *)(addr))
               : "Ir" (nr));
}

static inline void change_bit(int nr, volatile unsigned long *addr)
{
   __asm__ volatile("btc %1,%0"
               : "=m" (*(volatile long *)(addr))
               : "Ir" (nr));
}

static inline int test_and_set_bit(int nr, volatile unsigned long *addr)
{
   int oldbit;

   __asm__ volatile("bts %2,%1\n\t"
               "sbb %0,%0"
               : "=r" (oldbit), "=m" (*(volatile long *)(addr))
               : "Ir" (nr));

   return oldbit;
}

static inline int test_and_clear_bit(int nr, volatile unsigned long *addr)
{
   int oldbit;

   __asm__ volatile("btr %2,%1\n\t"
               "sbb %0,%0"
               : "=r" (oldbit), "=m" (*(volatile long *)(addr))
               : "Ir" (nr));

   return oldbit;
}

static inline int test_and_change_bit(int nr, volatile unsigned long *addr)
{
   int oldbit;

   __asm__ volatile("btc %2,%1\n\t"
               "sbb %0,%0"
               : "=r" (oldbit), "=m" (*(volatile long *)(addr))
               : "Ir" (nr) : "memory");

   return oldbit;
}

static inline int constant_test_bit(unsigned int nr, const volatile unsigned long *addr)
{
   return ((1UL << (nr % BITS_PER_LONG)) & (((unsigned long *) addr)[nr / BITS_PER_LONG])) != 0;
}

static inline int variable_test_bit(int nr, volatile const unsigned long *addr)
{
   int oldbit;

   __asm__ volatile("bt %2,%1\n\t"
               "sbb %0,%0"
               : "=r" (oldbit)
               : "m" (*(unsigned long *)addr), "Ir" (nr));

   return oldbit;
}

#define test_bit(nr, addr)			\
	(__builtin_constant_p((nr)) ? constant_test_bit((nr), (addr)) : variable_test_bit((nr), (addr)))

#if 0
static inline unsigned long __ffs(unsigned long i)
{
   __asm__("bsf %1,%0"
               : "=r" (i)
               : "rm" (i));
   return i;
}

static inline unsigned long __ffz(unsigned long i)
{
   __asm__("bsf %1,%0"
               : "=r" (i)
               : "rm" (~i));
   return i;
}

static inline unsigned long __fls(unsigned long i)
{
   __asm__("bsr %1,%0"
               : "=r" (i)
               : "rm" (i));
   return i;
}

static inline unsigned long __flz(unsigned long i)
{
   __asm__("bsr %1,%0"
               : "=r" (i)
               : "rm" (~i));
   return i;
}
#endif

static inline int __ffs(int x)
{
   int r;
   __asm__("bsfl %1,%0\n\t"
               "jnz 1f\n\t"
               "movl $-1,%0\n"
               "1:" : "=r" (r) : "rm" (x));

   return r + 1;
}

static inline int __fls(int x)
{
   int r;
   __asm__("bsrl %1,%0\n\t"
               "jnz 1f\n\t"
               "movl $-1,%0\n"
               "1:" : "=r" (r) : "rm" (x));
   return r + 1;
}

#else

#define set_bit(nr, addr) (*(addr) |= 1UL<<(nr))

#define clear_bit(nr, addr) (*(addr) &= ~(1UL<<(nr)))

#define change_bit(nr, addr) (*(addr) ^= 1UL<<(nr))

#define test_bit(nr, addr) (!!(*(addr) & 1UL<<(nr)))

extern int __ffs(int x);

extern int __fls(int x);

#endif

#define __bitrol(type, x, shift) (type)(((x) << (shift)) | ((x) >> ((sizeof(type)*BITS_PER_BYTE) - (shift))))
#define __bitror(type, x, shift) (type)(((x) >> (shift)) | ((x) << ((sizeof(type)*BITS_PER_BYTE) - (shift))))

#define bitrol(x, shift) __bitrol(typeof(x), x, shift)
#define bitror(x, shift) __bitror(typeof(x), x, shift)

#define bitrol32(x, shift) __bitrol(unsigned long, x, shift)
#define bitror32(x, shift) __bitror(unsigned long, x, shift)

#define bitrol16(x, shift) __bitrol(unsigned short, x, shift)
#define bitror16(x, shift) __bitror(unsigned short, x, shift)

#define bitrol8(x, shift) __bitrol(unsigned char, x, shift)
#define bitror8(x, shift) __bitror(unsigned char, x, shift)

unsigned char bitrev8(unsigned char x);
unsigned short bitrev16(unsigned short x);
unsigned long bitrev32(unsigned long x);
void bitrev(unsigned char *buf, int size);

unsigned long bitweight32(unsigned long w);
unsigned long bitweight16(unsigned long w);
unsigned long bitweight8(unsigned long w);

#define none_bits(n) (!n)
#define multi_bits(n) (!!((n)&((n)-1)))
#define single_bit(n) ((n)?!((n)&((n)-1)):0)

#ifdef __cplusplus
}
#endif

#endif /* __BITOPZ_H_2009_0815__ */

