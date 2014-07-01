#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

#include "jeffz.h"

#ifdef __cplusplus
extern "C" {
#endif

#define M (32)

#define FIELD_BYTE   0 /*can't modified*/
#define FIELD_WORD   1 /*can't modified*/
#define FIELD_DWORD  2 /*can't modified*/
#define FIELD_QWORD  3 /*can't modified*/
#define FIELD_ADDR   4 
#define FIELD_CHAR   5 
#define FIELD_END    9

#define REAL_ADDRESS 0
#define RELATED_ADDRESS 1
#define INDEX_ADDRESS 2

#define HEX_FMT 0 /*can't modified*/
#define DEC_FMT 1 /*can't modified*/
#define OCT_FMT 2 /*can't modified*/
#define BIN_FMT 3 /*can't modified*/
#define CHR_FMT 4 

#define DATA_UNSIGNED 0 /*can't modified*/
#define DATA_SIGNED 1   /*can't modified*/

#define UPPER_CASE 0 
#define LOWER_CASE 1 

#define LONG_INDEX 0
#define LOOP_INDEX 1

#define DATA_ALIGN 0
#define DATA_PACK 1

#define TYPE_SHORT 0
#define TYPE_ABBR 1
#define TYPE_FULL 2
#define TYPE_ABBR1 3
#define TYPE_ABBR2 4

static char *__dig_vec[2] = { "0123456789abcdefghijklmnopqrstuvwxyz", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" };

#define __QQQ(q, lu, ls, base, up, sign, negative) \
   do {                                            \
      if (sign == DATA_SIGNED && ls < 0) {         \
         lu = ls >= 0 ? ls : -ls;                  \
         negative = 1;                             \
      }                                            \
      do {                                         \
         *--q = __dig_vec[up][lu % base];          \
      } while ( (lu /= base) > 0 );                \
   } while(0)

char* __print_integer(void *addr, int bytes, int sign, int hdob, int cases, int pad, int zero)
{
   static char buffer[65];
   static int max_len[][4][2] = {
   /*        hex      dec      oct      bin */
   /*        u  s     u  s     u  s     u  s   */
   /*1B*/{ { 2, 3 }, { 3, 4 }, { 3, 4 }, { 8, 8 } },
   /*2B*/{ { 4, 5 }, { 5, 6 }, { 6, 7 }, { 16, 16 } },
   /*4B*/{ { 8, 9 }, { 10, 11 }, { 11, 12 }, { 32, 32 } },
   /*8B*/{ { 16, 17 }, { 20, 20 }, { 22, 22 }, { 64, 64 } } };

   int base, negative = 0;
   char *p, *q;
   assert(addr);
   assert(bytes == 1 || bytes == 2 || bytes == 4 || bytes == 8);
   assert(sign == DATA_SIGNED || sign == DATA_UNSIGNED);
   assert(hdob == HEX_FMT || hdob == DEC_FMT || hdob == OCT_FMT || hdob == BIN_FMT || hdob == CHR_FMT);
   assert(cases == UPPER_CASE || cases == LOWER_CASE);
   assert(&pad || &zero);

   memset(buffer, zero ? '0' : ' ', 64);
   p = &buffer[0];
   q = p + 64;
   *q = '\0';

   switch (hdob) {
   case HEX_FMT:
      base = 16;
      break;
   case DEC_FMT:
      base = 10;
      break;
   case OCT_FMT:
      base = 8;
      break;
   case BIN_FMT:
      base = 2;
      sign = DATA_UNSIGNED;
      break;
   default:
      return q;
   }

   switch (bytes) {
   case 1: {
      unsigned char lu08 = *((unsigned char *) addr);
      signed char ls08 = *((signed char *) addr);
      __QQQ(q, lu08, ls08, base, cases == UPPER_CASE, sign, negative);
   }
      break;
   case 2: {
      unsigned short lu16 = *((unsigned short *) addr);
      signed short ls16 = *((signed short *) addr);
      __QQQ(q, lu16, ls16, base, cases == UPPER_CASE, sign, negative);
   }
      break;
   case 4: {
      unsigned long lu32 = *((unsigned long *) addr);
      signed long ls32 = *((signed long *) addr);
      __QQQ(q, lu32, ls32, base, cases == UPPER_CASE, sign, negative);
   }
      break;
   case 8: {
      unsigned llong lu64 = *((unsigned llong *) addr);
      signed llong ls64 = *((signed llong *) addr);
      __QQQ(q, lu64, ls64, base, cases == UPPER_CASE, sign, negative);
   }
      break;
   default:
      return q;
   }

   if (pad) {
      p = p + 64 - max_len[(int) floor(log(bytes + 0.1) / log(2))][hdob][sign];
      if (negative)
         zero ? (*p = '-') : (*(q - 1) = '-');
      return p;
   } else {
      if (negative)
         *--q = '-';
      return q;
   }
}

static int __sprintf_cat(char *sp, const char * fmt, ...)
{
   int sz;
   va_list ap;
   assert(sp && fmt);

   va_start(ap, fmt);
   sz = vsprintf(sp + strlen(sp), fmt, ap);
   va_end(ap);
   return sz;
}

static int __sprintf_pad(char *sp, const char * fmt, ...)
{
   int sz;
   va_list ap;
   assert(sp && fmt);

   sp = sp + strlen(sp);
   va_start(ap, fmt);
   sz = vsprintf(sp, fmt, ap);
   va_end(ap);
   while (*sp)
      *sp++ = ' ';
   return sz;
}

void __arrange_str(char *buff, int blen, char *str, int slen, int bal)
{
   int i, j, bmid, smid, bsrt;
   assert(buff);
   assert(blen >= 0);
   assert(str);
   assert(slen >= 0);
   assert(bal == -1 || bal == 0 || bal == 1);

   if (blen <= 0 || slen <= 0)
      return;

   if (bal == 1) { /*right align*/
      for (i = blen - 1, j = slen - 1; i >= 0; i--, j--)
         buff[i] = j >= 0 ? str[j] : ' ';
   } else {
      if (bal == -1) { /*left align*/
         bsrt = 0;
      } else { /*middle*/
         bmid = (blen + 1) / 2 - 1;
         smid = (slen + 1) / 2 - 1;
         if (slen % 2 == 0 && blen % 2 == 1)
            smid += 1;
         bsrt = bmid - smid;
      }

      for (i = 0; i < bsrt; i++)
         buff[i] = ' ';
      for (j = 0; i < blen; i++, j++)
         buff[i] = j < slen ? str[j] : ' ';
   }
   buff[blen] = '\0';
}

static int __in_range(int prefix_bytes, int data_bytes, int i, int len, int pad)
{
   int j = i + len - 1;
   int p = prefix_bytes;
   int q = prefix_bytes + data_bytes - 1;
   assert(&prefix_bytes || &data_bytes);
   assert(&i || &len || &pad);

   if (p > j || q < i) {
      return 0;
   } else {
      if (p <= i && j <= q)
         return 1;
      else {
         if (pad)
            return 1;
         else
            return 0;
      }
   }
}

static void __type_field(char *buff, int lim, int type_case, char *field, int fmt, int type_name)
{
   char *tmp = "";
   assert(buff);
   assert(lim > 0);
   assert(&type_case);
   assert(field);
   assert(&fmt || &type_name);

   if (type_name == TYPE_FULL) {
      switch (fmt) {
      case HEX_FMT:
         tmp = "hexadecimal";
         break;
      case DEC_FMT:
         tmp = "decimal";
         break;
      case OCT_FMT:
         tmp = "octal";
         break;
      case BIN_FMT:
         tmp = "binary";
         break;
      case CHR_FMT:
         tmp = "character";
         break;
      }
      if (sprintf(buff, "%s(%s)", field, tmp) > lim) {
         type_name = TYPE_ABBR;
      }
   }
   if (type_name == TYPE_ABBR) {
      switch (fmt) {
      case HEX_FMT:
         tmp = "hex";
         break;
      case DEC_FMT:
         tmp = "dec";
         break;
      case OCT_FMT:
         tmp = "oct";
         break;
      case BIN_FMT:
         tmp = "bin";
         break;
      case CHR_FMT:
         tmp = "chr";
         break;
      }
      if (sprintf(buff, "%s(%s)", field, tmp) > lim) {
         type_name = TYPE_ABBR1;
      }
   }

   if (type_name == TYPE_ABBR1) {
      switch (fmt) {
      case HEX_FMT:
         tmp = "h";
         break;
      case DEC_FMT:
         tmp = "d";
         break;
      case OCT_FMT:
         tmp = "o";
         break;
      case BIN_FMT:
         tmp = "b";
         break;
      case CHR_FMT:
         tmp = "c";
         break;
      }
      if (sprintf(buff, "%s(%s)", field, tmp) > lim) {
         type_name = TYPE_SHORT;
      }
   }

   if (type_name == TYPE_SHORT) {
      if (sprintf(buff, "%s", field) > lim) {

      }
   }

   while (*buff) {
      if (type_case == UPPER_CASE)
         *buff = toupper((int)*buff);
      else
         *buff = tolower((int)*buff);
      buff++;
   }
}

typedef struct config_struct
{
   FILE * fp;
   char * sp;
   void * ptr;
   int size;
   int wbytes;
   int flags;

   int state;
   int index;
   int len;
   char buff[512];

   unsigned char type_field :1, type_case :1, type_name :3;

   unsigned char index_field :1, index_mode :1, index_case :1, index_fmt :1;

   unsigned char addr_field :1, addr_type :2, addr_fmt :2, addr_case :1;

   unsigned char qword_field :1, qword_fmt :2, qword_case :1, qword_sign :1;

   unsigned char dword_field :1, dword_fmt :2, dword_case :1, dword_sign :1;

   unsigned char word_field :1, word_fmt :2, word_case :1, word_sign :1;

   unsigned char byte_field :1, byte_fmt :2, byte_case :1, byte_sign :1;

   unsigned char char_field :1;

   unsigned char data_align :1, data_pad :1;

} config_struct;

static void __default_config(config_struct *cfg)
{
   assert(cfg);
   memset(cfg, 0, sizeof(config_struct));
   cfg->wbytes = 16;

   cfg->type_field = 1;
   cfg->type_case = LOWER_CASE;
   cfg->type_name = TYPE_ABBR;

   cfg->index_field = 1;
   cfg->index_mode = LOOP_INDEX;
   cfg->index_fmt = HEX_FMT;
   cfg->index_case = LOWER_CASE;

   cfg->addr_field = 1;
   cfg->addr_type = REAL_ADDRESS;
   cfg->addr_fmt = HEX_FMT;
   cfg->addr_case = LOWER_CASE;

   cfg->qword_field = 0;
   cfg->qword_fmt = HEX_FMT;
   cfg->qword_case = UPPER_CASE;
   cfg->qword_sign = DATA_UNSIGNED;

   cfg->dword_field = 1;
   cfg->dword_fmt = HEX_FMT;
   cfg->dword_case = UPPER_CASE;
   cfg->dword_sign = DATA_UNSIGNED;

   cfg->word_field = 1;
   cfg->word_fmt = HEX_FMT;
   cfg->word_case = UPPER_CASE;
   cfg->word_sign = DATA_UNSIGNED;

   cfg->byte_field = 1;
   cfg->byte_fmt = HEX_FMT;
   cfg->byte_case = UPPER_CASE;
   cfg->byte_sign = DATA_UNSIGNED;

   cfg->char_field = 1;

   cfg->data_align = DATA_ALIGN;
   cfg->data_pad = 1;
}

static void __output_raw(char *content, config_struct *cfg)
{
   int len = 0;
   assert(content && cfg);

   if (cfg->fp) {
      len = fprintf(cfg->fp, "%s", content);
   }
   if (cfg->sp) {
      len = sprintf(cfg->sp, "%s", content);
      cfg->sp += len;
   }
   cfg->len += len;
}

static void __output_field(int field, config_struct *cfg)
{
   assert(&field);
   assert(cfg);

   switch (field) {
   case FIELD_ADDR:
      __output_raw(cfg->buff, cfg);
      cfg->state = 1;
      break;
   case FIELD_QWORD:
   case FIELD_DWORD:
   case FIELD_WORD:
   case FIELD_BYTE:
   case FIELD_CHAR:
      if (cfg->state == 1)
         __output_raw(" ", cfg);
      else if (cfg->state == 2)
         __output_raw(" | ", cfg);
      __output_raw(cfg->buff, cfg);
      cfg->state = 2;
      break;
   case FIELD_END:
      if (cfg->state > 0)
         __output_raw("\n", cfg);
      cfg->state = 0;
      break;
   default:
      break;
   }
   cfg->buff[0] = '\0';
}

static int __format_addr(unsigned char *line_ptr, config_struct *cfg)
{
   unsigned int address;
   const char * format;
   assert(&line_ptr);
   assert(cfg);

   if (cfg->addr_type == INDEX_ADDRESS)
      address = cfg->index;
   else if (cfg->addr_type == RELATED_ADDRESS)
      address = cfg->index * cfg->wbytes;
   else
      address = (unsigned int) line_ptr;

   if (cfg->addr_fmt == DEC_FMT) {
      format = (cfg->addr_case == LOWER_CASE) ? "%010ud:" : "%010uD:";
   } else /*(cfg->addr_fmt == HEX_FMT)*/{
      format = (cfg->addr_case == LOWER_CASE) ? "0x%08xh:" : "0x%08XH:";
   }

   return sprintf(cfg->buff, format, address);
}

static int __format_qword(unsigned char *line, int prefix_bytes, int data_bytes, config_struct *cfg)
{
   int i, len;
   assert(line);
   assert(&prefix_bytes && &data_bytes);
   assert(cfg);

   cfg->buff[0] = '\0';
   for (i = 0; i < cfg->wbytes / 8; i++) {
      char * tmp = __print_integer(&line[i * 8], 8, cfg->qword_sign, cfg->qword_fmt, cfg->qword_case, 1, 1);
      if (__in_range(prefix_bytes, data_bytes, i * 8, 8, cfg->data_pad))
         __sprintf_cat(cfg->buff, "%s ", tmp);
      else
         __sprintf_pad(cfg->buff, "%s ", tmp);
   }
   len = strlen(cfg->buff);
   cfg->buff[len - 1] = '\0';
   return len - 1;
}

static int __format_dword(unsigned char *line, int prefix_bytes, int data_bytes, config_struct *cfg)
{
   int i, len;
   assert(line);
   assert(&prefix_bytes && &data_bytes);
   assert(cfg);

   cfg->buff[0] = '\0';
   for (i = 0; i < cfg->wbytes / 4; i++) {
      char * tmp = __print_integer(&line[i * 4], 4, cfg->dword_sign, cfg->dword_fmt, cfg->dword_case, 1, 1);
      if (__in_range(prefix_bytes, data_bytes, i * 4, 4, cfg->data_pad))
         __sprintf_cat(cfg->buff, "%s ", tmp);
      else
         __sprintf_pad(cfg->buff, "%s ", tmp);
   }
   len = strlen(cfg->buff);
   cfg->buff[len - 1] = '\0';
   return len - 1;
}

static int __format_word(unsigned char *line, int prefix_bytes, int data_bytes, config_struct *cfg)
{
   int i, len;
   assert(line);
   assert(&prefix_bytes && &data_bytes);
   assert(cfg);

   cfg->buff[0] = '\0';
   for (i = 0; i < cfg->wbytes / 2; i++) {
      char * tmp = __print_integer(&line[i * 2], 2, cfg->word_sign, cfg->word_fmt, cfg->word_case, 1, 1);
      if (__in_range(prefix_bytes, data_bytes, i * 2, 2, cfg->data_pad))
         __sprintf_cat(cfg->buff, "%s ", tmp);
      else
         __sprintf_pad(cfg->buff, "%s ", tmp);
   }
   len = strlen(cfg->buff);
   cfg->buff[len - 1] = '\0';
   return len - 1;
}

static int __format_byte(unsigned char *line, int prefix_bytes, int data_bytes, config_struct *cfg)
{
   int i, len;
   assert(line);
   assert(&prefix_bytes && &data_bytes);
   assert(cfg);

   cfg->buff[0] = '\0';
   for (i = 0; i < cfg->wbytes; i++) {
      char * tmp = __print_integer(&line[i], 1, cfg->byte_sign, cfg->byte_fmt, cfg->byte_case, 1, 1);
      if (__in_range(prefix_bytes, data_bytes, i, 1, cfg->data_pad))
         __sprintf_cat(cfg->buff, "%s ", tmp);
      else
         __sprintf_pad(cfg->buff, "%s ", tmp);
   }
   len = strlen(cfg->buff);
   cfg->buff[len - 1] = '\0';
   return len - 1;
}

static int __format_char(unsigned char *line, int prefix_bytes, int data_bytes, config_struct *cfg)
{
   int i;
   assert(line);
   assert(&prefix_bytes && &data_bytes);
   assert(cfg);

   cfg->buff[0] = '\0';
   for (i = 0; i < cfg->wbytes; i++) {
      char k = (!(line[i] & 0x80) && isprint(line[i])) ? line[i] : '.';
      if (__in_range(prefix_bytes, data_bytes, i, 1, cfg->data_pad))
         __sprintf_cat(cfg->buff, "%c", k);
      else
         __sprintf_pad(cfg->buff, "%c", k);
   }
   return strlen(cfg->buff);
}

static void __format_line(unsigned char *data_ptr, int prefix_bytes, int data_bytes, config_struct *cfg)
{
   unsigned char line[M];
   assert(data_ptr);
   assert(&prefix_bytes && &data_bytes);
   assert(cfg);

   memset(line, 0, sizeof(line));
   memcpy(&line[prefix_bytes], data_ptr, data_bytes);

   /*address*/
   if (cfg->addr_field) {
      __format_addr(data_ptr - prefix_bytes, cfg);
      __output_field(FIELD_ADDR, cfg);
   }

   /*dword*/
   if (cfg->qword_field) {
      __format_qword(line, prefix_bytes, data_bytes, cfg);
      __output_field(FIELD_QWORD, cfg);
   }

   /*dword*/
   if (cfg->dword_field) {
      __format_dword(line, prefix_bytes, data_bytes, cfg);
      __output_field(FIELD_DWORD, cfg);
   }

   /*word*/
   if (cfg->word_field) {
      __format_word(line, prefix_bytes, data_bytes, cfg);
      __output_field(FIELD_WORD, cfg);
   }

   /*byte*/
   if (cfg->byte_field) {
      __format_byte(line, prefix_bytes, data_bytes, cfg);
      __output_field(FIELD_BYTE, cfg);
   }

   /*char*/
   if (cfg->char_field) {
      __format_char(line, prefix_bytes, data_bytes, cfg);
      __output_field(FIELD_CHAR, cfg);
   }
   __output_field(FIELD_END, cfg);
}

static unsigned char *__split_line(unsigned char *data_ptr, int *size_bytes, config_struct *cfg)
{
   int prefix_bytes = 0, data_bytes;
   assert(data_ptr);
   assert(size_bytes);
   assert(cfg);

   if (cfg->data_align == DATA_ALIGN)
      prefix_bytes = data_ptr - (unsigned char*) ___ALIGN_DN((int)data_ptr, cfg->wbytes);

   data_bytes = cfg->wbytes - prefix_bytes;
   if (data_bytes > *size_bytes)
      data_bytes = *size_bytes;

   __format_line(data_ptr, prefix_bytes, data_bytes, cfg);

   *size_bytes -= data_bytes;
   return data_ptr + data_bytes;
}

static void __output_type(config_struct *cfg)
{
   int field_len;
   char buff[65];
   assert(cfg);
   if (!cfg->type_field)
      return;

   if (cfg->addr_field) {
      field_len = __format_addr(0, cfg);
      cfg->buff[0] = '\0';
      __arrange_str(cfg->buff, field_len, " ", 1, 0/*middle*/);
      __output_field(FIELD_ADDR, cfg);
   }
   if (cfg->qword_field) {
      field_len = __format_qword((unsigned char*) buff, 0, cfg->wbytes, cfg);
      __type_field(buff, field_len, cfg->type_case, "qword", cfg->qword_fmt, cfg->type_name);
      cfg->buff[0] = '\0';
      __arrange_str(cfg->buff, field_len, buff, strlen(buff), 0/*middle*/);
      __output_field(FIELD_QWORD, cfg);
   }
   if (cfg->dword_field) {
      field_len = __format_dword((unsigned char*) buff, 0, cfg->wbytes, cfg);
      __type_field(buff, field_len, cfg->type_case, "dword", cfg->dword_fmt, cfg->type_name);
      cfg->buff[0] = '\0';
      __arrange_str(cfg->buff, field_len, buff, strlen(buff), 0/*middle*/);
      __output_field(FIELD_DWORD, cfg);
   }
   if (cfg->word_field) {
      field_len = __format_word((unsigned char*) buff, 0, cfg->wbytes, cfg);
      __type_field(buff, field_len, cfg->type_case, "word", cfg->word_fmt, cfg->type_name);
      cfg->buff[0] = '\0';
      __arrange_str(cfg->buff, field_len, buff, strlen(buff), 0/*middle*/);
      __output_field(FIELD_WORD, cfg);
   }
   if (cfg->byte_field) {
      field_len = __format_byte((unsigned char*) buff, 0, cfg->wbytes, cfg);
      __type_field(buff, field_len, cfg->type_case, "byte", cfg->byte_fmt, cfg->type_name);
      cfg->buff[0] = '\0';
      __arrange_str(cfg->buff, field_len, buff, strlen(buff), 0/*middle*/);
      __output_field(FIELD_BYTE, cfg);
   }
   if (cfg->char_field) {
      field_len = __format_char((unsigned char*) buff, 0, cfg->wbytes, cfg);
      __type_field(buff, field_len, cfg->type_case, "char", CHR_FMT, 0);
      cfg->buff[0] = '\0';
      __arrange_str(cfg->buff, field_len, buff, strlen(buff), 0/*middle*/);
      __output_field(FIELD_CHAR, cfg);
   }

   __output_field(FIELD_END, cfg);
}

static void __output_index(config_struct *cfg)
{
   int i, field_len, loop, index_hex;
   char buff[65];
   const char *format;
   assert(cfg);

   if (!cfg->index_field)
      return;

   loop = cfg->index_mode == LOOP_INDEX ? 16 : 0x7fffffff;
   index_hex = cfg->index_fmt == HEX_FMT;

   if (cfg->addr_field) {
      field_len = __format_addr(0, cfg);
      cfg->buff[0] = '\0';
      __arrange_str(cfg->buff, field_len, " ", 1, 0/*middle*/);
      __output_field(FIELD_ADDR, cfg);
   }

   if (cfg->qword_field) {
      unsigned char t[8];
      *((unsigned llong*) t) = LLONG_C(0x0001020304050607);
      field_len = __format_qword((unsigned char*) buff, 0, cfg->wbytes, cfg);
      cfg->buff[0] = '\0';
      switch (cfg->qword_fmt) {
      case DEC_FMT:
      case OCT_FMT: {
         format = index_hex ? "%x-%-x" : "%d-%-d";
         for (i = 0; i < cfg->wbytes / 8; i++) {
            sprintf(buff, format, (8 * i) % loop, (8 * i + 7) % loop);
            __arrange_str(cfg->buff + strlen(cfg->buff), (field_len + 1) / (cfg->wbytes / 8), buff,
                          strlen(buff), 0/*middle*/);
         }
      }
         break;
      case BIN_FMT: {
         for (i = 0; i < cfg->wbytes / 8; i++) {
            __sprintf_cat(cfg->buff,
                          "7654321%x7654321%x7654321%x7654321%x7654321%x7654321%x7654321%x7654321%x ",
                          (8 * i + t[0]) % 16, (8 * i + t[1]) % 16, (8 * i + t[2]) % 16, (8 * i + t[3]) % 16,
                          (8 * i + t[4]) % 16, (8 * i + t[5]) % 16, (8 * i + t[6]) % 16, (8 * i + t[7]) % 16);
         }
      }
         break;
      case HEX_FMT:
      default: {
         format = index_hex ? "%2x%2x%2x%2x%2x%2x%2x%2x " : "%2d%2d%2d%2d%2x%2x%2x%2x ";
         for (i = 0; i < cfg->wbytes / 8; i++) {
            sprintf(buff, format, (8 * i + t[0]) % loop, (8 * i + t[1]) % loop, (8 * i + t[2]) % loop,
                    (8 * i + t[3]) % loop, (8 * i + t[4]) % loop, (8 * i + t[5]) % loop, (8 * i + t[6]) % loop,
                    (8 * i + t[7]) % loop);
            __arrange_str(cfg->buff + strlen(cfg->buff), (field_len + 1) / (cfg->wbytes / 8), buff,
                          strlen(buff), 1/*right align*/);
         }
      }
         break;
      }
      cfg->buff[strlen(cfg->buff) - 1] = '\0';
      __output_field(FIELD_QWORD, cfg);
   }

   if (cfg->dword_field) {
      unsigned char t[4];
      *((unsigned int*) t) = 0x00010203UL;
      field_len = __format_dword((unsigned char*) buff, 0, cfg->wbytes, cfg);
      cfg->buff[0] = '\0';
      switch (cfg->dword_fmt) {
      case DEC_FMT:
      case OCT_FMT: {
         format = index_hex ? "%x-%-x" : "%d-%-d";
         for (i = 0; i < cfg->wbytes / 4; i++) {
            sprintf(buff, format, (4 * i) % loop, (4 * i + 3) % loop);
            __arrange_str(cfg->buff + strlen(cfg->buff), (field_len + 1) / (cfg->wbytes / 4), buff,
                          strlen(buff), 0/*middle*/);
         }
      }
         break;
      case BIN_FMT: {
         for (i = 0; i < cfg->wbytes / 4; i++) {
            __sprintf_cat(cfg->buff, "7654321%x7654321%x7654321%x7654321%x ", (4 * i + t[0]) % 16,
                          (4 * i + t[1]) % 16, (4 * i + t[2]) % 16, (4 * i + t[3]) % 16);
         }
      }
         break;
      case HEX_FMT:
      default: {
         format = index_hex ? "%2x%2x%2x%2x " : "%2d%2d%2d%2d ";
         for (i = 0; i < cfg->wbytes / 4; i++) {
            sprintf(buff, format, (4 * i + t[0]) % loop, (4 * i + t[1]) % loop, (4 * i + t[2]) % loop,
                    (4 * i + t[3]) % loop);
            __arrange_str(cfg->buff + strlen(cfg->buff), (field_len + 1) / (cfg->wbytes / 4), buff,
                          strlen(buff), 1/*right align*/);
         }
      }
         break;
      }
      cfg->buff[strlen(cfg->buff) - 1] = '\0';
      __output_field(FIELD_DWORD, cfg);
   }

   if (cfg->word_field) {
      unsigned char t[2];
      *((unsigned short*) t) = 0x0001;
      field_len = __format_word((unsigned char*) buff, 0, cfg->wbytes, cfg);
      cfg->buff[0] = '\0';
      switch (cfg->word_fmt) {
      case DEC_FMT:
      case OCT_FMT: {
         format = index_hex ? "%x-%-x" : "%d-%-d";
         for (i = 0; i < cfg->wbytes / 2; i++) {
            sprintf(buff, format, (2 * i) % loop, (2 * i + 1) % loop);
            __arrange_str(cfg->buff + strlen(cfg->buff), (field_len + 1) / (cfg->wbytes / 2), buff,
                          strlen(buff), 0/*middle*/);
         }
      }
         break;
      case BIN_FMT: {
         for (i = 0; i < cfg->wbytes / 2; i++) {
            __sprintf_cat(cfg->buff, "7654321%x7654321%x ", (2 * i + t[0]) % 16, (2 * i + t[1]) % 16);
         }
      }
         break;
      case HEX_FMT:
      default: {
         format = index_hex ? "%2x%2x " : "%2d%2d ";
         for (i = 0; i < cfg->wbytes / 2; i++) {
            sprintf(buff, format, (2 * i + t[0]) % loop, (2 * i + t[1]) % loop);
            __arrange_str(cfg->buff + strlen(cfg->buff), (field_len + 1) / (cfg->wbytes / 2), buff,
                          strlen(buff), 1/*right align*/);
         }
      }
         break;
      }
      cfg->buff[strlen(cfg->buff) - 1] = '\0';
      __output_field(FIELD_WORD, cfg);
   }

   if (cfg->byte_field) {
      field_len = __format_byte((unsigned char*) buff, 0, cfg->wbytes, cfg);
      cfg->buff[0] = '\0';
      switch (cfg->byte_fmt) {
      case DEC_FMT:
      case OCT_FMT: {
         format = index_hex ? "%x" : "%d";
         for (i = 0; i < cfg->wbytes; i++) {
            sprintf(buff, format, i);
            __arrange_str(cfg->buff + strlen(cfg->buff), (field_len + 1) / cfg->wbytes, buff, strlen(buff), 0/*middle*/);
         }
      }
         break;
      case BIN_FMT: {
         for (i = 0; i < cfg->wbytes; i++) {
            __sprintf_cat(cfg->buff, "7654321%x ", i % 16);
         }
      }
         break;
      case HEX_FMT:
      default: {
         format = index_hex ? "%2x " : "%2d ";
         for (i = 0; i < cfg->wbytes; i++) {
            sprintf(buff, format, i);
            __arrange_str(cfg->buff + strlen(cfg->buff), (field_len + 1) / cfg->wbytes, buff, strlen(buff), 1/*right align*/);
         }
      }
         break;
      }
      cfg->buff[strlen(cfg->buff) - 1] = '\0';
      __output_field(FIELD_BYTE, cfg);
   }

   if (cfg->char_field) {
      cfg->buff[0] = '\0';
      for (i = 0; i < cfg->wbytes; i++) {
         __sprintf_cat(cfg->buff, "%x", i % 16);
      }
      __output_field(FIELD_CHAR, cfg);
   }

   __output_field(FIELD_END, cfg);
}

static void __user_config(config_struct *cfg, FILE *fp, char *sp, void *ptr, int size, int wide, llong flags)
{
   assert(cfg);
   assert(&fp||&sp||&ptr||&size||&wide||&flags);

   cfg->fp = fp;
   cfg->sp = sp;
   cfg->ptr = ptr;
   cfg->size = size;

   if (wide) {
      cfg->wbytes = wide > M ? M : wide;
   }

   if (flags & JD_NO_TYPE)
      cfg->type_field = 0;
   if (flags & JD_NO_INDEX)
      cfg->index_field = 0;
   if (flags & JD_NO_ADDR)
      cfg->addr_field = 0;
   if (flags & JD_NO_DWORD)
      cfg->dword_field = 0;
   if (flags & JD_NO_WORD)
      cfg->word_field = 0;
   if (flags & JD_NO_BYTE)
      cfg->byte_field = 0;
   if (flags & JD_NO_CHAR)
      cfg->char_field = 0;
   if (flags & JD_HAS_QWORD)
      cfg->qword_field = 1;

   if (flags & JD_ADDR_DEC)
      cfg->addr_fmt = DEC_FMT;
   if (flags & JD_ADDR_INDEX)
      cfg->addr_type = INDEX_ADDRESS;
   if (flags & JD_ADDR_REL)
      cfg->addr_type = RELATED_ADDRESS;
   if (flags & JD_ADDR_UPPER)
      cfg->addr_case = UPPER_CASE;

   if (flags & JD_DATA_NOPAD)
      cfg->data_pad = 0;
   if (flags & JD_DATA_PACK)
      cfg->data_align = DATA_PACK;

   if (flags & JD_QWORD_LOWER)
      cfg->qword_case = LOWER_CASE;
   if (flags & JD_DWORD_LOWER)
      cfg->dword_case = LOWER_CASE;
   if (flags & JD_WORD_LOWER)
      cfg->word_case = LOWER_CASE;
   if (flags & JD_BYTE_LOWER)
      cfg->byte_case = LOWER_CASE;

   if (flags & JD_QWORD_SIGNED)
      cfg->qword_sign = DATA_SIGNED;
   if (flags & JD_DWORD_SIGNED)
      cfg->dword_sign = DATA_SIGNED;
   if (flags & JD_WORD_SIGNED)
      cfg->word_sign = DATA_SIGNED;
   if (flags & JD_BYTE_SIGNED)
      cfg->byte_sign = DATA_SIGNED;

   switch (flags & (JD_QWORD_HEX | JD_QWORD_DEC | JD_QWORD_OCT | JD_QWORD_BIN)) {
   case JD_QWORD_HEX:
      cfg->qword_fmt = HEX_FMT;
      break;
   case JD_QWORD_DEC:
      cfg->qword_fmt = DEC_FMT;
      break;
   case JD_QWORD_OCT:
      cfg->qword_fmt = OCT_FMT;
      break;
   case JD_QWORD_BIN:
      cfg->qword_fmt = BIN_FMT;
      break;
   }
   switch (flags & (JD_DWORD_HEX | JD_DWORD_DEC | JD_DWORD_OCT | JD_DWORD_BIN)) {
   case JD_DWORD_HEX:
      cfg->dword_fmt = HEX_FMT;
      break;
   case JD_DWORD_DEC:
      cfg->dword_fmt = DEC_FMT;
      break;
   case JD_DWORD_OCT:
      cfg->dword_fmt = OCT_FMT;
      break;
   case JD_DWORD_BIN:
      cfg->dword_fmt = BIN_FMT;
      break;
   }
   switch (flags & (JD_WORD_HEX | JD_WORD_DEC | JD_WORD_OCT | JD_WORD_BIN)) {
   case JD_WORD_HEX:
      cfg->word_fmt = HEX_FMT;
      break;
   case JD_WORD_DEC:
      cfg->word_fmt = DEC_FMT;
      break;
   case JD_WORD_OCT:
      cfg->word_fmt = OCT_FMT;
      break;
   case JD_WORD_BIN:
      cfg->word_fmt = BIN_FMT;
      break;
   }
   switch (flags & (JD_BYTE_HEX | JD_BYTE_DEC | JD_BYTE_OCT | JD_BYTE_BIN)) {
   case JD_BYTE_HEX:
      cfg->byte_fmt = HEX_FMT;
      break;
   case JD_BYTE_DEC:
      cfg->byte_fmt = DEC_FMT;
      break;
   case JD_BYTE_OCT:
      cfg->byte_fmt = OCT_FMT;
      break;
   case JD_BYTE_BIN:
      cfg->byte_fmt = BIN_FMT;
      break;
   }

   if (flags & JD_INDEX_LONG)
      cfg->index_mode = LONG_INDEX;
   if (flags & JD_INDEX_DEC)
      cfg->index_fmt = DEC_FMT;

   if (flags & JD_TYPE_UPPER)
      cfg->type_case = UPPER_CASE;
   if (flags & JD_TYPE_SHORT)
      cfg->type_name = TYPE_SHORT;
   if (flags & JD_TYPE_FULL)
      cfg->type_name = TYPE_FULL;

   /*calucate configuration*/
   if (cfg->wbytes % 4) {
      cfg->dword_field = 0;
   }
   if (cfg->wbytes % 2) {
      cfg->word_field = 0;
   }
   if (cfg->wbytes & (cfg->wbytes - 1)) { /*not 2^n*/
      cfg->data_align = DATA_PACK;
   }

}

int jeff_dump(FILE *fp, char *sp, void *ptr, int size, int wide, llong flags)
{
   config_struct cfg;
   assert(&fp || &sp);
   assert(&ptr);
   assert(size >= 0);
   assert(wide >= 0);
   assert(&flags);

   if (!ptr || !size)
      return 0;
   if (!fp && !sp)
      return 0;
   __default_config(&cfg);
   __user_config(&cfg, fp, sp, ptr, size, wide, flags);

   //__output_raw("\n", &cfg);
   __output_type(&cfg);
   __output_index(&cfg);

   while (size) {
      ptr = (void*) __split_line((unsigned char*) ptr, &size, &cfg);
      cfg.index++;
   }
   return cfg.len;
}

#ifdef __cplusplus
}
#endif
/*end of jeffz*/

