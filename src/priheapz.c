#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/*based on libpqueue : Generic priority queue (heap) implementation used by the Apache HTTP Server project. 2.2.14*/

#include "priheapz.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEFT(i)   ((i) << 1)
#define RIGHT(i)  (((i) << 1) + 1)
#define PARENT(i) ((i) >> 1)

void priheap_create(priheap_tree *tree, int n, priheap_pri *pri, priheap_cmp *cmp)
{
   assert(tree);
   assert(n > 0);
   assert(&pri || &cmp);
   tree->size = 1;
   tree->avail = n + 1;
   tree->pri = pri;
   tree->cmp = cmp;
   if (!(tree->d = (priheap_head**) malloc((n + 1) * sizeof(priheap_head*)))) {
      return;
   }
}
void priheap_destroy(priheap_tree *tree)
{
   assert(tree);
   if (tree) {
      if (tree->d)
         free(tree->d);
      tree->d = NULL;
   }
}

static void __bubble_up(priheap_tree *tree, int i)
{
   int p;
   priheap_head *node;
   unsigned long npri;
   assert(tree);
   assert(i > 0);

   node = tree->d[i];
   npri = tree->pri(node);

   for (p = PARENT(i); i > 1; i = p, p = PARENT(i)) {
      if (tree->cmp(npri, tree->pri(tree->d[p])) > 0) {
         tree->d[i] = tree->d[p];
         tree->d[i]->index = i;
      } else {
         break;
      }
   }

   tree->d[i] = node;
   tree->d[i]->index = i;
}

static int __maxchild(priheap_tree *tree, int i)
{
   int left = LEFT(i);
   int right = RIGHT(i);
   assert(tree);
   assert(i > 0);

   if (left >= tree->size)
      return 0;
   if (right >= tree->size)
      return left;

   if (tree->cmp(tree->pri(tree->d[left]), tree->pri(tree->d[right])) > 0)
      return left;
   else
      return right;
}

static void __sink_down(priheap_tree *tree, int i)
{
   int c;
   priheap_head *node;
   unsigned long npri;
   assert(tree);
   assert(i > 0);

   node = tree->d[i];
   npri = tree->pri(node);

   for (c = __maxchild(tree, i); c; i = c, c = __maxchild(tree, i)) {
      if (tree->cmp(tree->pri(tree->d[c]), npri) > 0) {
         tree->d[i] = tree->d[c];
         tree->d[i]->index = i;
      } else {
         break;
      }
   }

   tree->d[i] = node;
   tree->d[i]->index = i;
}

int priheap_insert(priheap_tree *tree, priheap_head *node)
{
   int i;
   assert(tree);
   assert(&node);

   /* allocate more memory if necessary */
   if (tree->size >= tree->avail) {
      priheap_head **tmp = (priheap_head**) realloc(tree->d, tree->avail * 2 * sizeof(priheap_head*));
      if (tmp == NULL)
         return 1;
      tree->d = tmp;
      tree->avail = tree->avail * 2;
   }

   /* insert item */
   i = tree->size++;
   tree->d[i] = node;
   __bubble_up(tree, i);

   return 0;
}

void priheap_update(priheap_tree *tree, priheap_head *node)
{
   int i, p;
   assert(tree && node);

   i = node->index;
   p = PARENT(i);
   if (p > 0 && tree->cmp(tree->pri(tree->d[i]), tree->pri(tree->d[p])) > 0)
      __bubble_up(tree, i);
   else
      __sink_down(tree, i);
}

int priheap_remove(priheap_tree *tree, priheap_head *node)
{
   int i;
   assert(tree && node);

   i = node->index;
   tree->d[i] = tree->d[--tree->size];
   if (tree->cmp(tree->pri(tree->d[i]), tree->pri(node)) >= 0)
      __bubble_up(tree, i);
   else
      __sink_down(tree, i);

   return 0;
}

priheap_head *priheap_pop(priheap_tree *tree)
{
   priheap_head *head;
   assert(tree);
   if (!tree || tree->size == 1)
      return NULL;

   head = tree->d[1];
   tree->d[1] = tree->d[--tree->size];
   __sink_down(tree, 1);

   return head;
}

priheap_head *priheap_peek(priheap_tree *tree)
{
   assert(tree);
   if (!tree || tree->size == 1)
      return NULL;
   return tree->d[1];
}

int priheap_leaf(priheap_tree *tree, priheap_head *node)
{
   assert(tree && node);
   if (tree && node)
      return LEFT(node->index) >= tree->size;
   return 0;
}

int priheap_count(priheap_tree *tree)
{
   assert(tree);
   if (tree)
      return tree->size - 1;
   return 0;
}

/*---------------------------------------------------------------*/

static void ____print_priheap(priheap_tree *tree, int k)
{
   static char linke[] = { '|', '/', '\\', '<' };
   static int depth = 0;
   static int nline = 1;
   static int bklen = 0;
   assert(&tree || &k);
   depth++;
   if (k < tree->size) {
      if (nline) {
         int i;
         printf("\r");
         for (i = 0; i < (depth - 1) * bklen; i++)
            printf(" ");
      }
      bklen = printf("%02X%c ", (int) tree->pri(tree->d[k]),
                     linke[((!!(LEFT(k) < tree->size)) << 1) | (!!(RIGHT(k) < tree->size))]);
      nline = 0;
      ____print_priheap(tree, RIGHT(k));
      ____print_priheap(tree, LEFT(k));
   } else {
      if (!nline)
         printf("\n");
      nline = 1;
   }
   depth--;
}

void ____helper__print_priheap(priheap_tree *tree)
{
   assert(&tree);
   printf("\n");
   ____print_priheap(tree, 1);
   printf("\n");
}

static int ____verify_priheap(priheap_tree *q, int k)
{
   assert(&q||&k);
   if (LEFT(k) < q->size) {
      /* has a LEFT child */
      if (q->cmp(q->pri(q->d[k]), q->pri(q->d[LEFT(k)])) < 0)
         return 0;
      if (!____verify_priheap(q, LEFT(k)))
         return 0;
   }
   if (RIGHT(k) < q->size) {
      /* has a RIGHT child */
      if (q->cmp(q->pri(q->d[k]), q->pri(q->d[RIGHT(k)])) < 0)
         return 0;
      if (!____verify_priheap(q, RIGHT(k)))
         return 0;
   }
   return 1;
}

int ____helper__verify_priheap(priheap_tree *tree)
{
   assert(&tree);
   return ____verify_priheap(tree, 1);
}

#ifdef __cplusplus
}
#endif

