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

char g_buffer[10000];

void eg_jeffz()
{
   unsigned int i;
   FILE * fp = stdout;
   char *mymemory, mybuffer[256];
  	mymemory = (char*)__ALIGN32((int)&mybuffer);
   strcpy(mymemory, "abcdefghijklmnopqrstuvwxyz0123456789");

   for (i=0; i<sizeof(g_buffer); i++)   	
      g_buffer[i] = rand()%256;

   jeff_dump(fp, 0, g_buffer, sizeof(g_buffer), 8, JD_BYTE_BIN | JD_WORD_DEC | JD_DATA_SIGNED);

   jeff_dump(fp, g_buffer, mymemory, strlen(mymemory)+1, 16, 0);
   jeff_dump(fp, g_buffer, mymemory, strlen(mymemory)+1, 16, JD_NO_DWORD | JD_DATA_LOWER | JD_ADDR_DEC);
   jeff_dump(fp, g_buffer, mymemory, strlen(mymemory)+1, 10, 0);
   jeff_dump(fp, g_buffer, mymemory, strlen(mymemory)+1, 21, 0);
   jeff_dump(fp, g_buffer, mymemory+7, strlen(mymemory)+1-7, 16, 0);
   jeff_dump(fp, g_buffer, mymemory+7, strlen(mymemory)+1-7, 16, JD_DATA_PACK);
   jeff_dump(fp, g_buffer, mymemory, strlen(mymemory)+1, 1, 0);
   jeff_dump(fp, g_buffer, mymemory, strlen(mymemory)+1, 2, 0);
   jeff_dump(fp, g_buffer, mymemory, strlen(mymemory)+1, 3, 0);
   jeff_dump(fp, g_buffer, mymemory, strlen(mymemory)+1, 4, 0); 
   jeff_dump(fp, g_buffer, mymemory, strlen(mymemory)+1, 5, 0);
   jeff_dump(fp, g_buffer, mymemory, strlen(mymemory)+1, 6, 0);
   jeff_dump(fp, g_buffer, mymemory, strlen(mymemory)+1, 7, 0);
   jeff_dump(fp, g_buffer, mymemory, strlen(mymemory)+1, 8, JD_BYTE_DEC | JD_DATA_SIGNED); 
   jeff_dump(fp, g_buffer, mymemory, strlen(mymemory)+1, 8, JD_ADDR_REL | JD_BYTE_BIN );
}

int jeffz_eg_main()
{ 
   eg_jeffz();
   return 0;
}



