#ifndef __OCTREEZ_H_e2c20cc7_069f_468e_8d8c_e75b6fc8a7cf__
#define __OCTREEZ_H_e2c20cc7_069f_468e_8d8c_e75b6fc8a7cf__

#ifdef __cplusplus
extern "C" {
#endif

/**
http://en.wikipedia.org/wiki/Quadtree


  O|--------------------------------->x
   |           |          |
   |           |          |   
   |    00     |    01    |
   |           |          |
   |           |          |
   |-----------|----------|
   |           |     |    |
   |           |     |    |
   |    10     |----11----|
   |           |     |    |
   |           |     |    |
   |----------------------|
   |
   V
   y


http://en.wikipedia.org/wiki/Octree


                O/---------/---------/-------------->x
                /. 000    /   001   /|
               / .       /         / |
              /---------/---------/  |
             /  010    /   011   /|  |
            /    .    /         / | /|
           /---------/---------/  |/ |
          /|     .   |         |  /  |
         / |     .   |         | /|101
        /  |    (100)|         |/ | /
       /   |---------|---------|  |/
      /    |     .   |         |  /
     y     |   110   |   111   | /
           |     .   |         |/
           |-------------------|
                 |
                 |
                 V
                 z

*/

#include "listz.h"

typedef struct octree_head {
   struct octree_head *cup;
	struct octree_head *sub[8];
   int width;
   list_head payloads;
}octree_head;


typedef unsigned long (bintree_key)(octree_head *node);
typedef int (bintree_cmp)(unsigned long a, unsigned long b);

typedef struct octree_tree {
  octree_head *root;
  int width;
}octree_tree;


void octree_create(octree_tree *tree, int width);
void octree_destroy(octree_tree *tree);

#if 0


octree_head * bintree_insert(octree_tree *tree, octree_head *node);
octree_head * bintree_remove(octree_tree *tree, octree_head *node);
octree_head * bintree_lookup(octree_tree *tree, unsigned long key);

octree_head * bintree_min(octree_tree *tree);
octree_head * bintree_max(octree_tree *tree);
octree_head * bintree_inc(octree_head *node);
octree_head * bintree_dec(octree_head *node);


octree_head * bintree_root(octree_head *node);
int            bintree_empty(octree_tree *tree);
int            bintree_leaf(octree_head *node);
int            bintree_count(octree_head *node);



#endif

#ifdef __cplusplus
}
#endif

#endif

