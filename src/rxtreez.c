#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "rxtreez.h"

void rxtree_init(rxtree_node *head)
{
   assert(head);
   head->path = 0x0;
   head->mask = 0xFFFFFFFF;
   head->srt = 31;
   head->len = 32;
   head->usr = 0;
   head->son[0] = head->son[1] = 0;
}

static void __node_destroy(rxtree_node *p)
{
   assert(p);
   if (p->son[0])
      __node_destroy(p->son[0]);
   if (p->son[1])
      __node_destroy(p->son[1]);

   free(p);
}

void rxtree_destroy(rxtree_node *head)
{
   assert(head);
   if (head) {
      if (head->son[0])
         __node_destroy(head->son[0]);
      if (head->son[1])
         __node_destroy(head->son[1]);
   }
}

int __node_match(rxtree_node *p, unsigned long idx, int *diff)
{
   int i, _srt, _len;
   unsigned long _xor;
   assert(p);
   assert(&idx);
   assert(diff);
   _srt = p->srt;
   _len = p->len;
   _xor = (idx & p->mask) ^ p->path;
   if (!_xor) {
      *diff = _srt - _len;
      return 1;
   }
   for (i = 0; i < _len; i++) {
      if (_xor & (1 << (_srt - i))) {
         *diff = _srt - i;
         return 0;
      }
   }

   /*should never reach here*/
   while (1) {
      printf("BUG");
   }
   return 0;
}

void rxtree_insert(rxtree_node *head, unsigned long idx, void *usr)
{
   rxtree_node * p = head;
   assert(head);
   assert(&idx);
   assert(&usr);
   for (; p;) {
      int diff;
      if (__node_match(p, idx, &diff)) {
         if (diff < 0) {
            p->usr = usr;
            return;
         }
         p = p->son[(idx >> diff) & 0x1];
      } else {
         rxtree_node * k = (rxtree_node*) malloc(sizeof(rxtree_node));
         rxtree_node * n = (rxtree_node*) malloc(sizeof(rxtree_node));
         if (k && n) {
            memcpy(k, p, sizeof(rxtree_node));
            /*recalu k*/
            k->len -= p->srt - diff;
            k->srt = diff;
            /*recalu n*/
            n->path = idx;
            n->mask = 0xFFFFFFFF;
            n->srt = diff;
            n->len = diff + 1;
            n->usr = usr;
            n->son[0] = n->son[1] = 0;
            /*recalu p*/
            p->path = (p->path >> diff >> 1) << (diff + 1);
            p->mask = (p->mask >> diff >> 1) << (diff + 1);
            p->len = p->srt - diff;
            p->usr = 0;
            p->son[(idx >> diff) & 0x1] = n;
            p->son[!(idx >> diff) & 0x1] = k;
         }
         return;
      }
   }
}

static rxtree_node *__node_lookup(rxtree_node *head, unsigned long idx)
{
   rxtree_node * p = head;
   assert(head);
   assert(&idx);

   for (; p;) {
      int diff;
      if (__node_match(p, idx, &diff)) {
         if (diff < 0)
            return p;
         p = p->son[(idx >> diff) & 0x1];
      } else {
         return NULL;
      }
   }
   return NULL;
}

void *rxtree_lookup(rxtree_node *head, unsigned long idx)
{
   rxtree_node *p;
   assert(head);
   assert(&idx);
   p = __node_lookup(head, idx);
   return p ? p->usr : NULL;
}

void rxtree_remove(rxtree_node *head, unsigned long idx)
{
   int i, sp = -1;
   rxtree_node * s[32];
   rxtree_node * p = head;
   assert(head);
   assert(&idx);

   for (; p;) {
      int diff;
      if (__node_match(p, idx, &diff)) {
         s[++sp] = p;
         if (diff < 0)
            goto got;
         p = p->son[(idx >> diff) & 0x1];
      } else {
         return;
      }
   }
   return;

   got: if (idx == 0x0) {
      p->usr = 0;
      return;
   }

   for (i = sp; i >= 0; i--) {
      p = s[i];
      if (p->son[0] == NULL && p->son[1] == NULL) {
         rxtree_node *k = s[i - 1];
         k->son[0] == p ? (k->son[0] = NULL) : (k->son[1] = NULL);
         free(p);
      } else {
         if (p->son[0] == NULL || p->son[1] == NULL) {
            rxtree_node *k = p->son[0] ? p->son[0] : p->son[1];
            p->path = k->path;
            p->mask = k->mask;
            p->len += k->len;
            p->usr = k->usr;
            p->son[0] = k->son[0];
            p->son[1] = k->son[1];
            free(k);
         }
         break;
      }
   }
}

/*---------------------------------------------------------------*/

void ____print_rxtree(rxtree_node * p)
{
   int i;
   static char linke[] = { '|', '/', '\\', '<' };
   static int depth = 0;
   static int nline = 1;
   static int bklen = 0;
   assert(&p);
   depth++;
   if (p) {
      if (nline) {
         printf("\r");
         for (i = 0; i < (depth - 1) * bklen; i++)
            printf(" ");
      }
      bklen = printf("%08X:%02d%c ", (unsigned int) p->path, p->len/*, depth*/,
                     linke[((!!p->son[0]) << 1) | (!!p->son[1])]);
      nline = 0;
      ____print_rxtree(p->son[1]);
      ____print_rxtree(p->son[0]);
   } else {
      if (!nline)
         printf("\n");
      nline = 1;
   }
   depth--;
}

void ____helper__print_rxtree(rxtree_node * p)
{
   assert(&p);
   printf("\n");
   ____print_rxtree(p);
   printf("\n");
}

