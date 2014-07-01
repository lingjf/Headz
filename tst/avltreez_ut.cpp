#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "listz.h"
#include "avltreez.h"

extern "C" {
void ____helper__print_avltree(avltree_tree * tree);
int ____helper__verify_avltree(avltree_tree * tree);
} 


typedef struct avltree_body_t{
   avltree_head avl;int value;
   struct avltree_body_t *next;
}avltree_body_t;

unsigned long avltree_body_key(avltree_head * x) 
{
   avltree_body_t * body;
   body = list_entry(x, avltree_body_t, avl);
   return body->value;
}

int avltree_body_cmp(unsigned long x, unsigned long y)
{
   return (int)x-(int)y;
}

static char g_buffer[1024 * 4];

static char* __pass_avltree(avltree_head * p)
{
   if (p == NULL)
      return g_buffer;
   
   if (p->parent == NULL)
      sprintf(g_buffer, "[");

   sprintf(g_buffer + strlen(g_buffer), "%d,", (int)avltree_body_key(p));
   __pass_avltree(p->left);   
   __pass_avltree(p->right);

   if (p->parent == NULL)
      g_buffer[strlen(g_buffer)-1] = ']';
   return g_buffer;
}

static char* __helper__pass_avltree(avltree_tree * tree)
{
   if (tree) {
      sprintf(g_buffer, "[]");
      return __pass_avltree(tree->root);
   }
   return 0;
}


H2SUITE(avltreet)
{ 
public :
   int h;
   avltree_tree tree;
   avltree_body_t vals[7];
   
   void setup()
   { 
      h = 0;
     
      avltree_create(&tree, &avltree_body_key, &avltree_body_cmp,0);
      for (int i=0; i<7; i++) {
         avltree_init(&vals[i].avl);
         vals[i].value = i;
      }
   }

   void teardown()
   {  		
   }  	  	
} ;

H2CASE(avltreet, AVLTREE_HEAD_INIT)
{
   avltree_head x = AVLTREE_HEAD_INIT(x);

   H2EQUAL_INTEGER((avltree_head*)NULL, x.parent);
   H2EQUAL_INTEGER((avltree_head*)NULL, x.left);
   H2EQUAL_INTEGER((avltree_head*)NULL, x.right);
   H2EQUAL_INTEGER(1, x.height);
}

H2CASE(avltreet, avltree_init)
{
   avltree_head x;

   avltree_init(&x);

   H2EQUAL_INTEGER((avltree_head*)NULL, x.parent);
   H2EQUAL_INTEGER((avltree_head*)NULL, x.left);
   H2EQUAL_INTEGER((avltree_head*)NULL, x.right);
   H2EQUAL_INTEGER(1, x.height);
}


/*
   3         3         3          2
            /         /         /  \
      =>   2   =>    2   =>    1    3
                   /
                  1 
*/
H2CASE(avltreet, avltree_add_left_left)
{
   vals[0].value = 3;
   vals[1].value = 2;
   vals[2].value = 1;

   for (int i=0; i<3; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }

   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
   H2EQUAL_STRCMP("[2,1,3]", __helper__pass_avltree(&tree));
   //____helper__print_avltree(&tree);
}

/*
                      6                      4
                     / \                    / \
                    4   7                  2   6
                   / \       =>           /   / \
                  2   5                  1   5   7
                 /
                1
*/
H2CASE(avltreet, avltree_add_left_left_left)
{
   vals[0].value = 6;
   vals[1].value = 4;
   vals[2].value = 7;
   vals[3].value = 2;
   vals[4].value = 5;
   vals[5].value = 1;

   for (int i=0; i<6; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }

   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
   H2EQUAL_STRCMP("[4,2,1,6,5,7]", __helper__pass_avltree(&tree));
   //____helper__print_avltree(&tree);
}

/*
                      6                       4
                     / \                    /   \
                    4   7                  2     6
                   / \       =>             \   / \
                  2   5                      3 5   7
                   \
                    3
*/
H2CASE(avltreet, avltree_add_left_left_right)
{
   vals[0].value = 6;
   vals[1].value = 4;
   vals[2].value = 7;
   vals[3].value = 2;
   vals[4].value = 5;
   vals[5].value = 3;

   for (int i=0; i<6; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }

   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
   H2EQUAL_STRCMP("[4,2,3,6,5,7]", __helper__pass_avltree(&tree));
   //____helper__print_avltree(&tree);
}

/*
   3         3         3          2
            /         /         /  \
      =>   1   =>    1   =>    1    3
                      \
                       2 
*/
H2CASE(avltreet, avltree_add_left_right)
{
   vals[0].value = 3;
   vals[1].value = 1;
   vals[2].value = 2;

   for (int i=0; i<3; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }
   
   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
   H2EQUAL_STRCMP("[2,1,3]", __helper__pass_avltree(&tree));
   //____helper__print_avltree(&tree);
}

/*
                      6                       4
                     / \                    /   \
                    2   7                  2     6
                   / \       =>           / \     \
                  1   4                  1   3     7
                     /
                    3
*/
H2CASE(avltreet, avltree_add_left_right_left)
{
   vals[0].value = 6;
   vals[1].value = 2;
   vals[2].value = 7;
   vals[3].value = 1;
   vals[4].value = 4;
   vals[5].value = 3;

   for (int i=0; i<6; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }

   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
   H2EQUAL_STRCMP("[4,2,1,3,6,7]", __helper__pass_avltree(&tree));
   //____helper__print_avltree(&tree);
}

/*
                      6                       4
                     / \                    /   \
                    2   7                  2     6
                   / \       =>           /     / \
                  1   4                  1     5   7
                       \
                        5
*/
H2CASE(avltreet, avltree_add_left_right_right)
{
   vals[0].value = 6;
   vals[1].value = 2;
   vals[2].value = 7;
   vals[3].value = 1;
   vals[4].value = 4;
   vals[5].value = 5;

   for (int i=0; i<6; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }

   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
   H2EQUAL_STRCMP("[4,2,1,6,5,7]", __helper__pass_avltree(&tree)); 
   //____helper__print_avltree(&tree);
}

H2CASE(avltreet, avltree_add_right_right)
{
   vals[0].value = 1;
   vals[1].value = 2;
   vals[2].value = 3;

   for (int i=0; i<3; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }

   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
   H2EQUAL_STRCMP("[2,1,3]", __helper__pass_avltree(&tree)); 
   //____helper__print_avltree(&tree);
}

H2CASE(avltreet, avltree_add_right_right_right)
{
   vals[0].value = 2;
   vals[1].value = 4;
   vals[2].value = 1;
   vals[3].value = 3;
   vals[4].value = 6;
   vals[5].value = 7;

   for (int i=0; i<6; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }

   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
   H2EQUAL_STRCMP("[4,2,1,3,6,7]", __helper__pass_avltree(&tree)); 
   //____helper__print_avltree(&tree);
}

H2CASE(avltreet, avltree_add_right_right_left)
{
   vals[0].value = 2;
   vals[1].value = 4;
   vals[2].value = 1;
   vals[3].value = 3;
   vals[4].value = 6;
   vals[5].value = 5;

   for (int i=0; i<6; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }
   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
   H2EQUAL_STRCMP("[4,2,1,3,6,5]", __helper__pass_avltree(&tree)); 
   //____helper__print_avltree(&tree);
}

H2CASE(avltreet, __rebalance_right_left_left)
{
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 7;
   vals[4].value = 4;
   vals[5].value = 3;

   for (int i=0; i<6; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }
   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
   H2EQUAL_STRCMP("[4,2,1,3,6,7]", __helper__pass_avltree(&tree)); 
   //____helper__print_avltree(&tree);
}

H2CASE(avltreet, __rebalance_right_left_right)
{
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 7;
   vals[4].value = 4;
   vals[5].value = 5;

   for (int i=0; i<6; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }
   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
   H2EQUAL_STRCMP("[4,2,1,6,5,7]", __helper__pass_avltree(&tree)); 
   //____helper__print_avltree(&tree);
}

H2CASE(avltreet, avltree_add_first)
{
   avltree_body_t x = {AVLTREE_HEAD_INIT(x),3};
   H2EQUAL_INTEGER(&x.avl, avltree_insert(&tree,&x.avl));

   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
   H2EQUAL_STRCMP("[3]", __helper__pass_avltree(&tree)); 
   //____helper__print_avltree(&tree);
}

H2CASE(avltreet, avltree_lookup)
{
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 5;
   vals[4].value = 4;
   vals[5].value = 3;

   for (int i=0; i<6; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }

   H2EQUAL_INTEGER(&vals[3].avl, avltree_lookup(&tree,5));
}

H2CASE(avltreet, avltree_min_max)
{
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 5;
   vals[4].value = 4;
   vals[5].value = 3;

   for (int i=0; i<6; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }

   H2EQUAL_INTEGER(&vals[2].avl, avltree_min(&tree));
   H2EQUAL_INTEGER(&vals[1].avl, avltree_max(&tree));
}

H2CASE(avltreet, avltree_inc_dec)
{
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 5;
   vals[4].value = 4;
   vals[5].value = 3;

   for (int i=0; i<6; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }

   H2EQUAL_INTEGER( &vals[5].avl, avltree_inc(&vals[0].avl));
   H2EQUAL_INTEGER( (avltree_head*)NULL,       avltree_inc(&vals[1].avl));
   H2EQUAL_INTEGER( &vals[0].avl, avltree_inc(&vals[2].avl));
   H2EQUAL_INTEGER( &vals[1].avl, avltree_inc(&vals[3].avl) );
   H2EQUAL_INTEGER( &vals[3].avl, avltree_inc(&vals[4].avl));
   H2EQUAL_INTEGER( &vals[4].avl, avltree_inc(&vals[5].avl));

      
   H2EQUAL_INTEGER( &vals[2].avl, avltree_dec(&vals[0].avl));
   H2EQUAL_INTEGER( &vals[3].avl, avltree_dec(&vals[1].avl));
   H2EQUAL_INTEGER( (avltree_head*)NULL,       avltree_dec(&vals[2].avl));
   H2EQUAL_INTEGER( &vals[4].avl, avltree_dec(&vals[3].avl));
   H2EQUAL_INTEGER( &vals[5].avl, avltree_dec(&vals[4].avl));
   H2EQUAL_INTEGER( &vals[0].avl, avltree_dec(&vals[5].avl));
}

H2CASE(avltreet, avltree_remove7)
{
   avltree_body_t * body;
   avltree_head  *p;
   
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 5;
   vals[4].value = 4;
   vals[5].value = 3;
   vals[6].value = 7;

   for (int i=0; i<7; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }

   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
   H2EQUAL_STRCMP("[4,2,1,3,6,5,7]", __helper__pass_avltree(&tree));
   
   p = avltree_lookup(&tree,4);
   body = list_entry(p, avltree_body_t, avl);
   H2EQUAL_INTEGER(4, body->value);
   H2EQUAL_INTEGER(p, avltree_remove(&tree, p));
   //____helper__print_avltree(&tree);

   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
}

H2CASE(avltreet, avltree_remove)
{
   avltree_body_t * body = NULL;
   avltree_tree root;
   avltree_create(&root, avltree_body_key, avltree_body_cmp,1);
   const int N = 99;
   avltree_body_t array[N];
   srand(time(0));
   for (int i=0; i<N; i++) {
      avltree_body_t * t = &array[i];      
     	avltree_init(&t->avl);
      t->value = i+1;
      t->next = body;
      body = t;
      H2EQUAL_INTEGER(&t->avl, avltree_insert(&root,&t->avl));
   }
   H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));

   while (body) {
      avltree_body_t * t = body;
      body = body->next;
      H2EQUAL_INTEGER(&t->avl, avltree_remove(&root, &t->avl));
      H2EQUAL_INTEGER(1, ____helper__verify_avltree(&tree));
   }
   //____helper__print_avltree(tree); 
}

H2CASE(avltreet, avltree_for_each)
{
   int i;
   vals[0].value = 2;
   vals[1].value = 6;
   vals[2].value = 1;
   vals[3].value = 5;
   vals[4].value = 4;
   vals[5].value = 3;
   vals[6].value = 7;

   for ( i=0; i<7; i++)  {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }

   int last;
   avltree_head *h, *ht;
   avltree_body_t *p;

   /*avltree_for_each*/
   tree.root = NULL; 
   for ( i=0; i<7; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }
   last = 0;
   avltree_for_each(h,&tree) {
      p = avltree_entry(h,avltree_body_t,avl);
      H2CHECK(p->value > last);
      last = p->value;
   }

   /*avltree_for_each_prev*/
   tree.root = NULL; 
   for ( i=0; i<7; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }
   last = 9;
   avltree_for_each_prev(h,&tree) {
      p = avltree_entry(h,avltree_body_t,avl);
      H2CHECK(p->value < last);
      last = p->value;
   }

   /*avltree_for_each_safe*/
   tree.root = NULL; 
   for ( i=0; i<7; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }
   last = 0;
   avltree_for_each_safe(h,ht,&tree) {
      p = avltree_entry(h,avltree_body_t,avl);
      H2CHECK(p->value > last);
      last = p->value;
      H2EQUAL_INTEGER(h, avltree_remove(&tree,h));
   }

   /*avltree_for_each_prev_safe*/
   tree.root = NULL; 
   for ( i=0; i<7; i++) {
      H2EQUAL_INTEGER(&vals[i].avl, avltree_insert(&tree, &vals[i].avl));
   }
   last = 9;
   avltree_for_each_prev_safe(h,ht,&tree) {
      p = avltree_entry(h,avltree_body_t,avl);
      H2CHECK(p->value < last);
      last = p->value;
      H2EQUAL_INTEGER(h, avltree_remove(&tree,h));
   }


}


