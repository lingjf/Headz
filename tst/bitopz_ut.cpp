#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "jeffz.h"
#include "utilz.h"
#include "bitopz.h"

extern "C" {

}


H2SUITE(bitopz)
{ 
   void setup()
   { 
   }

   void teardown()
   {  		
   }  	  	
} ;

H2CASE(bitopz, set_bit0)
{ 
   for (unsigned int i=0; i<32; i++) {
      unsigned long x = 0;
      set_bit(i, &x);
      H2EQUAL_INTEGER(1<<i, x);
   }
}

H2CASE(bitopz, set_bit1)
{ 
   for (unsigned int i=0; i<32; i++) {
      unsigned long x = 0xFFFFFFFF;
      set_bit(i, &x);
      H2EQUAL_INTEGER(0xFFFFFFFF, x);
   }
}

H2CASE(bitopz, set_bit2)
{ 
   for (unsigned int i=0; i<32; i++) {
      unsigned long x = 0x12345678;
      set_bit(i, &x);
      H2EQUAL_INTEGER((1<<i)|0x12345678, x);
   }
}


H2CASE(bitopz, clear_bit0)
{ 
   for (unsigned int i=0; i<32; i++) {
      unsigned long x = 0;
      clear_bit(i, &x);
      H2EQUAL_INTEGER(0, x);
   }
}

H2CASE(bitopz, clear_bit1)
{ 
   for (unsigned int i=0; i<32; i++) {
      unsigned long x = 0xFFFFFFFF;
      clear_bit(i, &x);
      H2EQUAL_INTEGER(~(1<<i), x);
   }
}

H2CASE(bitopz, clear_bit2)
{ 
   for (unsigned int i=0; i<32; i++) {
      unsigned long x = 0x12345678;
      clear_bit(i, &x);
      H2EQUAL_INTEGER(0x12345678&(~(1<<i)), x);
   }
}


H2CASE(bitopz, change_bit0)
{ 
   for (unsigned int i=0; i<32; i++) {
      unsigned long x = 0;
      change_bit(i, &x);
      H2EQUAL_INTEGER((1<<i), x);
   }
}

H2CASE(bitopz, change_bit1)
{ 
   for (unsigned int i=0; i<32; i++) {
      unsigned long x = 0xFFFFFFFF;
      change_bit(i, &x);
      H2EQUAL_INTEGER(~(1<<i), x);
   }
}

H2CASE(bitopz, change_bit2)
{ 
   for (unsigned int i=0; i<32; i++) {
      unsigned long x = 0x12345678;
      unsigned long y = (0x12345678^(1<<i));
      change_bit(i, &x);
      H2EQUAL_INTEGER(y, x);
   }
}

H2CASE(bitopz, test_bit)
{ 
   unsigned long x = 0xA5A5A5A5, y = 0x5A5A5A5A;
   H2EQUAL_INTEGER(1, test_bit(0, &x));
   H2EQUAL_INTEGER(0, test_bit(1, &x));
   H2EQUAL_INTEGER(1, test_bit(2, &x));
   H2EQUAL_INTEGER(0, test_bit(3, &x));

   H2EQUAL_INTEGER(0, test_bit(4, &x));
   H2EQUAL_INTEGER(1, test_bit(5, &x));
   H2EQUAL_INTEGER(0, test_bit(6, &x));
   H2EQUAL_INTEGER(1, test_bit(7, &x));

   H2EQUAL_INTEGER(1, test_bit(8, &x));
   H2EQUAL_INTEGER(0, test_bit(9, &x));
   H2EQUAL_INTEGER(1, test_bit(10, &x));
   H2EQUAL_INTEGER(0, test_bit(11, &x));

   H2EQUAL_INTEGER(0, test_bit(12, &x));
   H2EQUAL_INTEGER(1, test_bit(13, &x));
   H2EQUAL_INTEGER(0, test_bit(14, &x));
   H2EQUAL_INTEGER(1, test_bit(15, &x));

   H2EQUAL_INTEGER(1, test_bit(16, &x));
   H2EQUAL_INTEGER(0, test_bit(17, &x));
   H2EQUAL_INTEGER(1, test_bit(18, &x));
   H2EQUAL_INTEGER(0, test_bit(19, &x));

   H2EQUAL_INTEGER(0, test_bit(20, &x));
   H2EQUAL_INTEGER(1, test_bit(21, &x));
   H2EQUAL_INTEGER(0, test_bit(22, &x));
   H2EQUAL_INTEGER(1, test_bit(23, &x));

   H2EQUAL_INTEGER(1, test_bit(24, &x));
   H2EQUAL_INTEGER(0, test_bit(25, &x));
   H2EQUAL_INTEGER(1, test_bit(26, &x));
   H2EQUAL_INTEGER(0, test_bit(27, &x));

   H2EQUAL_INTEGER(0, test_bit(28, &x));
   H2EQUAL_INTEGER(1, test_bit(29, &x));
   H2EQUAL_INTEGER(0, test_bit(30, &x));
   H2EQUAL_INTEGER(1, test_bit(31, &x));

   H2EQUAL_INTEGER(0, test_bit(0, &y));
   H2EQUAL_INTEGER(1, test_bit(1, &y));
   H2EQUAL_INTEGER(0, test_bit(2, &y));
   H2EQUAL_INTEGER(1, test_bit(3, &y));

   H2EQUAL_INTEGER(1, test_bit(4, &y));
   H2EQUAL_INTEGER(0, test_bit(5, &y));
   H2EQUAL_INTEGER(1, test_bit(6, &y));
   H2EQUAL_INTEGER(0, test_bit(7, &y));

   H2EQUAL_INTEGER(0, test_bit(8, &y));
   H2EQUAL_INTEGER(1, test_bit(9, &y));
   H2EQUAL_INTEGER(0, test_bit(10, &y));
   H2EQUAL_INTEGER(1, test_bit(11, &y));

   H2EQUAL_INTEGER(1, test_bit(12, &y));
   H2EQUAL_INTEGER(0, test_bit(13, &y));
   H2EQUAL_INTEGER(1, test_bit(14, &y));
   H2EQUAL_INTEGER(0, test_bit(15, &y));

   H2EQUAL_INTEGER(0, test_bit(16, &y));
   H2EQUAL_INTEGER(1, test_bit(17, &y));
   H2EQUAL_INTEGER(0, test_bit(18, &y));
   H2EQUAL_INTEGER(1, test_bit(19, &y));

   H2EQUAL_INTEGER(1, test_bit(20, &y));
   H2EQUAL_INTEGER(0, test_bit(21, &y));
   H2EQUAL_INTEGER(1, test_bit(22, &y));
   H2EQUAL_INTEGER(0, test_bit(23, &y));

   H2EQUAL_INTEGER(0, test_bit(24, &y));
   H2EQUAL_INTEGER(1, test_bit(25, &y));
   H2EQUAL_INTEGER(0, test_bit(26, &y));
   H2EQUAL_INTEGER(1, test_bit(27, &y));

   H2EQUAL_INTEGER(1, test_bit(28, &y));
   H2EQUAL_INTEGER(0, test_bit(29, &y));
   H2EQUAL_INTEGER(1, test_bit(30, &y));
   H2EQUAL_INTEGER(0, test_bit(31, &y));
}

H2CASE(bitopz, bitrotate32)
{ 
   unsigned long x = 0x12345678;
   unsigned long y;
   for (int i=1; i<32; i++) {
      H2EQUAL_INTEGER(x, (y=bitrol32(x, i), bitror32(y, i))); 
      H2EQUAL_INTEGER(x, (y=bitror32(x, i), bitrol32(y, i))); 
   }
}

H2CASE(bitopz, bitrotate16)
{ 
   unsigned short x = 0x1234;
   unsigned short y;
   for (int i=1; i<16; i++) {
      H2EQUAL_INTEGER(x, (y=bitrol16(x, i), bitror16(y, i))); 
      H2EQUAL_INTEGER(x, (y=bitror16(x, i), bitrol16(y, i))); 
   }
}

H2CASE(bitopz, bitrotate8)
{ 
   unsigned char x = 0xF5;
   unsigned char y;
   for (int i=1; i<8; i++) {
      H2EQUAL_INTEGER(x, (y=bitrol8(x, i), bitror8(y, i))); 
      H2EQUAL_INTEGER(x, (y=bitror8(x, i), bitrol8(y, i))); 
   }
}

H2CASE(bitopz, bitrev8)
{ 
   unsigned char x[][2] = {
      {0x00, 0x00},
      {0x01, 0x80},
   };
   
   for (unsigned int i=0; i<sizeof(x)/2; i++) {
      H2EQUAL_INTEGER(x[i][1], bitrev8(x[i][0])); 
      H2EQUAL_INTEGER(x[i][0], bitrev8(x[i][1]));  
   }
}

H2CASE(bitopz, bitrev16)
{ 
   unsigned short x[][2] = {
      {0x0000, 0x0000},
      {0x0001, 0x8000},
      {0x00F1, 0x8F00}, 
   };
   
   for (unsigned int i=0; i<sizeof(x)/4; i++) {
      H2EQUAL_INTEGER(x[i][1], bitrev16(x[i][0])); 
      H2EQUAL_INTEGER(x[i][0], bitrev16(x[i][1]));  
   }
}


H2CASE(bitopz, bitrev32)
{ 
   unsigned long x[][2] = {
      {0x00000000, 0x00000000},
      {0x00000001, 0x80000000},
      {0x000000F1, 0x8F000000},
      {0x0000F1F1, 0x8F8F0000},
   };
   
   for (unsigned int i=0; i<sizeof(x)/8; i++) {
      H2EQUAL_INTEGER(x[i][1], bitrev32(x[i][0])); 
      H2EQUAL_INTEGER(x[i][0], bitrev32(x[i][1]));  
   }
}

H2CASE(bitopz, bitrev_0)
{ 
   unsigned char x[] = {0x14, 0xE5, 0x38};
   unsigned char y[] = {0x14, 0xE5, 0x38};
   bitrev(x, 0);
   H2EQUAL_INTEGER(y[0], x[0]);
   H2EQUAL_INTEGER(y[1], x[1]);
   H2EQUAL_INTEGER(y[2], x[2]);   
}

H2CASE(bitopz, bitrev_odd)
{ 
   unsigned char x[] = {0x01, 0xA5, 0x30};
   unsigned char y[] = {0x0C, 0xA5, 0x80};
   bitrev(x, 3);
   H2EQUAL_INTEGER(y[0], x[0]);
   H2EQUAL_INTEGER(y[1], x[1]);
   H2EQUAL_INTEGER(y[2], x[2]);   
}

H2CASE(bitopz, bitrev_even)
{ 
   unsigned char x[] = {0x3D, 0xA5, 0x72, 0xF6};
   unsigned char y[] = {0x6F, 0x4E, 0xA5, 0xBC};
   bitrev(x, 4);
   H2EQUAL_INTEGER(y[0], x[0]);
   H2EQUAL_INTEGER(y[1], x[1]);
   H2EQUAL_INTEGER(y[2], x[2]);   
   H2EQUAL_INTEGER(y[3], x[3]); 
}

H2CASE(bitopz, single_bit_u8)
{ 
   unsigned char x[256];
   memset(x, 0, sizeof(x));
   x[0] = 0;
   x[1<<0] = 1;
   x[1<<1] = 1;
   x[1<<2] = 1;
   x[1<<3] = 1;
   x[1<<4] = 1;
   x[1<<5] = 1;
   x[1<<6] = 1;
   x[1<<7] = 1;
   for (unsigned int i=0; i<sizeof(x); i++) {
      H2EQUAL_INTEGER(x[i], single_bit(i));
   }
}

H2CASE(bitopz, multi_bits_u8)
{ 
   unsigned char x[256];
   memset(x, 1, sizeof(x));
   x[0] = 0;
   x[1<<0] = 0;
   x[1<<1] = 0;
   x[1<<2] = 0;
   x[1<<3] = 0;
   x[1<<4] = 0;
   x[1<<5] = 0;
   x[1<<6] = 0;
   x[1<<7] = 0;
   for (unsigned int i=0; i<sizeof(x); i++) {
      H2EQUAL_INTEGER(x[i], (unsigned char)multi_bits(i));
   }
}

H2CASE(bitopz, none_bits_word)
{ 
   H2EQUAL_INTEGER(1, none_bits(0x0));   
   H2EQUAL_INTEGER(0, none_bits(0xfe12));
   H2EQUAL_INTEGER(0, none_bits(0x0800));
   H2EQUAL_INTEGER(0, none_bits(0x0804));
   H2EQUAL_INTEGER(0, none_bits(0x080400C0));
   H2EQUAL_INTEGER(0, none_bits(0x80000000));
   H2EQUAL_INTEGER(0, none_bits(0x00000001));
   H2EQUAL_INTEGER(0, none_bits(0x80000001));
}

H2CASE(bitopz, multi_bits_word)
{ 
   H2EQUAL_INTEGER(1, multi_bits(0xfe12));
   H2EQUAL_INTEGER(0, multi_bits(0x0800));
   H2EQUAL_INTEGER(1, multi_bits(0x0804));
   H2EQUAL_INTEGER(1, multi_bits(0x080400C0));
   H2EQUAL_INTEGER(0, multi_bits(0x80000000));
   H2EQUAL_INTEGER(0, multi_bits(0x00000001));
   H2EQUAL_INTEGER(1, multi_bits(0x80000001));
}


H2CASE(bitopz, single_bit_word)
{ 
   H2EQUAL_INTEGER(0, single_bit(0x0));
   H2EQUAL_INTEGER(1, single_bit(0x1));
   H2EQUAL_INTEGER(1, single_bit(0x2));  
   H2EQUAL_INTEGER(1, single_bit(0x4));
   H2EQUAL_INTEGER(1, single_bit(0x8));
   H2EQUAL_INTEGER(1, single_bit(0x10));
   H2EQUAL_INTEGER(1, single_bit(0x20));  
   H2EQUAL_INTEGER(1, single_bit(0x40));
   H2EQUAL_INTEGER(1, single_bit(0x80));
   H2EQUAL_INTEGER(1, single_bit(0x10));
   H2EQUAL_INTEGER(1, single_bit(0x20));  
   H2EQUAL_INTEGER(1, single_bit(0x40));
   H2EQUAL_INTEGER(1, single_bit(0x80));
   H2EQUAL_INTEGER(1, single_bit(0x100));
   H2EQUAL_INTEGER(1, single_bit(0x200));  
   H2EQUAL_INTEGER(1, single_bit(0x400));
   H2EQUAL_INTEGER(1, single_bit(0x800));

   H2EQUAL_INTEGER(0, single_bit(0xfe12));
   H2EQUAL_INTEGER(0, single_bit(0x0804));
   H2EQUAL_INTEGER(0, single_bit(0x080400C0));
   H2EQUAL_INTEGER(1, single_bit(0x80000000));
   H2EQUAL_INTEGER(1, single_bit(0x00000001));
   H2EQUAL_INTEGER(0, single_bit(0x80000001));
}



