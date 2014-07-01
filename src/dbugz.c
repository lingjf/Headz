#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h> /* <winsock2.h> included */
#include <process.h> 
#else
#include <unistd.h> 
#include <sys/time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#endif

#if defined(HAVE_GETRUSAGE)
#include <sys/param.h>
#include <sys/resource.h>
#endif

#ifndef SOCKET
#define SOCKET int
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif

#ifdef DBUG_COMPILE_TIME_OFF
#undef DBUG_COMPILE_TIME_OFF
#endif

#ifdef DBUG_COMPILE_TIME_TRACE_OFF
#undef DBUG_COMPILE_TIME_TRACE_OFF
#endif

#ifdef DBUG_COMPILE_TIME_LOG_OFF
#undef DBUG_COMPILE_TIME_LOG_OFF
#endif

#ifdef __cplusplus
extern "C"
{
#endif 

#include "jeffz.h"
#include "dbugz.h"

/* Manifest constants which may be "tuned" if desired. */

#define OUTPUT_BUFFER_SIZE  (1024 * 128)  /* Maximum buffer length each output line can use Bytes */

#define DEFAULT_LOG_LEVEL   (9)           /* Log level <= it will output */ 
#define DEFAULT_MAX_DEPTH   (200)         /* Maximum trace depth default */ 

/*                                B      K      M      G   -Bytes  */
#define DEFAULT_FILE_LIMIT       (1024 * 1024 * 1024 * 3UL)
#define DEFAULT_FILE_COUNT        1 
#define DEFAULT_FILE_FLUSH_LEVEL  9
#define DEFAULT_FILE_NAME         "dbugz.log"
#define DEFAULT_FILE_DELAY        0
#define DEFAULT_FILE_APPEND       0
#define DEFAULT_PIPE_NAME         "dbugz.pipe"
#define DEFAULT_PIPE_MASK         0444
#define DEFAULT_UDP_ADDRESS       "127.0.0.1"
#define DEFAULT_UDP_PORT          9527

#define TRACE_ON        0x0001   /* Trace enabled */
#define LOG_ON          0x0002   /* Log enabled */
#define FILE_ON         0x0004   /* File name print enabled */
#define LINE_ON         0x0008   /* Line number print enabled */
#define DEPTH_ON        0x0010   /* Function nest level print enabled */
#define INDEX_ON        0x0020   /* Number each line of output */
#define FUNC_ON         0x0040   /* Function of output */
#define PID_ON          0x0100   /* Identify each line with process id */
#define STACK_ON        0x0200   /* Stack dump enabled */

long _no_db_ = 0; /* 1. if no debugging at all */
long _log_on_ = 0; /* 1. if logging currently on */
long _trace_on_ = 0; /* 1. if trace currently on */

struct filter
{
   struct filter *next;
   char *pattern;
   char buffer[4];
};

/*
 *	Debugging states can be pushed or popped off of a
 *	set which is implemented as a linked list.  Note
 *	that the head of the list is the current setting and the
 *	set is pushed by adding a new setting to the head of the
 *	list or popped by removing the first filter.
 */

struct setting
{
   struct setting *next; /* Next setting in the list */
   unsigned long flags; /* Current setting flags */
   unsigned long maxdepth; /* Current maximum trace depth */
   unsigned long sub_level; /* Sub this from context->nest_level */
   unsigned long log_level; /* Output log level */
   struct filter *included_funcs; /* List of functions */
   struct filter *excluded_funcs; /* Exception list of functions*/
   struct filter *included_files; /* List of files */
   struct filter *excluded_files; /* Exception list of files*/

   /* terminal observer */
   FILE *term_handle; /* Current terminal stream */
   unsigned long term_enable; /* Terminal stream is enable */

   /* file observer */
   FILE *file_handle; /* Current file stream */
   unsigned long file_limit; /* Max length of output file */
   unsigned long file_length; /* Current length of output file */
   unsigned short file_count; /* Max file count of output file */
   unsigned short file_index; /* Current count of output file */
   unsigned long file_flush_level; /* Flush log level */
   unsigned long file_last_flush; /* Timestamp of last flush */
   unsigned long file_delay; /* file_delay after each output line */
   unsigned long file_append; /* First log file append or overwrited */
   char file_name[256]; /* Name of output file */
   unsigned long file_enable; /* File stream is enable */

   /* udp observer */SOCKET udp_socket; /* Current upd socket stream */
   struct sockaddr_in udp_address; /* Remote IP address */
   unsigned long udp_bytes; /* Total bytes udp sendto */
   unsigned long udp_enable; /* UDP stream is enable */

   /* pipe observer */
   int pipe_handle; /* Current pipe stream */
   unsigned long pipe_bytes; /* Total bytes pipe sendto */
   char pipe_name[256]; /* Name of output pipe */
   unsigned long pipe_mask; /* User mask of output pipe */
   unsigned long pipe_enable; /* Pipe stream is enable */
};

struct errcode
{
   const char *file;
   const char *func;
   int line;
   int code;
};

struct context
{
   unsigned long pid; /* Thread id */
   struct context *next; /* Next context in the list */
   unsigned long index; /* Current debugger index */
   unsigned long nest_level; /* Current function nesting level */
   _db_frame_ *frame; /* Pointer to current frame */
   int jmplevel; /* Remember nesting level at setjmp () */

   const char *u_file; /* Used only by _db_args_() and _db_print_(). */
   const char *u_func; /* Used only by _db_args_() and _db_print_(). */
   int u_line; /* Used only by _db_args_() and _db_print_(). */

   unsigned ec_index;
   struct errcode ec_stack[256];

   unsigned bufpos;
   char buffer[OUTPUT_BUFFER_SIZE];
};

static int init_done = 0;
static struct setting *set = 0;
static struct context *tls[256];

static __inline struct context *
__context(void)
{
   unsigned long pid;
   struct context *ctx;
#ifdef _WIN32
   pid = (unsigned long)GetCurrentThreadId();
#else
   pid = (unsigned long) pthread_self();
#endif

   ctx = tls[pid & 0xff];
   while (ctx) {
      if (ctx->pid == pid)
         return ctx;
      ctx = ctx->next;
   }
   ctx = (struct context *) malloc(sizeof(*ctx));
   if (ctx) {
      memset(ctx, 0, sizeof(*ctx));
      ctx->pid = pid;
      ctx->next = tls[pid & 0xff];
      tls[pid & 0xff] = ctx;
   }
   return ctx;
}

int __ptncmp(const char *pattern, const char *source)
{
   int negate, found;
   assert(pattern);
   assert(source);
   for (;;) {
      switch (*pattern) {
      case '\0':
         return *source ? 1 : 0;
      case '?':
         if (!*source++)
            return 1;
         pattern++;
         break;
      case '*':
         while (*++pattern == '*') {
         }
         if (!*pattern)
            return 0;
         for (; *source; source++) {
            if (!__ptncmp(pattern, source))
               return 0;
         }
         return 1;
      case '[':
         if (!*source)
            return 1;
         pattern++;
         negate = *pattern == '!' || *pattern == '^';
         if (negate)
            pattern++;
         for (found = 0; *pattern != ']'; pattern++) {
            if (!*pattern)
               return 1;
            if (*pattern == '-') {
               char a = *(pattern - 1);
               char z = *(pattern + 1);
               if (a == '[' || a == '!' || a == '^' || a == '-')
                  a = 0x00;
               if (z == ']' || z == '!' || z == '^' || z == '-')
                  z = 0x7f;
               if (a <= *source && *source <= z)
                  found = 1;
            } else {
               if (*pattern == *source)
                  found = 1;
            }
         }
         if ((negate && found) || (!negate && !found))
            return 1;
         pattern++, source++;
         break;
      default:
         if (*pattern++ != *source++)
            return 1;
         break;
      }
   }
   return 1;
}

static int __in_list(struct filter *fr, char *source)
{
   struct filter *p;
   assert(&fr);
   assert(source);

   for (p = fr; p; p = p->next) {
      if (!__ptncmp(p->pattern, source))
         return 1;
   }
   return 0;
}

static void __freelist(struct filter *fr)
{
   struct filter *t;
   assert(&fr);

   while (fr) {
      t = fr;
      fr = fr->next;
      free(t);
   }
}

static void __pushset(void)
{
   struct setting *tmp;
   tmp = (struct setting *) malloc(sizeof(struct setting));
   if (tmp) {
      tmp->flags = 0;
      tmp->maxdepth = DEFAULT_MAX_DEPTH;
      tmp->sub_level = 0;
      tmp->log_level = DEFAULT_LOG_LEVEL;
      tmp->included_funcs = NULL;
      tmp->excluded_funcs = NULL;
      tmp->included_files = NULL;
      tmp->excluded_files = NULL;

      tmp->term_handle = stdout;
      tmp->term_enable = 0;

      tmp->file_handle = NULL;
      tmp->file_limit = DEFAULT_FILE_LIMIT;
      tmp->file_count = DEFAULT_FILE_COUNT;
      tmp->file_length = 0;
      tmp->file_index = 0;
      tmp->file_flush_level = DEFAULT_FILE_FLUSH_LEVEL;
      tmp->file_last_flush = 0;
      tmp->file_delay = DEFAULT_FILE_DELAY;
      tmp->file_append = DEFAULT_FILE_APPEND;
      strcpy(tmp->file_name, DEFAULT_FILE_NAME);
      tmp->file_enable = 0;

      tmp->udp_socket = INVALID_SOCKET;
      tmp->udp_address.sin_family = AF_INET;
      tmp->udp_address.sin_addr.s_addr = inet_addr(DEFAULT_UDP_ADDRESS);
      tmp->udp_address.sin_port = htons(DEFAULT_UDP_PORT);
      tmp->udp_bytes = 0;
      tmp->udp_enable = 0;

      tmp->pipe_handle = -1;
      strcpy(tmp->file_name, DEFAULT_PIPE_NAME);
      tmp->pipe_mask = DEFAULT_PIPE_MASK;
      tmp->pipe_bytes = 0;
      tmp->pipe_enable = 0;

      tmp->next = set;
      set = tmp;
   }
}

static __inline unsigned long __cpuclock()
{
#if defined(HAVE_GETRUSAGE)   
   struct rusage ru;
   (void) getrusage(RUSAGE_SELF, &ru);
   return ru.ru_utime.tv_sec*1000 + ru.ru_utime.tv_usec/1000;
#else
   return clock() * (1000 / CLOCKS_PER_SEC);
#endif
}

static __inline char *
__basename(const char *pathname)
{
   char *p;
   assert(pathname);

#ifdef _WIN32
   p = strrchr(pathname, '\\');
   if (!p)
   p = strrchr(pathname, '/');
#else
   p = strrchr(pathname, '/');
   if (!p)
      p = strrchr(pathname, '\\');
#endif
   if (p)
      return p + 1;
   else
      return (char*) pathname;
}

static __inline void __prefix(struct context *ctx, const char *file, int line)
{
   assert(ctx);
   assert(file);
   assert(&line);

   ctx->index++;
   if (set->flags & INDEX_ON) {
      ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "\n%5lu: ", ctx->index);
   } else {
      ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "\n");
   }

   /* if (set->flags & CALENDOR_ON) */
   {
      const char *format = "%04d-%02d-%02d %02d:%02d:%02d.%03ld ";
#ifdef _WIN32
      SYSTEMTIME t;
      GetLocalTime(&t);
      ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], format,
                  t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
#else
      struct timeval tv;
      struct tm *t;
      gettimeofday(&tv, 0);
      t = localtime((time_t*) &tv.tv_sec);
      ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], format, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                             t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec / 1000);
#endif 
   }
   if (set->flags & PID_ON) {
      ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "%5lu ", ctx->pid);
   }
   if (set->flags & FILE_ON) {
      if (set->flags & LINE_ON) {
         char temp[300];
         int len = sprintf(temp, "%s:%d", __basename(file), line) - 14;
         if (len < 0)
            len = 0;
         ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "%14s ", temp + len);
      } else {
         ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "%14s ", __basename(file));
      }
   } else {
      if (set->flags & LINE_ON) {
         ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "%5d ", line);
      }
   }
   if (set->flags & DEPTH_ON) {
      ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "%4lu ", ctx->nest_level);
   }
}

static __inline void __indent(struct context *ctx, int indent)
{
   const int INDENT_INTVAL = 2;
   int i;
   assert(ctx);
   assert(&indent);

   indent = (indent - 1 - set->sub_level) * INDENT_INTVAL;
   for (i = 0; i < indent; i++) {
      if ((i % INDENT_INTVAL) == 0)
         ctx->buffer[ctx->bufpos++] = '|';
      else
         ctx->buffer[ctx->bufpos++] = ' ';
   }
   ctx->buffer[ctx->bufpos] = '\0';
}

static __inline int __tracable(struct context *ctx, const char *file, const char *func)
{
   assert(ctx);
   assert(file);
   assert(func);

   if (!(set->flags & TRACE_ON))
      return 0;
   if (ctx->nest_level > set->maxdepth)
      return 0;
   if (set->included_files && !__in_list(set->included_files, __basename(file)))
      return 0;
   if (__in_list(set->excluded_files, __basename(file)))
      return 0;
   if (set->included_funcs && !__in_list(set->included_funcs, (char*) func))
      return 0;
   if (__in_list(set->excluded_funcs, (char*) func))
      return 0;
   return 1;
}

static __inline int __logable(struct context *ctx, const char *file, const char *func)
{
   assert(ctx);
   assert(file);
   assert(func);

   if (!(set->flags & LOG_ON))
      return 0;
   if (set->included_files && !__in_list(set->included_files, __basename(file)))
      return 0;
   if (__in_list(set->excluded_files, __basename(file)))
      return 0;
   if (set->included_funcs && !__in_list(set->included_funcs, (char*) func))
      return 0;
   if (__in_list(set->excluded_funcs, (char*) func))
      return 0;
   return 1;
}

static void __openfile()
{
   FILE *fp;
   char file_name[300];

   if (!set->file_enable || set->file_handle)
      return;
   if (set->file_count == 0 || set->file_limit == 0)
      return;
   if (set->file_count == 1) {
      sprintf(file_name, "%s", set->file_name);
   } else {
      char format[16];
      sprintf(format, "%u", set->file_count);
      sprintf(format, "%%s.%%0%dd", strlen(format));
      sprintf(file_name, format, set->file_name, set->file_index % set->file_count + 1);
   }
   fp = fopen(file_name, (set->file_append && set->file_index == 0) ? "a+" : "w");
   if (!fp) {
      fprintf(stderr, "can't open debug output stream %s: ", file_name);
      fflush(stderr);
   } else {
      set->file_handle = fp;
      fseek(set->file_handle, 0, SEEK_END);
      set->file_length = ftell(set->file_handle);
#ifndef _WIN32      
      chown(file_name, getuid(), getgid());
#endif
   }

   /* increase file index , bypass 1st if append */
   set->file_index++;
   if (set->file_append)
      if (set->file_index % set->file_count == 0)
         set->file_index++;
}

static void __closefile(struct setting *tmp)
{
   assert(tmp);
   if (tmp->file_enable && tmp->file_handle != NULL) {
      if (fclose(tmp->file_handle) == EOF) {
         fprintf(set->file_handle, "can't close debug file");
      }
      tmp->file_handle = NULL;
   }
}

static __inline void __flushfile(void *data, unsigned long len, unsigned long log_level)
{
   size_t writed;
   struct timeval tv;
   assert(data);
   assert(&len);
   assert(&log_level);
   if (!set->file_handle)
      __openfile();

   if (!set->file_handle)
      return;

   writed = fwrite(data, 1, len, set->file_handle);
   if (writed != len) {
      __closefile(set);
      return;
   }
   set->file_length += writed;

#ifdef _WIN32
#define ustamp_common() GetTickCount() 
#define usleep_common(ms) Sleep(ms)
#else
#define ustamp_common() (gettimeofday(&tv, NULL)==-1)?0:tv.tv_sec*1000+tv.tv_usec/1000 
#define usleep_common(ms) usleep(ms)
#endif

   if (set->file_flush_level >= log_level) {
      if (set->file_delay) {
         unsigned long file_last_flush = set->file_last_flush;
         set->file_last_flush = ustamp_common();
         if (set->file_last_flush > file_last_flush) { /*varible not overlap*/
            unsigned long delta = set->file_last_flush - file_last_flush;
            if (set->file_delay > delta)
               usleep_common(set->file_delay - delta);
         }
      }
      fflush(set->file_handle);
   }

   if (set->file_length > set->file_limit) {
      __closefile(set);
   }
}

static void __openudp()
{
   if (!set->udp_enable)
      return;
   if (set->udp_socket != INVALID_SOCKET)
      return;
   set->udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
}

static void __closeudp(struct setting *tmp)
{
   assert(tmp);
   if (tmp->udp_enable && tmp->udp_socket != INVALID_SOCKET) {
#ifdef _WIN32
      if (closesocket(tmp->udp_socket) == SOCKET_ERROR)
#else
      if (close(tmp->udp_socket) == -1)
#endif
      {
         fprintf(stderr, "can't close udp socket");
      }
      tmp->udp_socket = INVALID_SOCKET;
   }
}

static __inline void __flushudp(void *data, unsigned long len)
{
   assert(data);
   assert(&len);
   if (set->udp_socket == INVALID_SOCKET)
      __openudp();

   if (set->udp_socket != INVALID_SOCKET) {
      size_t writed = sendto(set->udp_socket, data, len, 0, (struct sockaddr *) &set->udp_address,
                             sizeof(set->udp_address));
      if (writed != len) {
         __closeudp(set);
      } else {
         set->udp_bytes += writed;
      }
   }
}

/**
 A FIFO special file (a named pipe) is similar to a pipe, except that it
 is accessed as part of the file system.  It can be opened  by  multiple
 processes  for  reading  or writing. When processes are exchanging data
 via the FIFO, the kernel passes all data internally without writing  it
 to  the file system. Thus, the FIFO special file has no contents on the
 file system, the file system entry merely serves as a  reference  point
 so  that processes can access the pipe using a name in the file system.

 The kernel maintains exactly one pipe object for each FIFO special file
 that  is  opened  by  at least one process.  The FIFO must be opened on
 both ends (reading and writing) before data can  be  passed.  Normally,
 opening the FIFO blocks until the other end is opened also.

 A  process  can open a FIFO in non-blocking mode. In this case, opening
 for read only will succeed even if noone has opened on the  write  side
 yet;  opening  for  write  only will fail with ENXIO (no such device or
 address) unless the other end has already been opened.

 Under Linux, opening a FIFO for read and write  will  succeed  both  in
 blocking  and non-blocking mode. POSIX leaves this behaviour undefined.
 This can be used to open a FIFO for writing while there are no  readers
 available.  A process that uses both ends of the connection in order to
 communicate with itself should be very careful to avoid deadlocks.
 **/

static void __openpipe()
{
#ifndef _WIN32
   if (!set->pipe_enable)
      return;
   if (set->pipe_handle != -1)
      return;
   set->pipe_handle = open(set->pipe_name, O_WRONLY | O_NONBLOCK);
#endif
}

static void __closepipe(struct setting *tmp)
{
#ifndef _WIN32   
   assert(tmp);
   if (tmp->pipe_enable && tmp->pipe_handle != -1) {
      if (close(tmp->pipe_handle) == -1) {
         fprintf(stderr, "can't close debug file");
         fflush(stderr);
      }
      tmp->pipe_handle = -1;
   }
#endif
}

static __inline void __flushpipe(void *data, unsigned long len)
{
#ifndef _WIN32   
   assert(data);
   assert(len);
   if (set->pipe_handle == -1)
      __openpipe();

   if (set->pipe_handle != -1) {
      size_t writed = write(set->pipe_handle, data, len);
      if (writed == len) {
         set->pipe_bytes += writed;
      }
   }
#endif
}

static __inline void __output(struct context *ctx, unsigned long log_level)
{
   assert(ctx);
   assert(&log_level);

   if (set->term_enable)
      fwrite(ctx->buffer, 1, ctx->bufpos, set->term_handle);
   if (set->udp_enable)
      __flushudp((void*) ctx->buffer, ctx->bufpos);
   if (set->file_enable)
      __flushfile((void*) ctx->buffer, ctx->bufpos, log_level);
   if (set->pipe_enable)
      __flushpipe((void*) ctx->buffer, ctx->bufpos);

   ctx->bufpos = 0;
}

static __inline unsigned long __levelnumb(char level)
{
   assert(&level);
   switch (level) {
   case '0':
   case 'F':
   case 'f':
      return 0;
   case '1':
   case 'E':
   case 'e':
      return 1;
   case '2':
   case 'W':
   case 'w':
      return 2;
   case '3':
   case 'I':
   case 'i':
      return 3;
   case '4':
   case 'D':
   case 'd':
      return 4;
   case '5':
   case 'T':
   case 't':
      return 5;
   case '6':
      return 6;
   case '7':
      return 7;
   case '8':
      return 8;
   case '9':
      return 9;
   default:
      return 9;
   }
}

static __inline char __levelsymb(unsigned long level)
{
   assert(&level);
   switch (level) {
   case 0:
      return 'F';
   case 1:
      return 'E';
   case 2:
      return 'W';
   case 3:
      return 'I';
   case 4:
      return 'D';
   case 5:
      return 'T';
   case 6:
   case 7:
   case 8:
   case 9:
      return '0' + (signed char) level;
   default:
      return '?';
      break;
   }
}

void _db_init_(void)
{
   if (!init_done) {
      init_done = 1;

      set = 0;
      memset(tls, 0, sizeof(tls));
      __pushset();
   }
}

void _db_exit_(void)
{
   int i;
   if (init_done) {
      while (set) {
         _db_pop_();
      }
      for (i = 0; i < 256; i++) {
         while (tls[i] != NULL) {
            struct context *old = tls[i];
            tls[i] = tls[i]->next;
            free(old);
         }
      }
   }
   _no_db_ = 1;
   init_done = 0;
}

static char *
__next_field(char *control, char *prev)
{
   char *p;
   assert(&control);
   assert(&prev);

   if (prev == NULL)
      p = control;
   else {
      for (p = prev; *p != '\0'; p++) {
         if (*p == ';') {
            ++p;
            break;
         }
      }
   }
   if (p == NULL)
      return NULL;

   while (isspace((int)*p))
      p++;
   if (*p)
      return p;
   else
      return NULL;
}

static char *
__next_modifer(char *prev)
{
   assert(&prev);
   if (prev) {
      char *p = prev + 1;
      while (*p != '\0' && *p != ';') {
         if (*p == ',')
            return p;
         p++;
      }
   }
   return NULL;
}

static char *
__modifer_str(char *m, char *str, int len)
{
   char *p, *q;
   assert(m);
   assert(str);
   assert(&len);

   p = m + 1;
   q = str;

   while (isspace((int)*p))
      p++;
   while (*p != '\0' && *p != ',' && *p != ';' && q < str + len)
      *q++ = *p++;
   do {
      *q-- = '\0';
   } while (q >= str && isspace((int)*q));

   return str;
}

static int __modifer_num(char *m, double *v)
{
   char temp[256];
   int has0x = 0;
   double unit = 1.0;
   assert(m);
   assert(v);

   __modifer_str(m, temp, sizeof(temp));
   if (!strlen(temp)) {
      return 0;
   }

   has0x = strstr(temp, "0x") || strstr(temp, "0X");

   if (strchr(temp, (int) 'k') || strchr(temp, (int) 'K')) {
      unit = 1024.0;
   } else if (strchr(temp, (int) 'm') || strchr(temp, (int) 'M')) {
      unit = 1024 * 1024.0;
   } else if (strchr(temp, (int) 'g') || strchr(temp, (int) 'G')) {
      unit = 1024 * 1024 * 1024.0;
   } else if (strchr(temp, (int) 't') || strchr(temp, (int) 'T')) {
      unit = 1024 * 1024 * 1024 * 1024.0;
   }
   if (has0x) {
      *v = strtol(temp, NULL, 16) * unit;
   } else {
      *v = atof(temp) * unit;
   }

   return 1;
}

static int __modifer_has(char *m, ...)
{
   va_list ap;
   char *cp;
   char temp[256];
   int has = 0;

   assert(m);
   __modifer_str(m, temp, sizeof(temp));
   va_start(ap, m);
   while ((cp = va_arg(ap, char *))) {
      if (!strcmp(temp, cp)) {
         has = 1;
         break;
      }
   }
   va_end(ap);

   return has;
}

void _db_push_(char *control)
{
   double t;
   char *f = 0, *m = 0, temp[256];
   assert(&control);

   if (!init_done)
      _db_init_();

   if (control && *control == '-') {
      if (*++control == '#')
         control++;
   }
   __pushset();

   while ((f = __next_field(control, f))) {
      switch (*f) {
      case 'O':
         if (!(m = __next_modifer(f))) {
            break;
         }
         if (__modifer_has(m, "t", "term", "terminal", "console", 0)) {
            set->term_enable = 1;
         } else if (__modifer_has(m, "u", "udp", "network", 0)) {
            set->udp_enable = 1;
            if ((m = __next_modifer(m))) { /*<remote_ip_address>*/
               __modifer_str(m, temp, sizeof(temp));
               if (strlen(temp))
                  set->udp_address.sin_addr.s_addr = inet_addr(temp);
            }
            if ((m = __next_modifer(m))) { /*<remote_ip_port>*/
               if (__modifer_num(m, &t))
                  set->udp_address.sin_port = htons((unsigned short) t);
            }
         } else if (__modifer_has(m, "f", "a", "file", "disk", "append", 0)) {
            set->file_enable = 1;
            if (__modifer_has(m, "a", "append", 0))
               set->file_append = 1;
            if ((m = __next_modifer(m))) { /*<file_path>*/
               __modifer_str(m, set->file_name, sizeof(set->file_name));
            }
            if ((m = __next_modifer(m))) { /*<file_size_limit>*/
               if (__modifer_num(m, &t))
                  set->file_limit = (unsigned long) t;
            }
            if ((m = __next_modifer(m))) { /*<file_count_limit>*/
               if (__modifer_num(m, &t))
                  set->file_count = (unsigned short) t;
            }
            if ((m = __next_modifer(m))) { /*<flush_level>*/
               if (__modifer_num(m, &t))
                  set->file_flush_level = (unsigned long) t;
            }
            if ((m = __next_modifer(m))) { /*<flush_delay>*/
               if (__modifer_num(m, &t))
                  set->file_delay = (unsigned long) t;
            }
         } else if (__modifer_has(m, "p", "pipe", "fifo", 0)) {
#ifndef _WIN32            
            set->pipe_enable = 1;
            /* simple mask SIGPIPE code ~ */
#if 1               
            signal(SIGPIPE, SIG_IGN);
#endif
            /* general mask SIGPIPE code ~ */
#if 0
            struct sigaction sa;
            sa.sa_handler = SIG_IGN;
            sa.sa_flags = 0;
            sigemptyset(&sa.sa_mask);
            sigaction(SIGPIPE, &sa, 0);
#endif

            /* pthread mask SIGPIPE code ~ */
#if 0
            sigset_t signal_mask;
            sigemptyset(&signal_mask);
            sigaddset(&signal_mask, SIGPIPE);
            pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
#endif

            if ((m = __next_modifer(m))) { /*<pipe_name>*/
               __modifer_str(m, set->pipe_name, sizeof(set->pipe_name));
            }
            if ((m = __next_modifer(m))) { /*<pipe_mask>*/
               if (__modifer_num(m, &t))
                  set->pipe_mask = (unsigned long) t;
            }
            unlink(set->pipe_name);
            if (mkfifo(set->pipe_name, set->pipe_mask) == -1) {
               fprintf(stderr, "mkfifo error. errno = %d\n", errno);
               fflush(stderr);
            }
#endif
         }
         break;
      case 'L':
         set->flags |= LOG_ON;
         if ((m = __next_modifer(f))) {
            if (__modifer_num(m, &t))
               set->log_level = (unsigned long) t;
         }
         break;
      case 'T':
         set->flags |= TRACE_ON;
         if ((m = __next_modifer(f))) { /* <dump_stack> */
            if (__modifer_has(m, "s", "stack", 0))
               set->flags |= STACK_ON;
         }
         if ((m = __next_modifer(m))) { /* <inheritless> */
            if (__modifer_has(m, "r", "i", "inheritless", 0)) {
               struct context *ctx = __context();
               if (ctx)
                  set->sub_level = ctx->nest_level;
            }
         }
         if ((m = __next_modifer(m))) { /* <maximum_trace_depth> */
            if (__modifer_num(m, &t))
               set->maxdepth = (unsigned long) t;
         }
         break;
      case 'H':
         for (m = __next_modifer(f); m; m = __next_modifer(m)) {
            if (__modifer_has(m, "i", "index", 0))
               set->flags |= INDEX_ON;
            else if (__modifer_has(m, "s", "file", "source", 0))
               set->flags |= FILE_ON;
            else if (__modifer_has(m, "l", "line", 0))
               set->flags |= LINE_ON;
            else if (__modifer_has(m, "f", "func", "function", 0))
               set->flags |= FUNC_ON;
            else if (__modifer_has(m, "p", "pid", 0))
               set->flags |= PID_ON;
            else if (__modifer_has(m, "d", "depth", 0))
               set->flags |= DEPTH_ON;
         }
         break;
      case 'F':
         if ((m = __next_modifer(f))) {
            struct filter **pf = NULL;
            if (__modifer_has(m, "+s", "+file", "+source", 0)) {
               pf = &set->included_files;
            } else if (__modifer_has(m, "-s", "-file", "-source", 0)) {
               pf = &set->excluded_files;
            } else if (__modifer_has(m, "+f", "+func", "+function", 0)) {
               pf = &set->included_funcs;
            } else if (__modifer_has(m, "-f", "-func", "-function", 0)) {
               pf = &set->excluded_funcs;
            } else {
               break;
            }
            while ((m = __next_modifer(m))) {
               struct filter *fr;
               __modifer_str(m, temp, sizeof(temp));
               if (strlen(temp)) {
                  fr = (struct filter *) malloc(sizeof(struct filter) + strlen(temp));
                  if (fr) {
                     fr->pattern = fr->buffer;
                     strcpy(fr->pattern, temp);
                     fr->next = *pf;
                     *pf = fr;
                  }
               }
            }
         }
         break;
      }
   }

   _log_on_ = !!(set->flags & LOG_ON);
   _trace_on_ = !!(set->flags & TRACE_ON);
}

void _db_pop_(void)
{
   struct setting *discard;
   if (!init_done)
      _db_init_();

   discard = set;
   set = set->next;
   if (discard->included_funcs != NULL) {
      __freelist(discard->included_funcs);
   }
   if (discard->excluded_funcs != NULL) {
      __freelist(discard->excluded_funcs);
   }
   if (discard->included_files != NULL) {
      __freelist(discard->included_files);
   }
   if (discard->excluded_files != NULL) {
      __freelist(discard->excluded_files);
   }
   __closefile(discard);
   __closeudp(discard);
   __closepipe(discard);
   free((char *) discard);

   if (set) {
      _log_on_ = !!(set->flags & LOG_ON);
      _trace_on_ = !!(set->flags & TRACE_ON);
   } else {
      _log_on_ = 0;
      _trace_on_ = 0;
   }
}

long _db_enter_(_db_frame_ *frame, _db_stats_ *stats)
{
   struct context *ctx;
   long stackused = 0;
   int save_errno = errno;
   assert(frame);
   assert(stats);

   if (!init_done)
      _db_init_();
   if (!(ctx = __context()))
      return 0;

   frame->caller = ctx->frame;
   ctx->frame = frame;
   ctx->frame->level = ++ctx->nest_level;
   ctx->frame->clock = __cpuclock();
   stats->count++;
   if (!__tracable(ctx, ctx->frame->file, ctx->frame->func))
      return 0;

   __prefix(ctx, ctx->frame->file, ctx->frame->line);
   __indent(ctx, ctx->nest_level);

   if (((unsigned long) (ctx->frame->caller)) > ((unsigned long) (ctx->frame + 1))) {
      stackused = ((unsigned long) (ctx->frame->caller)) - ((unsigned long) (ctx->frame + 1));
   }

   ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], ">%s 0x%08lx/%ld *%lu", ctx->frame->func,
                          (unsigned long) ctx->frame, stackused, stats->count);

   if ((set->flags & STACK_ON) && (stackused > 0)) {
      ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "\n");
      ctx->bufpos += jeff_dump(0, &ctx->buffer[ctx->bufpos], ctx->frame + 1, stackused, 16,
                               JD_NO_BYTE | JD_NO_WORD | JD_NO_TYPE | JD_NO_INDEX | JD_DATA_PACK);
   }
   __output(ctx, 5 /*L_TRACE_*/);

   errno = save_errno;

   return 0;
}

long _db_return_(_db_stats_ *stats)
{
   struct context *ctx;
   assert(stats);

   if (!init_done)
      _db_init_();
   if (!(ctx = __context()))
      return 0;

   assert(ctx->nest_level == ctx->frame->level);

   if (__tracable(ctx, ctx->frame->file, ctx->frame->func)) {
      char retval[32];
      int spend, save_errno = errno;

      __prefix(ctx, ctx->frame->file, ctx->frame->line);
      __indent(ctx, ctx->nest_level);
      switch ((unsigned int) ctx->frame->retval) {
      case 0xB1B2B3B4:
         sprintf(retval, "????");
         break;
      case 0xB5B6B7B8:
         sprintf(retval, "void");
         break;
      case 0xB0B3B6B9:
         sprintf(retval, "none");
         break;
      default:
         sprintf(retval, "0x%08lx", ctx->frame->retval);
         break;
      }
      spend = __cpuclock() - ctx->frame->clock;
      stats->spent += spend / 1000.0;
      ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "<%s %s/%d %dms *%gs", ctx->frame->func, retval,
                             save_errno, spend, stats->spent);

      if (set->flags & STACK_ON) {
         long stackused = ((unsigned long) ctx->frame) - ((unsigned long) (&stats)) - 4;
         if (stackused > 0) {
            ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "\n");
            ctx->bufpos += jeff_dump(0, &ctx->buffer[ctx->bufpos], (void*) (((unsigned long) (&stats)) + 4),
                                     stackused, 16,
                                     JD_NO_BYTE | JD_NO_WORD | JD_NO_TYPE | JD_NO_INDEX | JD_DATA_PACK);
         }
      }

      __output(ctx, 5 /*L_TRACE_*/);
      errno = save_errno;
   }
   ctx->nest_level = ctx->frame->level - 1;
   if (ctx->frame)
      ctx->frame = ctx->frame->caller;

   return 0;
}

void _db_args_(const char *file, const char *func, const int line)
{
   struct context *ctx;
   assert(file);
   assert(func);
   assert(&line);

   if (!init_done)
      _db_init_();
   if (!(ctx = __context()))
      return;
   ctx->u_file = file;
   ctx->u_func = func;
   ctx->u_line = line;
}

void _db_print_(char *format, ...)
{
   int save_errno = errno;
   unsigned long level;
   va_list args;
   struct context *ctx;
   assert(format);

   if (!init_done)
      _db_init_();
   if (!(ctx = __context()))
      return;
   if (!__logable(ctx, ctx->u_file, ctx->u_func))
      return;

   if (strlen(format) < 3)
      return;
   if (format[0] != '<' || format[2] != '>')
      return;
   level = __levelnumb(format[1]);
   if (level > set->log_level)
      return;
   format += 3;

   va_start(args, format);

   __prefix(ctx, ctx->u_file, ctx->u_line);
   if (set->flags & TRACE_ON) {
      __indent(ctx, ctx->nest_level + 1);
   } else {
      if (set->flags & FUNC_ON)
         ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "%s ", ctx->u_func);
   }
   ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "[%c] ", __levelsymb(level));
   ctx->bufpos += vsprintf(&ctx->buffer[ctx->bufpos], format, args);
   va_end(args);
   __output(ctx, level);
   errno = save_errno;
}

void _db_dump_(const char *file, const char *func, const int line, char *memory, int length)
{
   struct context *ctx;
   assert(file);
   assert(func);
   assert(&line);
   assert(memory);
   assert(&length);

   if (!init_done)
      _db_init_();
   if (!(ctx = __context()))
      return;

   if (!__logable(ctx, file, func))
      return;
   __prefix(ctx, file, line);
   if (set->flags & TRACE_ON) {
      __indent(ctx, ctx->nest_level + 1);
   } else {
      if (set->flags & FUNC_ON)
         ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "%s: ", func);
   }
   ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "[%c]: Memory: 0x%08lx  Bytes: (%d)\n",
                          __levelsymb(3/*L_INFO_*/), (unsigned long) memory, length);

   ctx->bufpos
               += jeff_dump(0, &ctx->buffer[ctx->bufpos], (void*) memory, length, 16, JD_NO_WORD | JD_DATA_PACK);
   __output(ctx, 3 /*L_INFO_*/);
}

void _db_setec_(const char *file, const char *func, const int line, int ec)
{
   int idx;
   struct context *ctx;
   assert(file);
   assert(func);
   assert(&line);
   assert(&ec);

   if (!init_done)
      _db_init_();
   if (!(ctx = __context()))
      return;

   idx = ctx->ec_index++ % (sizeof(ctx->ec_stack) / sizeof(ctx->ec_stack[0]));
   ctx->ec_stack[idx].file = file;
   ctx->ec_stack[idx].func = func;
   ctx->ec_stack[idx].line = line;
   ctx->ec_stack[idx].code = ec;
}

void _db_getec_(const char *file, const char *func, const int line)
{
   struct context *ctx;
   int save_errno = errno;
   int i, idx, len;
   unsigned long level = 1;
   assert(file);
   assert(func);
   assert(&line);

   if (!init_done)
      _db_init_();
   if (!(ctx = __context()))
      return;

   if (!__logable(ctx, file, func))
      return;
   if (level > set->log_level)
      return;

   if (ctx->ec_index > (sizeof(ctx->ec_stack) / sizeof(ctx->ec_stack[0]))) { /*overlapped*/
      idx = ctx->ec_index;
      len = (sizeof(ctx->ec_stack) / sizeof(ctx->ec_stack[0]));
   } else {
      idx = 0;
      len = ctx->ec_index;
   }
   if (len > 0) {
      __prefix(ctx, file, line);
      if (set->flags & TRACE_ON) {
         __indent(ctx, ctx->nest_level + 1);
      } else {
         if (set->flags & FUNC_ON)
            ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "%s ", func);
      }
      ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "[%c] error code stack :", __levelsymb(level));
      for (i = 0; i < len; i++) {
         struct errcode *ec = &ctx->ec_stack[i % (sizeof(ctx->ec_stack) / sizeof(ctx->ec_stack[0]))];
         ctx->bufpos += sprintf(&ctx->buffer[ctx->bufpos], "\n(%03d) 0x%08x %d %s %s:%d", i + 1, ec->code,
                                ec->code, ec->func, __basename(ec->file), ec->line);
      }
      ctx->ec_index = 0;
      __output(ctx, level);
   }
   errno = save_errno;
}

void _db_setjmp_()
{
   struct context *ctx;
   if (!init_done)
      _db_init_();
   ctx = __context();

   ctx->jmplevel = ctx->nest_level;
}

void _db_longjmp_()
{
   struct context *ctx;
   if (!init_done)
      _db_init_();
   ctx = __context();

   ctx->nest_level = ctx->jmplevel;
}

#ifdef __cplusplus
}
#endif

