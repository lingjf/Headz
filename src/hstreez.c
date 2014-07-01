#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <math.h>

#include "hstreez.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define snprintf _snprintf
#endif

#ifdef _WIN32
#define _llong_  __int64
#else
#define _llong_  long long int
#endif

#define null_head ((bintree_head*)0)

hstree_tree * hstree_create(hstree_tree *tree, int width, hstree_hash_key hashkey, hstree_hash_value hashvalue,
                            hstree_cmp_key cmpkey)
{
   assert(tree);
   assert(width == 2 || width == 4 || width == 16);
   assert(hashkey);
   assert(hashvalue);
   assert(cmpkey);

   switch (width) {
   case 2:
      tree->width = 2;
      tree->height = 32;
      break;
   case 4:
      tree->width = 4;
      tree->height = 16;
      break;
   case 16:
   default:
      tree->width = 16;
      tree->height = 8;
      break;
   }
   tree->hashkey = hashkey;
   tree->hashvalue = hashvalue;
   tree->cmpkey = cmpkey;

   tree->root.count = 0;
   tree->root.hash = 0;
   tree->root.childs = NULL;

   tree->nodes = 1;
   tree->items = 0;

   return tree;
}

static void __free_node(hstree_tree *tree, hstree_node *node, int depth)
{
   int i;
   assert(tree);
   assert(node);
   assert(depth >= 0);

   if (depth == tree->height) {
      return;
   }
   if (node->childs) {
      for (i = 0; i < tree->width; i++) {
         if (node->childs[i]) {
            __free_node(tree, node->childs[i], depth + 1);
            free(node->childs[i]);
         }
      }
   }
   return;
}

void hstree_destroy(hstree_tree *tree)
{
   assert(tree);

   __free_node(tree, &tree->root, 0);
}

static void __insert(hstree_tree *tree, hstree_node *node, int depth, unsigned long leftkey,
                     unsigned long rightkey, list_head *item, unsigned long keyhash)
{
   int range, offset;
   assert(tree);
   assert(node);
   assert(0 <= depth && depth <= 32);
   assert(&leftkey);
   assert(&rightkey);
   assert(item);
   assert(&keyhash);

   if (depth == tree->height) {
      if (node->count == 0)
         list_init(&node->heads);
      list_add(item, &node->heads);
      tree->items++;
      node->hash += tree->hashvalue(item);
      node->count++;
      return;
   }
   range = ((_llong_) rightkey - (_llong_) leftkey + 1) / tree->width;
   offset = (keyhash - leftkey) / range;
   //printf("%02d [%08x-%08x] %d  -- %d\n",depth, leftkey, rightkey, range, tree->width);
   if (!node->childs) {
      node->childs = (hstree_node **) malloc(sizeof(hstree_node *) * tree->width);
      if (!node->childs)
         return;
      memset(node->childs, 0, sizeof(hstree_node*) * tree->width);
   }
   if (!node->childs[offset]) {
      node->childs[offset] = (hstree_node *) malloc(sizeof(hstree_node));
      if (!node->childs[offset])
         return;
      memset(node->childs[offset], 0, sizeof(hstree_node));
      tree->nodes++;
   }
   node->hash -= node->childs[offset]->hash;
   __insert(tree, node->childs[offset], depth + 1, leftkey + range * offset,
            leftkey + range * offset + range - 1, item, keyhash);
   node->hash += node->childs[offset]->hash;
   node->count++;
   return;
}

void hstree_insert(hstree_tree *tree, list_head *item)
{
   unsigned long keyhash;
   assert(tree);
   assert(item);

   keyhash = tree->hashkey(item);
   __insert(tree, &tree->root, 0, 0x0, 0xffffffff, item, keyhash);
}

static void __remove(hstree_tree *tree, hstree_node *node, int depth, unsigned long leftkey,
                     unsigned long rightkey, list_head *item, unsigned long keyhash)
{
   int range, offset;
   assert(tree);
   assert(node);
   assert(0 <= depth && depth <= 32);
   assert(&leftkey);
   assert(&rightkey);
   assert(item);
   assert(&keyhash);

   if (depth == tree->height) {
      list_head *p, *deleted = NULL;
      list_for_each(p, &node->heads) {
         if (!tree->cmpkey(p, item)) {
            list_del(p);
            deleted = p;
            break;
         }
      }
      if (deleted) {
         tree->items--;
         node->hash -= tree->hashvalue(deleted);
         node->count--;
      }
      return;
   }
   range = ((_llong_) rightkey - (_llong_) leftkey + 1) / tree->width;
   offset = (keyhash - leftkey) / range;
   if (!node->childs || !node->childs[offset]) {
      return;
   }
   node->hash -= node->childs[offset]->hash;
   __remove(tree, node->childs[offset], depth + 1, leftkey + range * offset,
            leftkey + range * offset + range - 1, item, keyhash);
   node->hash += node->childs[offset]->hash;
   node->count--;

   if (node->childs[offset]->count == 0) {
      free(node->childs[offset]);
      node->childs[offset] = NULL;
      tree->nodes--;
   }
   return;
}

void hstree_remove(hstree_tree *tree, list_head *item)
{
   unsigned long keyhash;
   assert(tree);
   assert(item);

   keyhash = tree->hashkey(item);
   __remove(tree, &tree->root, 0, 0x0, 0xffffffff, item, keyhash);
}

static unsigned long __gethash(hstree_tree *tree, hstree_node *node, int depth, unsigned long leftkey,
                               unsigned long rightkey, unsigned long startkey, unsigned long endkey)
{
   int range, offset;
   assert(tree);
   assert(node);
   assert(0 <= depth && depth <= 32);
   assert(&leftkey);
   assert(&rightkey);
   assert(&startkey);
   assert(&endkey);

   if (leftkey == startkey && rightkey == endkey) {
      return node->hash;
   }
   if (rightkey < endkey) {
      return 0;
   }
   if (depth == tree->height) {
      return node->hash;
   }
   range = ((_llong_) rightkey - (_llong_) leftkey + 1) / tree->width;
   offset = (startkey - leftkey) / range;
   if (!node->childs || !node->childs[offset]) {
      return 0;
   }
   return __gethash(tree, node->childs[offset], depth + 1, leftkey + range * offset,
                    leftkey + range * offset + range - 1, startkey, endkey);
}

unsigned long hstree_gethash(hstree_tree *tree, unsigned long startkey, unsigned long endkey)
{
   assert(tree);
   assert(&startkey);
   assert(&endkey);
   return __gethash(tree, &tree->root, 0, 0x0, 0xffffffff, startkey, endkey);
}

static void __synchronize(hstree_tree *tree, hstree_node *node, int depth, unsigned long leftkey,
                          unsigned long rightkey, hstree_visitor visitor, void *args)
{
   int i, range;
   assert(tree);
   assert(node);
   assert(0 <= depth && depth <= 32);
   assert(&leftkey);
   assert(&rightkey);
   assert(visitor);
   assert(&args);

   if (depth == tree->height) {
      visitor(args, leftkey, rightkey, node->hash, &node->heads);
      return;
   }

   if (visitor(args, leftkey, rightkey, node->hash, NULL))
      return;

   range = ((_llong_) rightkey - (_llong_) leftkey + 1) / tree->width;
   if (!node->childs) {
      node->childs = (hstree_node **) malloc(sizeof(hstree_node *) * tree->width);
      if (!node->childs)
         return;
      memset(node->childs, 0, sizeof(hstree_node*) * tree->width);
   }
   for (i = 0; i < tree->width; i++) {
      if (!node->childs[i]) {
         node->childs[i] = (hstree_node *) malloc(sizeof(hstree_node));
         if (!node->childs[i])
            return;
         memset(node->childs[i], 0, sizeof(hstree_node));
         tree->nodes++;
         __synchronize(tree, node->childs[i], depth + 1, leftkey + range * i, leftkey + range * i + range - 1,
                       visitor, args);
      }
   }
}

void hstree_synchronize(hstree_tree *tree, hstree_visitor visitor, void *args)
{
   assert(tree);
   assert(visitor);
   assert(&args);

   __synchronize(tree, &tree->root, 0, 0x0, 0xffffffff, visitor, args);
}

static int __node_snprintf(char *i_buf, int size, hstree_tree *tree, hstree_node *node, int depth, int detail)
{
   int i, len = 0;
   assert(i_buf);
   assert(size >= 0);
   assert(tree);
   assert(node);
   assert(0 <= depth && depth <= 32);
   assert(&detail);

   len += snprintf(i_buf, size, ",%d-%lu-%X", depth, node->count, (unsigned int) node->hash);
   if (node->childs && depth < tree->height) {
      for (i = 0; i < tree->width; i++) {
         if (node->childs[i]) {
            len += __node_snprintf(i_buf + len, size - len, tree, node->childs[i], depth + 1, 9);
         }
      }
   }

   return len;
}

char *hstree_tostring(hstree_tree *tree, int detail)
{
   int len = 0;
   static char s_buf[1024 * 4];
   assert(tree);
   assert(&detail);

   if (detail <= 0) {
      return "";
   }
   len += snprintf(s_buf + len, sizeof(s_buf) - len, "%s", "");
   if (tree) {
      if (detail >= 1) {
         len += snprintf(s_buf + len, sizeof(s_buf) - len, "width=%d,height=%d", tree->width, tree->height);
      }
      if (detail >= 2) {
         len += snprintf(s_buf + len, sizeof(s_buf) - len, ",NI=%d,%d", tree->nodes, tree->items);
      }
      if (detail >= 3) {
         len += __node_snprintf(s_buf + len, sizeof(s_buf) - len, tree, &tree->root, 0, 9);
      }
   } else {
      len += snprintf(s_buf + len, sizeof(s_buf) - len, "-null-");
   }
   return s_buf;
}

#ifdef __cplusplus
}
#endif

