#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

#include "listz.h"
#include "jeffz.h"
#include "utilz.h"
#include "rankz.h"


static int factorial[] =
{
	1,
	1,
	1*2,
	1*2*3,
	1*2*3*4,
	1*2*3*4*5,
	1*2*3*4*5*6,
	1*2*3*4*5*6*7,
	1*2*3*4*5*6*7*8,
	1*2*3*4*5*6*7*8*9,
	1*2*3*4*5*6*7*8*9*10,
	1*2*3*4*5*6*7*8*9*10*11,
	1*2*3*4*5*6*7*8*9*10*11*12,
};

int Pnm(int n, int m)
{  /*[ s = n*n-1*n-2*...*n-m+1 ]*/
   int i, s = 1;
   assert(n >= 0 || m >= 0);
   if (n < m)
      return 0;
   
   for (i=n; i>n-m; i--)
      s *= i;
   
   return s;
}

int Ann(int n)
{  /*[ s = n*n-1*...*1 ]*/
   assert(n >= 0);
   if (n>=0 && n<=12)
      return factorial[n];
   return Pnm(n, n);
}

int Cnm(int n, int m)
{  /*[ s = n*n-1*n-2*...*n-m+1 / m*m-1*...*1 ]*/
   int i, s = 1, d = 1;
   assert(n >= 0 || m >= 0);
   if (n < m)
      return 0;
   
   m = n-m >= m ? m : n-m;
   
   for (i=n; i>n-m; i--)
      s *= i;

   for (i=1; i<m+1; i++)
      d *= i;

   return s/d;
}

int seq_2_idx(int seq[], int n)
{
   int i, r = 0;
   assert(&seq);
   assert(n >= 0);
   if (n > 12)
      return -1;

   for (i = 0; i < n - 1; ++i) {
      int c = 0, j = i;
      while(++j<n)if(seq[i]>seq[j])c++;
      
      r += c * factorial[n-1-i];
   }
   return r;
}

int idx_2_seq(int seq[], int n, int idx)
{
   int   i, j;
   int   b[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
   assert(&seq);
   assert(n >= 0);
   assert(&idx);
   if (n > 12)
      return -1;
   	  
   for (i = 0; i < n; ++i) {
      int u = factorial[n-1-i];
      int c = idx/u;
      idx = idx-c*u;

      j = -1;
      do{j++;while(b[j]==-1)j++;}while(c--);
      seq[i] = b[j];
      b[j] = -1;
   }
   return 1;
}

void seq_gen(int seq[], int n, int x, void(*cb)(int*,int,int))
{
	int i;
   assert(&seq);
   assert(n >= 0);
   assert(&x);
   assert(&cb);
	if (x==n-1) {
		int idx = seq_2_idx(seq, n);
      cb(seq,n,idx);
		return;
	}
	for (i=0; i<n-x; i++) {
		int b[12];
		memcpy(b, seq, n*4);
		__swap(int, b[x], b[x+i]);
		seq_gen(b, n, x+1, cb);
	}
}

void seq_rand(int seq[], int n)
{
   int i;
   assert(&seq);
   assert(n >= 0);
   for (i=0; i<n; i++) {
      int r = rand()%n;
      __swap(int, seq[i], seq[r]);
   }
}


