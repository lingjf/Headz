#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "multreez.h"

void multree_zero(multree_head *node)
{
   assert(&node);
   if (node == NULL)
      return;

   node->parent = node->child = NULL;
   list_init(&node->sibling);
}

int multree_add_child(multree_head *child, multree_head *parent)
{
   assert(&child || &parent);
   if (child == NULL || parent == NULL)
      return 0;

   child->parent = parent;
   if (parent->child) {
      list_add_prev(&child->sibling, &parent->child->sibling);
   } else {
      parent->child = child;
   }

   return 1;
}

int multree_add_sibling_left(multree_head *node, multree_head *sibling)
{
   assert(&node || &sibling);
   if (node == NULL || sibling == NULL)
      return 0;
   if (multree_is_root(sibling))
      return 0;

   node->parent = sibling->parent;
   list_add_prev(&node->sibling, &sibling->sibling);

   return 1;
}

int multree_add_sibling_right(multree_head *node, multree_head *sibling)
{
   assert(&node || &sibling);
   if (node == NULL || sibling == NULL)
      return 0;
   if (multree_is_root(sibling))
      return 0;

   node->parent = sibling->parent;
   list_add(&node->sibling, &sibling->sibling);

   return 1;
}

multree_head * multree_root(multree_head * node)
{
   assert(&node);
   while (node && node->parent)
      node = node->parent;

   return node;
}

int multree_depth(multree_head * node)
{
   int depth = 0;
   assert(&node);
   if (node == NULL)
      return -1;

   while (node && node->parent) {
      node = node->parent;
      depth++;
   }
   return depth;
}

void multree_del(multree_head * node)
{
   assert(&node);
   if (node == NULL)
      return;

   /* if parent's child pointer me, make it point to my sibling
    even if my do not have any sibling, make it point to NULL */

   if (!multree_is_root(node) && multree_is_first_child(node)) {
      if (multree_is_singleton(node))
         node->parent->child = NULL;
      else
         node->parent->child = multree_right_sibling(node);
   }

   node->parent = NULL;

   list_del(&node->sibling);
}

