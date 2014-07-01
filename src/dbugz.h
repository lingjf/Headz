#ifndef __DBUG_H_6512d9f2_7f4e_4f07_aeb6_c230e0643fb4__
#define __DBUG_H_6512d9f2_7f4e_4f07_aeb6_c230e0643fb4__

/***************************************************************
 *   Memory to Fred Fish (November 4, 1952 - April 20, 2007)   *
 ***************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* Force a compilation error if condition is constant and true */
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1-2*!!(condition)]))

/*
 *
 *	The debug control string is a sequence of colon separated fields as follows:
 *		<field_1>;<field_2>;...;<field_N>
 *
 *	Each field consists of a mandatory flag character followed by
 *	an optional "," and comma separated list of modifers:
 *
 *		flag[,modifer,modifer,...,modifer]
 *
 *	The currently recognized flag characters are:
 *
 * O  Redirect the debugger output stream to the specified Observer.
 *    The default output is stdout. use "tail -f output.txt" to observe logs.
 *    O,<t>
 *    O,<f/a>,<file_path>,<file_size_limit>,<file_count_limit>,<flush_level>,<flush_delay>;
 *    O,<u>,<remote_ip_address>,<remote_ip_port>
 *    O,<p>,<pipe_name>,<pipe_mask> 
 *    t : Terminal. alias name: term, terminal, console
 *    f : Normal file. alias name: file, disk
 *    a : Normal file. Append to the firt file. alias name: append
 *    u : UDP network stream. alias name: udp, network
 *    p : Pipe. alias name: pipe, fifo
 *    file_path : you know, default dbugz.log
 *    file_size_limit : single log file will not exceed it. new log will write into new file.
 *    file_count_limit : total log files will be created. more log will overwrite the oldest file.
 *    flush_level : equal and higher log will trigger fflush.
 *    flush_delay : will sleep several milliseconds after fflush.
 *    remote_ip_address : U know , default is 127.0.0.1
 *    remote_ip_port : U know , default is 9527
 *    pipe_name : U known, default is dbugz.pipe 
 *    pipe_mask : Same as chmod
 *    i.e. -#O,f,/tmp/headz/app.log,2000000000,100,2,10;
 *    i.e. -#O,a,C:\\temp\\headz\\app.log,,,2;
 *    i.e. -#O,t
 *    i.e. -#O,u,127.0.0.1,9527
 *
 * L  Enable output from DBUG_LOG** for the current setting.
 *    Specifiy the output level, default is 9.
 *    L,<log_level>
 *    i.e. -#L,3; will output 0,1,2,3 level logs.
 *
 * T  Enable function call/exit trace lines.
 *    T,<dump_stack>,<inheritless>,<maximum_trace_depth>
 *    dump_stack : dump stack data when enter and return functions.
 *    inheritless : do not inherit the previous setting's function nesting level. Useful when the output is to start at the left margin.
 *    maximum_trace_depth : beyond which no output.
 *    i.e. -#T,s,r,3; 
 *    i.e. -#T,,r,3;
 *    i.e. -#T,,,3;
 *
 * H  Format header information.
 *    H,<i>,<s>,<l>,<f>,<p>,<d>
 *    i : Number each line of dbug output.
 *    s : Identify the source file name for each line of log or trace output.
 *    l : Identify the source file name for each line of debug or trace output.
 *    f : Function name for each line debug output.
 *    p : Identify the process with the pid(thread) for each line of debug or trace output.
 *    d : Print the current function nesting depth for each line of debug or trace output.
 *    i.e. H,i,s,l,p,d;
 *    i.e. H,s;
 *
 * F  Filter limit to the listed of source file name or named functions. 
 *    F,<+/-file>,<file_expr>,<file_expr>,<file_expr>,...,<file_expr>;
 *    F,<+/-func>,<function_expr>,<function_expr>,...,<function_expr>;
 *    + : including files/functions, null list will include ALL.
 *    - : excluding files/functions, null list will exclude NONE.
 *    file : source file name filter. alias name: source
 *    func : function file filter. alias name: function routine
 *    file_expr/function_expr : file/function expression, support *? regex
 *    i.e. -#F,+file,slabz_kalloc.c,rbtree*;
 *    i.e. -#F,-file,avl*.c; 
 *    i.e. -#F,+func,*create,??tree*; 
 *    i.e. -#F,-func,rbtree_create,rbtree_*free;		
 *    
 */

#define	L_FATAL_     "<0>"	/* system is unusable */
#define	L_ERROR_     "<1>"	/* error conditions */
#define	L_WARNING_   "<2>"	/* warning conditions */
#define	L_INFO_      "<3>"	/* informational */
#define	L_DEBUG_     "<4>"	/* debug level messages */
#define	L_TRACE_     "<5>"	/* trace level messages */
#define	L_LAST_      "<9>"	/* last level */

typedef struct _db_stats_
{
   unsigned long count;
   double spent;
} _db_stats_;

typedef struct _db_frame_
{
   const char *file;
   const char *func;
   unsigned long line;
   unsigned long retval;
   unsigned long clock;
   unsigned long level;
   struct _db_frame_ *caller;
} _db_frame_;

typedef struct _db_label_
{
   unsigned long feedcafe;
} _db_label_;

extern long _log_on_, _trace_on_, _no_db_;

extern void _db_push_(char*);
extern void _db_pop_(void);
extern long _db_enter_(_db_frame_*, _db_stats_*);
extern long _db_return_(_db_stats_*);
extern void _db_args_(const char*, const char*, const int);
extern void _db_print_(char*, ...);
extern void _db_dump_(const char*, const char*, const int, char*, int);
extern void _db_setec_(const char*, const char*, const int, int);
extern void _db_getec_(const char*, const char*, const int);
extern void _db_setjmp_(void);
extern void _db_longjmp_(void);
extern void _db_exit_(void);

/*VC6.0 SP6 and early versions : none __FUNCTION__*/
#if defined(_WIN32) && _MSC_VER <= 1200 
#define __FUNCTION__ "?func"
#endif

#if !defined(DBUG_COMPILE_TIME_OFF) && !defined(DBUG_COMPILE_TIME_TRACE_OFF)

#define DBUG_ENTER   \
   static _db_stats_  ___db_stats_ = {0,0};                                     \
   auto   _db_frame_  ___db_frame_ = {__FILE__,__FUNCTION__,__LINE__,0,0,0,0};  \
   auto   _db_label_  ___db_label_ = {(_trace_on_&&!_no_db_&&_db_enter_(&___db_frame_,&___db_stats_),0xFEEDCAFE)} 

#define _DBUG_LEAVE(a1)  \
   ((void)(((_trace_on_&&!_no_db_)?(                        \
               ___db_frame_.retval = (unsigned long)(a1),   \
               ___db_frame_.line = __LINE__,                \
               _db_return_(&___db_stats_)                   \
            ):___db_label_.feedcafe),0))

#define DBUG_RETURN  \
   for(_DBUG_LEAVE(0xB1B2B3B4); 1;) return

#if defined(__GNUC__)
#define DBUG_RETURN_DATA(a1)     \
   {                             \
      typeof(a1) __tmp = a1;     \
      _DBUG_LEAVE(__tmp);        \
      return(__tmp);             \
   }
#else
#define DBUG_RETURN_DATA(a1)     \
   {                             \
      unsigned long __tmp = a1;  \
      _DBUG_LEAVE(__tmp);        \
      return(__tmp);             \
   }
#endif

#define DBUG_RETURN_VOID         \
   {                             \
      _DBUG_LEAVE(0xB5B6B7B8);   \
      return;                    \
   }

#define DBUG_RETURN_NONE         \
   {                             \
      _DBUG_LEAVE(0xB0B3B6B9);   \
   }

#else

#define DBUG_ENTER {}
#define DBUG_RETURN return 
#define DBUG_RETURN_DATA return 
#define DBUG_RETURN_VOID return 
#define DBUG_RETURN_NONE

#endif

#if !defined(DBUG_COMPILE_TIME_OFF) && !defined(DBUG_COMPILE_TIME_LOG_OFF)

#define DBUG_LOG_OLD(parenthesed_argument_list) \
	do {                                               \
      if (_log_on_&&!_no_db_) {                       \
         _db_args_(__FILE__,__FUNCTION__,__LINE__);   \
         _db_print_ parenthesed_argument_list;        \
      }                                               \
   } while(0)

/* GCC or VS2005(8.0) and later versions */
#if defined(__GNUC__) || (defined(_WIN32) && _MSC_VER >= 1400)
#define DBUG_LOG(...)  \
   do {                                               \
      if (_log_on_&&!_no_db_) {                       \
         _db_args_(__FILE__,__FUNCTION__,__LINE__);   \
         _db_print_(__VA_ARGS__);                     \
      }                                               \
   } while(0)

#define DBUG_LOG_ONCE(...) \
   do {                                               \
      static int c = 0;                               \
      if ((_log_on_&&!_no_db_) && !c) {               \
         _db_args_(__FILE__,__FUNCTION__,__LINE__);   \
         _db_print_(__VA_ARGS__);                     \
         c = 1;                                       \
      }                                               \
   } while(0)

#define DBUG_LOG_MORE(n,...) \
   do {                                               \
      static int c = 0;                               \
      if ((_log_on_&&!_no_db_) && c < n) {            \
         _db_args_(__FILE__,__FUNCTION__,__LINE__);   \
         _db_print_(__VA_ARGS__);                     \
         ++c;                                         \
      }                                               \
   } while(0)

#define DBUG_LOG_LOOP(n,...) \
   do {                                               \
      static int c = 0;                               \
      if ((_log_on_&&!_no_db_) && !(c++ % n)) {       \
         _db_args_(__FILE__,__FUNCTION__,__LINE__);   \
         _db_print_(__VA_ARGS__);                     \
      }                                               \
   } while(0)   

#else

#define DBUG_LOG(x) {}  
#define DBUG_LOG_ONCE(x) {}
#define DBUG_LOG_MORE(x) {}
#define DBUG_LOG_LOOP(x) {}
#endif   

#define DBUG_DUMP(a1,a2) \
   do {                                                  \
      if (_log_on_&&!_no_db_) {                          \
         _db_dump_(__FILE__,__FUNCTION__,__LINE__,a1,a2);\
      }                                                  \
   } while(0) /* log level is INFO */

#define DBUG_EXECUTE(a1) \
	do {                             \
      if (_log_on_&&!_no_db_) {     \
         a1                         \
      }                             \
   } while(0)  

#define DBUG_SETEC(ec) \
   do {                                                     \
      if (_log_on_&&!_no_db_) {                             \
         _db_setec_(__FILE__,__FUNCTION__,__LINE__,ec);     \
      }                                                     \
   } while(0) 

#define DBUG_GETEC() \
   do {                                                     \
      if (_log_on_&&!_no_db_) {                             \
         _db_getec_(__FILE__,__FUNCTION__,__LINE__);        \
      }                                                     \
   } while(0)

#else

#define DBUG_LOG_OLD(x) {}

#if defined(__GNUC__) || (defined(_WIN32) && _MSC_VER >= 1400)
#define DBUG_LOG(...) {}
#define DBUG_LOG_ONCE(...) {}
#define DBUG_LOG_MORE(n,...) {}
#define DBUG_LOG_LOOP(n,...) {}
#endif
#define DBUG_DUMP(a1,a2) {}
#define DBUG_EXECUTE(a1) {}
#define DBUG_SETEC(ec) {}
#define DBUG_GETEC() {}
#endif

#if !defined(DBUG_COMPILE_TIME_OFF)

#define DBUG_PUSH(a1) _db_push_(a1)
#define DBUG_POP() _db_pop_()
#define DBUG_SETJMP(a1) (_db_setjmp_(), setjmp(a1))
#define DBUG_LONGJMP(a1,a2) (_db_longjmp_(), longjmp(a1, a2))
#define DBUG_CLOSE() _no_db_=1
#define DBUG_OPEN()  _no_db_=0
#define DBUG_EXIT() _db_exit_()

#else 

#define DBUG_PUSH(a1) {}
#define DBUG_POP() {} 
#define DBUG_SETJMP setjmp
#define DBUG_LONGJMP longjmp 
#define DBUG_CLOSE()
#define DBUG_OPEN()
#define DBUG_EXIT() 

#endif

#ifdef __cplusplus
}
#endif

#endif

