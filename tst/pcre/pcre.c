/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/*
This is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language. See
the file Tech.Notes for some information on the internals.

Written by: Philip Hazel <ph10@cam.ac.uk>

           Copyright (c) 1997 University of Cambridge

-----------------------------------------------------------------------------
Permission is granted to anyone to use this software for any purpose on any
computer system, and to redistribute it freely, subject to the following
restrictions:

1. This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

2. The origin of this software must not be misrepresented, either by
   explicit claim or by omission.

3. Altered versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
-----------------------------------------------------------------------------
*/


/* Define DEBUG to get debugging output on stdout. */

/* #define DEBUG */


#define PCRE_VERSION       "1.00 18-Nov-1997"


/* This is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language. See
the file Tech.Notes for some information on the internals.

Written by: Philip Hazel <ph10@cam.ac.uk>

           Copyright (c) 1997 University of Cambridge

-----------------------------------------------------------------------------
Permission is granted to anyone to use this software for any purpose on any
computer system, and to redistribute it freely, subject to the following
restrictions:

1. This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

2. The origin of this software must not be misrepresented, either by
   explicit claim or by omission.

3. Altered versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
-----------------------------------------------------------------------------
*/

/* This header contains definitions that are shared between the different
modules, but which are not relevant to the outside. */

/* To cope with SunOS4 and other systems that lack memmove() but have bcopy(),
define a macro for memmove() if USE_BCOPY is defined. */

#ifdef USE_BCOPY
#define memmove(a, b, c) bcopy(b, a, c)
#endif

/* Standard C headers plus the external interface definition */

#include <ctype.h>
#include <limits.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcre.h"

/* Private options flags start at the most significant end of the two bytes.
The public options defined in pcre.h start at the least significant end. Make
sure they don't overlap! */

#define PCRE_FIRSTSET           0x8000  /* first_char is set */
#define PCRE_STARTLINE          0x4000  /* start after \n for multiline */
#define PCRE_COMPILED_CASELESS  0x2000  /* like it says */

/* Options for the "extra" block produced by pcre_study(). */

#define PCRE_STUDY_CASELESS 0x01     /* study was caseless */
#define PCRE_STUDY_MAPPED   0x02     /* a map of starting chars exists */

/* Masks for identifying the public options: all permitted at compile time,
only some permitted at run or study time. */

#define PUBLIC_OPTIONS \
  (PCRE_CASELESS|PCRE_EXTENDED|PCRE_ANCHORED|PCRE_MULTILINE| \
   PCRE_DOTALL|PCRE_DOLLAR_ENDONLY|PCRE_EXTRA)

#define PUBLIC_EXEC_OPTIONS \
  (PCRE_CASELESS|PCRE_ANCHORED|PCRE_MULTILINE|PCRE_NOTBOL|PCRE_NOTEOL| \
   PCRE_DOTALL|PCRE_DOLLAR_ENDONLY)

#define PUBLIC_STUDY_OPTIONS (PCRE_CASELESS)

/* Magic number to provide a small check against being handed junk. */

#define MAGIC_NUMBER  0x50435245   /* 'PCRE' */

/* Miscellaneous definitions */

typedef int BOOL;

#define FALSE   0
#define TRUE    1

/* These are escaped items that aren't just an encoding of a particular data
value such as \n. They must have non-zero values, as check_escape() returns
their negation. Also, they must appear in the same order as in the opcode
definitions below, up to ESC_Z. The final one must be ESC_REF as subsequent
values are used for \1, \2, \3, etc. There is a test in the code for an escape
greater than ESC_b and less than ESC_X to detect the types that may be
repeated. If any new escapes are put in-between that don't consume a character,
that code will have to change. */

enum { ESC_A = 1, ESC_B, ESC_b, ESC_D, ESC_d, ESC_S, ESC_s, ESC_W, ESC_w,

                    /* These are not Perl escapes, so can't appear in the */
       ESC_X,       /* simple table-lookup because they must be conditional */
                    /* on PCRE_EXTRA. */
       ESC_Z,
       ESC_REF };

/* Opcode table: OP_BRA must be last, as all values >= it are used for brackets
that extract substrings. Starting from 1 (i.e. after OP_END), the values up to
OP_EOL must correspond in order to the list of escapes immediately above. */

enum {
  OP_END,            /* End of pattern */

  /* Values corresponding to backslashed metacharacters */

  OP_SOD,            /* Start of data: \A */
  OP_NOT_WORD_BOUNDARY,  /* \B */
  OP_WORD_BOUNDARY,      /* \b */
  OP_NOT_DIGIT,          /* \D */
  OP_DIGIT,              /* \d */
  OP_NOT_WHITESPACE,     /* \S */
  OP_WHITESPACE,         /* \s */
  OP_NOT_WORDCHAR,       /* \W */
  OP_WORDCHAR,           /* \w */
  OP_CUT,            /* The analogue of Prolog's "cut" operation (extension) */
  OP_EOD,            /* End of data: or \Z. This must always be the last
                        of the backslashed meta values. */

  OP_CIRC,           /* Start of line - varies with multiline switch */
  OP_DOLL,           /* End of line - varies with multiline switch */
  OP_ANY,            /* Match any character */
  OP_CHARS,          /* Match string of characters */
  OP_NOT,            /* Match anything but the following char */

  OP_STAR,           /* The maximizing and minimizing versions of */
  OP_MINSTAR,        /* all these opcodes must come in pairs, with */
  OP_PLUS,           /* the minimizing one second. */
  OP_MINPLUS,        /* This first set applies to single characters */
  OP_QUERY,
  OP_MINQUERY,
  OP_UPTO,           /* From 0 to n matches */
  OP_MINUPTO,
  OP_EXACT,          /* Exactly n matches */

  OP_NOTSTAR,        /* The maximizing and minimizing versions of */
  OP_NOTMINSTAR,     /* all these opcodes must come in pairs, with */
  OP_NOTPLUS,        /* the minimizing one second. */
  OP_NOTMINPLUS,     /* This first set applies to "not" single characters */
  OP_NOTQUERY,
  OP_NOTMINQUERY,
  OP_NOTUPTO,        /* From 0 to n matches */
  OP_NOTMINUPTO,
  OP_NOTEXACT,       /* Exactly n matches */

  OP_TYPESTAR,       /* The maximizing and minimizing versions of */
  OP_TYPEMINSTAR,    /* all these opcodes must come in pairs, with */
  OP_TYPEPLUS,       /* the minimizing one second. These codes must */
  OP_TYPEMINPLUS,    /* be in exactly the same order as those above. */
  OP_TYPEQUERY,      /* This set applies to character types such as \d */
  OP_TYPEMINQUERY,
  OP_TYPEUPTO,       /* From 0 to n matches */
  OP_TYPEMINUPTO,
  OP_TYPEEXACT,      /* Exactly n matches */

  OP_CRSTAR,         /* The maximizing and minimizing versions of */
  OP_CRMINSTAR,      /* all these opcodes must come in pairs, with */
  OP_CRPLUS,         /* the minimizing one second. These codes must */
  OP_CRMINPLUS,      /* be in exactly the same order as those above. */
  OP_CRQUERY,        /* These are for character classes and back refs */
  OP_CRMINQUERY,
  OP_CRRANGE,        /* These are different to the three seta above. */
  OP_CRMINRANGE,

  OP_CLASS,          /* Match a character class */
  OP_REF,            /* Match a back reference */

  OP_ALT,            /* Start of alternation */
  OP_KET,            /* End of group that doesn't have an unbounded repeat */
  OP_KETRMAX,        /* These two must remain together and in this */
  OP_KETRMIN,        /* order. They are for groups the repeat for ever. */

  OP_ASSERT,
  OP_ASSERT_NOT,
  OP_ONCE,           /* Once matched, don't back up into the subpattern */

  OP_BRAZERO,        /* These two must remain together and in this */
  OP_BRAMINZERO,     /* order. */

  OP_BRA             /* This and greater values are used for brackets that
                        extract substrings. */
};

/* The highest extraction number. This is limited by the number of opcodes
left after OP_BRA, i.e. 255 - OP_BRA. We actually set it somewhat lower. */

#define EXTRACT_MAX  99

/* The texts of compile-time error messages are defined as macros here so that
they can be accessed by the POSIX wrapper and converted into error codes.  Yes,
I could have used error codes in the first place, but didn't feel like changing
just to accommodate the POSIX wrapper. */

#define ERR1  "\\ at end of pattern"
#define ERR2  "\\c at end of pattern"
#define ERR3  "unrecognized character follows \\"
#define ERR4  "numbers out of order in {} quantifier"
#define ERR5  "number too big in {} quantifier"
#define ERR6  "missing terminating ] for character class"
#define ERR7  "invalid escape sequence in character class"
#define ERR8  "range out of order in character class"
#define ERR9  "nothing to repeat"
#define ERR10 "operand of unlimited repeat could match the empty string"
#define ERR11 "internal error: unexpected repeat"
#define ERR12 "unrecognized character after (?"
#define ERR13 "too many capturing parenthesized sub-patterns"
#define ERR14 "missing )"
#define ERR15 "back reference to non-existent subpattern"
#define ERR16 "erroffset passed as NULL"
#define ERR17 "unknown option bit(s) set"
#define ERR18 "missing ) after comment"
#define ERR19 "too many sets of parentheses"
#define ERR20 "regular expression too large"
#define ERR21 "failed to get memory"
#define ERR22 "unmatched brackets"
#define ERR23 "internal error: code overflow"

/* All character handling must be done as unsigned characters. Otherwise there
are problems with top-bit-set characters and functions such as isspace().
However, we leave the interface to the outside world as char *, because that
should make things easier for callers. We define a short type for unsigned char
to save lots of typing. I tried "uchar", but it causes problems on Digital
Unix, where it is defined in sys/types, so use "uschar" instead. */

typedef unsigned char uschar;

/* The real format of the start of the pcre block; the actual code vector
runs on as long as necessary after the end. */

typedef struct real_pcre {
  unsigned int  magic_number;
  unsigned short int options;
  unsigned char top_bracket;
  unsigned char top_backref;
  unsigned char first_char;
  unsigned char code[1];
} real_pcre;

/* The real format of the extra block returned by pcre_study(). */

typedef struct real_pcre_extra {
  unsigned char options;
  unsigned char start_bits[32];
} real_pcre_extra;

/* Global tables from chartables.c */

extern uschar pcre_lcc[];
extern uschar pcre_fcc[];
extern uschar pcre_cbits[];
extern uschar pcre_ctypes[];

/* Bit definitions for entries in pcre_ctypes[]. */

#define ctype_space   0x01
#define ctype_letter  0x02
#define ctype_digit   0x04
#define ctype_xdigit  0x08
#define ctype_word    0x10   /* alphameric or '_' */
#define ctype_meta    0x80   /* regexp meta char or zero (end pattern) */

/* Offsets for the bitmap tables */

#define cbit_digit    0
#define cbit_letter  32
#define cbit_word    64
#define cbit_space   96
#define cbit_length 128      /* Length of the cbits table */


/* This table is a lower casing table. */

unsigned char pcre_lcc[] = {
    0,  1,  2,  3,  4,  5,  6,  7,
    8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23,
   24, 25, 26, 27, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39,
   40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55,
   56, 57, 58, 59, 60, 61, 62, 63,
   64, 97, 98, 99,100,101,102,103,
  104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,
  120,121,122, 91, 92, 93, 94, 95,
   96, 97, 98, 99,100,101,102,103,
  104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,
  120,121,122,123,124,125,126,127,
  128,129,130,131,132,133,134,135,
  136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,
  152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,
  168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,
  184,185,186,187,188,189,190,191,
  192,193,194,195,196,197,198,199,
  200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,
  216,217,218,219,220,221,222,223,
  224,225,226,227,228,229,230,231,
  232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,
  248,249,250,251,252,253,254,255 };

/* This table is a case flipping table. */

unsigned char pcre_fcc[] = {
    0,  1,  2,  3,  4,  5,  6,  7,
    8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23,
   24, 25, 26, 27, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39,
   40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55,
   56, 57, 58, 59, 60, 61, 62, 63,
   64, 97, 98, 99,100,101,102,103,
  104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,
  120,121,122, 91, 92, 93, 94, 95,
   96, 65, 66, 67, 68, 69, 70, 71,
   72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87,
   88, 89, 90,123,124,125,126,127,
  128,129,130,131,132,133,134,135,
  136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,
  152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,
  168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,
  184,185,186,187,188,189,190,191,
  192,193,194,195,196,197,198,199,
  200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,
  216,217,218,219,220,221,222,223,
  224,225,226,227,228,229,230,231,
  232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,
  248,249,250,251,252,253,254,255 };

/* This table contains bit maps for digits, letters, 'word' chars, and
white space. Each map is 32 bytes long and the bits run from the least
significant end of each byte. */

unsigned char pcre_cbits[] = {
  0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x03,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xfe,0xff,0xff,0x07,0xfe,0xff,0xff,0x07,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x03,
  0xfe,0xff,0xff,0x87,0xfe,0xff,0xff,0x07,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x3e,0x00,0x00,0x01,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

/* This table identifies various classes of character by individual bits:
  0x01   white space character
  0x02   letter
  0x04   decimal digit
  0x08   hexadecimal digit
  0x10   alphanumeric or '_'
  0x80   regular expression metacharacter or binary zero
*/

unsigned char pcre_ctypes[] = {
  0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*   0-  7 */
  0x00,0x01,0x01,0x01,0x01,0x01,0x00,0x00, /*   8- 15 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  16- 23 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  24- 31 */
  0x01,0x00,0x00,0x00,0x80,0x00,0x00,0x00, /*    - '  */
  0x80,0x80,0x80,0x80,0x00,0x00,0x80,0x00, /*  ( - /  */
  0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c, /*  0 - 7  */
  0x1c,0x1c,0x00,0x00,0x00,0x00,0x00,0x80, /*  8 - ?  */
  0x00,0x1a,0x1a,0x1a,0x1a,0x1a,0x1a,0x12, /*  @ - G  */
  0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12, /*  H - O  */
  0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12, /*  P - W  */
  0x12,0x12,0x12,0x80,0x00,0x00,0x80,0x10, /*  X - _  */
  0x00,0x1a,0x1a,0x1a,0x1a,0x1a,0x1a,0x12, /*  ` - g  */
  0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12, /*  h - o  */
  0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12, /*  p - w  */
  0x12,0x12,0x12,0x80,0x80,0x00,0x00,0x00, /*  x -127 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 128-135 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 136-143 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 144-151 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 152-159 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 160-167 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 168-175 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 176-183 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 184-191 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 192-199 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 200-207 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 208-215 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 216-223 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 224-231 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 232-239 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 240-247 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};/* 248-255 */

/* End of chartables.c */

/* Min and max values for the common repeats; for the maxima, 0 => infinity */

static char rep_min[] = { 0, 0, 1, 1, 0, 0 };
static char rep_max[] = { 0, 0, 0, 0, 1, 1 };

/* Text forms of OP_ values and things, for debugging */

#ifdef DEBUG
static char *OP_names[] = { "End", "\\A", "\\B", "\\b", "\\D", "\\d",
  "\\S", "\\s", "\\W", "\\w", "Cut", "\\Z", "^", "$", "Any", "chars",
  "not",
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",
  "*", "*?", "+", "+?", "?", "??", "{", "{",
  "class", "Ref",
  "Alt", "Ket", "KetRmax", "KetRmin", "Assert", "Assert not", "Once",
  "Brazero", "Braminzero", "Bra"
};
#endif

/* Table for handling escaped characters in the range '0'-'z'. Positive returns
are simple data values; negative values are for special things like \d and so
on. Zero means further processing is needed (for things like \x), or the escape
is invalid. */

static short int escapes[] = {
    0,      0,      0,      0,      0,      0,      0,      0,   /* 0 - 7 */
    0,      0,    ':',    ';',    '<',    '=',    '>',    '?',   /* 8 - ? */
  '@', -ESC_A, -ESC_B,      0, -ESC_D,      0,      0,      0,   /* @ - G */
    0,      0,      0,      0,      0,      0,      0,      0,   /* H - O */
    0,      0,      0, -ESC_S,      0,      0,      0, -ESC_W,   /* P - W */
    0,      0, -ESC_Z,    '[',   '\\',    ']',    '^',    '_',   /* X - _ */
  '`',      7, -ESC_b,      0, -ESC_d,     27,   '\f',      0,   /* ` - g */
    0,      0,      0,      0,      0,      0,   '\n',      0,   /* h - o */
    0,      0,   '\r', -ESC_s,   '\t',      0,      0, -ESC_w,   /* p - w */
    0,      0,      0                                            /* x - z */
};

/* Definition to allow mutual recursion */

static BOOL compile_regex(int, int *,uschar **,uschar **,char **);

/* Structure for passing "static" information around between the functions
doing the matching, so that they are thread-safe. */

typedef struct match_data {
  int    errorcode;             /* As it says */
  int   *offset_vector;         /* Offset vector */
  int    offset_end;            /* One past the end */
  BOOL   offset_overflow;       /* Set if too many extractions */
  BOOL   caseless;              /* Case-independent flag */
  BOOL   runtime_caseless;      /* Caseless forced at run time */
  BOOL   multiline;             /* Multiline flag */
  BOOL   notbol;                /* NOTBOL flag */
  BOOL   noteol;                /* NOTEOL flag */
  BOOL   dotall;                /* Dot matches any char */
  BOOL   endonly;               /* Dollar not before final \n */
  uschar *start_subject;        /* Start of the subject string */
  uschar *end_subject;          /* End of the subject string */
  jmp_buf fail_env;             /* Environment for longjump() break out */
  uschar *end_match_ptr;        /* Subject position at end match */
  int     end_offset_top;       /* Highwater mark at end of match */
} match_data;



/*************************************************
*               Global variables                 *
*************************************************/

/* PCRE is thread-clean and doesn't use any global variables in the normal
sense. However, it calls memory allocation and free functions via the two
indirections below, which are can be changed by the caller, but are shared
between all threads. */

static unsigned char reserved_buffer[1024 * 8];
static unsigned long reserved_bufpos;
void *pcre_malloc(size_t size)
{
   void * p = (void*)&reserved_buffer[reserved_bufpos];
   reserved_bufpos += size;
   return p;
}
void pcre_free(void *ptr)
{
}


/*************************************************
*          Return version string                 *
*************************************************/

char *
pcre_version(void)
{
return PCRE_VERSION;
}




/*************************************************
*       Return info about a compiled pattern     *
*************************************************/

/* This function picks potentially useful data out of the private
structure.

Arguments:
  external_re   points to compiled code
  optptr        where to pass back the options
  first_char    where to pass back the first character,
                or -1 if multiline and all branches start ^,
                or -2 otherwise

Returns:        number of identifying extraction brackets
                or negative values on error
*/

int
pcre_info(const pcre *external_re, int *optptr, int *first_char)
{
real_pcre *re = (real_pcre *)external_re;
if (re == NULL) return PCRE_ERROR_NULL;
if (re->magic_number != MAGIC_NUMBER) return PCRE_ERROR_BADMAGIC;
if (optptr != NULL) *optptr = (re->options & PUBLIC_OPTIONS);
if (first_char != NULL)
  *first_char = ((re->options & PCRE_FIRSTSET) != 0)? re->first_char :
     ((re->options & PCRE_STARTLINE) != 0)? -1 : -2;
return re->top_bracket;
}




#ifdef DEBUG
/*************************************************
*        Debugging function to print chars       *
*************************************************/

/* Print a sequence of chars in printable format, stopping at the end of the
subject if the requested.

Arguments:
  p           points to characters
  length      number to print
  is_subject  TRUE if printing from within md->start_subject
  md          pointer to matching data block, if is_subject is TRUE

Returns:     nothing
*/

static pchars(uschar *p, int length, BOOL is_subject, match_data *md)
{
int c;
if (is_subject && length > md->end_subject - p) length = md->end_subject - p;
while (length-- > 0)
  if (isprint(c = *(p++))) printf("%c", c); else printf("\\x%02x", c);
}
#endif




/*************************************************
*         Check subpattern for empty operand     *
*************************************************/

/* This function checks a bracketed subpattern to see if any of the paths
through it could match an empty string. This is used to diagnose an error if
such a subpattern is followed by a quantifier with an unlimited upper bound.

Argument:
  code      points to the opening bracket

Returns:    TRUE or FALSE
*/

static BOOL
could_be_empty(uschar *code)
{
do {
  uschar *cc = code + 3;

  /* Scan along the opcodes for this branch; as soon as we find something
  that matches a non-empty string, break out and advance to test the next
  branch. If we get to the end of the branch, return TRUE for the whole
  sub-expression. */

  for (;;)
    {
    /* Test an embedded subpattern; if it could not be empty, break the
    loop. Otherwise carry on in the branch. */

    if ((int)(*cc) >= OP_BRA)
      {
      if (!could_be_empty(cc)) break;
      do cc += (cc[1] << 8) + cc[2]; while (*cc == OP_ALT);
      cc += 3;
      }

    else switch (*cc)
      {
      /* Reached end of a branch: the subpattern may match the empty string */

      case OP_ALT:
      case OP_KET:
      case OP_KETRMAX:
      case OP_KETRMIN:
      return TRUE;

      /* Skip over assertive subpatterns */

      case OP_ASSERT:
      case OP_ASSERT_NOT:
      do cc += (cc[1] << 8) + cc[2]; while (*cc == OP_ALT);
      cc += 3;
      break;

      /* Skip over things that don't match chars */

      case OP_SOD:
      case OP_EOD:
      case OP_CIRC:
      case OP_DOLL:
      case OP_BRAZERO:
      case OP_BRAMINZERO:
      case OP_NOT_WORD_BOUNDARY:
      case OP_WORD_BOUNDARY:
      cc++;
      break;

      /* Skip over simple repeats with zero lower bound */

      case OP_STAR:
      case OP_MINSTAR:
      case OP_QUERY:
      case OP_MINQUERY:
      case OP_TYPESTAR:
      case OP_TYPEMINSTAR:
      case OP_TYPEQUERY:
      case OP_TYPEMINQUERY:
      cc += 2;
      break;

      /* Skip over UPTOs (lower bound is zero) */

      case OP_UPTO:
      case OP_MINUPTO:
      case OP_TYPEUPTO:
      case OP_TYPEMINUPTO:
      cc += 4;
      break;

      /* Check a class or a back reference for a zero minimum */

      case OP_CLASS:
      case OP_REF:
      cc += (*cc == OP_REF)? 2 : 4 + 2 * cc[2] + cc[3];

      switch (*cc)
        {
        case OP_CRSTAR:
        case OP_CRMINSTAR:
        case OP_CRQUERY:
        case OP_CRMINQUERY:
        cc++;
        break;

        case OP_CRRANGE:
        case OP_CRMINRANGE:
        if ((cc[1] << 8) + cc[2] != 0) goto NEXT_BRANCH;
        cc += 3;
        break;

        default:
        goto NEXT_BRANCH;
        }
      break;

      /* Anything else matches at least one character */

      default:
      goto NEXT_BRANCH;
      }
    }

  NEXT_BRANCH:
  code += (code[1] << 8) + code[2];
  }
while (*code == OP_ALT);

/* No branches match the empty string */

return FALSE;
}



/*************************************************
*            Handle escapes                      *
*************************************************/

/* This function is called when a \ has been encountered. It either returns a
positive value for a simple escape such as \n, or a negative value which
encodes one of the more complicated things such as \d. On entry, ptr is
pointing at the \. On exit, it is on the final character of the escape
sequence.

Arguments:
  ptrptr     points to the pattern position pointer
  errorptr   points to the pointer to the error message
  bracount   number of previous extracting brackets
  options    the options bits
  isclass    TRUE if inside a character class

Returns:     zero or positive => a data character
             negative => a special escape sequence
             on error, errorptr is set
*/

static int
check_escape(uschar **ptrptr, char **errorptr, int bracount, int options,
  BOOL isclass)
{
uschar *ptr = *ptrptr;
int c = *(++ptr) & 255;   /* Ensure > 0 on signed-char systems */
int i;

if (c == 0) *errorptr = ERR1;

/* Digits or letters may have special meaning; all others are literals. */

else if (c < '0' || c > 'z') {}

/* Do an initial lookup in a table. A non-zero result is something that can be
returned immediately. Otherwise further processing may be required. */

else if ((i = escapes[c - '0']) != 0) c = i;

/* Escapes that need further processing, or are illegal. */

else
  {
  uschar *oldptr;
  switch (c)
    {
    /* The handling of escape sequences consisting of a string of digits
    starting with one that is not zero is not straightforward. By experiment,
    the way Perl works seems to be as follows:

    Outside a character class, the digits are read as a decimal number. If the
    number is less than 10, or if there are that many previous extracting
    left brackets, then it is a back reference. Otherwise, up to three octal
    digits are read to form an escaped byte. Thus \123 is likely to be octal
    123 (cf \0123, which is octal 012 followed by the literal 3). If the octal
    value is greater than 377, the least significant 8 bits are taken. Inside a
    character class, \ followed by a digit is always an octal number. */

    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':

    if (!isclass)
      {
      oldptr = ptr;
      c -= '0';
      while ((pcre_ctypes[ptr[1]] & ctype_digit) != 0)
        c = c * 10 + *(++ptr) - '0';
      if (c < 10 || c <= bracount)
        {
        c = -(ESC_REF + c);
        break;
        }
      ptr = oldptr;      /* Put the pointer back and fall through */
      }

    /* Handle an octal number following \. If the first digit is 8 or 9, Perl
    generates a binary zero byte and treats the digit as a following literal.
    Thus we have to pull back the pointer by one. */

    if ((c = *ptr) >= '8')
      {
      ptr--;
      c = 0;
      break;
      }

    /* \0 always starts an octal number, but we may drop through to here with a
    larger first octal digit */

    case '0':
    c -= '0';
    while(i++ < 2 && (pcre_ctypes[ptr[1]] & ctype_digit) != 0 &&
      ptr[1] != '8' && ptr[1] != '9')
        c = c * 8 + *(++ptr) - '0';
    break;

    /* Special escapes not starting with a digit are straightforward */

    case 'x':
    c = 0;
    while (i++ < 2 && (pcre_ctypes[ptr[1]] & ctype_xdigit) != 0)
      {
      ptr++;
      c = c * 16 + pcre_lcc[*ptr] -
        (((pcre_ctypes[*ptr] & ctype_digit) != 0)? '0' : 'W');
      }
    break;

    case 'c':
    c = *(++ptr);
    if (c == 0)
      {
      *errorptr = ERR2;
      return 0;
      }

    /* A letter is upper-cased; then the 0x40 bit is flipped */

    if (c >= 'a' && c <= 'z') c = pcre_fcc[c];
    c ^= 0x40;
    break;

    /* PCRE_EXTRA enables extensions to Perl in the matter of escapes. Any
    other alphameric following \ is an error if PCRE_EXTRA was set; otherwise,
    for Perl compatibility, it is a literal. */

    default:
    if ((options & PCRE_EXTRA) != 0) switch(c)
      {
      case 'X':
      c = -ESC_X;      /* This could be a lookup if it ever got into Perl */
      break;

      default:
      *errorptr = ERR3;
      break;
      }
    break;
    }
  }

*ptrptr = ptr;
return c;
}



/*************************************************
*            Check for counted repeat            *
*************************************************/

/* This function is called when a '{' is encountered in a place where it might
start a quantifier. It looks ahead to see if it really is a quantifier or not.
It is only a quantifier if it is one of the forms {ddd} {ddd,} or {ddd,ddd}
where the ddds are digits.

Arguments:
  p         pointer to the first char after '{'

Returns:    TRUE or FALSE
*/

static BOOL
is_counted_repeat(uschar *p)
{
if ((pcre_ctypes[*p++] & ctype_digit) == 0) return FALSE;
while ((pcre_ctypes[*p] & ctype_digit) != 0) p++;
if (*p == '}') return TRUE;

if (*p++ != ',') return FALSE;
if (*p == '}') return TRUE;

if ((pcre_ctypes[*p++] & ctype_digit) == 0) return FALSE;
while ((pcre_ctypes[*p] & ctype_digit) != 0) p++;
return (*p == '}');
}



/*************************************************
*         Read repeat counts                     *
*************************************************/

/* Read an item of the form {n,m} and return the values. This is called only
after is_counted_repeat() has confirmed that a repeat-count quantifier exists,
so the syntax is guaranteed to be correct, but we need to check the values.

Arguments:
  p          pointer to first char after '{'
  minp       pointer to int for min
  maxp       pointer to int for max
             returned as -1 if no max
  errorptr   points to pointer to error message

Returns:     pointer to '}' on success;
             current ptr on error, with errorptr set
*/

static uschar *
read_repeat_counts(uschar *p, int *minp, int *maxp, char **errorptr)
{
int min = 0;
int max = -1;

while ((pcre_ctypes[*p] & ctype_digit) != 0) min = min * 10 + *p++ - '0';

if (*p == '}') max = min; else
  {
  if (*(++p) != '}')
    {
    max = 0;
    while((pcre_ctypes[*p] & ctype_digit) != 0) max = max * 10 + *p++ - '0';
    if (max < min)
      {
      *errorptr = ERR4;
      return p;
      }
    }
  }

/* Do paranoid checks, then fill in the required variables, and pass back the
pointer to the terminating '}'. */

if (min > 65535 || max > 65535)
  *errorptr = ERR5;
else
  {
  *minp = min;
  *maxp = max;
  }
return p;
}



/*************************************************
*           Compile one branch                   *
*************************************************/

/* Scan the pattern, compiling it into the code vector.

Arguments:
  options    the option bits
  bracket    points to number of brackets used
  code       points to the pointer to the current code point
  ptrptr     points to the current pattern pointer
  errorptr   points to pointer to error message

Returns:     TRUE on success
             FALSE, with *errorptr set on error
*/

static BOOL
compile_branch(int options, int *brackets, uschar **codeptr, uschar **ptrptr,
  char **errorptr)
{
int repeat_type, op_type;
int repeat_min, repeat_max;
int bravalue, length;
register int c;
register uschar *code = *codeptr;
uschar *ptr = *ptrptr;
uschar *previous = NULL;
uschar *oldptr;
uschar class1[32];

/* Switch on next character until the end of the branch */

for (;; ptr++)
  {
  BOOL negate_class;
  int  class_charcount;
  int  class_lastchar;

  c = *ptr;
  if ((options & PCRE_EXTENDED) != 0)
    {
    if ((pcre_ctypes[c] & ctype_space) != 0) continue;
    if (c == '#')
      {
      while ((c = *(++ptr)) != 0 && c != '\n');
      continue;
      }
    }

  switch(c)
    {
    /* The branch terminates at end of string, |, or ). */

    case 0:
    case '|':
    case ')':
    *codeptr = code;
    *ptrptr = ptr;
    return TRUE;

    /* Handle single-character metacharacters */

    case '^':
    previous = NULL;
    *code++ = OP_CIRC;
    break;

    case '$':
    previous = NULL;
    *code++ = OP_DOLL;
    break;

    case '.':
    previous = code;
    *code++ = OP_ANY;
    break;

    /* Character classes. These always build a 32-byte bitmap of the permitted
    characters, except in the special case where there is only one character.
    For negated classes, we build the map as usual, then invert it at the end.
    */

    case '[':
    previous = code;
    *code++ = OP_CLASS;

    /* If the first character is '^', set the negation flag */

    if ((c = *(++ptr)) == '^')
      {
      negate_class = TRUE;
      c = *(++ptr);
      }
    else negate_class = FALSE;

    /* Keep a count of chars so that we can optimize the case of just a single
    character. */

    class_charcount = 0;
    class_lastchar = -1;

    /* Initialize the 32-char bit map to all zeros. We have to build the
    map in a temporary bit of store, in case the class contains only 1
    character, because in that case the compiled code doesn't use the
    bit map. */

    memset(class1, 0, 32 * sizeof(uschar));

    /* Process characters until ] is reached. By writing this as a "do" it
    means that an initial ] is taken as a data character. */

    do
      {
      if (c == 0)
        {
        *errorptr = ERR6;
        goto FAILED;
        }

      /* Backslash may introduce a single character, or it may introduce one
      of the specials, which just set a flag. Escaped items are checked for
      validity in the pre-compiling pass. The sequence \b is a special case.
      Inside a class (and only there) it is treated as backslash. Elsewhere
      it marks a word boundary. Other escapes have preset maps ready to
      or into the one we are building. We assume they have more than one
      character in them, so set class_count bigger than one. */

      if (c == '\\')
        {
        c = check_escape(&ptr, errorptr, *brackets, options, TRUE);
        if (-c == ESC_b) c = '\b';
        else if (c < 0)
          {
          class_charcount = 10;
          switch (-c)
            {
            case ESC_d:
            for (c = 0; c < 32; c++) class1[c] |= pcre_cbits[c+cbit_digit];
            continue;

            case ESC_D:
            for (c = 0; c < 32; c++) class1[c] |= ~pcre_cbits[c+cbit_digit];
            continue;

            case ESC_w:
            for (c = 0; c < 32; c++)
              class1[c] |= (pcre_cbits[c] | pcre_cbits[c+cbit_word]);
            continue;

            case ESC_W:
            for (c = 0; c < 32; c++)
              class1[c] |= ~(pcre_cbits[c] | pcre_cbits[c+cbit_word]);
            continue;

            case ESC_s:
            for (c = 0; c < 32; c++) class1[c] |= pcre_cbits[c+cbit_space];
            continue;

            case ESC_S:
            for (c = 0; c < 32; c++) class1[c] |= ~pcre_cbits[c+cbit_space];
            continue;

            default:
            *errorptr = ERR7;
            goto FAILED;
            }
          }
        /* Fall through if single character */
        }

      /* A single character may be followed by '-' to form a range. However,
      Perl does not permit ']' to be the end of the range. A '-' character
      here is treated as a literal. */

      if (ptr[1] == '-' && ptr[2] != ']')
        {
        int d;
        ptr += 2;
        d = *ptr;

        if (d == 0)
          {
          *errorptr = ERR6;
          goto FAILED;
          }

        /* The second part of a range can be a single-character escape, but
        not any of the other escapes. */

        if (d == '\\')
          {
          d = check_escape(&ptr, errorptr, *brackets, options, TRUE);
          if (d < 0)
            {
            if (d == -ESC_b) d = '\b'; else
              {
              *errorptr = ERR7;
              goto FAILED;
              }
            }
          }

        if (d < c)
          {
          *errorptr = ERR8;
          goto FAILED;
          }

        for (; c <= d; c++)
          {
          class1[c/8] |= (1 << (c&7));
          if ((options & PCRE_CASELESS) != 0)
            {
            int uc = pcre_fcc[c];           /* flip case */
            class1[uc/8] |= (1 << (uc&7));
            }
          class_charcount++;                /* in case a one-char range */
          class_lastchar = c;
          }
        continue;   /* Go get the next char in the class */
        }

      /* Handle a lone single character - we can get here for a normal
      non-escape char, or after \ that introduces a single character. */

      class1 [c/8] |= (1 << (c&7));
      if ((options & PCRE_CASELESS) != 0)
        {
        c = pcre_fcc[c];   /* flip case */
        class1[c/8] |= (1 << (c&7));
        }
      class_charcount++;
      class_lastchar = c;
      }

    /* Loop until ']' reached; the check for end of string happens inside the
    loop. This "while" is the end of the "do" above. */

    while ((c = *(++ptr)) != ']');

    /* If class_charcount is 1 and class_lastchar is not negative, we saw
    precisely one character. This doesn't need the whole 32-byte bit map.
    We turn it into a 1-character OP_CHAR if it's positive, or OP_NOT if
    it's negative. */

    if (class_charcount == 1 && class_lastchar >= 0)
      {
      if (negate_class)
        {
        code[-1] = OP_NOT;
        }
      else
        {
        code[-1] = OP_CHARS;
        *code++ = 1;
        }
      *code++ = class_lastchar;
      }

    /* Otherwise, negate the 32-byte map if necessary, and copy it into
    the code vector. */

    else
      {
      if (negate_class)
        for (c = 0; c < 32; c++) code[c] = ~class1[c];
      else
        memcpy(code, class1, 32);
      code += 32;
      }
    break;

    /* Various kinds of repeat */

    case '{':
    if (!is_counted_repeat(ptr+1)) goto NORMAL_CHAR;
    ptr = read_repeat_counts(ptr+1, &repeat_min, &repeat_max, errorptr);
    if (*errorptr != NULL) goto FAILED;
    goto REPEAT;

    case '*':
    repeat_min = 0;
    repeat_max = -1;
    goto REPEAT;

    case '+':
    repeat_min = 1;
    repeat_max = -1;
    goto REPEAT;

    case '?':
    repeat_min = 0;
    repeat_max = 1;

    REPEAT:
    if (previous == NULL)
      {
      *errorptr = ERR9;
      goto FAILED;
      }

    /* If the next character is '?' this is a minimizing repeat. Advance to the
    next character. */

    if (ptr[1] == '?') { repeat_type = 1; ptr++; } else repeat_type = 0;

    /* If the maximum is zero then the minimum must also be zero; Perl allows
    this case, so we do too - by simply omitting the item altogether. */

    if (repeat_max == 0) code = previous;

    /* If previous was a string of characters, chop off the last one and use it
    as the subject of the repeat. If there was only one character, we can
    abolish the previous item altogether. */

    else if (*previous == OP_CHARS)
      {
      int len = previous[1];
      if (len == 1)
        {
        c = previous[2];
        code = previous;
        }
      else
        {
        c = previous[len+1];
        previous[1]--;
        code--;
        }
      op_type = 0;                 /* Use single-char op codes */
      goto OUTPUT_SINGLE_REPEAT;   /* Code shared with single character types */
      }

    /* If previous was a single negated character ([^a] or similar), we use
    one of the special opcodes, replacing it. The code is shared with single-
    character repeats by adding a suitable offset into repeat_type. */

    else if ((int)*previous == OP_NOT)
      {
      op_type = OP_NOTSTAR - OP_STAR;  /* Use "not" opcodes */
      c = previous[1];
      code = previous;
      goto OUTPUT_SINGLE_REPEAT;
      }

    /* If previous was a character type match (\d or similar), abolish it and
    create a suitable repeat item. The code is shared with single-character
    repeats by adding a suitable offset into repeat_type. */

    else if ((int)*previous < OP_EOD || *previous == OP_ANY)
      {
      op_type = OP_TYPESTAR - OP_STAR;  /* Use type opcodes */
      c = *previous;
      code = previous;

      OUTPUT_SINGLE_REPEAT:
      repeat_type += op_type;      /* Combine both values for many cases */

      /* A minimum of zero is handled either as the special case * or ?, or as
      an UPTO, with the maximum given. */

      if (repeat_min == 0)
        {
        if (repeat_max == -1) *code++ = OP_STAR + repeat_type;
          else if (repeat_max == 1) *code++ = OP_QUERY + repeat_type;
        else
          {
          *code++ = OP_UPTO + repeat_type;
          *code++ = repeat_max >> 8;
          *code++ = (repeat_max & 255);
          }
        }

      /* The case {1,} is handled as the special case + */

      else if (repeat_min == 1 && repeat_max == -1)
        *code++ = OP_PLUS + repeat_type;

      /* The case {n,n} is just an EXACT, while the general case {n,m} is
      handled as an EXACT followed by an UPTO. An EXACT of 1 is optimized. */

      else
        {
        if (repeat_min != 1)
          {
          *code++ = OP_EXACT + op_type;  /* NB EXACT doesn't have repeat_type */
          *code++ = repeat_min >> 8;
          *code++ = (repeat_min & 255);
          }

        /* If the mininum is 1 and the previous item was a character string,
        we either have to put back the item that got cancelled if the string
        length was 1, or add the character back onto the end of a longer
        string. For a character type nothing need be done; it will just get put
        back naturally. */

        else if (*previous == OP_CHARS)
          {
          if (code == previous) code += 2; else previous[1]++;
          }

        /* Insert an UPTO if the max is greater than the min. */

        if (repeat_max != repeat_min)
          {
          *code++ = c;
          repeat_max -= repeat_min;
          *code++ = OP_UPTO + repeat_type;
          *code++ = repeat_max >> 8;
          *code++ = (repeat_max & 255);
          }
        }

      /* The character or character type itself comes last in all cases. */

      *code++ = c;
      }

    /* If previous was a character class or a back reference, we put the repeat
    stuff after it. */

    else if (*previous == OP_CLASS || *previous == OP_REF)
      {
      if (repeat_min == 0 && repeat_max == -1)
        *code++ = OP_CRSTAR + repeat_type;
      else if (repeat_min == 1 && repeat_max == -1)
        *code++ = OP_CRPLUS + repeat_type;
      else if (repeat_min == 0 && repeat_max == 1)
        *code++ = OP_CRQUERY + repeat_type;
      else
        {
        *code++ = OP_CRRANGE + repeat_type;
        *code++ = repeat_min >> 8;
        *code++ = repeat_min & 255;
        if (repeat_max == -1) repeat_max = 0;  /* 2-byte encoding for max */
        *code++ = repeat_max >> 8;
        *code++ = repeat_max & 255;
        }
      }

    /* If previous was a bracket group, we may have to replicate it in certain
    cases. If the maximum repeat count is unlimited, check that the bracket
    group cannot match the empty string, and diagnose an error if it can. */

    else if ((int)*previous >= OP_BRA)
      {
      int i;
      int length = code - previous;

      if (repeat_max == -1 && could_be_empty(previous))
        {
        *errorptr = ERR10;
        goto FAILED;
        }

      /* If the minimum is greater than zero, and the maximum is unlimited or
      equal to the minimum, the first copy remains where it is, and is
      replicated up to the minimum number of times. This case includes the +
      repeat, but of course no replication is needed in that case. */

      if (repeat_min > 0 && (repeat_max == -1 || repeat_max == repeat_min))
        {
        for (i = 1; i < repeat_min; i++)
          {
          memcpy(code, previous, length);
          code += length;
          }
        }

      /* If the minimum is zero, stick BRAZERO in front of the first copy.
      Then, if there is a fixed upper limit, replicated up to that many times,
      sticking BRAZERO in front of all the optional ones. */

      else
        {
        if (repeat_min == 0)
          {
          memmove(previous+1, previous, length);
          code++;
          *previous++ = OP_BRAZERO + repeat_type;
          }

        for (i = 1; i < repeat_min; i++)
          {
          memcpy(code, previous, length);
          code += length;
          }

        for (i = (repeat_min > 0)? repeat_min : 1; i < repeat_max; i++)
          {
          *code++ = OP_BRAZERO + repeat_type;
          memcpy(code, previous, length);
          code += length;
          }
        }

      /* If the maximum is unlimited, set a repeater in the final copy. */

      if (repeat_max == -1) code[-3] = OP_KETRMAX + repeat_type;
      }

    /* Else there's some kind of shambles */

    else
      {
      *errorptr = ERR11;
      goto FAILED;
      }

    /* In all case we no longer have a previous item. */

    previous = NULL;
    break;


    /* Start of nested bracket sub-expression, or comment or lookahead.
    First deal with special things that can come after a bracket; all are
    introduced by ?, and the appearance of any of them means that this is not a
    referencing group. They were checked for validity in the first pass over
    the string, so we don't have to check for syntax errors here.  */

    case '(':
    previous = code;              /* Only real brackets can be repeated */
    if (*(++ptr) == '?')
      {
      bravalue = OP_BRA;

      switch (*(++ptr))
        {
        case '#':
        case 'i':
        case 'm':
        case 's':
        case 'x':
        ptr++;
        while (*ptr != ')') ptr++;
        previous = NULL;
        continue;

        case ':':                 /* Non-extracting bracket */
        ptr++;
        break;

        case '=':                 /* Assertions can't be repeated */
        bravalue = OP_ASSERT;
        ptr++;
        previous = NULL;
        break;

        case '!':
        bravalue = OP_ASSERT_NOT;
        ptr++;
        previous = NULL;
        break;

        case '>':                         /* "Match once" brackets */
        if ((options & PCRE_EXTRA) != 0)  /* Not yet standard */
          {
          bravalue = OP_ONCE;
          ptr++;
          previous = NULL;
          break;
          }
        /* Else fall through */

        default:
        *errorptr = ERR12;
        goto FAILED;
        }
      }

    /* Else we have a referencing group */

    else
      {
      if (++(*brackets) > EXTRACT_MAX)
        {
        *errorptr = ERR13;
        goto FAILED;
        }
      bravalue = OP_BRA + *brackets;
      }

    /* Process nested bracketed re; at end pointer is on the bracket. We copy
    code into a non-register variable in order to be able to pass its address
    because some compilers complain otherwise. */

    *code = bravalue;
      {
      uschar *mcode = code;
      if (!compile_regex(options, brackets, &mcode, &ptr, errorptr))
        goto FAILED;
      code = mcode;
      }

    if (*ptr != ')')
      {
      *errorptr = ERR14;
      goto FAILED;
      }
    break;

    /* Check \ for being a real metacharacter; if not, fall through and handle
    it as a data character at the start of a string. Escape items are checked
    for validity in the pre-compiling pass. */

    case '\\':
    oldptr = ptr;
    c = check_escape(&ptr, errorptr, *brackets, options, FALSE);

    /* Handle metacharacters introduced by \. For ones like \d, the ESC_ values
    are arranged to be the negation of the corresponding OP_values. For the
    back references, the values are ESC_REF plus the reference number. Only
    back references and those types that consume a character may be repeated.
    We can test for values between ESC_b and ESC_Z for the latter; this may
    have to change if any new ones are ever created. */

    if (c < 0)
      {
      if (-c >= ESC_REF)
        {
        int refnum = -c - ESC_REF;
        if (*brackets < refnum)
          {
          *errorptr = ERR15;
          goto FAILED;
          }
        previous = code;
        *code++ = OP_REF;
        *code++ = refnum;
        }
      else
        {
        previous = (-c > ESC_b && -c < ESC_X)? code : NULL;
        *code++ = -c;
        }
      continue;
      }

    /* Reset and fall through */

    ptr = oldptr;
    c = '\\';

    /* Handle a run of data characters until a metacharacter is encountered.
    The first character is guaranteed not to be whitespace or # when the
    extended flag is set. */

    NORMAL_CHAR:
    default:
    previous = code;
    *code = OP_CHARS;
    code += 2;
    length = 0;

    do
      {
      if ((options & PCRE_EXTENDED) != 0)
        {
        if ((pcre_ctypes[c] & ctype_space) != 0) continue;
        if (c == '#')
          {
          while ((c = *(++ptr)) != 0 && c != '\n');
          if (c == 0) break;
          continue;
          }
        }

      /* Backslash may introduce a data char or a metacharacter. Escaped items
      are checked for validity in the pre-compiling pass. Stop the string
      before a metaitem. */

      if (c == '\\')
        {
        oldptr = ptr;
        c = check_escape(&ptr, errorptr, *brackets, options, FALSE);
        if (c < 0) { ptr = oldptr; break; }
        }

      /* Ordinary character or single-char escape */

      *code++ = c;
      length++;
      }

    /* This "while" is the end of the "do" above. */

    while (length < 255 && (pcre_ctypes[c = *(++ptr)] & ctype_meta) == 0);

    /* Compute the length and set it in the data vector, and advance to
    the next state. */

    previous[1] = length;
    ptr--;
    break;
    }
  }                   /* end of big loop */

/* Control never reaches here by falling through, only by a goto for all the
error states. Pass back the position in the pattern so that it can be displayed
to the user for diagnosing the error. */

FAILED:
*ptrptr = ptr;
return FALSE;
}




/*************************************************
*     Compile sequence of alternatives           *
*************************************************/

/* On entry, ptr is pointing past the bracket character, but on return
it points to the closing bracket, or vertical bar, or end of string.
The code variable is pointing at the byte into which the BRA operator has been
stored.

Argument:
  options   the option bits
  brackets  -> int containing the number of extracting brackets used
  codeptr   -> the address of the current code pointer
  ptrptr    -> the address of the current pattern pointer
  errorptr  -> pointer to error message

Returns:    TRUE on success
*/

static BOOL
compile_regex(int options, int *brackets, uschar **codeptr, uschar **ptrptr,
  char **errorptr)
{
uschar *ptr = *ptrptr;
uschar *code = *codeptr;
uschar *start_bracket = code;

for (;;)
  {
  int length;
  uschar *last_branch = code;

  code += 3;
  if (!compile_branch(options, brackets, &code, &ptr, errorptr))
    {
    *ptrptr = ptr;
    return FALSE;
    }

  /* Fill in the length of the last branch */

  length = code - last_branch;
  last_branch[1] = length >> 8;
  last_branch[2] = length & 255;

  /* Reached end of expression, either ')' or end of pattern. Insert a
  terminating ket and the length of the whole bracketed item, and return,
  leaving the pointer at the terminating char. */

  if (*ptr != '|')
    {
    length = code - start_bracket;
    *code++ = OP_KET;
    *code++ = length >> 8;
    *code++ = length & 255;
    *codeptr = code;
    *ptrptr = ptr;
    return TRUE;
    }

  /* Another branch follows; insert an "or" node and advance the pointer. */

  *code = OP_ALT;
  ptr++;
  }
/* Control never reaches here */
}



/*************************************************
*          Check for anchored expression         *
*************************************************/

/* Try to find out if this is an anchored regular expression. Consider each
alternative branch. If they all start with OP_SOD or OP_CIRC, or with a bracket
all of whose alternatives start with OP_SOD or OP_CIRC (recurse ad lib), then
it's anchored. However, if this is a multiline pattern, then only OP_SOD
counts, since OP_CIRC can match in the middle.

A branch is also implicitly anchored if it starts with .* because that will try
the rest of the pattern at all possible matching points, so there is no point
trying them again.

Argument:  points to start of expression (the bracket)
Returns:   TRUE or FALSE
*/

static BOOL
is_anchored(register uschar *code, BOOL multiline)
{
do {
   int op = (int)code[3];
   if (op >= OP_BRA || op == OP_ASSERT || op == OP_ONCE)
     { if (!is_anchored(code+3, multiline)) return FALSE; }
   else if (op == OP_TYPESTAR || op == OP_TYPEMINSTAR)
     { if (code[4] != OP_ANY) return FALSE; }
   else if (op != OP_SOD && (multiline || op != OP_CIRC)) return FALSE;
   code += (code[1] << 8) + code[2];
   }
while (*code == OP_ALT);
return TRUE;
}



/*************************************************
*     Check for start with \n line expression    *
*************************************************/

/* This is called for multiline expressions to try to find out if every branch
starts with ^ so that "first char" processing can be done to speed things up.

Argument:  points to start of expression (the bracket)
Returns:   TRUE or FALSE
*/

static BOOL
is_startline(uschar *code)
{
do {
   if ((int)code[3] >= OP_BRA || code[3] == OP_ASSERT)
     { if (!is_startline(code+3)) return FALSE; }
   else if (code[3] != OP_CIRC) return FALSE;
   code += (code[1] << 8) + code[2];
   }
while (*code == OP_ALT);
return TRUE;
}



/*************************************************
*          Check for fixed first char            *
*************************************************/

/* Try to find out if there is a fixed first character. This is called for
unanchored expressions, as it speeds up their processing quite considerably.
Consider each alternative branch. If they all start with the same char, or with
a bracket all of whose alternatives start with the same char (recurse ad lib),
then we return that char, otherwise -1.

Argument:  points to start of expression (the bracket)
Returns:   -1 or the fixed first char
*/

static int
find_firstchar(uschar *code)
{
register int c = -1;
do
  {
  register int charoffset = 4;

  if ((int)code[3] >= OP_BRA || code[3] == OP_ASSERT)
    {
    register int d;
    if ((d = find_firstchar(code+3)) < 0) return -1;
    if (c < 0) c = d; else if (c != d) return -1;
    }

  else switch(code[3])
    {
    default:
    return -1;

    case OP_EXACT:       /* Fall through */
    charoffset++;

    case OP_CHARS:       /* Fall through */
    charoffset++;

    case OP_PLUS:
    case OP_MINPLUS:
    if (c < 0) c = code[charoffset]; else if (c != code[charoffset]) return -1;
    break;
    }
  code += (code[1] << 8) + code[2];
  }
while (*code == OP_ALT);
return c;
}



/*************************************************
*        Compile a Regular Expression            *
*************************************************/

/* This function takes a string and returns a pointer to a block of store
holding a compiled version of the expression.

Arguments:
  pattern      the regular expression
  options      various option bits
  errorptr     pointer to pointer to error text
  erroroffset  ptr offset in pattern where error was detected

Returns:       pointer to compiled data block, or NULL on error,
               with errorptr and erroroffset set
*/

pcre *
pcre_compile(const char *pattern, int options, char **errorptr,
  int *erroroffset)
{
real_pcre *re;
int spaces = 0;
int length = 3;      /* For initial BRA plus length */
int runlength;
int c, size;
int bracount = 0;
int brastack[200];
int brastackptr = 0;
int top_backref = 0;
uschar *code, *ptr;

reserved_bufpos = 0;

#ifdef DEBUG
uschar *code_base, *code_end;
#endif

/* We can't pass back an error message if errorptr is NULL; I guess the best we
can do is just return NULL. */

if (errorptr == NULL) return NULL;
*errorptr = NULL;

/* However, we can give a message for this error */

if (erroroffset == NULL)
  {
  *errorptr = ERR16;
  return NULL;
  }
*erroroffset = 0;

if ((options & ~PUBLIC_OPTIONS) != 0)
  {
  *errorptr = ERR17;
  return NULL;
  }

#ifdef DEBUG
printf("------------------------------------------------------------------\n");
printf("%s\n", pattern);
#endif

/* The first thing to do is to make a pass over the pattern to compute the
amount of store required to hold the compiled code. This does not have to be
perfect as long as errors are overestimates. At the same time we can detect any
internal flag settings. Make an attempt to correct for any counted white space
if an "extended" flag setting appears late in the pattern. We can't be so
clever for #-comments. */

ptr = (uschar *)(pattern - 1);
while ((c = *(++ptr)) != 0)
  {
  int min, max;
  int class_charcount;

  if ((pcre_ctypes[c] & ctype_space) != 0)
    {
    if ((options & PCRE_EXTENDED) != 0) continue;
    spaces++;
    }

  if (c == '#' && (options & PCRE_EXTENDED) != 0)
    {
    while ((c = *(++ptr)) != 0 && c != '\n');
    continue;
    }

  switch(c)
    {
    /* A backslashed item may be an escaped "normal" character or a
    character type. For a "normal" character, put the pointers and
    character back so that tests for whitespace etc. in the input
    are done correctly. */

    case '\\':
      {
      uschar *save_ptr = ptr;
      c = check_escape(&ptr, errorptr, bracount, options, FALSE);
      if (*errorptr != NULL) goto PCRE_ERROR_RETURN;
      if (c >= 0)
        {
        ptr = save_ptr;
        c = '\\';
        goto NORMAL_CHAR;
        }
      }
    length++;

    /* A back reference needs an additional char, plus either one or 5
    bytes for a repeat. We also need to keep the value of the highest
    back reference. */

    if (c <= -ESC_REF)
      {
      int refnum = -c - ESC_REF;
      if (refnum > top_backref) top_backref = refnum;
      length++;   /* For single back reference */
      if (ptr[1] == '{' && is_counted_repeat(ptr+2))
        {
        ptr = read_repeat_counts(ptr+2, &min, &max, errorptr);
        if (*errorptr != NULL) goto PCRE_ERROR_RETURN;
        if ((min == 0 && (max == 1 || max == -1)) ||
          (min == 1 && max == -1))
            length++;
        else length += 5;
        if (ptr[1] == '?') ptr++;
        }
      }
    continue;

    case '^':
    case '.':
    case '$':
    case '*':     /* These repeats won't be after brackets; */
    case '+':     /* those are handled separately */
    case '?':
    length++;
    continue;

    /* This covers the cases of repeats after a single char, metachar, class,
    or back reference. */

    case '{':
    if (!is_counted_repeat(ptr+1)) goto NORMAL_CHAR;
    ptr = read_repeat_counts(ptr+1, &min, &max, errorptr);
    if (*errorptr != NULL) goto PCRE_ERROR_RETURN;
    if ((min == 0 && (max == 1 || max == -1)) ||
      (min == 1 && max == -1))
        length++;
    else
      {
      length--;   /* Uncount the original char or metachar */
      if (min == 1) length++; else if (min > 0) length += 4;
      if (max > 0) length += 4; else length += 2;
      }
    if (ptr[1] == '?') ptr++;
    continue;

    /* An alternation contains an offset to the next branch or ket. */
    case '|':
    length += 3;
    continue;

    /* A character class uses 33 characters. Don't worry about character types
    that aren't allowed in classes - they'll get picked up during the compile.
    A character class that contains only one character uses 2 or 3 bytes,
    depending on whether it is negated or not. Notice this where we can. */

    case '[':
    class_charcount = 0;
    if (*(++ptr) == '^') ptr++;
    do
      {
      if (*ptr == '\\')
        {
        int c = check_escape(&ptr, errorptr, bracount, options, TRUE);
        if (*errorptr != NULL) goto PCRE_ERROR_RETURN;
        if (-c == ESC_b) class_charcount++; else class_charcount = 10;
        }
      else class_charcount++;
      ptr++;
      }
    while (*ptr != 0 && *ptr != ']');

    /* Repeats for negated single chars are handled by the general code */

    if (class_charcount == 1) length += 3; else
      {
      length += 33;

      /* A repeat needs either 1 or 5 bytes. */

      if (ptr[1] == '{' && is_counted_repeat(ptr+2))
        {
        ptr = read_repeat_counts(ptr+2, &min, &max, errorptr);
        if (*errorptr != NULL) goto PCRE_ERROR_RETURN;
        if ((min == 0 && (max == 1 || max == -1)) ||
          (min == 1 && max == -1))
            length++;
        else length += 5;
        if (ptr[1] == '?') ptr++;
        }
      }
    continue;

    /* Brackets may be genuine groups or special things */

    case '(':

    /* Handle special forms of bracket, which all start (? */

    if (ptr[1] == '?') switch (c = ptr[2])
      {
      /* Skip over comments entirely */
      case '#':
      ptr += 3;
      while (*ptr != 0 && *ptr != ')') ptr++;
      if (*ptr == 0)
        {
        *errorptr = ERR18;
        goto PCRE_ERROR_RETURN;
        }
      continue;

      /* Non-referencing groups and lookaheads just move the pointer on, and
      then behave like a non-special bracket, except that they don't increment
      the count of extracting brackets. */

      case ':':
      case '=':
      case '!':
      ptr += 2;
      break;

      /* Ditto for the "once only" bracket, allowed only if the extra bit
      is set. */

      case '>':
      if ((options & PCRE_EXTRA) != 0)
        {
        ptr += 2;
        break;
        }
      /* Else fall thourh */

      /* Else loop setting valid options until ) is met. Anything else is an
      error. */

      default:
      ptr += 2;
      for (;; ptr++)
        {
        if ((c = *ptr) == 'i')
          {
          options |= PCRE_CASELESS;
          continue;
          }
        else if ((c = *ptr) == 'm')
          {
          options |= PCRE_MULTILINE;
          continue;
          }
        else if (c == 's')
          {
          options |= PCRE_DOTALL;
          continue;
          }
        else if (c == 'x')
          {
          options |= PCRE_EXTENDED;
          length -= spaces;          /* Already counted spaces */
          continue;
          }
        else if (c == ')') break;

        *errorptr = ERR12;
        goto PCRE_ERROR_RETURN;
        }
      continue;                      /* End of this bracket handling */
      }

    /* Extracting brackets must be counted so we can process escapes in a
    Perlish way. */

    else bracount++;

    /* Non-special forms of bracket. Save length for computing whole length
    at end if there's a repeat that requires duplication of the group. */

    if (brastackptr >= sizeof(brastack)/sizeof(int))
      {
      *errorptr = ERR19;
      goto PCRE_ERROR_RETURN;
      }

    brastack[brastackptr++] = length;
    length += 3;
    continue;

    /* Handle ket. Look for subsequent max/min; for certain sets of values we
    have to replicate this bracket up to that many times. */

    case ')':
    length += 3;
      {
      int min = 1;
      int max = 1;
      int duplength = length - brastack[--brastackptr];

      /* Leave ptr at the final char; for read_repeat_counts this happens
      automatically; for the others we need an increment. */

      if ((c = ptr[1]) == '{' && is_counted_repeat(ptr+2))
        {
        ptr = read_repeat_counts(ptr+2, &min, &max, errorptr);
        if (*errorptr != NULL) goto PCRE_ERROR_RETURN;
        }
      else if (c == '*') { min = 0; max = -1; ptr++; }
      else if (c == '+') { max = -1; ptr++; }
      else if (c == '?') { min = 0; ptr++; }

      /* If there is a minimum > 1 we have to replicate up to min-1 times; if
      there is a limited maximum we have to replicate up to max-1 times and
      allow for a BRAZERO item before each optional copy, as we also have to
      do before the first copy if the minimum is zero. */

      if (min == 0) length++;
        else if (min > 1) length += (min - 1) * duplength;
      if (max > min) length += (max - min) * (duplength + 1);
      }

    continue;

    /* Non-special character. For a run of such characters the length required
    is the number of characters + 2, except that the maximum run length is 255.
    We won't get a skipped space or a non-data escape or the start of a #
    comment as the first character, so the length can't be zero. */

    NORMAL_CHAR:
    default:
    length += 2;
    runlength = 0;
    do
      {
      if ((pcre_ctypes[c] & ctype_space) != 0)
        {
        if ((options & PCRE_EXTENDED) != 0) continue;
        spaces++;
        }

      if (c == '#' && (options & PCRE_EXTENDED) != 0)
        {
        while ((c = *(++ptr)) != 0 && c != '\n');
        continue;
        }

      /* Backslash may introduce a data char or a metacharacter; stop the
      string before the latter. */

      if (c == '\\')
        {
        uschar *saveptr = ptr;
        c = check_escape(&ptr, errorptr, bracount, options, FALSE);
        if (*errorptr != NULL) goto PCRE_ERROR_RETURN;
        if (c < 0) { ptr = saveptr; break; }
        }

      /* Ordinary character or single-char escape */

      runlength++;
      }

    /* This "while" is the end of the "do" above. */

    while (runlength < 255 && (pcre_ctypes[c = *(++ptr)] & ctype_meta) == 0);

    ptr--;
    length += runlength;
    continue;
    }
  }

length += 4;    /* For final KET and END */

if (length > 65539)
  {
  *errorptr = ERR20;
  return NULL;
  }

/* Compute the size of data block needed and get it, either from malloc or
externally provided function. Put in the magic number and the options. */

size = length + offsetof(real_pcre, code);
re = (real_pcre *)(pcre_malloc)(size);

if (re == NULL)
  {
  *errorptr = ERR21;
  return NULL;
  }

re->magic_number = MAGIC_NUMBER;
re->options = options;

/* Set up a starting, non-extracting bracket, then compile the expression. On
error, *errorptr will be set non-NULL, so we don't need to look at the result
of the function here. */

ptr = (uschar *)pattern;
code = re->code;
*code = OP_BRA;
bracount = 0;
(void)compile_regex(options, &bracount, &code, &ptr, errorptr);
re->top_bracket = bracount;
re->top_backref = top_backref;

/* If not reached end of pattern on success, there's an excess bracket. */

if (*errorptr == NULL && *ptr != 0) *errorptr = ERR22;

/* Fill in the terminating state and check for disastrous overflow, but
if debugging, leave the test till after things are printed out. */

*code++ = OP_END;

#ifndef DEBUG
if (code - re->code > length) *errorptr = ERR23;
#endif

/* Failed to compile */

if (*errorptr != NULL)
  {
  (pcre_free)(re);
  PCRE_ERROR_RETURN:
  *erroroffset = ptr - (uschar *)pattern;
  return NULL;
  }

/* If the anchored option was not passed, set flag if we can determine that it
is anchored by virtue of ^ characters or \A or anything else. Otherwise, see if
we can determine what the first character has to be, because that speeds up
unanchored matches no end. In the case of multiline matches, an alternative is
to set the PCRE_STARTLINE flag if all branches start with ^. */

if ((options & PCRE_ANCHORED) == 0)
  {
  if (is_anchored(re->code, (options & PCRE_MULTILINE) != 0))
    re->options |= PCRE_ANCHORED;
  else
    {
    int c = find_firstchar(re->code);
    if (c >= 0)
      {
      re->first_char = c;
      re->options |= PCRE_FIRSTSET;
      }
    else if (is_startline(re->code))
      re->options |= PCRE_STARTLINE;
    }
  }

/* Print out the compiled data for debugging */

#ifdef DEBUG

printf("Length = %d top_bracket = %d top_backref=%d\n",
  length, re->top_bracket, re->top_backref);

if (re->options != 0)
  {
  printf("%s%s%s%s%s%s%s\n",
    ((re->options & PCRE_ANCHORED) != 0)? "anchored " : "",
    ((re->options & PCRE_CASELESS) != 0)? "caseless " : "",
    ((re->options & PCRE_EXTENDED) != 0)? "extended " : "",
    ((re->options & PCRE_MULTILINE) != 0)? "multiline " : "",
    ((re->options & PCRE_DOTALL) != 0)? "dotall " : "",
    ((re->options & PCRE_DOLLAR_ENDONLY) != 0)? "endonly " : "",
    ((re->options & PCRE_EXTRA) != 0)? "extra " : "");
  }

if ((re->options & PCRE_FIRSTSET) != 0)
  {
  if (isprint(re->first_char)) printf("First char = %c\n", re->first_char);
    else printf("First char = \\x%02x\n", re->first_char);
  }

code_end = code;
code_base = code = re->code;

while (code < code_end)
  {
  int charlength;

  printf("%3d ", code - code_base);

  if (*code >= OP_BRA)
    {
    printf("%3d Bra %d", (code[1] << 8) + code[2], *code - OP_BRA);
    code += 2;
    }

  else switch(*code)
    {
    case OP_CHARS:
    charlength = *(++code);
    printf("%3d ", charlength);
    while (charlength-- > 0)
      if (isprint(c = *(++code))) printf("%c", c); else printf("\\x%02x", c);
    break;

    case OP_KETRMAX:
    case OP_KETRMIN:
    case OP_ALT:
    case OP_KET:
    case OP_ASSERT:
    case OP_ASSERT_NOT:
    case OP_ONCE:
    printf("%3d %s", (code[1] << 8) + code[2], OP_names[*code]);
    code += 2;
    break;

    case OP_STAR:
    case OP_MINSTAR:
    case OP_PLUS:
    case OP_MINPLUS:
    case OP_QUERY:
    case OP_MINQUERY:
    case OP_TYPESTAR:
    case OP_TYPEMINSTAR:
    case OP_TYPEPLUS:
    case OP_TYPEMINPLUS:
    case OP_TYPEQUERY:
    case OP_TYPEMINQUERY:
    if (*code >= OP_TYPESTAR)
      printf("    %s", OP_names[code[1]]);
    else if (isprint(c = code[1])) printf("    %c", c);
      else printf("    \\x%02x", c);
    printf("%s", OP_names[*code++]);
    break;

    case OP_EXACT:
    case OP_UPTO:
    case OP_MINUPTO:
    if (isprint(c = code[3])) printf("    %c{", c);
      else printf("    \\x%02x{", c);
    if (*code != OP_EXACT) printf(",");
    printf("%d}", (code[1] << 8) + code[2]);
    if (*code == OP_MINUPTO) printf("?");
    code += 3;
    break;

    case OP_TYPEEXACT:
    case OP_TYPEUPTO:
    case OP_TYPEMINUPTO:
    printf("    %s{", OP_names[code[3]]);
    if (*code != OP_TYPEEXACT) printf(",");
    printf("%d}", (code[1] << 8) + code[2]);
    if (*code == OP_TYPEMINUPTO) printf("?");
    code += 3;
    break;

    case OP_NOT:
    if (isprint(c = *(++code))) printf("    [^%c]", c);
      else printf("    [^\\x%02x]", c);
    break;

    case OP_NOTSTAR:
    case OP_NOTMINSTAR:
    case OP_NOTPLUS:
    case OP_NOTMINPLUS:
    case OP_NOTQUERY:
    case OP_NOTMINQUERY:
    if (isprint(c = code[1])) printf("    [^%c]", c);
      else printf("    [^\\x%02x]", c);
    printf("%s", OP_names[*code++]);
    break;

    case OP_NOTEXACT:
    case OP_NOTUPTO:
    case OP_NOTMINUPTO:
    if (isprint(c = code[3])) printf("    [^%c]{", c);
      else printf("    [^\\x%02x]{", c);
    if (*code != OP_NOTEXACT) printf(",");
    printf("%d}", (code[1] << 8) + code[2]);
    if (*code == OP_NOTMINUPTO) printf("?");
    code += 3;
    break;

    case OP_REF:
    printf("    \\%d", *(++code));
    break;

    case OP_CLASS:
      {
      int i, min, max;

      code++;
      printf("    [");

      for (i = 0; i < 256; i++)
        {
        if ((code[i/8] & (1 << (i&7))) != 0)
          {
          int j;
          for (j = i+1; j < 256; j++)
            if ((code[j/8] & (1 << (j&7))) == 0) break;
          if (i == '-' || i == ']') printf("\\");
          if (isprint(i)) printf("%c", i); else printf("\\x%02x", i);
          if (--j > i)
            {
            printf("-");
            if (j == '-' || j == ']') printf("\\");
            if (isprint(j)) printf("%c", j); else printf("\\x%02x", j);
            }
          i = j;
          }
        }
      printf("]");
      code += 32;

      switch(*code)
        {
        case OP_CRSTAR:
        case OP_CRMINSTAR:
        case OP_CRPLUS:
        case OP_CRMINPLUS:
        case OP_CRQUERY:
        case OP_CRMINQUERY:
        printf("%s", OP_names[*code]);
        break;

        case OP_CRRANGE:
        case OP_CRMINRANGE:
        min = (code[1] << 8) + code[2];
        max = (code[3] << 8) + code[4];
        if (max == 0) printf("{%d,}", min);
        else printf("{%d,%d}", min, max);
        if (*code == OP_CRMINRANGE) printf("?");
        code += 4;
        break;

        default:
        code--;
        }
      }
    break;

    /* Anything else is just a one-node item */

    default:
    printf("    %s", OP_names[*code]);
    break;
    }

  code++;
  printf("\n");
  }
printf("------------------------------------------------------------------\n");

/* This check is done here in the debugging case so that the code that
was compiled can be seen. */

if (code - re->code > length)
  {
  *errorptr = ERR23;
  (pcre_free)(re);
  *erroroffset = ptr - (uschar *)pattern;
  return NULL;
  }
#endif

return (pcre *)re;
}



/*************************************************
*        Match a character type                  *
*************************************************/

/* Not used in all the places it might be as it's sometimes faster
to put the code inline.

Arguments:
  type        the character type
  c           the character
  dotall      the dotall flag

Returns:      TRUE if character is of the type
*/

static BOOL
match_type(int type, int c, BOOL dotall)
{

#ifdef DEBUG
if (isprint(c)) printf("matching subject %c against ", c);
  else printf("matching subject \\x%02x against ", c);
printf("%s\n", OP_names[type]);
#endif

switch(type)
  {
  case OP_ANY:            return dotall || c != '\n';
  case OP_NOT_DIGIT:      return (pcre_ctypes[c] & ctype_digit) == 0;
  case OP_DIGIT:          return (pcre_ctypes[c] & ctype_digit) != 0;
  case OP_NOT_WHITESPACE: return (pcre_ctypes[c] & ctype_space) == 0;
  case OP_WHITESPACE:     return (pcre_ctypes[c] & ctype_space) != 0;
  case OP_NOT_WORDCHAR:   return (pcre_ctypes[c] & ctype_word) == 0;
  case OP_WORDCHAR:       return (pcre_ctypes[c] & ctype_word) != 0;
  }
return FALSE;
}



/*************************************************
*          Match a back-reference                *
*************************************************/

/* If a back reference hasn't been set, the match fails.

Arguments:
  number      reference number
  eptr        points into the subject
  length      length to be matched
  md          points to match data block

Returns:      TRUE if matched
*/

static BOOL
match_ref(int number, register uschar *eptr, int length, match_data *md)
{
uschar *p = md->start_subject + md->offset_vector[number];

#ifdef DEBUG
if (eptr >= md->end_subject)
  printf("matching subject <null>");
else
  {
  printf("matching subject ");
  pchars(eptr, length, TRUE, md);
  }
printf(" against backref ");
pchars(p, length, FALSE, md);
printf("\n");
#endif

/* Always fail if not enough characters left */

if (length > md->end_subject - p) return FALSE;

/* Separate the caselesss case for speed */

if (md->caseless)
  { while (length-- > 0) if (pcre_lcc[*p++] != pcre_lcc[*eptr++]) return FALSE; }
else
  { while (length-- > 0) if (*p++ != *eptr++) return FALSE; }

return TRUE;
}



/*************************************************
*         Match from current position            *
*************************************************/

/* On entry ecode points to the first opcode, and eptr to the first character.

Arguments:
   eptr        pointer in subject
   ecode       position in code
   offset_top  current top pointer
   md          pointer to "static" info for the match

Returns:       TRUE if matched
*/

static BOOL
match(register uschar *eptr, register uschar *ecode, int offset_top,
  match_data *md)
{
for (;;)
  {
  int min, max, ctype;
  register int i;
  register int c;
  BOOL minimize;

  /* Opening bracket. Check the alternative branches in turn, failing if none
  match. We have to set the start offset if required and there is space
  in the offset vector so that it is available for subsequent back references
  if the bracket matches. However, if the bracket fails, we must put back the
  previous value of both offsets in case they were set by a previous copy of
  the same bracket. Don't worry about setting the flag for the error case here;
  that is handled in the code for KET. */

  if ((int)*ecode >= OP_BRA)
    {
    int number = (*ecode - OP_BRA) << 1;
    int save_offset1, save_offset2;

    #ifdef DEBUG
    printf("start bracket %d\n", number/2);
    #endif

    if (number > 0 && number < md->offset_end)
      {
      save_offset1 = md->offset_vector[number];
      save_offset2 = md->offset_vector[number+1];
      md->offset_vector[number] = eptr - md->start_subject;

      #ifdef DEBUG
      printf("saving %d %d\n", save_offset1, save_offset2);
      #endif
      }

    /* Recurse for all the alternatives. */

    do
      {
      if (match(eptr, ecode+3, offset_top, md)) return TRUE;
      ecode += (ecode[1] << 8) + ecode[2];
      }
    while (*ecode == OP_ALT);

    #ifdef DEBUG
    printf("bracket %d failed\n", number/2);
    #endif

    if (number > 0 && number < md->offset_end)
      {
      md->offset_vector[number] = save_offset1;
      md->offset_vector[number+1] = save_offset2;
      }

    return FALSE;
    }

  /* Other types of node can be handled by a switch */

  switch(*ecode)
    {
    case OP_END:
    md->end_match_ptr = eptr;          /* Record where we ended */
    md->end_offset_top = offset_top;   /* and how many extracts were taken */
    return TRUE;

    /* The equivalent of Prolog's "cut" - if the rest doesn't match, the
    whole thing doesn't match, so we have to get out via a longjmp(). */

    case OP_CUT:
    if (match(eptr, ecode+1, offset_top, md)) return TRUE;
    longjmp(md->fail_env, 1);

    /* Assertion brackets. Check the alternative branches in turn - the
    matching won't pass the KET for an assertion. If any one branch matches,
    the assertion is true. */

    case OP_ASSERT:
    do
      {
      if (match(eptr, ecode+3, offset_top, md)) break;
      ecode += (ecode[1] << 8) + ecode[2];
      }
    while (*ecode == OP_ALT);
    if (*ecode == OP_KET) return FALSE;

    /* Continue from after the assertion, updating the offsets high water
    mark, since extracts may have been taken during the assertion. */

    do ecode += (ecode[1] << 8) + ecode[2]; while (*ecode == OP_ALT);
    ecode += 3;
    offset_top = md->end_offset_top;
    continue;

    /* Negative assertion: all branches must fail to match */

    case OP_ASSERT_NOT:
    do
      {
      if (match(eptr, ecode+3, offset_top, md)) return FALSE;
      ecode += (ecode[1] << 8) + ecode[2];
      }
    while (*ecode == OP_ALT);
    ecode += 3;
    continue;

    /* "Once" brackets are like assertion brackets except that after a match,
    the point in the subject string is not moved back. Thus there can never be
    a back into the brackets. Check the alternative branches in turn - the
    matching won't pass the KET for this kind of subpattern. If any one branch
    matches, we carry on, leaving the subject pointer. */

    case OP_ONCE:
    do
      {
      if (match(eptr, ecode+3, offset_top, md)) break;
      ecode += (ecode[1] << 8) + ecode[2];
      }
    while (*ecode == OP_ALT);
    if (*ecode == OP_KET) return FALSE;

    /* Continue as from after the assertion, updating the offsets high water
    mark, since extracts may have been taken. */

    do ecode += (ecode[1] << 8) + ecode[2]; while (*ecode == OP_ALT);
    ecode += 3;
    offset_top = md->end_offset_top;
    eptr = md->end_match_ptr;
    continue;

    /* An alternation is the end of a branch; scan along to find the end of the
    bracketed group and go to there. */

    case OP_ALT:
    do ecode += (ecode[1] << 8) + ecode[2]; while (*ecode == OP_ALT);
    break;

    /* BRAZERO and BRAMINZERO occur just before a bracket group, indicating
    that it may occur zero times. It may repeat infinitely, or not at all -
    i.e. it could be ()* or ()? in the pattern. Brackets with fixed upper
    repeat limits are compiled as a number of copies, with the optional ones
    preceded by BRAZERO or BRAMINZERO. */

    case OP_BRAZERO:
      {
      uschar *next = ecode+1;
      if (match(eptr, next, offset_top, md)) return TRUE;
      do next += (next[1] << 8) + next[2]; while (*next == OP_ALT);
      ecode = next + 3;
      }
    break;

    case OP_BRAMINZERO:
      {
      uschar *next = ecode+1;
      do next += (next[1] << 8) + next[2]; while (*next == OP_ALT);
      if (match(eptr, next+3, offset_top, md)) return TRUE;
      ecode++;
      }
    break;;

    /* End of a group, repeated or non-repeating. If we are at the end of
    an assertion "group", stop matching and return TRUE, but record the
    current high water mark for use by positive assertions. */

    case OP_KET:
    case OP_KETRMIN:
    case OP_KETRMAX:
      {
      int number;
      uschar *prev = ecode - (ecode[1] << 8) - ecode[2];

      if (*prev == OP_ASSERT || *prev == OP_ASSERT_NOT || *prev == OP_ONCE)
        {
        md->end_match_ptr = eptr;      /* For ONCE */
        md->end_offset_top = offset_top;
        return TRUE;
        }

      /* In all other cases we have to check the group number back at the
      start and if necessary complete handling an extraction by setting the
      final offset and bumping the high water mark. */

      number = (*prev - OP_BRA) << 1;

      #ifdef DEBUG
      printf("end bracket %d\n", number/2);
      #endif

      if (number > 0)
        {
        if (number >= md->offset_end) md->offset_overflow = TRUE; else
          {
          md->offset_vector[number+1] = eptr - md->start_subject;
          if (offset_top <= number) offset_top = number + 2;
          }
        }

      /* For a non-repeating ket, just advance to the next node and continue at
      this level. */

      if (*ecode == OP_KET)
        {
        ecode += 3;
        break;
        }

      /* The repeating kets try the rest of the pattern or restart from the
      preceding bracket, in the appropriate order. */

      if (*ecode == OP_KETRMIN)
        {
        if (match(eptr, ecode+3, offset_top, md) ||
            match(eptr, prev, offset_top, md)) return TRUE;
        }
      else  /* OP_KETRMAX */
        {
        if (match(eptr, prev, offset_top, md) ||
            match(eptr, ecode+3, offset_top, md)) return TRUE;
        }
      }
    return FALSE;

    /* Start of subject unless notbol, or after internal newline if multiline */

    case OP_CIRC:
    if (md->notbol && eptr == md->start_subject) return FALSE;
    if (md->multiline)
      {
      if (eptr != md->start_subject && eptr[-1] != '\n') return FALSE;
      ecode++;
      break;
      }
    /* ... else fall through */

    /* Start of subject assertion */

    case OP_SOD:
    if (eptr != md->start_subject) return FALSE;
    ecode++;
    break;

    /* Assert before internal newline if multiline, or before
    a terminating newline unless endonly is set, else end of subject unless
    noteol is set. */

    case OP_DOLL:
    if (md->noteol && eptr >= md->end_subject) return FALSE;
    if (md->multiline)
      {
      if (eptr < md->end_subject && *eptr != '\n') return FALSE;
      ecode++;
      break;
      }
    else if (!md->endonly)
      {
      if (eptr < md->end_subject - 1 ||
         (eptr == md->end_subject - 1 && *eptr != '\n')) return FALSE;
      ecode++;
      break;
      }
    /* ... else fall through */

    /* End of subject assertion */

    case OP_EOD:
    if (eptr < md->end_subject) return FALSE;
    ecode++;
    break;

    /* Word boundary assertions */

    case OP_NOT_WORD_BOUNDARY:
    case OP_WORD_BOUNDARY:
      {
      BOOL prev_is_word = (eptr != md->start_subject) &&
        ((pcre_ctypes[eptr[-1]] & ctype_word) != 0);
      BOOL cur_is_word = (eptr < md->end_subject) &&
        ((pcre_ctypes[*eptr] & ctype_word) != 0);
      if ((*ecode++ == OP_WORD_BOUNDARY)?
           cur_is_word == prev_is_word : cur_is_word != prev_is_word)
        return FALSE;
      }
    break;

    /* Match a single character type; inline for speed */

    case OP_ANY:
    if (!md->dotall && eptr < md->end_subject && *eptr == '\n') return FALSE;
    if (eptr++ >= md->end_subject) return FALSE;
    ecode++;
    break;

    case OP_NOT_DIGIT:
    if (eptr >= md->end_subject || (pcre_ctypes[*eptr++] & ctype_digit) != 0)
      return FALSE;
    ecode++;
    break;

    case OP_DIGIT:
    if (eptr >= md->end_subject || (pcre_ctypes[*eptr++] & ctype_digit) == 0)
      return FALSE;
    ecode++;
    break;

    case OP_NOT_WHITESPACE:
    if (eptr >= md->end_subject || (pcre_ctypes[*eptr++] & ctype_space) != 0)
      return FALSE;
    ecode++;
    break;

    case OP_WHITESPACE:
    if (eptr >= md->end_subject || (pcre_ctypes[*eptr++] & ctype_space) == 0)
      return FALSE;
    ecode++;
    break;

    case OP_NOT_WORDCHAR:
    if (eptr >= md->end_subject || (pcre_ctypes[*eptr++] & ctype_word) != 0)
      return FALSE;
    ecode++;
    break;

    case OP_WORDCHAR:
    if (eptr >= md->end_subject || (pcre_ctypes[*eptr++] & ctype_word) == 0)
      return FALSE;
    ecode++;
    break;

    /* Match a back reference, possibly repeatedly. Look past the end of the
    item to see if there is repeat information following. The code is similar
    to that for character classes, but repeated for efficiency. Then obey
    similar code to character type repeats - written out again for speed.
    However, if the referenced string is the empty string, always treat
    it as matched, any number of times (otherwise there could be infinite
    loops). */

    case OP_REF:
      {
      int length;
      int number = ecode[1] << 1;                /* Doubled reference number */
      ecode += 2;                                /* Advance past the item */

      if (number >= offset_top || md->offset_vector[number] < 0)
        {
        md->errorcode = PCRE_ERROR_BADREF;
        return FALSE;
        }

      length = md->offset_vector[number+1] - md->offset_vector[number];

      switch (*ecode)
        {
        case OP_CRSTAR:
        case OP_CRMINSTAR:
        case OP_CRPLUS:
        case OP_CRMINPLUS:
        case OP_CRQUERY:
        case OP_CRMINQUERY:
        c = *ecode++ - OP_CRSTAR;
        minimize = (c & 1) != 0;
        min = rep_min[c];                 /* Pick up values from tables; */
        max = rep_max[c];                 /* zero for max => infinity */
        if (max == 0) max = INT_MAX;
        break;

        case OP_CRRANGE:
        case OP_CRMINRANGE:
        minimize = (*ecode == OP_CRMINRANGE);
        min = (ecode[1] << 8) + ecode[2];
        max = (ecode[3] << 8) + ecode[4];
        if (max == 0) max = INT_MAX;
        ecode += 5;
        break;

        default:               /* No repeat follows */
        if (!match_ref(number, eptr, length, md)) return FALSE;
        eptr += length;
        continue;              /* With the main loop */
        }

      /* If the length of the reference is zero, just continue with the
      main loop. */

      if (length == 0) continue;

      /* First, ensure the minimum number of matches are present. We get back
      the length of the reference string explicitly rather than passing the
      address of eptr, so that eptr can be a register variable. */

      for (i = 1; i <= min; i++)
        {
        if (!match_ref(number, eptr, length, md)) return FALSE;
        eptr += length;
        }

      /* If min = max, continue at the same level without recursion.
      They are not both allowed to be zero. */

      if (min == max) continue;

      /* If minimizing, keep trying and advancing the pointer */

      if (minimize)
        {
        for (i = min;; i++)
          {
          if (match(eptr, ecode, offset_top, md)) return TRUE;
          if (i >= max || !match_ref(number, eptr, length, md))
            return FALSE;
          eptr += length;
          }
        /* Control never gets here */
        }

      /* If maximizing, find the longest string and work backwards */

      else
        {
        uschar *pp = eptr;
        for (i = min; i < max; i++)
          {
          if (!match_ref(number, eptr, length, md)) break;
          eptr += length;
          }
        while (eptr >= pp)
          {
          if (match(eptr, ecode, offset_top, md)) return TRUE;
          eptr -= length;
          }
        return FALSE;
        }
      }
    /* Control never gets here */

    /* Match a character class, possibly repeatedly. Look past the end of the
    item to see if there is repeat information following. Then obey similar
    code to character type repeats - written out again for speed. If caseless
    matching was set at runtime but not at compile time, we have to check both
    versions of a character. */

    case OP_CLASS:
      {
      uschar *data = ecode + 1;  /* Save for matching */
      ecode += 33;               /* Advance past the item */

      switch (*ecode)
        {
        case OP_CRSTAR:
        case OP_CRMINSTAR:
        case OP_CRPLUS:
        case OP_CRMINPLUS:
        case OP_CRQUERY:
        case OP_CRMINQUERY:
        c = *ecode++ - OP_CRSTAR;
        minimize = (c & 1) != 0;
        min = rep_min[c];                 /* Pick up values from tables; */
        max = rep_max[c];                 /* zero for max => infinity */
        if (max == 0) max = INT_MAX;
        break;

        case OP_CRRANGE:
        case OP_CRMINRANGE:
        minimize = (*ecode == OP_CRMINRANGE);
        min = (ecode[1] << 8) + ecode[2];
        max = (ecode[3] << 8) + ecode[4];
        if (max == 0) max = INT_MAX;
        ecode += 5;
        break;

        default:               /* No repeat follows */
        if (eptr >= md->end_subject) return FALSE;
        c = *eptr++;
        if ((data[c/8] & (1 << (c&7))) != 0) continue;    /* With main loop */
        if (md->runtime_caseless)
          {
          c = pcre_fcc[c];
          if ((data[c/8] & (1 << (c&7))) != 0) continue;  /* With main loop */
          }
        return FALSE;
        }

      /* First, ensure the minimum number of matches are present. */

      for (i = 1; i <= min; i++)
        {
        if (eptr >= md->end_subject) return FALSE;
        c = *eptr++;
        if ((data[c/8] & (1 << (c&7))) != 0) continue;
        if (md->runtime_caseless)
          {
          c = pcre_fcc[c];
          if ((data[c/8] & (1 << (c&7))) != 0) continue;
          }
        return FALSE;
        }

      /* If max == min we can continue with the main loop without the
      need to recurse. */

      if (min == max) continue;

      /* If minimizing, keep testing the rest of the expression and advancing
      the pointer while it matches the class. */

      if (minimize)
        {
        for (i = min;; i++)
          {
          if (match(eptr, ecode, offset_top, md)) return TRUE;
          if (i >= max || eptr >= md->end_subject) return FALSE;
          c = *eptr++;
          if ((data[c/8] & (1 << (c&7))) != 0) continue;
          if (md->runtime_caseless)
            {
            c = pcre_fcc[c];
            if ((data[c/8] & (1 << (c&7))) != 0) continue;
            }
          return FALSE;
          }
        /* Control never gets here */
        }

      /* If maximizing, find the longest possible run, then work backwards. */

      else
        {
        uschar *pp = eptr;
        for (i = min; i < max; eptr++, i++)
          {
          if (eptr >= md->end_subject) break;
          c = *eptr;
          if ((data[c/8] & (1 << (c&7))) != 0) continue;
          if (md->runtime_caseless)
            {
            c = pcre_fcc[c];
            if ((data[c/8] & (1 << (c&7))) != 0) continue;
            }
          break;
          }

        while (eptr >= pp)
          if (match(eptr--, ecode, offset_top, md)) return TRUE;
        return FALSE;
        }
      }
    /* Control never gets here */

    /* Match a run of characters */

    case OP_CHARS:
      {
      register int length = ecode[1];
      ecode += 2;

      #ifdef DEBUG
      if (eptr >= md->end_subject)
        printf("matching subject <null> against pattern ");
      else
        {
        printf("matching subject ");
        pchars(eptr, length, TRUE, md);
        printf(" against pattern ");
        }
      pchars(ecode, length, FALSE, md);
      printf("\n");
      #endif

      if (length > md->end_subject - eptr) return FALSE;
      if (md->caseless)
        {
        while (length-- > 0) if (pcre_lcc[*ecode++] != pcre_lcc[*eptr++]) return FALSE;
        }
      else
        {
        while (length-- > 0) if (*ecode++ != *eptr++) return FALSE;
        }
      }
    break;

    /* Match a single character repeatedly; different opcodes share code. */

    case OP_EXACT:
    min = max = (ecode[1] << 8) + ecode[2];
    ecode += 3;
    goto REPEATCHAR;

    case OP_UPTO:
    case OP_MINUPTO:
    min = 0;
    max = (ecode[1] << 8) + ecode[2];
    minimize = *ecode == OP_MINUPTO;
    ecode += 3;
    goto REPEATCHAR;

    case OP_STAR:
    case OP_MINSTAR:
    case OP_PLUS:
    case OP_MINPLUS:
    case OP_QUERY:
    case OP_MINQUERY:
    c = *ecode++ - OP_STAR;
    minimize = (c & 1) != 0;
    min = rep_min[c];                 /* Pick up values from tables; */
    max = rep_max[c];                 /* zero for max => infinity */
    if (max == 0) max = INT_MAX;

    /* Common code for all repeated single-character matches. We can give
    up quickly if there are fewer than the minimum number of characters left in
    the subject. */

    REPEATCHAR:
    if (min > md->end_subject - eptr) return FALSE;
    c = *ecode++;

    /* The code is duplicated for the caseless and caseful cases, for speed,
    since matching characters is likely to be quite common. First, ensure the
    minimum number of matches are present. If min = max, continue at the same
    level without recursing. Otherwise, if minimizing, keep trying the rest of
    the expression and advancing one matching character if failing, up to the
    maximum. Alternatively, if maximizing, find the maximum number of
    characters and work backwards. */

    #ifdef DEBUG
    printf("matching %c{%d,%d} against subject %.*s\n", c, min, max,
      max, eptr);
    #endif

    if (md->caseless)
      {
      c = pcre_lcc[c];
      for (i = 1; i <= min; i++) if (c != pcre_lcc[*eptr++]) return FALSE;
      if (min == max) continue;
      if (minimize)
        {
        for (i = min;; i++)
          {
          if (match(eptr, ecode, offset_top, md)) return TRUE;
          if (i >= max || eptr >= md->end_subject || c != pcre_lcc[*eptr++])
            return FALSE;
          }
        /* Control never gets here */
        }
      else
        {
        uschar *pp = eptr;
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || c != pcre_lcc[*eptr]) break;
          eptr++;
          }
        while (eptr >= pp)
          if (match(eptr--, ecode, offset_top, md)) return TRUE;
        return FALSE;
        }
      /* Control never gets here */
      }

    /* Caseful comparisons */

    else
      {
      for (i = 1; i <= min; i++) if (c != *eptr++) return FALSE;
      if (min == max) continue;
      if (minimize)
        {
        for (i = min;; i++)
          {
          if (match(eptr, ecode, offset_top, md)) return TRUE;
          if (i >= max || eptr >= md->end_subject || c != *eptr++) return FALSE;
          }
        /* Control never gets here */
        }
      else
        {
        uschar *pp = eptr;
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || c != *eptr) break;
          eptr++;
          }
        while (eptr >= pp)
         if (match(eptr--, ecode, offset_top, md)) return TRUE;
        return FALSE;
        }
      }
    /* Control never gets here */

    /* Match a negated single character */

    case OP_NOT:
    if (eptr > md->end_subject) return FALSE;
    ecode++;
    if (md->caseless)
      {
      if (pcre_lcc[*ecode++] == pcre_lcc[*eptr++]) return FALSE;
      }
    else
      {
      if (*ecode++ == *eptr++) return FALSE;
      }
    break;

    /* Match a negated single character repeatedly. This is almost a repeat of
    the code for a repeated single character, but I haven't found a nice way of
    commoning these up that doesn't require a test of the positive/negative
    option for each character match. Maybe that wouldn't add very much to the
    time taken, but character matching *is* what this is all about... */

    case OP_NOTEXACT:
    min = max = (ecode[1] << 8) + ecode[2];
    ecode += 3;
    goto REPEATNOTCHAR;

    case OP_NOTUPTO:
    case OP_NOTMINUPTO:
    min = 0;
    max = (ecode[1] << 8) + ecode[2];
    minimize = *ecode == OP_NOTMINUPTO;
    ecode += 3;
    goto REPEATNOTCHAR;

    case OP_NOTSTAR:
    case OP_NOTMINSTAR:
    case OP_NOTPLUS:
    case OP_NOTMINPLUS:
    case OP_NOTQUERY:
    case OP_NOTMINQUERY:
    c = *ecode++ - OP_NOTSTAR;
    minimize = (c & 1) != 0;
    min = rep_min[c];                 /* Pick up values from tables; */
    max = rep_max[c];                 /* zero for max => infinity */
    if (max == 0) max = INT_MAX;

    /* Common code for all repeated single-character matches. We can give
    up quickly if there are fewer than the minimum number of characters left in
    the subject. */

    REPEATNOTCHAR:
    if (min > md->end_subject - eptr) return FALSE;
    c = *ecode++;

    /* The code is duplicated for the caseless and caseful cases, for speed,
    since matching characters is likely to be quite common. First, ensure the
    minimum number of matches are present. If min = max, continue at the same
    level without recursing. Otherwise, if minimizing, keep trying the rest of
    the expression and advancing one matching character if failing, up to the
    maximum. Alternatively, if maximizing, find the maximum number of
    characters and work backwards. */

    #ifdef DEBUG
    printf("negative matching %c{%d,%d} against subject %.*s\n", c, min, max,
      max, eptr);
    #endif

    if (md->caseless)
      {
      c = pcre_lcc[c];
      for (i = 1; i <= min; i++) if (c == pcre_lcc[*eptr++]) return FALSE;
      if (min == max) continue;
      if (minimize)
        {
        for (i = min;; i++)
          {
          if (match(eptr, ecode, offset_top, md)) return TRUE;
          if (i >= max || eptr >= md->end_subject || c == pcre_lcc[*eptr++])
            return FALSE;
          }
        /* Control never gets here */
        }
      else
        {
        uschar *pp = eptr;
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || c == pcre_lcc[*eptr]) break;
          eptr++;
          }
        while (eptr >= pp)
          if (match(eptr--, ecode, offset_top, md)) return TRUE;
        return FALSE;
        }
      /* Control never gets here */
      }

    /* Caseful comparisons */

    else
      {
      for (i = 1; i <= min; i++) if (c == *eptr++) return FALSE;
      if (min == max) continue;
      if (minimize)
        {
        for (i = min;; i++)
          {
          if (match(eptr, ecode, offset_top, md)) return TRUE;
          if (i >= max || eptr >= md->end_subject || c == *eptr++) return FALSE;
          }
        /* Control never gets here */
        }
      else
        {
        uschar *pp = eptr;
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || c == *eptr) break;
          eptr++;
          }
        while (eptr >= pp)
         if (match(eptr--, ecode, offset_top, md)) return TRUE;
        return FALSE;
        }
      }
    /* Control never gets here */

    /* Match a single character type repeatedly; several different opcodes
    share code. This is very similar to the code for single characters, but we
    repeat it in the interests of efficiency. */

    case OP_TYPEEXACT:
    min = max = (ecode[1] << 8) + ecode[2];
    minimize = TRUE;
    ecode += 3;
    goto REPEATTYPE;

    case OP_TYPEUPTO:
    case OP_TYPEMINUPTO:
    min = 0;
    max = (ecode[1] << 8) + ecode[2];
    minimize = *ecode == OP_TYPEMINUPTO;
    ecode += 3;
    goto REPEATTYPE;

    case OP_TYPESTAR:
    case OP_TYPEMINSTAR:
    case OP_TYPEPLUS:
    case OP_TYPEMINPLUS:
    case OP_TYPEQUERY:
    case OP_TYPEMINQUERY:
    c = *ecode++ - OP_TYPESTAR;
    minimize = (c & 1) != 0;
    min = rep_min[c];                 /* Pick up values from tables; */
    max = rep_max[c];                 /* zero for max => infinity */
    if (max == 0) max = INT_MAX;

    /* Common code for all repeated single character type matches */

    REPEATTYPE:
    ctype = *ecode++;      /* Code for the character type */

    /* First, ensure the minimum number of matches are present. Use inline
    code for maximizing the speed, and do the type test once at the start
    (i.e. keep it out of the loop). Also test that there are at least the
    minimum number of characters before we start. */

    if (min > md->end_subject - eptr) return FALSE;
    if (min > 0) switch(ctype)
      {
      case OP_ANY:
      if (!md->dotall)
        { for (i = 1; i <= min; i++) if (*eptr++ == '\n') return FALSE; }
      else eptr += min;
      break;

      case OP_NOT_DIGIT:
      for (i = 1; i <= min; i++)
        if ((pcre_ctypes[*eptr++] & ctype_digit) != 0) return FALSE;
      break;

      case OP_DIGIT:
      for (i = 1; i <= min; i++)
        if ((pcre_ctypes[*eptr++] & ctype_digit) == 0) return FALSE;
      break;

      case OP_NOT_WHITESPACE:
      for (i = 1; i <= min; i++)
        if ((pcre_ctypes[*eptr++] & ctype_space) != 0) return FALSE;
      break;

      case OP_WHITESPACE:
      for (i = 1; i <= min; i++)
        if ((pcre_ctypes[*eptr++] & ctype_space) == 0) return FALSE;
      break;

      case OP_NOT_WORDCHAR:
      for (i = 1; i <= min; i++) if ((pcre_ctypes[*eptr++] & ctype_word) != 0)
        return FALSE;
      break;

      case OP_WORDCHAR:
      for (i = 1; i <= min; i++) if ((pcre_ctypes[*eptr++] & ctype_word) == 0)
        return FALSE;
      break;
      }

    /* If min = max, continue at the same level without recursing */

    if (min == max) continue;

    /* If minimizing, we have to test the rest of the pattern before each
    subsequent match, so inlining isn't much help; just use the function. */

    if (minimize)
      {
      for (i = min;; i++)
        {
        if (match(eptr, ecode, offset_top, md)) return TRUE;
        if (i >= max || eptr >= md->end_subject ||
          !match_type(ctype, *eptr++, md->dotall))
            return FALSE;
        }
      /* Control never gets here */
      }

    /* If maximizing it is worth using inline code for speed, doing the type
    test once at the start (i.e. keep it out of the loop). */

    else
      {
      uschar *pp = eptr;
      switch(ctype)
        {
        case OP_ANY:
        if (!md->dotall)
          {
          for (i = min; i < max; i++)
            {
            if (eptr >= md->end_subject || *eptr == '\n') break;
            eptr++;
            }
          }
        else
          {
          c = max - min;
          if (c > md->end_subject - eptr) c = md->end_subject - eptr;
          eptr += c;
          }
        break;

        case OP_NOT_DIGIT:
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || (pcre_ctypes[*eptr] & ctype_digit) != 0)
            break;
          eptr++;
          }
        break;

        case OP_DIGIT:
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || (pcre_ctypes[*eptr] & ctype_digit) == 0)
            break;
          eptr++;
          }
        break;

        case OP_NOT_WHITESPACE:
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || (pcre_ctypes[*eptr] & ctype_space) != 0)
            break;
          eptr++;
          }
        break;

        case OP_WHITESPACE:
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || (pcre_ctypes[*eptr] & ctype_space) == 0)
            break;
          eptr++;
          }
        break;

        case OP_NOT_WORDCHAR:
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || (pcre_ctypes[*eptr] & ctype_word) != 0)
            break;
          eptr++;
          }
        break;

        case OP_WORDCHAR:
        for (i = min; i < max; i++)
          {
          if (eptr >= md->end_subject || (pcre_ctypes[*eptr] & ctype_word) == 0)
            break;
          eptr++;
          }
        break;
        }

      while (eptr >= pp)
        if (match(eptr--, ecode, offset_top, md)) return TRUE;
      return FALSE;
      }
    /* Control never gets here */

    /* There's been some horrible disaster. */

    default:
    #ifdef DEBUG
    printf("Unknown opcode %d\n", *ecode);
    #endif
    md->errorcode = PCRE_ERROR_UNKNOWN_NODE;
    return FALSE;
    }

  /* Do not stick any code in here without much thought; it is assumed
  that "continue" in the code above comes out to here to repeat the main
  loop. */

  }             /* End of main loop */
/* Control never reaches here */
}



/*************************************************
*         Execute a Regular Expression           *
*************************************************/

/* This function applies a compiled re to a subject string and picks out
portions of the string if it matches. Two elements in the vector are set for
each substring: the offsets to the start and end of the substring.

Arguments:
  external_re     points to the compiled expression
  external_extra  points to "hints" from pcre_study() or is NULL
  subject         points to the subject string
  length          length of subject string (may contain binary zeros)
  options         option bits
  offsets         points to a vector of ints to be filled in with offsets
  offsetcount     the number of elements in the vector

Returns:          > 0 => success; value is the number of elements filled in
                  = 0 => success, but offsets is not big enough
                   -1 => failed to match
                 < -1 => some kind of unexpected problem
*/

int
pcre_exec(const pcre *external_re, const pcre_extra *external_extra,
  const char *subject, int length, int options, int *offsets, int offsetcount)
{
int resetcount;
int ocount = offsetcount;
int first_char = -1;
match_data match_block;
uschar *start_bits = NULL;
uschar *start_match = (uschar *)subject;
uschar *end_subject;
real_pcre *re = (real_pcre *)external_re;
real_pcre_extra *extra = (real_pcre_extra *)external_extra;
BOOL anchored = ((re->options | options) & PCRE_ANCHORED) != 0;
BOOL startline = (re->options & PCRE_STARTLINE) != 0;

if ((options & ~PUBLIC_EXEC_OPTIONS) != 0) return PCRE_ERROR_BADOPTION;

if (re == NULL || subject == NULL ||
   (offsets == NULL && offsetcount > 0)) return PCRE_ERROR_NULL;
if (re->magic_number != MAGIC_NUMBER) return PCRE_ERROR_BADMAGIC;

match_block.start_subject = (uschar *)subject;
match_block.end_subject = match_block.start_subject + length;
end_subject = match_block.end_subject;

match_block.caseless  = ((re->options | options) & PCRE_CASELESS) != 0;
match_block.runtime_caseless = match_block.caseless &&
  (re->options & PCRE_CASELESS) == 0;

match_block.multiline = ((re->options | options) & PCRE_MULTILINE) != 0;
match_block.dotall    = ((re->options | options) & PCRE_DOTALL) != 0;
match_block.endonly   = ((re->options | options) & PCRE_DOLLAR_ENDONLY) != 0;

match_block.notbol = (options & PCRE_NOTBOL) != 0;
match_block.noteol = (options & PCRE_NOTEOL) != 0;

match_block.errorcode = PCRE_ERROR_NOMATCH;     /* Default error */

/* If the expression has got more back references than the offsets supplied can
hold, we get a temporary bit of working store to use during the matching.
Otherwise, we can use the vector supplied, rounding down the size of it to a
multiple of 2. */

ocount &= (-2);
if (re->top_backref > 0 && re->top_backref + 1 >= ocount/2)
  {
  ocount = re->top_backref * 2 + 2;
  match_block.offset_vector = (int*)(pcre_malloc)(ocount * sizeof(int));
  if (match_block.offset_vector == NULL) return PCRE_ERROR_NOMEMORY;
  #ifdef DEBUG
  printf("Got memory to hold back references\n");
  #endif
  }
else match_block.offset_vector = offsets;

match_block.offset_end = ocount;
match_block.offset_overflow = FALSE;

/* Compute the minimum number of offsets that we need to reset each time. Doing
this makes a huge difference to execution time when there aren't many brackets
in the pattern. */

resetcount = 2 + re->top_bracket * 2;
if (resetcount > offsetcount) resetcount = ocount;

/* If MULTILINE is set at exec time but was not set at compile time, and the
anchored flag is set, we must re-check because a setting provoked by ^ in the
pattern is not right in multi-line mode. Calling is_anchored() again here does
the right check, because multiline is now set. If it now yields FALSE, the
expression must have had ^ starting some of its branches. Check to see if
that is true for *all* branches, and if so, set the startline flag. */

if (match_block. multiline && anchored && (re->options & PCRE_MULTILINE) == 0 &&
    !is_anchored(re->code, match_block.multiline))
  {
  anchored = FALSE;
  if (is_startline(re->code)) startline = TRUE;
  }

/* Set up the first character to match, if available. The first_char value is
never set for an anchored regular expression, but the anchoring may be forced
at run time, so we have to test for anchoring. The first char may be unset for
an unanchored pattern, of course. If there's no first char and the pattern was
studied, the may be a bitmap of possible first characters. However, we can
use this only if the caseless state of the studying was correct. */

if (!anchored)
  {
  if ((re->options & PCRE_FIRSTSET) != 0)
    {
    first_char = re->first_char;
    if (match_block.caseless) first_char = pcre_lcc[first_char];
    }
  else
    if (!startline && extra != NULL &&
      (extra->options & PCRE_STUDY_MAPPED) != 0 &&
      ((extra->options & PCRE_STUDY_CASELESS) != 0) == match_block.caseless)
        start_bits = extra->start_bits;
  }

/* Loop for unanchored matches; for anchored regexps the loop runs just once. */

do
  {
  register int *iptr = match_block.offset_vector;
  register int *iend = iptr + resetcount;

  /* Reset the maximum number of extractions we might see. */

  while (iptr < iend) *iptr++ = -1;

  /* Advance to a unique first char if possible */

  if (first_char >= 0)
    {
    if (match_block.caseless)
      while (start_match < end_subject && pcre_lcc[*start_match] != first_char)
        start_match++;
    else
      while (start_match < end_subject && *start_match != first_char)
        start_match++;
    }

  /* Or to just after \n for a multiline match if possible */

  else if (startline)
    {
    if (start_match > match_block.start_subject)
      {
      while (start_match < end_subject && start_match[-1] != '\n')
        start_match++;
      }
    }

  /* Or to a non-unique first char */

  else if (start_bits != NULL)
    {
    while (start_match < end_subject)
      {
      register int c = *start_match;
      if ((start_bits[c/8] & (1 << (c&7))) == 0) start_match++; else break;
      }
    }

  #ifdef DEBUG
  printf(">>>> Match against: ");
  pchars(start_match, end_subject - start_match, TRUE, &match_block);
  printf("\n");
  #endif

  /* When a match occurs, substrings will be set for all internal extractions;
  we just need to set up the whole thing as substring 0 before returning. If
  there were too many extractions, set the return code to zero. In the case
  where we had to get some local store to hold offsets for backreferences, copy
  those back references that we can. In this case there need not be overflow
  if certain parts of the pattern were not used.

  Before starting the match, we have to set up a longjmp() target to enable
  the "cut" operation to fail a match completely without backtracking. */

  if (setjmp(match_block.fail_env) == 0 &&
      match(start_match, re->code, 2, &match_block))
    {
    int rc;

    if (ocount != offsetcount)
      {
      if (offsetcount >= 4)
        {
        memcpy(offsets + 2, match_block.offset_vector + 2,
          (offsetcount - 2) * sizeof(int));
        #ifdef DEBUG
        printf("Copied offsets; freeing temporary memory\n");
        #endif
        }
      if (match_block.end_offset_top > offsetcount)
        match_block.offset_overflow = TRUE;

      #ifdef DEBUG
      printf("Freeing temporary memory\n");
      #endif

      (pcre_free)(match_block.offset_vector);
      }

    rc = match_block.offset_overflow? 0 : match_block.end_offset_top/2;

    if (match_block.offset_end < 2) rc = 0; else
      {
      offsets[0] = start_match - match_block.start_subject;
      offsets[1] = match_block.end_match_ptr - match_block.start_subject;
      }

    #ifdef DEBUG
    printf(">>>> returning %d\n", rc);
    #endif
    return rc;
    }
  }
while (!anchored &&
       match_block.errorcode == PCRE_ERROR_NOMATCH &&
       start_match++ < end_subject);

#ifdef DEBUG
printf(">>>> returning %d\n", match_block.errorcode);
#endif

return match_block.errorcode;
}

/* End of pcre.c */
