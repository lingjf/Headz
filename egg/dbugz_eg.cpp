#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <signal.h>

#ifdef _WIN32
#include <windows.h> 
#else
#include <unistd.h>
#endif

#include "listz.h"
#include "bintreez.h"
#include "multreez.h"
#include "avltreez.h"
#include "rbtreez.h"
#include "rxtreez.h"
#include "sptreez.h"
#include "sbtreez.h"
#include "priheapz.h"
#include "treapz.h"
#include "hashz.h"
#include "slabz.h"
#include "slakz.h"
#include "jeffz.h"
#include "dbugz.h"




int func3(int c)
{
   DBUG_ENTER;
   int d = 6;
   d += c;
   DBUG_RETURN_DATA(d);
}

int func2(int a, int b)
{
   DBUG_ENTER;
   DBUG_RETURN_DATA(func3(a+b));
}

void func1()
{   
   DBUG_ENTER;
   int a = 3;
   a = func2(1,2);
   if (a == 0)
      DBUG_RETURN_VOID;
   func3(a);
   DBUG_RETURN_VOID; 
}

void eg_trace()
{    
   DBUG_ENTER;
   DBUG_PUSH("T,s;H,s,l;O,t;");

   func1();
   DBUG_POP();
   DBUG_RETURN_VOID;
}


void eg_log()
{ 
   DBUG_ENTER;
   int i;
   DBUG_PUSH("L,3;H,i,s,l,f;O,t");
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
      DBUG_SETEC(i);
   }
   DBUG_GETEC();
   DBUG_POP();
   DBUG_RETURN_VOID;
}

void eg_slabz()
{
   DBUG_ENTER;
   void * slab = NULL;
   void * rest = NULL;
   slab_initialize();

   slab = slab_create("my_struct", 66, 1, 10000, 0, 0.0, 0);
   rest = slab_kalloc(slab, 0);
   slab_kfree(slab, rest);
   slab_uninitialize();
   DBUG_RETURN_VOID;
}


#ifdef __cplusplus
class myclass {
public:
   myclass() {
      DBUG_ENTER;
      DBUG_LOG_OLD(("<4>myclass created."));
      DBUG_RETURN_NONE;
   };
   ~myclass() {
      DBUG_ENTER;
      DBUG_LOG_OLD(("<4>myclass deleted."));
      DBUG_RETURN_NONE;      
   }
   int myfunc() {
      DBUG_ENTER;
      DBUG_LOG_OLD(("<4>myclass function."));
      DBUG_RETURN_DATA(0);
   }
};

void eg_cplus()
{
   DBUG_ENTER;
   DBUG_PUSH("L;T,s;H,s,l;O,t");
      {
         class myclass mc;
         mc.myfunc();
      }
   DBUG_POP();
   DBUG_RETURN;
}
#endif


void eg_file()
{ 
   DBUG_ENTER;
   int i;
   DBUG_PUSH("L,3;T;H,i,s,l;O,f,../bin/hello.log,20000,2,1");
   DBUG_LOG_OLD(("<0>ntest begin excute %d", 0));
   DBUG_LOG_OLD(("<1>ntest begin excute %d", 1));
   DBUG_LOG_OLD(("<2>ntest begin excute %d", 2));
   DBUG_LOG_OLD(("<3>ntest begin excute %d", 3));
   DBUG_LOG_OLD(("<4>ntest begin excute %d", 4));
   DBUG_LOG_OLD(("<5>ntest begin excute %d", 5));

   for(i=0; i<100; i++) {
      func1();
      DBUG_LOG_OLD(("<1>ntest begin excute %d", i));
   }
   DBUG_POP();
   DBUG_RETURN_VOID;
}


int dbugz_eg_main()
{ 
   DBUG_PUSH("L,3;T;H,i,l,s;O,t");
   
   eg_log();  
   eg_trace();
   eg_file();
#ifdef __cplusplus
   eg_cplus();
#endif
   DBUG_PUSH("L,3;T,s,r;O,t;H,i,s,l;F,+file,slabz.c;F,+func,slab_kalloc,slab_kfree;F,-func,*free;");
   eg_slabz();
   DBUG_POP();
   
   DBUG_POP();

   DBUG_POP();
   return 0;
}



