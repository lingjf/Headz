#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "octreez.h"

#define null_head ((octree_head*)0)

void octree_create(octree_tree *tree, int width)
{
   assert(tree);
   assert(width > 0);

   tree->root = (octree_head*) malloc(sizeof(octree_head));
   if (tree->root) {
      tree->root->cup = null_head;
      tree->root->sub[0] = null_head;
      tree->root->sub[1] = null_head;
      tree->root->sub[2] = null_head;
      tree->root->sub[3] = null_head;
      tree->root->sub[4] = null_head;
      tree->root->sub[5] = null_head;
      tree->root->sub[6] = null_head;
      tree->root->sub[7] = null_head;
      list_init(&tree->root->payloads);
   }
   tree->width = width;
}

void octree_destroy(octree_tree *tree)
{
   assert(tree);
}

octree_head * octree_insert(octree_tree *tree, int i, int j, int k, list_head *payload)
{
   int x, y, z, a, b, c, l, r;
   octree_head *p;
   assert(tree);
   assert(&i || &j || &k);
   assert(payload);

   x = y = z = 0;
   l = tree->root->width;
   p = tree->root;
   for (; l > 1;) {
      r = l >> 1; /*r = l/2*/
      a = i >= x + r;
      b = j >= y + r;
      c = k >= z + r;
      p = p->sub[c << 2 | b << 1 | a];
      x += a ? r : 0;
      y += b ? r : 0;
      z += c ? r : 0;
      l = r;
   }
   return p;
}

