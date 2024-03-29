/*************************************************
*       Perl-Compatible Regular Expressions      *
*************************************************/

/* Copyright (c) 1997 University of Cambridge */

#ifndef _PCRE_H
#define _PCRE_H

/* Have to include stdlib.h in order to ensure that size_t is defined;
it is needed here for malloc. */

#include <stdlib.h>

/* Options */

#define PCRE_CASELESS        0x0001
#define PCRE_EXTENDED        0x0002
#define PCRE_ANCHORED        0x0004
#define PCRE_MULTILINE       0x0008
#define PCRE_DOTALL          0x0010
#define PCRE_DOLLAR_ENDONLY  0x0020
#define PCRE_EXTRA           0x0040
#define PCRE_NOTBOL          0x0080
#define PCRE_NOTEOL          0x0100

/* Exec-time error codes */

#define PCRE_ERROR_NOMATCH        (-1)
#define PCRE_ERROR_BADREF         (-2)
#define PCRE_ERROR_NULL           (-3)
#define PCRE_ERROR_BADOPTION      (-4)
#define PCRE_ERROR_BADMAGIC       (-5)
#define PCRE_ERROR_UNKNOWN_NODE   (-6)
#define PCRE_ERROR_NOMEMORY       (-7)

/* Types */

typedef void pcre;
typedef void pcre_extra;

/* Functions */

extern pcre *pcre_compile(const char *, int, char **, int *);
extern int pcre_exec(const pcre *, const pcre_extra *, const char *,
  int, int, int *, int);
extern int pcre_info(const pcre *, int *, int *);
extern pcre_extra *pcre_study(const pcre *, int, char **);
extern char *pcre_version(void);

#endif /* End of pcre.h */
