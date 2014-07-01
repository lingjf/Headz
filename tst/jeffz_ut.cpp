#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jeffz.h"

extern "C" { 
extern void __arrange_str(char *buff, int blen, char *str, int slen, int bal);
extern char* __print_integer(void *addr, int bytes, int sign, int base, int cases, int pad, int zero);

}



H2SUITE(jeffz_internal)
{ 
public:
   char * uu;
	void setup()
  	{ 
  	}

  	void teardown()
  	{  		
  	}  	  	
} ;

#define HEX_FMT 0 /*can't modified*/
#define DEC_FMT 1 /*can't modified*/
#define OCT_FMT 2 /*can't modified*/
#define BIN_FMT 3 /*can't modified*/
#define DATA_UNSIGNED 0 /*can't modified*/
#define DATA_SIGNED 1   /*can't modified*/
#define UPPER_CASE 0 
#define LOWER_CASE 1

char * __printf_bin8(char *buff, unsigned char k, int pad, int zero)
{
   int i;
   sprintf(buff, "%c%c%c%c%c%c%c%c", k>> 7&1?'1':'0',
                                     k>> 6&1?'1':'0',
                                     k>> 5&1?'1':'0',
                                     k>> 4&1?'1':'0',
                                     k>> 3&1?'1':'0',
                                     k>> 2&1?'1':'0',
                                     k>> 1&1?'1':'0',
                                     k    &1?'1':'0');

   for (i=0; i<7; i++) {
      if (buff[i] == '0') {
         if (!zero) 
            buff[i] = ' ';
      }else
         break;
   }
   if (pad)
      return buff;
   else
      return &buff[i];
}


char * __printf_bin16(char *buff, unsigned short k, int pad, int zero)
{
   int i;
   sprintf(buff, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 
                                     k>>15&1?'1':'0',
                                     k>>14&1?'1':'0',
                                     k>>13&1?'1':'0',
                                     k>>12&1?'1':'0',
                                     k>>11&1?'1':'0',
                                     k>>10&1?'1':'0',
                                     k>> 9&1?'1':'0',
                                     k>> 8&1?'1':'0',
                                     k>> 7&1?'1':'0',
                                     k>> 6&1?'1':'0',
                                     k>> 5&1?'1':'0',
                                     k>> 4&1?'1':'0',
                                     k>> 3&1?'1':'0',
                                     k>> 2&1?'1':'0',
                                     k>> 1&1?'1':'0',
                                     k    &1?'1':'0');

   for (i=0; i<15; i++) {
      if (buff[i] == '0') {
         if (!zero) 
            buff[i] = ' ';
      }else
         break;
   }
   if (pad)
      return buff;
   else
      return &buff[i];
}


char * __printf_bin32(char *buff, unsigned int k, int pad, int zero)
{
   int i;
   sprintf(buff, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 
                                          k>>31&1?'1':'0', 
                                          k>>30&1?'1':'0',
                                          k>>29&1?'1':'0',
                                          k>>28&1?'1':'0',
                                          k>>27&1?'1':'0',
                                          k>>26&1?'1':'0',
                                          k>>25&1?'1':'0',
                                          k>>24&1?'1':'0',
                                          k>>23&1?'1':'0',
                                          k>>22&1?'1':'0',
                                          k>>21&1?'1':'0',
                                          k>>20&1?'1':'0',
                                          k>>19&1?'1':'0',
                                          k>>18&1?'1':'0',
                                          k>>17&1?'1':'0',
                                          k>>16&1?'1':'0',
                                          k>>15&1?'1':'0',
                                          k>>14&1?'1':'0',
                                          k>>13&1?'1':'0',
                                          k>>12&1?'1':'0',
                                          k>>11&1?'1':'0',
                                          k>>10&1?'1':'0',
                                          k>> 9&1?'1':'0',
                                          k>> 8&1?'1':'0',
                                          k>> 7&1?'1':'0',
                                          k>> 6&1?'1':'0',
                                          k>> 5&1?'1':'0',
                                          k>> 4&1?'1':'0',
                                          k>> 3&1?'1':'0',
                                          k>> 2&1?'1':'0',
                                          k>> 1&1?'1':'0',
                                          k    &1?'1':'0');

   for (i=0; i<31; i++) {
      if (buff[i] == '0') {
         if (!zero) 
            buff[i] = ' ';
      }else
         break;
   }
   if (pad)
      return buff;
   else
      return &buff[i];
}

#ifdef _WIN32
#define LL "I64"
#else
#define LL "ll"
#endif

char * __printf_bin64(char *buff, unsigned llong k, int pad, int zero)
{
   int i;
   unsigned int * t = (unsigned int*)&k;
   
   __printf_bin32(buff+0, *(t+1), 1, 1);
   __printf_bin32(buff+32, *t, 1, 1);
   
   for (i=0; i<63; i++) {
      if (buff[i] == '0') {
         if (!zero) 
            buff[i] = ' ';
      }else
         break;
   }
   if (pad)
      return buff;
   else
      return &buff[i];
  
}

H2CASE(jeffz_internal, __print_integer_unsigned_char)
{ 
   char expd[66], *rest;
   int i; unsigned char value = 0;
   for (i=0; i<256; i++, value++) {
      rest = __print_integer(&value, 1, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%X", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%X", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%2X", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%02X", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%x", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%x", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%2x", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%02x", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%u", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%u", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%3u", value); H2EQUAL_STRCMP(expd, rest); 
         
      rest = __print_integer(&value, 1, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%03u", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
         sprintf(expd, "%o", value); H2EQUAL_STRCMP(expd, rest); 
         
      rest = __print_integer(&value, 1, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
         sprintf(expd, "%o", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
         sprintf(expd, "%3o", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
         sprintf(expd, "%03o", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 2));
         H2EQUAL_STRCMP(__printf_bin8(expd,value,0, 0), rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 2));
         H2EQUAL_STRCMP(__printf_bin8(expd,value,0, 1), rest); 
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 2));
         H2EQUAL_STRCMP(__printf_bin8(expd,value,1, 0), rest);  
      
      rest = __print_integer(&value, 1, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 2));
         H2EQUAL_STRCMP(__printf_bin8(expd,value,1, 1), rest); 
   }
}


H2CASE(jeffz_internal, __print_integer_signed_char)
{ 
   char expd[66], *rest;
   int i; signed char value = 0;
   for (i=0; i<256; i++, value++) {   
      rest = __print_integer(&value, 1, DATA_SIGNED, HEX_FMT, UPPER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 1, DATA_SIGNED, HEX_FMT, UPPER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 1, DATA_SIGNED, HEX_FMT, UPPER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 1, DATA_SIGNED, HEX_FMT, UPPER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 1, DATA_SIGNED, HEX_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 1, DATA_SIGNED, HEX_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 1, DATA_SIGNED, HEX_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 1, DATA_SIGNED, HEX_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         
      rest = __print_integer(&value, 1, DATA_SIGNED, DEC_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%d", value); H2EQUAL_STRCMP(expd, rest); 
      rest = __print_integer(&value, 1, DATA_SIGNED, DEC_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%d", value); H2EQUAL_STRCMP(expd, rest); 
      rest = __print_integer(&value, 1, DATA_SIGNED, DEC_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%4d", value); H2EQUAL_STRCMP(expd, rest); 
      rest = __print_integer(&value, 1, DATA_SIGNED, DEC_FMT, LOWER_CASE, 1, 1); 
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%04d", value); H2EQUAL_STRCMP(expd, rest); 

      rest = __print_integer(&value, 1, DATA_SIGNED, OCT_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
      rest = __print_integer(&value, 1, DATA_SIGNED, OCT_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
      rest = __print_integer(&value, 1, DATA_SIGNED, OCT_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
      rest = __print_integer(&value, 1, DATA_SIGNED, OCT_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
    
      rest = __print_integer(&value, 1, DATA_SIGNED, BIN_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_STRCMP(__printf_bin8(expd,value,0, 0), rest); 
      rest = __print_integer(&value, 1, DATA_SIGNED, BIN_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_STRCMP(__printf_bin8(expd,value,0, 1), rest); 
      rest = __print_integer(&value, 1, DATA_SIGNED, BIN_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_STRCMP(__printf_bin8(expd,value,1, 0), rest);  
      rest = __print_integer(&value, 1, DATA_SIGNED, BIN_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_STRCMP(__printf_bin8(expd,value,1, 1), rest); 
    
   }
}


H2CASE(jeffz_internal, __print_integer_unsigned_short)
{ 
   char expd[66], *rest;
   int i; unsigned short value = 0;
   for (i=0; i<256; i++, value+=0x0101) {
      rest = __print_integer(&value, 2, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%X", value); H2EQUAL_STRCMP(expd, rest); 

      rest = __print_integer(&value, 2, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%X", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 2, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%4X", value); H2EQUAL_STRCMP(expd, rest); 
     
      rest = __print_integer(&value, 2, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%04X", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 2, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%x", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 2, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%x", value); H2EQUAL_STRCMP(expd, rest); 
     
      rest = __print_integer(&value, 2, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%4x", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 2, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
         sprintf(expd, "%04x", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 2, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%u", value); H2EQUAL_STRCMP(expd, rest); 
       
      rest = __print_integer(&value, 2, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%u", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 2, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%5u", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 2, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%05u", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 2, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
         sprintf(expd, "%o", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 2, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
         sprintf(expd, "%o", value); H2EQUAL_STRCMP(expd, rest); 
       
      rest = __print_integer(&value, 2, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
         sprintf(expd, "%6o", value); H2EQUAL_STRCMP(expd, rest); 
   
      rest = __print_integer(&value, 2, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
         sprintf(expd, "%06o", value); H2EQUAL_STRCMP(expd, rest); 
         
      rest = __print_integer(&value, 2, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 2));
         H2EQUAL_STRCMP(__printf_bin16(expd,value,0, 0), rest); 
     
      rest = __print_integer(&value, 2, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 2));
         H2EQUAL_STRCMP(__printf_bin16(expd,value,0, 1), rest); 
      
      rest = __print_integer(&value, 2, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 2));
         H2EQUAL_STRCMP(__printf_bin16(expd,value,1, 0), rest);  
      
      rest = __print_integer(&value, 2, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 2));
         H2EQUAL_STRCMP(__printf_bin16(expd,value,1, 1), rest);        
   }
}


H2CASE(jeffz_internal, __print_integer_signed_short)
{ 
   char expd[66], *rest;
   int i; signed short value = 0;
   for (i=0; i<256; i++, value+=0x0101) { 
      rest = __print_integer(&value, 2, DATA_SIGNED, HEX_FMT, UPPER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 2, DATA_SIGNED, HEX_FMT, UPPER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 2, DATA_SIGNED, HEX_FMT, UPPER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 2, DATA_SIGNED, HEX_FMT, UPPER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 2, DATA_SIGNED, HEX_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 2, DATA_SIGNED, HEX_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 2, DATA_SIGNED, HEX_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 2, DATA_SIGNED, HEX_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
       
      rest = __print_integer(&value, 2, DATA_SIGNED, DEC_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%d", value); H2EQUAL_STRCMP(expd, rest); 
      rest = __print_integer(&value, 2, DATA_SIGNED, DEC_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%d", value); H2EQUAL_STRCMP(expd, rest); 
      rest = __print_integer(&value, 2, DATA_SIGNED, DEC_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%6d", value); H2EQUAL_STRCMP(expd, rest); 
      rest = __print_integer(&value, 2, DATA_SIGNED, DEC_FMT, LOWER_CASE, 1, 1); 
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%06d", value); H2EQUAL_STRCMP(expd, rest); 
   
      rest = __print_integer(&value, 2, DATA_SIGNED, OCT_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
      rest = __print_integer(&value, 2, DATA_SIGNED, OCT_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
      rest = __print_integer(&value, 2, DATA_SIGNED, OCT_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
      rest = __print_integer(&value, 2, DATA_SIGNED, OCT_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));

      rest = __print_integer(&value, 2, DATA_SIGNED, BIN_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_STRCMP(__printf_bin16(expd,value,0, 0), rest); 
      rest = __print_integer(&value, 2, DATA_SIGNED, BIN_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_STRCMP(__printf_bin16(expd,value,0, 1), rest); 
      rest = __print_integer(&value, 2, DATA_SIGNED, BIN_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_STRCMP(__printf_bin16(expd,value,1, 0), rest);  
      rest = __print_integer(&value, 2, DATA_SIGNED, BIN_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_STRCMP(__printf_bin16(expd,value,1, 1), rest);     
   }
}


H2CASE(jeffz_internal, __print_integer_unsigned_long)
{ 
   char expd[66], *rest;
   int i; unsigned int value = 0;
   for (i=0; i<256; i++, value+=0x01010101) {
      rest = __print_integer(&value, 4, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 16));
         sprintf(expd, "%X", value); H2EQUAL_STRCMP(expd, rest); 

      rest = __print_integer(&value, 4, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 16));
         sprintf(expd, "%X", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 4, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 16));
         sprintf(expd, "%8X", value); H2EQUAL_STRCMP(expd, rest); 
     
      rest = __print_integer(&value, 4, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 16));
         sprintf(expd, "%08X", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 4, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 16));
         sprintf(expd, "%x", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 4, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 16));
         sprintf(expd, "%x", value); H2EQUAL_STRCMP(expd, rest); 
     
      rest = __print_integer(&value, 4, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 16));
         sprintf(expd, "%8x", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 4, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 16));
         sprintf(expd, "%08x", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 4, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 10));
         sprintf(expd, "%u", value); H2EQUAL_STRCMP(expd, rest); 
       
      rest = __print_integer(&value, 4, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 10));
         sprintf(expd, "%u", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 4, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 10));
         sprintf(expd, "%10u", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 4, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 10));
         sprintf(expd, "%010u", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 4, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 8));
         sprintf(expd, "%o", value); H2EQUAL_STRCMP(expd, rest); 
      
      rest = __print_integer(&value, 4, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 8));
         sprintf(expd, "%o", value); H2EQUAL_STRCMP(expd, rest); 
       
      rest = __print_integer(&value, 4, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 8));
         sprintf(expd, "%11o", value); H2EQUAL_STRCMP(expd, rest); 
   
      rest = __print_integer(&value, 4, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 8));
         sprintf(expd, "%011o", value); H2EQUAL_STRCMP(expd, rest); 
         
      rest = __print_integer(&value, 4, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 2));
         H2EQUAL_STRCMP(__printf_bin32(expd,value,0, 0), rest); 
     
      rest = __print_integer(&value, 4, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 2));
         H2EQUAL_STRCMP(__printf_bin32(expd,value,0, 1), rest); 
      
      rest = __print_integer(&value, 4, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 2));
         H2EQUAL_STRCMP(__printf_bin32(expd,value,1, 0), rest);  
      
      rest = __print_integer(&value, 4, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtoul(rest, &uu, 2));
         H2EQUAL_STRCMP(__printf_bin32(expd,value,1, 1), rest);        
   }
}

H2CASE(jeffz_internal, __print_integer_signed_long)
{ 
   char expd[66], *rest;
   int i; unsigned int value = 0;
   for (i=0; i<256; i++, value+=0x01010101) { 
      rest = __print_integer(&value, 4, DATA_SIGNED, HEX_FMT, UPPER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 4, DATA_SIGNED, HEX_FMT, UPPER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 4, DATA_SIGNED, HEX_FMT, UPPER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 4, DATA_SIGNED, HEX_FMT, UPPER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 4, DATA_SIGNED, HEX_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 4, DATA_SIGNED, HEX_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 4, DATA_SIGNED, HEX_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
      rest = __print_integer(&value, 4, DATA_SIGNED, HEX_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 16));
       
      rest = __print_integer(&value, 4, DATA_SIGNED, DEC_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%d", value); H2EQUAL_STRCMP(expd, rest); 
      rest = __print_integer(&value, 4, DATA_SIGNED, DEC_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%d", value); H2EQUAL_STRCMP(expd, rest); 
      rest = __print_integer(&value, 4, DATA_SIGNED, DEC_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%11d", value); H2EQUAL_STRCMP(expd, rest); 
      rest = __print_integer(&value, 4, DATA_SIGNED, DEC_FMT, LOWER_CASE, 1, 1); 
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 10));
         sprintf(expd, "%011d", value); H2EQUAL_STRCMP(expd, rest); 
   
      rest = __print_integer(&value, 4, DATA_SIGNED, OCT_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
      rest = __print_integer(&value, 4, DATA_SIGNED, OCT_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
      rest = __print_integer(&value, 4, DATA_SIGNED, OCT_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));
      rest = __print_integer(&value, 4, DATA_SIGNED, OCT_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_INTEGER(value, strtol(rest, &uu, 8));

      rest = __print_integer(&value, 4, DATA_SIGNED, BIN_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_STRCMP(__printf_bin32(expd,value,0, 0), rest); 
      rest = __print_integer(&value, 4, DATA_SIGNED, BIN_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_STRCMP(__printf_bin32(expd,value,0, 1), rest); 
      rest = __print_integer(&value, 4, DATA_SIGNED, BIN_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_STRCMP(__printf_bin32(expd,value,1, 0), rest);  
      rest = __print_integer(&value, 4, DATA_SIGNED, BIN_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_STRCMP(__printf_bin32(expd,value,1, 1), rest);     
   }
}


H2CASE(jeffz_internal, __print_integer_unsigned_long_long)
{ 
   char expd[66], *rest;
   int i; unsigned llong value = 0;
   for (i=0; i<256; i++, value+=LLONG_C(0x0101010101010101)) {
      rest = __print_integer(&value, 8, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 0, 0);
         sprintf(expd, "%" LL "X", value); H2EQUAL_STRCMP(expd, rest);

      rest = __print_integer(&value, 8, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 0, 1);
         sprintf(expd, "%" LL "X", value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 1, 0);
         sprintf(expd, "%16" LL "X", value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_UNSIGNED, HEX_FMT, UPPER_CASE, 1, 1);
         sprintf(expd, "%016" LL "X", value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 0, 0);
         sprintf(expd, "%" LL "x", value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 0, 1);
         sprintf(expd, "%" LL "x", value); H2EQUAL_STRCMP(expd, rest);
     
      rest = __print_integer(&value, 8, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 1, 0);
         sprintf(expd, "%16" LL "x", value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_UNSIGNED, HEX_FMT, LOWER_CASE, 1, 1);
         sprintf(expd, "%016" LL "x", value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 0, 0);
         sprintf(expd, "%" LL "u", value); H2EQUAL_STRCMP(expd, rest);
       
      rest = __print_integer(&value, 8, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 0, 1);
         sprintf(expd, "%" LL "u", value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 1, 0);
         sprintf(expd, "%20" LL "u", value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_UNSIGNED, DEC_FMT, LOWER_CASE, 1, 1);
         sprintf(expd, "%020" LL "u", value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 0, 0);
         sprintf(expd, "%" LL "o", value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 0, 1);
         sprintf(expd, "%" LL "o", value); H2EQUAL_STRCMP(expd, rest);
       
      rest = __print_integer(&value, 8, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 1, 0);
         sprintf(expd, "%22" LL "o", value); H2EQUAL_STRCMP(expd, rest);
   
      rest = __print_integer(&value, 8, DATA_UNSIGNED, OCT_FMT, LOWER_CASE, 1, 1);
         sprintf(expd, "%022" LL "o", value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_STRCMP(__printf_bin64(expd,value,0, 0), rest); 
        
      rest = __print_integer(&value, 8, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_STRCMP(__printf_bin64(expd,value,0, 1), rest); 
      
      rest = __print_integer(&value, 8, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_STRCMP(__printf_bin64(expd,value,1, 0), rest);  
      
      rest = __print_integer(&value, 8, DATA_UNSIGNED, BIN_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_STRCMP(__printf_bin64(expd,value,1, 1), rest);   
        
   }
}

H2CASE(jeffz_internal, __print_integer_signed_long_long)
{ 
   char expd[66], tmp[66], *rest;
   int i; signed llong value = 0;
   for (i=0; i<256; i++, value+=LLONG_C(0x0101010101010101)) {
      rest = __print_integer(&value, 8, DATA_SIGNED, HEX_FMT, UPPER_CASE, 0, 0);
         sprintf(expd, value<0 ? "-%" LL "X" : "%" LL "X", value<0?-value:value); H2EQUAL_STRCMP(expd, rest);

      rest = __print_integer(&value, 8, DATA_SIGNED, HEX_FMT, UPPER_CASE, 0, 1);
         sprintf(expd, value<0 ? "-%" LL "X" : "%" LL "X", value<0?-value:value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_SIGNED, HEX_FMT, UPPER_CASE, 1, 0);
         sprintf(tmp, value<0 ? "-%" LL "X" : "%" LL "X", value<0?-value:value); sprintf(expd, "%17s",tmp); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_SIGNED, HEX_FMT, UPPER_CASE, 1, 1);
         sprintf(expd, value<0 ? "-%016" LL "X" : "%017" LL "X", value<0?-value:value); H2EQUAL_STRCMP(expd, rest);
     
      rest = __print_integer(&value, 8, DATA_SIGNED, HEX_FMT, LOWER_CASE, 0, 0);
         sprintf(expd, value<0 ? "-%" LL "x" : "%" LL "x", value<0?-value:value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_SIGNED, HEX_FMT, LOWER_CASE, 0, 1);
         sprintf(expd, value<0 ? "-%" LL "x" : "%" LL "x", value<0?-value:value); H2EQUAL_STRCMP(expd, rest);
     
      rest = __print_integer(&value, 8, DATA_SIGNED, HEX_FMT, LOWER_CASE, 1, 0);
         sprintf(tmp, value<0 ? "-%" LL "x" : "%" LL "x", value<0?-value:value); sprintf(expd, "%17s",tmp); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_SIGNED, HEX_FMT, LOWER_CASE, 1, 1);
         sprintf(expd, value<0 ? "-%016" LL "x" : "%017" LL "x", value<0?-value:value); H2EQUAL_STRCMP(expd, rest);
       
      rest = __print_integer(&value, 8, DATA_SIGNED, DEC_FMT, LOWER_CASE, 0, 0);
         sprintf(expd, "%" LL "d", value); H2EQUAL_STRCMP(expd, rest);
       
      rest = __print_integer(&value, 8, DATA_SIGNED, DEC_FMT, LOWER_CASE, 0, 1);
         sprintf(expd, "%" LL "d", value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_SIGNED, DEC_FMT, LOWER_CASE, 1, 0);
         sprintf(expd, "%20" LL "d", value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_SIGNED, DEC_FMT, LOWER_CASE, 1, 1);
         sprintf(expd, "%020" LL "d", value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_SIGNED, OCT_FMT, LOWER_CASE, 0, 0);
         sprintf(expd, value<0 ? "-%" LL "o" : "%" LL "o", value<0?-value:value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_SIGNED, OCT_FMT, LOWER_CASE, 0, 1);
         sprintf(expd, value<0 ? "-%" LL "o" : "%" LL "o", value<0?-value:value); H2EQUAL_STRCMP(expd, rest);
       
      rest = __print_integer(&value, 8, DATA_SIGNED, OCT_FMT, LOWER_CASE, 1, 0);
         sprintf(tmp, value<0 ? "-%" LL "o" : "%" LL "o", value<0?-value:value); sprintf(expd, "%22s",tmp); H2EQUAL_STRCMP(expd, rest);
   
      rest = __print_integer(&value, 8, DATA_SIGNED, OCT_FMT, LOWER_CASE, 1, 1);
         sprintf(expd, value<0 ? "-%021" LL "o" : "%022" LL "o", value<0?-value:value); H2EQUAL_STRCMP(expd, rest);
      
      rest = __print_integer(&value, 8, DATA_SIGNED, BIN_FMT, LOWER_CASE, 0, 0);
         H2EQUAL_STRCMP(__printf_bin64(expd,value,0, 0), rest); 
        
      rest = __print_integer(&value, 8, DATA_SIGNED, BIN_FMT, LOWER_CASE, 0, 1);
         H2EQUAL_STRCMP(__printf_bin64(expd,value,0, 1), rest); 
      
      rest = __print_integer(&value, 8, DATA_SIGNED, BIN_FMT, LOWER_CASE, 1, 0);
         H2EQUAL_STRCMP(__printf_bin64(expd,value,1, 0), rest);  
      
      rest = __print_integer(&value, 8, DATA_SIGNED, BIN_FMT, LOWER_CASE, 1, 1);
         H2EQUAL_STRCMP(__printf_bin64(expd,value,1, 1), rest);           
   }
}

H2CASE(jeffz_internal, __arrange_str_middle)
{ 
   char buff[100];
   char *expected;
   
   char *str1 = "1";
      {
         expected = "1";
         __arrange_str(buff, 1, str1, strlen(str1), 0);
         H2EQUAL_STRCMP(expected, buff);
         
         expected = "1 ";
         __arrange_str(buff, 2, str1, strlen(str1), 0);
         H2EQUAL_STRCMP(expected, buff);

         expected = " 1 ";
         __arrange_str(buff, 3, str1, strlen(str1), 0);
         H2EQUAL_STRCMP(expected, buff);
      }

   char *str2 = "12";
      {
         expected = "1";
         __arrange_str(buff, 1, str2, strlen(str2), 0);
         H2EQUAL_STRCMP(expected, buff);
         
         expected = "12";
         __arrange_str(buff, 2, str2, strlen(str2), 0);
         H2EQUAL_STRCMP(expected, buff);

         expected = "12 ";
         __arrange_str(buff, 3, str2, strlen(str2), 0);
         H2EQUAL_STRCMP(expected, buff);

         expected = " 12 ";
         __arrange_str(buff, 4, str2, strlen(str2), 0);
         H2EQUAL_STRCMP(expected, buff);
      }
   char *str3 = "123";
      {
         expected = "1";
         __arrange_str(buff, 1, str3, strlen(str3), 0);
         H2EQUAL_STRCMP(expected, buff);
         
         expected = "12";
         __arrange_str(buff, 2, str3, strlen(str3), 0);
         H2EQUAL_STRCMP(expected, buff);

         expected = "123";
         __arrange_str(buff, 3, str3, strlen(str3), 0);
         H2EQUAL_STRCMP(expected, buff);

         expected = "123 ";
         __arrange_str(buff, 4, str3, strlen(str3), 0);
         H2EQUAL_STRCMP(expected, buff);

         expected = " 123 ";
         __arrange_str(buff, 5, str3, strlen(str3), 0);
         H2EQUAL_STRCMP(expected, buff);
      }
}


H2CASE(jeffz_internal, __arrange_str_left)
{ 
   char buff[100];
   char *expected;
   
   char *str1 = "1";
      {
         expected = "1";
         __arrange_str(buff, 1, str1, strlen(str1), -1);
         H2EQUAL_STRCMP(expected, buff);
         
         expected = "1 ";
         __arrange_str(buff, 2, str1, strlen(str1), -1);
         H2EQUAL_STRCMP(expected, buff);

         expected = "1  ";
         __arrange_str(buff, 3, str1, strlen(str1), -1);
         H2EQUAL_STRCMP(expected, buff);
      }

   char *str2 = "12";
      {
         expected = "1";
         __arrange_str(buff, 1, str2, strlen(str2), -1);
         H2EQUAL_STRCMP(expected, buff);
         
         expected = "12";
         __arrange_str(buff, 2, str2, strlen(str2), -1);
         H2EQUAL_STRCMP(expected, buff);

         expected = "12 ";
         __arrange_str(buff, 3, str2, strlen(str2), -1);
         H2EQUAL_STRCMP(expected, buff);

         expected = "12  ";
         __arrange_str(buff, 4, str2, strlen(str2), -1);
         H2EQUAL_STRCMP(expected, buff);
      }
   char *str3 = "123";
      {
         expected = "1";
         __arrange_str(buff, 1, str3, strlen(str3), -1);
         H2EQUAL_STRCMP(expected, buff);
         
         expected = "12";
         __arrange_str(buff, 2, str3, strlen(str3), -1);
         H2EQUAL_STRCMP(expected, buff);

         expected = "123";
         __arrange_str(buff, 3, str3, strlen(str3), -1);
         H2EQUAL_STRCMP(expected, buff);

         expected = "123 ";
         __arrange_str(buff, 4, str3, strlen(str3), -1);
         H2EQUAL_STRCMP(expected, buff);

         expected = "123  ";
         __arrange_str(buff, 5, str3, strlen(str3), -1);
         H2EQUAL_STRCMP(expected, buff);
      }
}
H2CASE(jeffz_internal, __arrange_str_right)
{ 
   char buff[100];
   char *expected;
   
   char *str1 = "1";
      {
         expected = "1";
         __arrange_str(buff, 1, str1, strlen(str1),1);
         H2EQUAL_STRCMP(expected, buff);
         
         expected = " 1";
         __arrange_str(buff, 2, str1, strlen(str1),1);
         H2EQUAL_STRCMP(expected, buff);

         expected = "  1";
         __arrange_str(buff, 3, str1, strlen(str1),1);
         H2EQUAL_STRCMP(expected, buff);
      }

   char *str2 = "12";
      {
         expected = "2";
         __arrange_str(buff, 1, str2, strlen(str2),1);
         H2EQUAL_STRCMP(expected, buff);
         
         expected = "12";
         __arrange_str(buff, 2, str2, strlen(str2),1);
         H2EQUAL_STRCMP(expected, buff);

         expected = " 12";
         __arrange_str(buff, 3, str2, strlen(str2),1);
         H2EQUAL_STRCMP(expected, buff);

         expected = "  12";
         __arrange_str(buff, 4, str2, strlen(str2),1);
         H2EQUAL_STRCMP(expected, buff);
      }
   char *str3 = "123";
      {
         expected = "3";
         __arrange_str(buff, 1, str3, strlen(str3),1);
         H2EQUAL_STRCMP(expected, buff);
         
         expected = "23";
         __arrange_str(buff, 2, str3, strlen(str3),1);
         H2EQUAL_STRCMP(expected, buff);

         expected = "123";
         __arrange_str(buff, 3, str3, strlen(str3),1);
         H2EQUAL_STRCMP(expected, buff);

         expected = " 123";
         __arrange_str(buff, 4, str3, strlen(str3),1);
         H2EQUAL_STRCMP(expected, buff);

         expected = "  123";
         __arrange_str(buff, 5, str3, strlen(str3),1);
         H2EQUAL_STRCMP(expected, buff);
      }
}
H2SUITE(jeffz)
{ 
public:
   char dpbuffer[1024 * 1024];
   char mybuffer[1000];
   char *mymemory;
	void setup()
  	{ 
  	   mymemory = (char*)__ALIGN32((int)&mybuffer);
      strcpy(mymemory, "abcdefghijklmnopqrstuvwxyz0123456789");
  	}

  	void teardown()
  	{  		
  	}  	  	
} ;

/*========================wide test cases =========================================================================*/

H2CASE(jeffz, jeff_dump_16wide)
{
   char * expected =
"                         dword(hex)              |                word(hex)                |                    byte(hex)                    |       char      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0x00000010h: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
   
   jeff_dump(NULL, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_8wide)
{
   char * expected =
"                dword(hex)     |      word(hex)      |        byte(hex)        |   char  \n"
"              3 2 1 0  7 6 5 4 |  1 0  3 2  5 4  7 6 |  0  1  2  3  4  5  6  7 | 01234567\n"
"0x00000000h: 64636261 68676665 | 6261 6463 6665 6867 | 61 62 63 64 65 66 67 68 | abcdefgh\n"
"0x00000008h: 6C6B6A69 706F6E6D | 6A69 6C6B 6E6D 706F | 69 6A 6B 6C 6D 6E 6F 70 | ijklmnop\n"
"0x00000010h: 74737271 78777675 | 7271 7473 7675 7877 | 71 72 73 74 75 76 77 78 | qrstuvwx\n"
"0x00000018h: 31307A79 35343332 | 7A79 3130 3332 3534 | 79 7A 30 31 32 33 34 35 | yz012345\n"
"0x00000020h: 39383736 00000000 | 3736 3938 0000      | 36 37 38 39 00          | 6789.   \n" ; 
   
   jeff_dump(NULL, dpbuffer, mymemory, strlen(mymemory)+1, 8, JD_ADDR_REL);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_7wide)
{
   char * expected =
"                  byte(hex)       |  char  \n"
"              0  1  2  3  4  5  6 | 0123456\n"
"0x00000000h: 61 62 63 64 65 66 67 | abcdefg\n"
"0x00000007h: 68 69 6A 6B 6C 6D 6E | hijklmn\n"
"0x0000000eh: 6F 70 71 72 73 74 75 | opqrstu\n"
"0x00000015h: 76 77 78 79 7A 30 31 | vwxyz01\n"
"0x0000001ch: 32 33 34 35 36 37 38 | 2345678\n"
"0x00000023h: 39 00                | 9.     \n" ; 
   
   jeff_dump(NULL, dpbuffer, mymemory, strlen(mymemory)+1, 7, JD_ADDR_REL);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_4wide)
{
   char * expected =
"             dword(h) | word(hex) |  byte(hex)  | char\n"
"              3 2 1 0 |  1 0  3 2 |  0  1  2  3 | 0123\n"
"0x00000000h: 64636261 | 6261 6463 | 61 62 63 64 | abcd\n"
"0x00000004h: 68676665 | 6665 6867 | 65 66 67 68 | efgh\n"
"0x00000008h: 6C6B6A69 | 6A69 6C6B | 69 6A 6B 6C | ijkl\n"
"0x0000000ch: 706F6E6D | 6E6D 706F | 6D 6E 6F 70 | mnop\n"
"0x00000010h: 74737271 | 7271 7473 | 71 72 73 74 | qrst\n"
"0x00000014h: 78777675 | 7675 7877 | 75 76 77 78 | uvwx\n"
"0x00000018h: 31307A79 | 7A79 3130 | 79 7A 30 31 | yz01\n"
"0x0000001ch: 35343332 | 3332 3534 | 32 33 34 35 | 2345\n"
"0x00000020h: 39383736 | 3736 3938 | 36 37 38 39 | 6789\n"
"0x00000024h: 00000000 | 0000      | 00          | .   \n" ; 
   
   jeff_dump(NULL, dpbuffer, mymemory, strlen(mymemory)+1, 4, JD_ADDR_REL);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_3wide)
{
   char * expected =
"             byte(h)  | cha\n"
"              0  1  2 | 012\n"
"0x00000000h: 61 62 63 | abc\n"
"0x00000003h: 64 65 66 | def\n"
"0x00000006h: 67 68 69 | ghi\n"
"0x00000009h: 6A 6B 6C | jkl\n"
"0x0000000ch: 6D 6E 6F | mno\n"
"0x0000000fh: 70 71 72 | pqr\n"
"0x00000012h: 73 74 75 | stu\n"
"0x00000015h: 76 77 78 | vwx\n"
"0x00000018h: 79 7A 30 | yz0\n"
"0x0000001bh: 31 32 33 | 123\n"
"0x0000001eh: 34 35 36 | 456\n"
"0x00000021h: 37 38 39 | 789\n"
"0x00000024h: 00       | .  \n" ; 
   
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 3, JD_ADDR_REL);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_2wide)
{
   char * expected =
"             word | byte  | ch\n"
"              1 0 |  0  1 | 01\n"
"0x00000000h: 6261 | 61 62 | ab\n"
"0x00000002h: 6463 | 63 64 | cd\n"
"0x00000004h: 6665 | 65 66 | ef\n"
"0x00000006h: 6867 | 67 68 | gh\n"
"0x00000008h: 6A69 | 69 6A | ij\n"
"0x0000000ah: 6C6B | 6B 6C | kl\n"
"0x0000000ch: 6E6D | 6D 6E | mn\n"
"0x0000000eh: 706F | 6F 70 | op\n"
"0x00000010h: 7271 | 71 72 | qr\n"
"0x00000012h: 7473 | 73 74 | st\n"
"0x00000014h: 7675 | 75 76 | uv\n"
"0x00000016h: 7877 | 77 78 | wx\n"
"0x00000018h: 7A79 | 79 7A | yz\n"
"0x0000001ah: 3130 | 30 31 | 01\n"
"0x0000001ch: 3332 | 32 33 | 23\n"
"0x0000001eh: 3534 | 34 35 | 45\n"
"0x00000020h: 3736 | 36 37 | 67\n"
"0x00000022h: 3938 | 38 39 | 89\n"
"0x00000024h: 0000 | 00    | . \n" ; 
   
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 2, JD_ADDR_REL);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_1wide)
{
   char * expected =
"             by | c\n"
"              0 | 0\n"
"0x00000000h: 61 | a\n"
"0x00000001h: 62 | b\n"
"0x00000002h: 63 | c\n"
"0x00000003h: 64 | d\n"
"0x00000004h: 65 | e\n"
"0x00000005h: 66 | f\n"
"0x00000006h: 67 | g\n"
"0x00000007h: 68 | h\n"
"0x00000008h: 69 | i\n"
"0x00000009h: 6A | j\n"
"0x0000000ah: 6B | k\n"
"0x0000000bh: 6C | l\n"
"0x0000000ch: 6D | m\n"
"0x0000000dh: 6E | n\n"
"0x0000000eh: 6F | o\n"
"0x0000000fh: 70 | p\n"
"0x00000010h: 71 | q\n"
"0x00000011h: 72 | r\n"
"0x00000012h: 73 | s\n"
"0x00000013h: 74 | t\n"
"0x00000014h: 75 | u\n"
"0x00000015h: 76 | v\n"
"0x00000016h: 77 | w\n"
"0x00000017h: 78 | x\n"
"0x00000018h: 79 | y\n"
"0x00000019h: 7A | z\n"
"0x0000001ah: 30 | 0\n"
"0x0000001bh: 31 | 1\n"
"0x0000001ch: 32 | 2\n"
"0x0000001dh: 33 | 3\n"
"0x0000001eh: 34 | 4\n"
"0x0000001fh: 35 | 5\n"
"0x00000020h: 36 | 6\n"
"0x00000021h: 37 | 7\n"
"0x00000022h: 38 | 8\n"
"0x00000023h: 39 | 9\n"
"0x00000024h: 00 | .\n" ; 
   
   jeff_dump(NULL, dpbuffer, mymemory, strlen(mymemory)+1, 1, JD_ADDR_REL);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

/*========================wide test cases end =========================================================================*/

/*========================field test cases =========================================================================*/

H2CASE(jeffz, jeff_dump_no_type)
{
   strcpy(mymemory, "abcdefghijklmnopqrstuvwxyz0123456789");
   char * expected =
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0x00000010h: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
   
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_NO_TYPE);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_no_index)
{
   strcpy(mymemory, "abcdefghijklmnopqrstuvwxyz0123456789");
   char * expected =
"                         dword(hex)              |                word(hex)                |                    byte(hex)                    |       char      \n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0x00000010h: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
   
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_NO_INDEX);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_no_addr)
{
   strcpy(mymemory, "abcdefghijklmnopqrstuvwxyz0123456789");
   char * expected =
"            dword(hex)              |                word(hex)                |                    byte(hex)                    |       char      \n"
" 3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"64636261 68676665 6C6B6A69 706F6E6D | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
  
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_NO_ADDR);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_no_dword)
{
   strcpy(mymemory, "abcdefghijklmnopqrstuvwxyz0123456789");
   char * expected =
"                            word(hex)                |                    byte(hex)                    |       char      \n"
"              1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h: 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0x00000010h: 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
 
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_NO_DWORD);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_no_word)
{
   strcpy(mymemory, "abcdefghijklmnopqrstuvwxyz0123456789");
   char * expected =
"                         dword(hex)              |                    byte(hex)                    |       char      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0x00000010h: 74737271 78777675 31307A79 35343332 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                   | 36 37 38 39 00                                  | 6789.           \n" ;

   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_NO_WORD);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_no_byte)
{
   strcpy(mymemory, "abcdefghijklmnopqrstuvwxyz0123456789");
   char * expected =
"                         dword(hex)              |                word(hex)                |       char      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e | 0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | abcdefghijklmnop\n"
"0x00000010h: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | qrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                   | 3736 3938 0000                          | 6789.           \n" ;

   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_NO_BYTE);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_no_char)
{
   strcpy(mymemory, "abcdefghijklmnopqrstuvwxyz0123456789");
   char * expected =
"                         dword(hex)              |                word(hex)                |                    byte(hex)                   \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70\n"
"0x00000010h: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35\n"
"0x00000020h: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                 \n" ;

   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_NO_CHAR);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_no_index_addr_word_byte)
{
   strcpy(mymemory, "abcdefghijklmnopqrstuvwxyz0123456789");
   char * expected =
"            dword(hex)              |       char      \n"
"64636261 68676665 6C6B6A69 706F6E6D | abcdefghijklmnop\n"
"74737271 78777675 31307A79 35343332 | qrstuvwxyz012345\n"
"39383736 00000000                   | 6789.           \n" ;

   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_NO_INDEX | JD_NO_ADDR | JD_NO_WORD | JD_NO_BYTE);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_has_qword_hex)
{
   unsigned char yourmemory[] = {0x82, 0x82, 0x12, 0x82, 0xC4, 0x14, 0xC4, 0xC4,
                                 0x25, 0xF4, 0x54, 0x99, 0xFF, 0x34, 0x9F, 0xFF};
   memcpy(mymemory, yourmemory, sizeof(yourmemory));
   
   char * expected =
"                        qword(hex)             |       char      \n"
"              7 6 5 4 3 2 1 0  f e d c b a 9 8 | 0123456789abcdef\n"
"0x00000000h: C4C414C482128282 FF9F34FF9954F425 | ........%.T..4..\n" ;

   jeff_dump(0, dpbuffer, mymemory, 16, 16, JD_ADDR_REL |JD_NO_BYTE| JD_NO_DWORD|JD_NO_WORD|
      JD_HAS_QWORD );
   H2EQUAL_STRCMP(expected, dpbuffer);
}


/*========================field test cases end=========================================================================*/

/*========================data align test cases =========================================================================*/

H2CASE(jeffz, jeff_dump_align_offset4)
{
   strcpy(mymemory, "abcdefghijklmnopqrstuvwxyz0123456789");
   char * expected =
"                         dword(hex)              |                word(hex)                |                    byte(hex)                    |       char      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h:          68676665 6C6B6A69 706F6E6D |           6665 6867 6A69 6C6B 6E6D 706F |             65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 |     efghijklmnop\n"
"0x00000010h: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ; 
   
   jeff_dump(NULL, dpbuffer, mymemory+4, strlen(mymemory)+1-4, 16, JD_ADDR_REL);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_align_offset3)
{
   char * expected =
"                         dword(hex)              |                word(hex)                |                    byte(hex)                    |       char      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h: 64000000 68676665 6C6B6A69 706F6E6D |      6400 6665 6867 6A69 6C6B 6E6D 706F |          64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 |    defghijklmnop\n"
"0x00000010h: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ; 
   
   jeff_dump(0, dpbuffer, mymemory+3, strlen(mymemory)+1-3, 16, JD_ADDR_REL);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_align_pack)
{
   char * expected =
"                         dword(hex)              |                word(hex)                |                    byte(hex)                    |       char      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h: 67666564 6B6A6968 6F6E6D6C 73727170 | 6564 6766 6968 6B6A 6D6C 6F6E 7170 7372 | 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 | defghijklmnopqrs\n"
"0x00000010h: 77767574 307A7978 34333231 38373635 | 7574 7776 7978 307A 3231 3433 3635 3837 | 74 75 76 77 78 79 7A 30 31 32 33 34 35 36 37 38 | tuvwxyz012345678\n"
"0x00000020h: 00000039                            | 0039                                    | 39 00                                           | 9.              \n" ; 
   
   jeff_dump(0, dpbuffer, mymemory+3, strlen(mymemory)+1-3, 16, JD_ADDR_REL | JD_DATA_PACK);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

/*========================data align test cases end =========================================================================*/

/*========================data case test cases =========================================================================*/
H2CASE(jeffz, jeff_dump_data_case)
{
   char * expected =
"                         dword(hex)              |                word(hex)                |                    byte(hex)                    |       char      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6c6b6a69 706f6e6d | 6261 6463 6665 6867 6a69 6c6b 6e6d 706f | 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 | abcdefghijklmnop\n"
"0x00000010h: 74737271 78777675 31307a79 35343332 | 7271 7473 7675 7877 7a79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7a 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
   
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_DATA_LOWER);
   H2EQUAL_STRCMP(expected, dpbuffer);
}
/*========================data case test cases end =========================================================================*/

/*========================data pad test cases =========================================================================*/

H2CASE(jeffz, jeff_dump_data_pad)
{
   char * expected =
"                         dword(hex)              |                word(hex)                |                    byte(hex)                    |       char      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h:          68676665 6C6B6A69 706F6E6D |           6665 6867 6A69 6C6B 6E6D 706F |          64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 |    defghijklmnop\n"
"0x00000010h: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 39383736                            | 3736 3938                               | 36 37 38 39 00                                  | 6789.           \n" ; 
   
   jeff_dump(0, dpbuffer, mymemory+3, strlen(mymemory)+1-3, 16, JD_ADDR_REL | JD_DATA_NOPAD);
   H2EQUAL_STRCMP(expected, dpbuffer);
}
/*========================data pad test cases end=========================================================================*/

/*========================data sign test cases =========================================================================*/

H2CASE(jeffz, jeff_dump_data_hex_sign)
{
   unsigned char yourmemory[] = {0x82, 0x82, 0x12, 0x82, 0xC4, 0x14, 0xC4, 0xC4};
   memcpy(mymemory, yourmemory, 8);
      
   char * expected =
"                 dword(hex)      |        word(hex)        |            byte(hex)            |   char  \n"
"               3 2 1 0   7 6 5 4 |   1 0   3 2   5 4   7 6 |   0   1   2   3   4   5   6   7 | 01234567\n"
"0x00000000h: -7DED7D7E -3B3BEB3C | -7D7E -7DEE 014C4 -3B3C | -7E -7E 012 -7E -3C 014 -3C -3C | ........\n" ;

   jeff_dump(0, dpbuffer, mymemory, 8, 8, JD_ADDR_REL |JD_DATA_SIGNED);
   
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_data_dec_sign)
{
   unsigned char yourmemory[] = {0x82, 0x82, 0x12, 0x82, 0xC4, 0x14, 0xC4, 0xC4};
   memcpy(mymemory, yourmemory, 8);
      
   char * expected =
"                   dword(dec)        |          word(dec)          |                byte(dec)                |   char  \n"
"                 0-3         4-7     |   0-1    2-3    4-5    6-7  |   0    1    2    3    4    5    6    7  | 01234567\n"
"0x00000000h: -2112716158 -0993782588 | -32126 -32238 005316 -15164 | -126 -126 0018 -126 -060 0020 -060 -060 | ........\n" ;
 
   jeff_dump(0, dpbuffer, mymemory, 8, 8, JD_ADDR_REL | JD_DATA_DEC |JD_DATA_SIGNED);
   
   H2EQUAL_STRCMP(expected, dpbuffer);
}


H2CASE(jeffz, jeff_dump_data_oct_sign)
{
   unsigned char yourmemory[] = {0x82, 0x82, 0x12, 0x82, 0xC4, 0x14, 0xC4, 0xC4};
   memcpy(mymemory, yourmemory, 8);
      
   char * expected =
"                    dword(oct)         |            word(oct)            |                byte(oct)                |   char  \n"
"                  0-3          4-7     |   0-1     2-3     4-5     6-7   |   0    1    2    3    4    5    6    7  | 01234567\n"
"0x00000000h: -17573276576 -07316765474 | -076576 -076756 0012304 -035474 | -176 -176 0022 -176 -074 0024 -074 -074 | ........\n" ;

   jeff_dump(0, dpbuffer, mymemory, 8, 8, JD_ADDR_REL | JD_DATA_OCT |JD_DATA_SIGNED);
   
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_data_mix_sign)
{
   unsigned char yourmemory[] = {0x82, 0x82, 0x12, 0x82, 0xC4, 0x14, 0xC4, 0xC4};
   memcpy(mymemory, yourmemory, 8);
      
   char * expected =
"                 dword(hex)      |          word(dec)          |                byte(oct)                |   char  \n"
"               3 2 1 0   7 6 5 4 |   0-1    2-3    4-5    6-7  |   0    1    2    3    4    5    6    7  | 01234567\n"
"0x00000000h: -7DED7D7E -3B3BEB3C | -32126 -32238 005316 -15164 | -176 -176 0022 -176 -074 0024 -074 -074 | ........\n" ;

   jeff_dump(0, dpbuffer, mymemory, 8, 8, JD_ADDR_REL | JD_DWORD_HEX | JD_WORD_DEC | JD_BYTE_OCT |JD_DATA_SIGNED);
   
   H2EQUAL_STRCMP(expected, dpbuffer);
}

/*========================data sign test cases end=========================================================================*/

/*========================data fmt test cases =========================================================================*/

H2CASE(jeffz, jeff_dump_data_fmt_dec)
{
   char * expected =
"                             dword(dec)                  |                    word(dec)                    |                            byte(dec)                            |       char      \n"
"                 0-3        4-7        8-b        c-f    |  0-1   2-3   4-5   6-7   8-9   a-b   c-d   e-f  |  0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f  | 0123456789abcdef\n"
"0x00000000h: 1684234849 1751606885 1818978921 1886350957 | 25185 25699 26213 26727 27241 27755 28269 28783 | 097 098 099 100 101 102 103 104 105 106 107 108 109 110 111 112 | abcdefghijklmnop\n"
"0x00000010h: 1953722993 2021095029 0825260665 0892613426 | 29297 29811 30325 30839 31353 12592 13106 13620 | 113 114 115 116 117 118 119 120 121 122 048 049 050 051 052 053 | qrstuvwxyz012345\n"
"0x00000020h: 0959985462 0000000000                       | 14134 14648 00000                               | 054 055 056 057 000                                             | 6789.           \n" ;
   
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_DATA_DEC);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_data_fmt_dec2)
{
   char * expected =
"                             dword(dec)                  |                word(hex)                |                    byte(hex)                    |       char      \n"
"                 0-3        4-7        8-b        c-f    |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h: 1684234849 1751606885 1818978921 1886350957 | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0x00000010h: 1953722993 2021095029 0825260665 0892613426 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 0959985462 0000000000                       | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
  
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_DWORD_DEC );
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_data_fmt_oct)
{
   char * expected =
"                               dword(oct)                    |                        word(oct)                        |                            byte(oct)                            |       char      \n"
"                 0-3         4-7         8-b         c-f     |   0-1    2-3    4-5    6-7    8-9    a-b    c-d    e-f  |  0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f  | 0123456789abcdef\n"
"0x00000000h: 14430661141 15031663145 15432665151 16033667155 | 061141 062143 063145 064147 065151 066153 067155 070157 | 141 142 143 144 145 146 147 150 151 152 153 154 155 156 157 160 | abcdefghijklmnop\n"
"0x00000010h: 16434671161 17035673165 06114075171 06515031462 | 071161 072163 073165 074167 075171 030460 031462 032464 | 161 162 163 164 165 166 167 170 171 172 060 061 062 063 064 065 | qrstuvwxyz012345\n"
"0x00000020h: 07116033466 00000000000                         | 033466 034470 000000                                    | 066 067 070 071 000                                             | 6789.           \n" ;
 
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_DATA_OCT );
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_data_fmt_bin)
{
   char * expected =
"                                        dword(bin)                             |                              word(bin)                              |                                byte(bin)                                |   char  \n"
"             76543213765432127654321176543210 76543217765432167654321576543214 | 7654321176543210 7654321376543212 7654321576543214 7654321776543216 | 76543210 76543211 76543212 76543213 76543214 76543215 76543216 76543217 | 01234567\n"
"0x00000000h: 01100100011000110110001001100001 01101000011001110110011001100101 | 0110001001100001 0110010001100011 0110011001100101 0110100001100111 | 01100001 01100010 01100011 01100100 01100101 01100110 01100111 01101000 | abcdefgh\n"
"0x00000008h: 01101100011010110110101001101001 01110000011011110110111001101101 | 0110101001101001 0110110001101011 0110111001101101 0111000001101111 | 01101001 01101010 01101011 01101100 01101101 01101110 01101111 01110000 | ijklmnop\n"
"0x00000010h: 01110100011100110111001001110001 01111000011101110111011001110101 | 0111001001110001 0111010001110011 0111011001110101 0111100001110111 | 01110001 01110010 01110011 01110100 01110101 01110110 01110111 01111000 | qrstuvwx\n"
"0x00000018h: 00110001001100000111101001111001 00110101001101000011001100110010 | 0111101001111001 0011000100110000 0011001100110010 0011010100110100 | 01111001 01111010 00110000 00110001 00110010 00110011 00110100 00110101 | yz012345\n"
"0x00000020h: 00111001001110000011011100110110 00000000000000000000000000000000 | 0011011100110110 0011100100111000 0000000000000000                  | 00110110 00110111 00111000 00111001 00000000                            | 6789.   \n" ;

   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 8, JD_ADDR_REL | JD_DATA_BIN );
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_data_fmt_bin2)
{
   char * expected =
"                dword(hex)     |      word(hex)      |                                byte(bin)                                |   char  \n"
"              3 2 1 0  7 6 5 4 |  1 0  3 2  5 4  7 6 | 76543210 76543211 76543212 76543213 76543214 76543215 76543216 76543217 | 01234567\n"
"0x00000000h: 64636261 68676665 | 6261 6463 6665 6867 | 01100001 01100010 01100011 01100100 01100101 01100110 01100111 01101000 | abcdefgh\n"
"0x00000008h: 6C6B6A69 706F6E6D | 6A69 6C6B 6E6D 706F | 01101001 01101010 01101011 01101100 01101101 01101110 01101111 01110000 | ijklmnop\n"
"0x00000010h: 74737271 78777675 | 7271 7473 7675 7877 | 01110001 01110010 01110011 01110100 01110101 01110110 01110111 01111000 | qrstuvwx\n"
"0x00000018h: 31307A79 35343332 | 7A79 3130 3332 3534 | 01111001 01111010 00110000 00110001 00110010 00110011 00110100 00110101 | yz012345\n"
"0x00000020h: 39383736 00000000 | 3736 3938 0000      | 00110110 00110111 00111000 00111001 00000000                            | 6789.   \n" ;

   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 8, JD_ADDR_REL | JD_BYTE_BIN );
   H2EQUAL_STRCMP(expected, dpbuffer);
}

/*========================data fmt test cases end=========================================================================*/


/*========================addr test cases =========================================================================*/

H2CASE(jeffz, jeff_dump_addr_fmt_dec)
{
   char * expected =
"                         dword(hex)              |                word(hex)                |                    byte(hex)                    |       char      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0000000000d: 64636261 68676665 6C6B6A69 706F6E6D | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0000000016d: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0000000032d: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
  
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_ADDR_DEC);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_addr_upper)
{
   char * expected =
"                         dword(hex)              |                word(hex)                |                    byte(hex)                    |       char      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000H: 64636261 68676665 6C6B6A69 706F6E6D | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0x00000010H: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020H: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
  
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_ADDR_UPPER);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_addr_upper2)
{
   char * expected =
"                         dword(hex)              |                word(hex)                |                    byte(hex)                    |       char      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0000000000D: 64636261 68676665 6C6B6A69 706F6E6D | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0000000016D: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0000000032D: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
  
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_ADDR_DEC | JD_ADDR_UPPER);
   H2EQUAL_STRCMP(expected, dpbuffer);
}


H2CASE(jeffz, jeff_dump_addr_index)
{
   char * expected =
"                         dword(hex)              |                word(hex)                |                    byte(hex)                    |       char      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0x00000001h: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000002h: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
  
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_INDEX);
   H2EQUAL_STRCMP(expected, dpbuffer);
}
/*========================addr test cases end=========================================================================*/

/*========================index test cases =========================================================================*/

H2CASE(jeffz, jeff_dump_index_fmt_dec)
{
   char * expected =
"                         dword(hex)              |                word(hex)                |                    byte(hex)                    |       char      \n"
"              3 2 1 0  7 6 5 4 1110 9 8 15141312 |  1 0  3 2  5 4  7 6  9 8 1110 1312 1514 |  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 | 0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0x00000010h: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
 
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_INDEX_DEC);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_index_fmt_dec2)
{
   char * expected =
"                             dword(dec)                  |                    word(dec)                    |                            byte(dec)                            |       char      \n"
"                 0-3        4-7       8-11       12-15   |  0-1   2-3   4-5   6-7   8-9  10-11 12-13 14-15 |  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15 | 0123456789abcdef\n"
"0x00000000h: 1684234849 1751606885 1818978921 1886350957 | 25185 25699 26213 26727 27241 27755 28269 28783 | 097 098 099 100 101 102 103 104 105 106 107 108 109 110 111 112 | abcdefghijklmnop\n"
"0x00000010h: 1953722993 2021095029 0825260665 0892613426 | 29297 29811 30325 30839 31353 12592 13106 13620 | 113 114 115 116 117 118 119 120 121 122 048 049 050 051 052 053 | qrstuvwxyz012345\n"
"0x00000020h: 0959985462 0000000000                       | 14134 14648 00000                               | 054 055 056 057 000                                             | 6789.           \n" ;


   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_INDEX_DEC | JD_DATA_DEC);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_index_loop)
{
   char * expected =
"                                           dword(hex)                                |                                    word(hex)                                    |                                            byte(hex)                                            |               char              \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c  3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f | 0123456789abcdef0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D 74737271 78777675 31307A79 35343332 | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F 7271 7473 7675 7877 7A79 3130 3332 3534 | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | abcdefghijklmnopqrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                                                       | 3736 3938 0000                                                                  | 36 37 38 39 00                                                                                  | 6789.                           \n" ;

   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 32, JD_ADDR_REL);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_index_long)
{
   char * expected =
"                                           dword(hex)                                |                                    word(hex)                                    |                                            byte(hex)                                            |               char              \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c 13121110 17161514 1b1a1918 1f1e1d1c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e 1110 1312 1514 1716 1918 1b1a 1d1c 1f1e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f | 0123456789abcdef0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D 74737271 78777675 31307A79 35343332 | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F 7271 7473 7675 7877 7A79 3130 3332 3534 | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | abcdefghijklmnopqrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                                                       | 3736 3938 0000                                                                  | 36 37 38 39 00                                                                                  | 6789.                           \n" ;

   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 32, JD_ADDR_REL | JD_INDEX_LONG);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_index_loop_dec)
{
   char * expected =
"                                           dword(hex)                                |                                    word(hex)                                    |                                            byte(hex)                                            |               char              \n"
"              3 2 1 0  7 6 5 4 1110 9 8 15141312  3 2 1 0  7 6 5 4 1110 9 8 15141312 |  1 0  3 2  5 4  7 6  9 8 1110 1312 1514  1 0  3 2  5 4  7 6  9 8 1110 1312 1514 |  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 | 0123456789abcdef0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D 74737271 78777675 31307A79 35343332 | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F 7271 7473 7675 7877 7A79 3130 3332 3534 | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | abcdefghijklmnopqrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                                                       | 3736 3938 0000                                                                  | 36 37 38 39 00                                                                                  | 6789.                           \n" ;

   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 32, JD_ADDR_REL | JD_INDEX_DEC);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_index_long_dec)
{
   char * expected =
"                                           dword(hex)                                |                                    word(hex)                                    |                                            byte(hex)                                            |               char              \n"
"              3 2 1 0  7 6 5 4 1110 9 8 15141312 19181716 23222120 27262524 31302928 |  1 0  3 2  5 4  7 6  9 8 1110 1312 1514 1716 1918 2120 2322 2524 2726 2928 3130 |  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 | 0123456789abcdef0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D 74737271 78777675 31307A79 35343332 | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F 7271 7473 7675 7877 7A79 3130 3332 3534 | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | abcdefghijklmnopqrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                                                       | 3736 3938 0000                                                                  | 36 37 38 39 00                                                                                  | 6789.                           \n" ;

   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 32, JD_ADDR_REL | JD_INDEX_LONG | JD_INDEX_DEC);
   H2EQUAL_STRCMP(expected, dpbuffer);
}
/*========================index fmt test cases end=========================================================================*/

/*========================type test cases =========================================================================*/

H2CASE(jeffz, jeff_dump_type_case)
{
   char * expected =
"                         DWORD(HEX)              |                WORD(HEX)                |                    BYTE(HEX)                    |       CHAR      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0x00000010h: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
 
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_TYPE_UPPER);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_type_name_abbr)
{
   char * expected =
"                         DWORD(HEX)              |                WORD(HEX)                |                    BYTE(HEX)                    |       CHAR      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0x00000010h: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
 
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_TYPE_UPPER);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

H2CASE(jeffz, jeff_dump_type_name_full)
{
   char * expected =
"                     DWORD(HEXADECIMAL)          |                  WORD(DECIMAL)                  |                BYTE(HEXADECIMAL)                |       CHAR      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  0-1   2-3   4-5   6-7   8-9   a-b   c-d   e-f  |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D | 25185 25699 26213 26727 27241 27755 28269 28783 | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0x00000010h: 74737271 78777675 31307A79 35343332 | 29297 29811 30325 30839 31353 12592 13106 13620 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                   | 14134 14648 00000                               | 36 37 38 39 00                                  | 6789.           \n" ;

   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_TYPE_UPPER | JD_TYPE_FULL | JD_WORD_DEC);
   H2EQUAL_STRCMP(expected, dpbuffer);
}


H2CASE(jeffz, jeff_dump_type_name_short)
{
   char * expected =
"                            dword                |                  word                   |                      byte                       |       char      \n"
"              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  1 0  3 2  5 4  7 6  9 8  b a  d c  f e |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef\n"
"0x00000000h: 64636261 68676665 6C6B6A69 706F6E6D | 6261 6463 6665 6867 6A69 6C6B 6E6D 706F | 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 | abcdefghijklmnop\n"
"0x00000010h: 74737271 78777675 31307A79 35343332 | 7271 7473 7675 7877 7A79 3130 3332 3534 | 71 72 73 74 75 76 77 78 79 7A 30 31 32 33 34 35 | qrstuvwxyz012345\n"
"0x00000020h: 39383736 00000000                   | 3736 3938 0000                          | 36 37 38 39 00                                  | 6789.           \n" ;
 
   jeff_dump(0, dpbuffer, mymemory, strlen(mymemory)+1, 16, JD_ADDR_REL | JD_TYPE_SHORT);
   H2EQUAL_STRCMP(expected, dpbuffer);
}

/*========================type test cases end=========================================================================*/


