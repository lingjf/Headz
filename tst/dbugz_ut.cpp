#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h> 
#else
#include <unistd.h> 
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#endif

#include "jeffz.h"
#include "dbugz.h"
#include "pcre/pcre.h" 

#include "pcre/pcre.c"

extern "C" {
int __ptncmp(const char *pattern, const char *source);
}

#if 0
static struct pattern_set_t {
    const char *pattern;
    const char *string;
    const bool  result;
} p_set[] = {

/*   Pattern,  String to Test,          Matched  */
    {"", "test",                        false},
    {"", "*",                           false},
    {"test", "*",                       false},
    {"test", "test",                    true},

    {"tesT", "test",                    false},
    {"test", "Test",                    false},
    {"tEst", "teSt",                    false},

    {"?est", "test",                    true},
    {"te?t", "test",                    true},
    {"tes?", "test",                    true},
    {"test?", "test",                   false},

    {"*", "",                           true},
    {"*", "test",                       true},
    {"*test", "test",                   true},
    {"*est", "test",                    true},
    {"*st", "test",                     true},
    {"t*t", "test",                     true},
    {"te*t", "test",                    true},
    {"te*st", "test",                   true},
    {"te*", "test",                     true},
    {"tes*", "test",                    true},
    {"test*", "test",                   true},

    {".[\\-\\t]", ".t",                 true},
    {"test*?*[a-z]*", "testgoop",       true},
    {"te[^x]t", "test",                 true},
    {"te[^abc]t", "test",               true},
    {"te[^x]t", "test",                 true},
    {"te[!x]t", "test",                 true},
    {"te[^x]t", "text",                 false},
    {"te[^a-k]t", "text",               true},
    {"te[^\\x]t", "text",               false},
    {"te[^x\\", "text",                 false},
    {"te[/]t", "text",                  false},
    {"te[S]t", "test",                  false},
    {"te[r-t]t", "test",                true},
    {"te[r-t]t", "teSt",                false},
    {"te[r-T]t", "test",                false},
    {"te[R-T]t", "test",                false},
    {"te[r-Tz]t", "tezt",               true},
    {"te[R-T]t", "tent",                false},
    {"tes[]t]", "test",                 false},
    {"tes[t-]", "test",                 true},
    {"tes[t-]]", "test]",               true},
    {"tes[t-]]", "test",                false},
    {"tes[u-]", "test",                 false},
    {"tes[t-]", "tes[t-]",              false},
    {"tes[-y]", "test",                 true},
    {"tes[-y]]", "test]",               true},
    {"tes[-y]]", "test",                false},
    {"tes[-p]", "test",                 false},
    {"tes[-y]", "tes[t-]",              false},
    {"tes[-]", "test",                  true},
    {"t[ao-qc-g]st", "test",            true},

    {".*", ".this",                     true},
    {"*", ".this",                      true},
    {"?this", ".this",                  true},
    {"[.]this", ".this",                true},

    {"test/this", "test/this",          true},
    {"test?this", "test/this",          true},
    {"test*this", "test/this",          true},
    {"test[/]this", "test/this",        true},

    {"test/.*", "test/.this",           true},
    {"test/*", "test/.this",            true},
    {"test/?this", "test/.this",        true},
    {"test/[.]this", "test/.this",      true},

    {"test", "test",                    true}
};

#endif


#define OVECCOUNT 300    /* should be a multiple of 3 */
static   pcre* re;
static   char *error;
static   int erroffset;
static   int ovector[OVECCOUNT];

H2SUITE(dbugz)
{ 
   void setup()
   { 
      DBUG_OPEN();
   }

   void teardown()
   {
      DBUG_CLOSE();
      DBUG_EXIT();
   }  	  	
} ;
#if 0
H2CASE(dbugz, ptncmp)
{
   unsigned int i;
   for (i=0; i<sizeof(p_set)/sizeof(p_set[0]); i++) {
      H2EQUAL_INTEGER(p_set[i].result, 0 == __ptncmp(p_set[i].pattern, p_set[i].string));
   }
}
#endif
/*
//      http://perldoc.perl.org/perlre.html
H2CASE(dbugz, test_pcre)
{
   int rc;

   const char *regex = "^\\d{2,4}\\w*";
   char *data  = "123abc ok";

   re = pcre_compile(regex, PCRE_DOTALL | PCRE_CASELESS, &error, &erroffset);  
   H2CHECK(re != NULL);

   rc = pcre_exec(re, NULL, data, strlen(data), 0, ovector, OVECCOUNT);    
   H2EQUAL_INTEGER(1, rc);
}
*/


#define regex_file(regex, file, strict) __regex_file(regex, sizeof(regex)/sizeof(regex[0]), file, strict)
char *__regex_file(char *regex[], int num, char *file, int strict)
{
   int rc = 0;
   char buf[256];
   static char ret[256];

   FILE *fp = fopen(file, "r");
   if (fp == NULL) 
      return "Open file failed";
   for (int i = 0; i < num; i++) {
      if (!regex[i])
         continue;
      re = pcre_compile(regex[i], PCRE_DOTALL | PCRE_CASELESS, &error, &erroffset);  
      if (re == NULL) {
         sprintf(ret, "pattern failed : %s", regex[i]);
         break;
      }
      //rc = -1;
      while (fgets(buf, sizeof(buf), fp)) {
         rc = pcre_exec(re, NULL, buf, strlen(buf), 0, ovector, OVECCOUNT);
         if (rc == 1 || strict) break; 
      }
      //printf("%d @ %s == %s \n", rc, regex[i], buff);
      if (rc != 1) {
         sprintf(ret, "not matched : \n%s\n%s", regex[i], buf);
         break;
      }
   }
   fclose(fp);
   if (rc != 1)
      return ret;
   else
      return "OK";
}


void func_push_L()
{
   DBUG_LOG_OLD(("<0>hello world %d", 0));
   DBUG_LOG_OLD(("<1>hello world %d", 1));
   DBUG_LOG_OLD(("<2>hello world %d", 2));
   DBUG_LOG_OLD(("<3>hello world %d", 3));
   DBUG_LOG_OLD(("<4>hello world %d", 4));
   DBUG_LOG_OLD(("<5>hello world %d", 5));
   DBUG_LOG_OLD(("<6>hello world %d", 6));
   DBUG_LOG_OLD(("<7>hello world %d", 7));
   DBUG_LOG_OLD(("<8>hello world %d", 8));
   DBUG_LOG_OLD(("<9>hello world %d", 9));
}

//L,<log_level>
H2CASE(dbugz, push_L)
{
   DBUG_ENTER;
   
   remove("../bin/5130.txt"); 
   DBUG_PUSH("-#L,0;O,f,../bin/5130.txt;");
   func_push_L();
   DBUG_POP();
char * regex0[] = {
//""
"^$",
//2011-03-08 20:16:33.921 [F] hello world 0
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[F\\] hello world 0$"
};

   H2EQUAL_STRCMP("OK", regex_file(regex0, "../bin/5130.txt", 1)); 

////////////////////////////////////////////////////////////////////////////
   remove("../bin/5133.txt");
   DBUG_PUSH("-#L,i;O,f,../bin/5133.txt;");
   func_push_L();
   DBUG_POP();
char * regex3[] = {
//""
"^$",
//2011-03-08 20:16:33.921 [F] hello world 0
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[F\\] hello world 0$", 
//2011-03-08 20:16:33.921 [W] hello world 1
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[E\\] hello world 1$", 
//2011-03-08 20:16:33.921 [I] hello world 2  
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] hello world 2$" 
};

   H2EQUAL_STRCMP("OK", regex_file(regex3, "../bin/5133.txt", 1));

////////////////////////////////////////////////////////////////////////////
   remove("../bin/5139.txt");
   DBUG_PUSH("-#L,0x19;O,f,../bin/5139.txt;");
   func_push_L();
   DBUG_POP();
char * regex9[] = {
//""
"^$",
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[F\\] hello world 0$", 
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[E\\] hello world 1$", 
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] hello world 2$", 
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[I\\] hello world 3$", 
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[D\\] hello world 4$", 
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[T\\] hello world 5$",
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[6\\] hello world 6$", 
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[7\\] hello world 7$", 
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[8\\] hello world 8$",
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[9\\] hello world 9$"
};

   H2EQUAL_STRCMP("OK", regex_file(regex9, "../bin/5139.txt", 1));   

   DBUG_RETURN;
}

void fsub_push_T()
{
   DBUG_ENTER;
   DBUG_RETURN_VOID;
}
void func_push_T()
{
   DBUG_ENTER;
   fsub_push_T();
   DBUG_RETURN_VOID;
}
//T,<dump_stack>,<not_inherit_level>,<maximum_trace_level>
H2CASE(dbugz, push_T)
{
   DBUG_ENTER;
   
   remove("../bin/5230.txt"); 
   DBUG_PUSH("-#T,,r,1;O,f,../bin/5230.txt;");
   func_push_T();
   DBUG_POP();
char * regex0[] = {
//""
"^$",
#if defined(_WIN32) && _MSC_VER <= 1200
//2011-03-09 11:08:41.434 >func_push_T 0xfefff620/0 *1
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} >\\?func 0x[0-9 a-f]{8}/\\d+ \\*1$", 
//2011-03-09 11:08:41.435 <func_push_T void/2 0ms *0s
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} <\\?func void/\\d+ \\d+ms \\*[\\d \\.]+s$" 
#else
//2011-03-09 11:08:41.434 >func_push_T 0xfefff620/0 *1
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} >func_push_T 0x[0-9 a-f]{8}/\\d+ \\*1$", 
//2011-03-09 11:08:41.435 <func_push_T void/2 0ms *0s
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} <func_push_T void/\\d+ \\d+ms \\*[\\d \\.]+s$"
#endif
};

   H2EQUAL_STRCMP("OK", regex_file(regex0, "../bin/5230.txt", 1)); 

   DBUG_RETURN;
}

void func_push_H()
{
   DBUG_LOG_OLD(("<2>hello world %d", 0));
}

//H,<i>,<s>,<l>,<f>,<p>,<d>
H2CASE(dbugz, push_H)
{
   DBUG_ENTER;
   
   remove("../bin/5330.txt"); 
   DBUG_PUSH("-#H,i,s,l,p,f,d;L;O,f,../bin/5330.txt;");
   func_push_H();
   DBUG_POP();
char * regex0[] = {
//""
"^$",
//    1: 2011-03-09 11:38:43.471 7360 ugz_ut.cpp:323    0 ?func [W] hello world 0
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3}\\s*\\d+ \\w+.cpp:\\d+\\s*\\d+ .\\w+ \\[W\\] hello world 0$", 
};

   H2EQUAL_STRCMP("OK", regex_file(regex0, "../bin/5330.txt", 1)); 

   DBUG_RETURN;
}

void func_push_O()
{
   int i;
   for (i = 0; i<5; i++)
      DBUG_LOG_OLD(("<2>push_O %d", i));
}

//O,<f>,<file_path>,<file_size_limit>,<file_count_limit>,<flush_level>,<flush_delay>;
H2CASE(dbugz, push_OF)
{
   DBUG_ENTER;
   
   remove("../bin/5430.txt.1"); 
   remove("../bin/5430.txt.2"); 
   remove("../bin/5430.txt.3"); 
   DBUG_PUSH("-#L;O,f,../bin/5430.txt,10,3;");
   func_push_O();
   DBUG_POP();
char * regex0[] = {
//""
"^$",
//2011-03-09 11:53:51.059 [W] push_O 0
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] push_O 3$", 
};
char * regex1[] = {
//""
"^$",
//2011-03-09 11:53:51.059 [W] push_O 0
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] push_O 4$", 
};
char * regex2[] = {
//""
"^$",
//2011-03-09 11:53:51.059 [W] push_O 0
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] push_O 2$", 
};

   H2EQUAL_STRCMP("OK", regex_file(regex0, "../bin/5430.txt.1", 1)); 
   H2EQUAL_STRCMP("OK", regex_file(regex1, "../bin/5430.txt.2", 1));  
   H2EQUAL_STRCMP("OK", regex_file(regex2, "../bin/5430.txt.3", 1));    

   DBUG_RETURN;
}

//O,<a>,<file_path>,<file_size_limit>,<file_count_limit>,<flush_level>,<flush_delay>;
H2CASE(dbugz, push_OA)
{
   DBUG_ENTER;
   
   remove("../bin/5430.txt.1"); 
   remove("../bin/5430.txt.2"); 
   remove("../bin/5430.txt.3"); 
   DBUG_PUSH("-#L;O,a,../bin/5430.txt,10,3;");
   func_push_O();
   DBUG_POP();
char * regex0[] = {
//""
"^$",
//2011-03-09 11:53:51.059 [W] push_O 0
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] push_O 0$",
};
char * regex1[] = {
//""
"^$",
//2011-03-09 11:53:51.059 [W] push_O 0
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] push_O 3$", 
};
char * regex2[] = {
//""
"^$",
//2011-03-09 11:53:51.059 [W] push_O 0
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] push_O 4$", 
};

   H2EQUAL_STRCMP("OK", regex_file(regex0, "../bin/5430.txt.1", 1)); 
   H2EQUAL_STRCMP("OK", regex_file(regex1, "../bin/5430.txt.2", 1));  
   H2EQUAL_STRCMP("OK", regex_file(regex2, "../bin/5430.txt.3", 1));    

   DBUG_RETURN;
}

#if 0
//O,<p>,<pipe_name>; testless
H2CASE(dbugz, push_OT)
{
   DBUG_ENTER;
   
   DBUG_PUSH("-#L;O,p,/tmp/dbugz.pp,0644;");
   for (int i=0; i<100000; i++) {
      func_push_O();
#ifdef _WIN32
    Sleep(1000);
#else
    usleep(1000);
#endif
   }
   DBUG_POP();

   DBUG_RETURN;
}
#endif

//O,<u>;
H2CASE(dbugz, push_OU)
{
   DBUG_ENTER;

   char buf[1024];
   struct sockaddr_in from, in;
   int n, len;
   
#ifdef _WIN32
   SOCKET s;
   int fl;
#else
   int s;
   socklen_t fl;
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif

   remove("../bin/7570.txt");
   FILE *fp = fopen("../bin/7570.txt", "w");
   H2CHECK(fp != NULL);

#ifdef _WIN32
   WORD wVersionRequested;
   WSADATA wsaData;
   wVersionRequested = MAKEWORD(2, 2); 
   H2EQUAL_INTEGER(0, WSAStartup(wVersionRequested, &wsaData));/* Tell the user that we could not find a usable Winsock DLL. */ 
#endif

   s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   H2CHECK(INVALID_SOCKET != s);
   in.sin_family = AF_INET;
   in.sin_port = htons(5527);
   in.sin_addr.s_addr = INADDR_ANY;
   bind(s, (struct sockaddr *)&in, sizeof(in));

   DBUG_PUSH("-#L;O,u,,5527;");
   func_push_O();
   DBUG_POP();

   for (int i=0; i<5; i++) {
      fl = sizeof(struct sockaddr_in);
      memset(&from, 0, sizeof(from));
      n = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&from, &fl);
      if(n <= 0) {
         break;
      }
      len = fwrite(buf, 1, n, fp);
      H2EQUAL_INTEGER(n, len);
   }
   fclose(fp);
#ifdef _WIN32
   closesocket(s);
   WSACleanup();
#else
   close(s); 
#endif

char * regex0[] = {
//""
"^$",
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] push_O 0$", 
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] push_O 1$",
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] push_O 2$", 
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] push_O 3$", 
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] push_O 4$"
};

   H2EQUAL_STRCMP("OK", regex_file(regex0, "../bin/7570.txt", 1)); 

   DBUG_RETURN;
}

void fsub_push_F()
{
   DBUG_LOG_OLD(("<2>fsub_F %d", 0));
}

void func_push_F()
{
   DBUG_LOG_OLD(("<2>push_F %d", 0));
   fsub_push_F();
}

//F,<+/-file>,<file_expr>,<file_expr>,<file_expr>,...,<file_expr>;
//F,<+/-func>,<function_expr>,<function_expr>,...,<function_expr>;
H2CASE(dbugz, push_F)
{
   DBUG_ENTER;
   
   remove("../bin/5430.txt"); 
   DBUG_PUSH("-#F,+file,dbugz_*.cpp;F,-func,fsub_?ush_F,abcd_r;L;O,f,../bin/5430.txt;");
   func_push_F();
   DBUG_POP();
char * regex0[] = {
//""
"^$",
//2011-03-09 14:45:29.132 [W] push_F 0
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] push_F 0$", 
//2011-03-09 14:45:29.132 [W] fsub_F 0
#if defined(_WIN32) && _MSC_VER <= 1200
"^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\[W\\] fsub_F 0$"
#endif
};

   H2EQUAL_STRCMP("OK", regex_file(regex0, "../bin/5430.txt", 1)); 

   DBUG_RETURN;
} 

void func_t2(int b)
{
   DBUG_ENTER;
   int c;
   c = b + 1;
   DBUG_RETURN_VOID;
}

int func_t1(int a)
{
   DBUG_ENTER;
   func_t2(a);
   DBUG_RETURN_DATA(2);
}


H2CASE(dbugz, trace_file)
{
   DBUG_ENTER;
   int result;
   remove("../bin/6567.txt");
   DBUG_PUSH("L,3;T,s;H,i,s,l,f,d;O,f,../bin/6567.txt;");
   result = func_t1(3);
   DBUG_POP();


char * regex [] = {
//""
"^$",
//"    1: 2011-03-06 21:33:58.917 ugz_ut.cpp:111    1 >func_t1 0xbfffd7b0/0 *1"
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+    1 >.\\w+ 0x[0-9 a-f]{8}/0 \\*1$",
//"    2: 2011-03-06 21:33:58.920 ugz_ut.cpp:103    2 | >func_t2 0xbfffd760/52 *1"
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+    2 \\| >.\\w+ 0x[0-9 a-f]{8}/\\d+ \\*1$",
//"0xbfffd77ch: 084FA9B4 0812885C BFFFD7CC BFFFD7D8 | ..O.\..........."
"^0x[0-9 a-f]{8}h: [0-9 A-F]{8} [0-9 A-F]{8} [0-9 A-F]{8} [0-9 A-F]{8} \\| ................$",
//"0xbfffd78ch: 081033CB 00000003 0815BE7C 08128966 | .3......|...f..."
"^0x[0-9 a-f]{8}h: [0-9 A-F]{8} [0-9 A-F]{8} [0-9 A-F]{8} [0-9 A-F]{8} \\| ................$", 
//"0xbfffd79ch: 0100007F 42021279 084FA844 BFFFD7D8 | ....y..BD.O....."
"^0x[0-9 a-f]{8}h: [0-9 A-F]{8} [0-9 A-F]{8} [0-9 A-F]{8} [0-9 A-F]{8} \\| ................$", 
//"0xbfffd7ach: FEEDCAFE                            | ....            "
"^0x[0-9 a-f]{8}h:[\\s 0-9 A-F]+FEEDCAFE\\s+\\| ................$",
//""
"^$", 
//"    3: 2011-03-06 21:33:58.920 ugz_ut.cpp:106    2 | <func_t2 void/2 0ms *0s"
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+    2 \\| <.\\w+ void/\\d+ \\d+ms \\*[\\d \\.]+s$",
//"0xbfffd744h: 0815BE70 00000005 4212EE20 084FBA13 | p....... ..B..O."
"^0x[0-9 a-f]{8}h: [0-9 a-f]{8} [0-9 a-f]{8} [0-9 a-f]{8} [0-9 a-f]{8} \\| ................$", 
//"0xbfffd754h: 08119EAC 00000004 FEEDCAFE          | ............    "
"^0x[0-9 a-f]{8}h:[\\s 0-9 A-F]+FEEDCAFE\\s+\\| ................$", 
//""
"^$", 
//"    4: 2011-03-06 21:33:58.921 ugz_ut.cpp:113    1 <func_t1 0x00000002/2 0ms *0s"
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+    1 <.\\w+ 0x00000002/\\d+ \\d+ms \\*[\\d \\.]+s$", 
//"0xbfffd794h: 0815BE7C 08128966 0100007F 42021279 | |...f.......y..B"
"^0x[0-9 a-f]{8}h: [0-9 a-f]{8} [0-9 a-f]{8} [0-9 a-f]{8} [0-9 a-f]{8} \\| ................$", 
//"0xbfffd7a4h: 084FA844 00000002 FEEDCAFE          | D.O.........    "
"^0x[0-9 a-f]{8}h:[\\s 0-9 A-F]+FEEDCAFE\\s+\\| ................$",
};


   H2EQUAL_STRCMP("OK", regex_file(regex, "../bin/6567.txt", 0));
   DBUG_RETURN;
}


void func_log()
{ 
   int i;
   DBUG_LOG_OLD(("<0>ntest begin excute %d", 0));
   DBUG_LOG_OLD(("<1>ntest begin excute %d", 1));
   DBUG_LOG_OLD(("<2>ntest begin excute %d", 2));
   DBUG_LOG_OLD(("<3>ntest begin excute %d", 3));
   DBUG_LOG_OLD(("<4>ntest begin excute %d", 4));
   DBUG_LOG_OLD(("<5>ntest begin excute %d", 5));

   for( i=0; i<10; i++) {
#if defined(__GNUC__) || (defined(_WIN32) && _MSC_VER >= 1400)
      DBUG_LOG("<0>log i=%d ever", i);
      DBUG_LOG_ONCE("<0>log i=%d once", i);
      DBUG_LOG_MORE(2, "<0>log i=%d twice", i);
      DBUG_LOG_LOOP(3, "<0>log i=%d 1/3", i);
#endif  
   }
}

H2CASE(dbugz, log_file)
{
   DBUG_ENTER;
   remove("../bin/6539.txt");
   DBUG_PUSH("L,3;T,s;H,i,s,l,f;O,f,../bin/6539.txt;");
   func_log();
   DBUG_POP();
   
char * regex [] = {
//""
"^$",
//    1: 2011-03-06 23:19:10.263 ugz_ut.cpp:216 [F] ntest begin excute 0
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] ntest begin excute 0$", 
//    2: 2011-03-06 23:19:10.267 ugz_ut.cpp:217 [E] ntest begin excute 1
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[E\\] ntest begin excute 1$", 
//    3: 2011-03-06 23:19:10.270 ugz_ut.cpp:218 [W] ntest begin excute 2
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[W\\] ntest begin excute 2$", 
//    4: 2011-03-06 23:19:10.270 ugz_ut.cpp:219 [I] ntest begin excute 3
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[I\\] ntest begin excute 3$", 
//    5: 2011-03-06 23:19:10.271 ugz_ut.cpp:225 [F] log i=0 ever
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=0 ever$", 
//    6: 2011-03-06 23:19:10.271 ugz_ut.cpp:226 [F] log i=0 once
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=0 once$", 
//    7: 2011-03-06 23:19:10.271 ugz_ut.cpp:227 [F] log i=0 twice
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=0 twice$", 
//    8: 2011-03-06 23:19:10.271 ugz_ut.cpp:228 [F] log i=0 1/3
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=0 1/3$", 
//    9: 2011-03-06 23:19:10.272 ugz_ut.cpp:225 [F] log i=1 ever
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=1 ever$", 
//   10: 2011-03-06 23:19:10.272 ugz_ut.cpp:227 [F] log i=1 twice
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=1 twice$", 
//   11: 2011-03-06 23:19:10.272 ugz_ut.cpp:225 [F] log i=2 ever
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=2 ever$", 
//   12: 2011-03-06 23:19:10.272 ugz_ut.cpp:225 [F] log i=3 ever
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=3 ever$", 
//   13: 2011-03-06 23:19:10.273 ugz_ut.cpp:228 [F] log i=3 1/3
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=3 1/3$", 
//   14: 2011-03-06 23:19:10.273 ugz_ut.cpp:225 [F] log i=4 ever
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=4 ever$", 
//   15: 2011-03-06 23:19:10.273 ugz_ut.cpp:225 [F] log i=5 ever
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=5 ever$", 
//   16: 2011-03-06 23:19:10.273 ugz_ut.cpp:225 [F] log i=6 ever
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=6 ever$", 
//   17: 2011-03-06 23:19:10.274 ugz_ut.cpp:228 [F] log i=6 1/3
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=6 1/3$", 
//   18: 2011-03-06 23:19:10.274 ugz_ut.cpp:225 [F] log i=7 ever
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=7 ever$", 
//   19: 2011-03-06 23:19:10.274 ugz_ut.cpp:225 [F] log i=8 ever
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=8 ever$", 
//   20: 2011-03-06 23:19:10.274 ugz_ut.cpp:225 [F] log i=9 ever
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=9 ever$", 
//   21: 2011-03-06 23:19:10.280 ugz_ut.cpp:228 [F] log i=9 1/3   
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ \\[F\\] log i=9 1/3$" 
};

   H2EQUAL_STRCMP("OK", regex_file(regex, "../bin/6539.txt", 1));
   DBUG_RETURN;
}

void func_ec()
{ 
   DBUG_SETEC(-1);
   DBUG_SETEC(-2);
   DBUG_SETEC(-3);


   DBUG_GETEC();
}

H2CASE(dbugz, ec_file)
{
   DBUG_ENTER;
   remove("../bin/6541.txt");
   DBUG_PUSH("L,9;H,i,s,l,f;O,f,../bin/6541.txt;");
   func_ec();
   DBUG_POP();

char * regex [] = {
//""
"^$",
//    1: 2011-03-07 11:16:15.519 ugz_ut.cpp:300 func_ec [E] error code stack :
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ .\\w+ \\[E\\] error code stack :$", 
//(001) 0xffffffff -1 func_ec dbugz_ut.cpp:295
"^\\(001\\) 0xffffffff -1 .\\w+ dbugz_ut\\.cpp:\\d+$",
//(002) 0xfffffffe -2 func_ec dbugz_ut.cpp:296
"^\\(002\\) 0xfffffffe -2 .\\w+ dbugz_ut\\.cpp:\\d+$",
//(003) 0xfffffffd -3 func_ec dbugz_ut.cpp:297   
"^\\(003\\) 0xfffffffd -3 .\\w+ dbugz_ut\\.cpp:\\d+$"
};

   H2EQUAL_STRCMP("OK", regex_file(regex, "../bin/6541.txt", 1));
   DBUG_RETURN;
}

void func_dump()
{ 
   unsigned long bu[] = {
      0x01020304, 0xabababab, 0x80808080, 0xffffffff,
      0xcccceeee, 0x12345678
   };

   DBUG_DUMP((char*)bu, sizeof(bu));
}

H2CASE(dbugz, dump_file)
{
   DBUG_ENTER;
   remove("../bin/6558.txt");
   DBUG_PUSH("L,9;H,i,s,l,f;O,f,../bin/6558.txt;");
   func_dump();
   DBUG_POP();
   
char * regex [] = {
//""
"^$",
//    1: 2011-03-07 12:46:01.211 ugz_ut.cpp:335 func_dump: [3]: Memory: 0x0022c9c0  Bytes: (24)
"^\\s*\\d+: \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\w+.cpp:\\d+ .\\w+: \\[I\\]: Memory: 0x[0-9 a-f]{8}  Bytes: \\(24\\)$", 
//                         dword(hex)              |                    byte(hex)                    |       char      
"^                         dword\\(hex\\)              \\|                    byte\\(hex\\)                    \\|       char      $",
//              3 2 1 0  7 6 5 4  b a 9 8  f e d c |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef
"^              3 2 1 0  7 6 5 4  b a 9 8  f e d c \\|  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f \\| 0123456789abcdef$",
//0x0022c9c0h: 01020304 ABABABAB 80808080 FFFFFFFF | 04 03 02 01 AB AB AB AB 80 80 80 80 FF FF FF FF | ................
"^0x[0-9 a-f]{8}h: 01020304 ABABABAB 80808080 FFFFFFFF \\| 04 03 02 01 AB AB AB AB 80 80 80 80 FF FF FF FF \\| ................$",
//0x0022c9d0h: CCCCEEEE 12345678                   | EE EE CC CC 78 56 34 12                         | ....xV4.        
"^0x[0-9 a-f]{8}h: CCCCEEEE 12345678                   \\| EE EE CC CC 78 56 34 12                         \\| ....xV4.        $",
};

   H2EQUAL_STRCMP("OK", regex_file(regex, "../bin/6558.txt", 1));
   DBUG_RETURN;
}

