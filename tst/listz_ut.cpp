#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "slabz.h"
#include "listz.h"

extern "C" {

}


static list_head a,b,c;
//static list_head d,e,f,g,h,i,j,k;
static list_head l;
//static list_head m,n,o,p,q;
static list_head r;
//static list_head s,t,u,v,w;
static list_head x,y,z;

char get_a_z(list_head * pt)
{
   if (pt == &a) return 'a';
   if (pt == &b) return 'b';
   if (pt == &c) return 'c';
   //if (pt == &d) return 'd';
   
   //if (pt == &e) return 'e';
   //if (pt == &f) return 'f';
   //if (pt == &g) return 'g'; 
   //if (pt == &h) return 'h';
   
   //if (pt == &i) return 'i';
   //if (pt == &j) return 'j';
   //if (pt == &k) return 'k';
   if (pt == &l) return 'l';
   
   //if (pt == &m) return 'm'; 
   //if (pt == &n) return 'n';
   //if (pt == &o) return 'o';
   //if (pt == &p) return 'p';
   //if (pt == &q) return 'q';
   if (pt == &r) return 'r';
   //if (pt == &s) return 's';

   //if (pt == &t) return 't';
   //if (pt == &u) return 'u';
   //if (pt == &v) return 'v';
   //if (pt == &w) return 'w';
   if (pt == &x) return 'x';
   if (pt == &y) return 'y';
   if (pt == &z) return 'z';

   return '-';
}


char *__EP(list_head *vec[], int len)
{
   int ip, jp=0;
   static char buff[100];
   for (ip=0; ip<len; ip++) {
      buff[jp++] = get_a_z(vec[ip]);
   }
   buff[jp++] = get_a_z(vec[0]);
   buff[jp++] = '-';
   buff[jp++] = get_a_z(vec[0]);
   for (ip=len-1; ip>=0; ip--) {
      buff[jp++] = get_a_z(vec[ip]);
   }
   buff[jp++] = '\0';
   return buff;
}

#define EP(ee) __EP(ee, sizeof(ee)/sizeof(ee[0]))

char *RP(list_head *hp)
{
   int jp=0;
   static char buff[100];
   list_head *pt;

   if (hp == NULL)
      return "";

   buff[jp++] = get_a_z(hp);
   for (pt = hp->next; pt != hp; pt = pt->next)
      buff[jp++] = get_a_z(pt);
   buff[jp++] = get_a_z(hp);
   buff[jp++] = '-';
   buff[jp++] = get_a_z(hp);
   for (pt = hp->prev; pt != hp; pt = pt->prev)
      buff[jp++] = get_a_z(pt);
   buff[jp++] = get_a_z(hp);
   buff[jp++] = '\0';
   return buff;
}

H2SUITE(listt)
{
   char buffer[27];
   int  buffei;
   
	void setup()
  	{ 
  	}

  	void teardown()
  	{  		
  	}  	  	
} ;

H2CASE(listt, LIST_HEAD_INIT)
{
   list_head tt = LIST_HEAD_INIT(tt);

   H2EQUAL_INTEGER(&tt, tt.next);
   H2EQUAL_INTEGER(&tt, tt.prev);
}

H2CASE(listt, LIST_HEAD_NULL)
{
   list_head tt = LIST_HEAD_NULL;

   H2EQUAL_INTEGER(0, tt.next);
   H2EQUAL_INTEGER(0, tt.prev);
}

H2CASE(listt, list_null)
{
   list_head tt;
   
   list_null(&tt);
   H2EQUAL_INTEGER(0, tt.next);
   H2EQUAL_INTEGER(0, tt.prev);
}

H2CASE(listt, list_init)
{
   list_head tt;
   
   list_init(&tt); 
   H2EQUAL_INTEGER(&tt, tt.next);
   H2EQUAL_INTEGER(&tt, tt.prev);
}

H2CASE(listt, list_add)
{
   list_init(&a);

   list_add(&b, &a);
   list_add(&c, &b);
   
   list_head * ee[] = {&a,&b,&c}; // a->b->c
   H2EQUAL_STRCMP(EP(ee), RP(&a));
}

H2CASE(listt, list_add_prev)
{
   list_init(&a);

   list_add_prev(&b, &a);   
   list_add_prev(&c, &b);

   list_head * ee[] = {&c,&b,&a}; // c->b->a
   H2EQUAL_STRCMP(EP(ee), RP(&c));
}

H2CASE(listt, list_add_next)
{
   list_init(&a);

   list_add_next(&b, &a);   
   list_add_next(&c, &b);

   list_head * ee[] = {&a,&b,&c}; // a->b->c
   H2EQUAL_STRCMP(EP(ee), RP(&a));
}

H2CASE(listt, list_add_tail)
{
   list_init(&a);

   list_add_tail(&b, &a);   
   list_add_tail(&c, &b);

   list_head * ee[] = {&c,&b,&a}; // c->b->a
   H2EQUAL_STRCMP(EP(ee), RP(&c));
}

H2CASE(listt, list_add_head)
{
   list_init(&a);

   list_add_head(&b, &a);   
   list_add_head(&c, &b);

   list_head * ee[] = {&a,&b,&c}; // a->b->c
   H2EQUAL_STRCMP(EP(ee), RP(&a));
}

H2CASE(listt, list_del)
{
   list_init(&a);

   list_add(&b, &a);
   list_add(&c, &b);

   list_del(&b);

   list_head * ee[] = {&a,&c}; // a->c
   H2EQUAL_STRCMP(EP(ee), RP(&a)); 
}

H2CASE(listt, list_del_init)
{
   list_init(&a);

   list_add(&b, &a);
   list_add(&c, &b);

   list_del_init(&b);

   list_head * ee[] = {&a,&c}; // a->c
   H2EQUAL_STRCMP(EP(ee), RP(&a)); 

   H2EQUAL_INTEGER(&b, b.next); 
   H2EQUAL_INTEGER(&b, b.prev);
}

H2CASE(listt, list_del_null)
{
   list_init(&a);

   list_add(&b, &a);
   list_add(&c, &b);

   list_del_null(&b);

   list_head * ee[] = {&a,&c}; // a->c
   H2EQUAL_STRCMP(EP(ee), RP(&a)); 

   H2EQUAL_INTEGER(0, b.next); 
   H2EQUAL_INTEGER(0, b.prev);
}

H2CASE(listt, list_empty)
{
   list_init(&a);

   H2CHECK(list_empty(&a));

   list_add(&b, &a);
   list_add(&c, &b);

   H2CHECK(!list_empty(&a));   
}

H2CASE(listt, list_get)
{
   list_head *pt;
   list_init(&r);

   list_add_tail(&a, &r);
   list_add_tail(&b, &r);
   list_add_tail(&c, &r);

   pt = list_get_head(&r);
   H2EQUAL_INTEGER(&a, pt);  
   pt = list_get_tail(&r);
   H2EQUAL_INTEGER(&c, pt);

   pt = list_get_top(&r);
   H2EQUAL_INTEGER(&a, pt);  
   pt = list_get_bottom(&r);
   H2EQUAL_INTEGER(&c, pt);
}

H2CASE(listt, list_queue_operation)
{
   list_head *pt;
   list_init(&r);

   list_add_tail(&a, &r);
   list_add_tail(&b, &r);
   list_add_tail(&c, &r);

   
   list_head * ee[] = {&r,&a,&b,&c}; // r->a->b->c
   H2EQUAL_STRCMP(EP(ee), RP(&r)); 
   
   list_enqueue(&x, &r);   
   list_head * e1[] = {&r,&a,&b,&c,&x}; // r->a->b->c->x
   H2EQUAL_STRCMP(EP(e1), RP(&r)); 

   list_enqueue(&y, &r); 
   list_head * e2[] = {&r,&a,&b,&c,&x,&y}; // r->a->b->c->x->y
   H2EQUAL_STRCMP(EP(e2), RP(&r)); 

   list_dequeue(pt, &r); 
   H2EQUAL_INTEGER(&a, pt);
   list_head * e3[] = {&r,&b,&c,&x,&y}; // r->b->c->x->y 
   H2EQUAL_STRCMP(EP(e3), RP(&r)); 
} 

H2CASE(listt, list_stack_operation)
{
   list_head *pt;
   list_init(&r);

   list_add_tail(&a, &r);
   list_add_tail(&b, &r);
   list_add_tail(&c, &r);

   list_head * ee[] = {&r,&a,&b,&c}; // r->a->b->c
   H2EQUAL_STRCMP(EP(ee), RP(&r)); 
   
   list_push(&x, &r);
   list_head * e1[] = {&r,&x,&a,&b,&c}; // r->x->a->b->c
   H2EQUAL_STRCMP(EP(e1), RP(&r));

   list_push(&y, &r);
   list_head * e2[] = {&r,&y,&x,&a,&b,&c}; // r->y->x->a->b->c
   H2EQUAL_STRCMP(EP(e2), RP(&r));

   list_pop(pt, &r);
   H2EQUAL_INTEGER(&y, pt);
   list_head * e3[] = {&r,&x,&a,&b,&c}; // r->x->a->b->c
   H2EQUAL_STRCMP(EP(e3), RP(&r));
}

H2CASE(listt, list_splice_head)
{
   list_init(&l);
   list_init(&r);
   list_add_tail(&a, &l);
   list_add_tail(&b, &l);
   list_add_tail(&c, &l);

   list_add_tail(&x, &r);
   list_add_tail(&y, &r);
   list_add_tail(&z, &r);

   list_splice_head(&l, &r);
/*
   l->a->b->c	r->x->y->z
==>r->a->b->c->x->y->z   
*/
   list_head * ee[] = {&r,&a,&b,&c,&x,&y,&z}; //r->a->b->c->x->y->z 
   H2EQUAL_STRCMP(EP(ee), RP(&r));
   
   list_head * e1[] = {&l}; //l 
   H2EQUAL_STRCMP(EP(e1), RP(&l));
}

H2CASE(listt, list_splice_tail)
{
   list_init(&l);
   list_init(&r);
   
   list_add_tail(&a, &l);
   list_add_tail(&b, &l);
   list_add_tail(&c, &l);

   list_add_tail(&x, &r);
   list_add_tail(&y, &r);
   list_add_tail(&z, &r);

   list_splice_tail(&l, &r);

   list_head * ee[] = {&r,&x,&y,&z,&a,&b,&c}; //r->x->y->z->a->b->c 
   H2EQUAL_STRCMP(EP(ee), RP(&r));

   list_head * e1[] = {&l}; //l 
   H2EQUAL_STRCMP(EP(e1), RP(&l));
}

H2CASE(listt, list_splice_head_empty_list)
{
   list_init(&l);
   list_init(&r);

   list_add_tail(&x, &r);
   list_add_tail(&y, &r);
   list_add_tail(&z, &r);

   list_splice_head(&l, &r); 

   list_head * ee[] = {&r,&x,&y,&z}; //r->x->y->z
   H2EQUAL_STRCMP(EP(ee), RP(&r));

   list_head * e1[] = {&l}; //l 
   H2EQUAL_STRCMP(EP(e1), RP(&l));
}

H2CASE(listt, list_splice_tail_empty_list)
{
   list_init(&l);
   list_init(&r);

   list_add_tail(&x, &r);
   list_add_tail(&y, &r);
   list_add_tail(&z, &r);

   list_splice_tail(&l, &r);


   list_head * ee[] = {&r,&x,&y,&z}; //r->x->y->z
   H2EQUAL_STRCMP(EP(ee), RP(&r));

   list_head * e1[] = {&l}; //l 
   H2EQUAL_STRCMP(EP(e1), RP(&l));
}

H2CASE(listt, list_splice_head_empty_head)
{
   list_init(&l);
   list_init(&r);

   list_add_tail(&x, &l);
   list_add_tail(&y, &l);
   list_add_tail(&z, &l);

   list_splice_head(&l, &r);


   list_head * ee[] = {&r,&x,&y,&z}; //r->x->y->z
   H2EQUAL_STRCMP(EP(ee), RP(&r));

   list_head * e1[] = {&l}; //l 
   H2EQUAL_STRCMP(EP(e1), RP(&l));
}

H2CASE(listt, list_splice_tail_empty_head)
{
   list_init(&l);
   list_init(&r);

   list_add_tail(&x, &l);
   list_add_tail(&y, &l);
   list_add_tail(&z, &l);

   list_splice_tail(&l, &r);


   list_head * ee[] = {&r,&x,&y,&z}; //r->x->y->z
   H2EQUAL_STRCMP(EP(ee), RP(&r));

   list_head * e1[] = {&l}; //l 
   H2EQUAL_STRCMP(EP(e1), RP(&l));
}

H2CASE(listt, list_replace)
{
   list_init(&l);
   list_init(&r);

   list_add_tail(&x, &r);
   list_add_tail(&y, &r);
   list_add_tail(&z, &r);

   // r->x->y->z 
   list_replace(&l, &y);
   // r->x->l->z 

   list_head * ee[] = {&r,&x,&l,&z}; //r->x->l->z
   H2EQUAL_STRCMP(EP(ee), RP(&r));

   list_head * e1[] = {&y}; //y 
   H2EQUAL_STRCMP(EP(e1), RP(&y)); 
}

H2CASE(listt, list_move_head)
{
   list_init(&l);
   list_init(&r);

   list_add_tail(&a, &l);
   list_add_tail(&b, &l);
   list_add_tail(&c, &l);
   list_add_tail(&x, &r);
   list_add_tail(&y, &r);
   list_add_tail(&z, &r);

   // l->a->b->c
   // r->x->y->z 
   list_move_head(&a, &r);
   // l->b->c
   // r->a->x->y->z
   list_move_tail(&z, &l);
   // l->b->c->z
   // r->a->x->y 

   list_head * ee[] = {&l,&b,&c,&z}; // l->b->c->z
   H2EQUAL_STRCMP(EP(ee), RP(&l));
   
   list_head * e1[] = {&r,&a,&x,&y}; // r->a->x->y
   H2EQUAL_STRCMP(EP(e1), RP(&r));

}


typedef struct list_body_t
{
   int x;
   list_head link;
   int y;
   char z;
	int value;
	unsigned serial;
}list_body_t;

typedef class list_class_t
{
private:
   int x;
protected:
   int y;
public:
   int z;
   list_head link;
   list_class_t() {x = 100; y = 200; z = 300;};
   ~list_class_t() {};
   int get_x(int **xx){*xx = &x; return x;};
   int get_y(int **yy){*yy = &y; return y;};
   int get_z(int **zz){*zz = &z; return z;};
}list_class_t;

H2CASE(listt, list_entry0)
{
   list_body_t body, * p1;
   body.x = 88;
   body.y = 99;

   p1 = list_entry(&body.link, list_body_t, link);

   H2EQUAL_INTEGER(88, p1->x);
   H2EQUAL_INTEGER(99, p1->y);

   int *xx, *yy, *zz;
   list_class_t cs, * c1;
   c1 = list_entry(&cs.link, list_class_t, link);
   H2EQUAL_INTEGER(100, c1->get_x(&xx));
   H2EQUAL_INTEGER(200, c1->get_y(&yy));
   H2EQUAL_INTEGER(300, c1->get_z(&zz));
}

H2CASE(listt, list_for_each0)
{
   list_head *p1;
   list_init(&r);

   list_add(&a, &r);
   list_add(&b, &a);
   list_add(&c, &b);

   buffei = 0;
   list_for_each(p1,&r) {
      buffer[buffei++] = get_a_z(p1);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("abc", buffer);
   
}

H2CASE(listt, list_for_each_safe0)
{
   list_head *p1,*t1;
   list_init(&r);

   list_add(&a, &r);
   list_add(&b, &a);
   list_add(&c, &b);

   buffei = 0;
   list_for_each_safe(p1,t1,&r) {
      buffer[buffei++] = get_a_z(p1);
      list_del(p1);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("abc", buffer);
}


H2CASE(listt, list_for_each_continue0)
{
   list_head *p1;
   list_init(&r);

   list_add(&a, &r);
   list_add(&b, &a);
   list_add(&c, &b);

   p1 = &a;

   buffei = 0;
   list_for_each_continue(p1,&r) {
      buffer[buffei++] = get_a_z(p1);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("bc", buffer);
   
}

H2CASE(listt, list_for_each_continue_safe0)
{
   list_head *p1,*t1;
   list_init(&r);

   list_add(&a, &r);
   list_add(&b, &a);
   list_add(&c, &b);

   p1 = &a;
   buffei = 0;
   list_for_each_continue_safe(p1,t1,&r) {
      buffer[buffei++] = get_a_z(p1);
      list_del(p1);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("bc", buffer);
}


H2CASE(listt, list_for_each_from0)
{
   list_head *p1;
   list_init(&r);

   list_add(&a, &r);
   list_add(&b, &a);
   list_add(&c, &b);

   p1 = &a;

   buffei = 0;
   list_for_each_from(p1,&r) {
      buffer[buffei++] = get_a_z(p1);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("abc", buffer);
   
}

H2CASE(listt, list_for_each_from_safe0)
{
   list_head *p1,*t1;
   list_init(&r);

   list_add(&a, &r);
   list_add(&b, &a);
   list_add(&c, &b);

   p1 = &a;
   buffei = 0;
   list_for_each_from_safe(p1,t1,&r) {
      buffer[buffei++] = get_a_z(p1);
      list_del(p1);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("abc", buffer);
}


H2CASE(listt, list_for_each_prev0)
{
   list_head *p1;
   list_init(&r);

   list_add(&a, &r);
   list_add(&b, &a);
   list_add(&c, &b);

   buffei = 0;
   list_for_each_prev(p1,&r) {
      buffer[buffei++] = get_a_z(p1);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("cba", buffer);
}

H2CASE(listt, list_for_each_prev_safe0)
{
   list_head *p1,*t1;
   list_init(&r);

   list_add(&a, &r);
   list_add(&b, &a);
   list_add(&c, &b);

   buffei = 0;
   list_for_each_prev_safe(p1,t1,&r) {
      buffer[buffei++] = get_a_z(p1);
      list_del(p1);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("cba", buffer);
}

H2CASE(listt, list_for_each_prev_continue0)
{
   list_head *p1;
   list_init(&r);

   list_add(&a, &r);
   list_add(&b, &a);
   list_add(&c, &b);

   p1 = &c;
   buffei = 0;
   list_for_each_prev_continue(p1,&r) {
      buffer[buffei++] = get_a_z(p1);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("ba", buffer);
}

H2CASE(listt, list_for_each_prev_continue_safe0)
{
   list_head *p1,*t1;
   list_init(&r);

   list_add(&a, &r);
   list_add(&b, &a);
   list_add(&c, &b);

   p1 = &c;
   buffei = 0;
   list_for_each_prev_continue_safe(p1,t1,&r) {
      buffer[buffei++] = get_a_z(p1);
      list_del(p1);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("ba", buffer);
}


H2CASE(listt, list_for_each_prev_from0)
{
   list_head *p1;
   list_init(&r);

   list_add(&a, &r);
   list_add(&b, &a);
   list_add(&c, &b);

   p1 = &c;
   buffei = 0;
   list_for_each_prev_from(p1,&r) {
      buffer[buffei++] = get_a_z(p1);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("cba", buffer);
}

H2CASE(listt, list_for_each_prev_from_safe0)
{
   list_head *p1,*t1;
   list_init(&r);

   list_add(&a, &r);
   list_add(&b, &a);
   list_add(&c, &b);

   p1 = &c;
   buffei = 0;
   list_for_each_prev_from_safe(p1,t1,&r) {
      buffer[buffei++] = get_a_z(p1);
      list_del(p1);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("cba", buffer);
}

H2CASE(listt, list_for_each_entry0)
{
   list_body_t a1,b1,c1, *p1;
   a1.z = 'a';
   b1.z = 'b';
   c1.z = 'c';
   list_init(&r);

   list_add(&c1.link, &r);
   list_add(&b1.link, &r);
   list_add(&a1.link, &r);
   buffei = 0;
   list_for_each_entry(p1,&r,list_body_t,link) {
      buffer[buffei++] = p1->z;
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("abc", buffer);
}

H2CASE(listt, list_for_each_entry_safe0)
{
   list_body_t a1,b1,c1, *p1,*t1;
   a1.z = 'a';
   b1.z = 'b';
   c1.z = 'c';
   list_init(&r);

   list_add(&c1.link, &r);
   list_add(&b1.link, &r);
   list_add(&a1.link, &r);
   buffei = 0;
   list_for_each_entry_safe(p1,t1,&r,list_body_t,link) {
      buffer[buffei++] = p1->z;
      list_del(&p1->link);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("abc", buffer);
}

H2CASE(listt, list_for_each_entry_continue0)
{
   list_body_t a1,b1,c1, *p1;
   a1.z = 'a';
   b1.z = 'b';
   c1.z = 'c';
   list_init(&r);

   list_add(&c1.link, &r);
   list_add(&b1.link, &r);
   list_add(&a1.link, &r);

   p1 = &a1;
   buffei = 0;
   list_for_each_entry_continue(p1,&r,list_body_t,link) {
      buffer[buffei++] = p1->z;
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("bc", buffer);
}

H2CASE(listt, list_for_each_entry_continue_safe0)
{
   list_body_t a1,b1,c1, *p1,*t1;
   a1.z = 'a';
   b1.z = 'b';
   c1.z = 'c';
   list_init(&r);

   list_add(&c1.link, &r);
   list_add(&b1.link, &r);
   list_add(&a1.link, &r);

   p1 = &a1;
   buffei = 0;
   list_for_each_entry_continue_safe(p1,t1,&r,list_body_t,link) {
      buffer[buffei++] = p1->z;
      list_del(&p1->link);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("bc", buffer);
}

H2CASE(listt, list_for_each_entry_from0)
{
   list_body_t a1,b1,c1, *p1;
   a1.z = 'a';
   b1.z = 'b';
   c1.z = 'c';
   list_init(&r);

   list_add(&c1.link, &r);
   list_add(&b1.link, &r);
   list_add(&a1.link, &r);

   p1 = &a1;
   buffei = 0;
   list_for_each_entry_from(p1,&r,list_body_t,link) {
      buffer[buffei++] = p1->z;
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("abc", buffer);
}

H2CASE(listt, list_for_each_entry_from_safe0)
{
   list_body_t a1,b1,c1, *p1,*t1;
   a1.z = 'a';
   b1.z = 'b';
   c1.z = 'c';
   list_init(&r);

   list_add(&c1.link, &r);
   list_add(&b1.link, &r);
   list_add(&a1.link, &r);

   p1 = &a1;
   buffei = 0;
   list_for_each_entry_from_safe(p1,t1,&r,list_body_t,link) {
      buffer[buffei++] = p1->z;
      list_del(&p1->link);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("abc", buffer);
}

H2CASE(listt, list_for_each_entry_prev0)
{
   list_body_t a1,b1,c1, *p1;
   a1.z = 'a';
   b1.z = 'b';
   c1.z = 'c';
   list_init(&r);

   list_add(&c1.link, &r);
   list_add(&b1.link, &r);
   list_add(&a1.link, &r);

   buffei = 0;
   list_for_each_entry_prev(p1,&r,list_body_t,link) {
      buffer[buffei++] = p1->z;
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("cba", buffer);
}

H2CASE(listt, list_for_each_entry_prev_safe0)
{
   list_body_t a1,b1,c1, *p1,*t1;
   a1.z = 'a';
   b1.z = 'b';
   c1.z = 'c';
   list_init(&r);

   list_add(&c1.link, &r);
   list_add(&b1.link, &r);
   list_add(&a1.link, &r);

   buffei = 0;
   list_for_each_entry_prev_safe(p1,t1,&r,list_body_t,link) {
      buffer[buffei++] = p1->z;
      list_del(&p1->link);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("cba", buffer);
}

H2CASE(listt, list_for_each_entry_prev_continue0)
{
   list_body_t a1,b1,c1, *p1;
   a1.z = 'a';
   b1.z = 'b';
   c1.z = 'c';
   list_init(&r);

   list_add(&c1.link, &r);
   list_add(&b1.link, &r);
   list_add(&a1.link, &r);

   p1 = &c1;
   buffei = 0;
   list_for_each_entry_prev_continue(p1,&r,list_body_t,link) {
      buffer[buffei++] = p1->z;
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("ba", buffer);
}

H2CASE(listt, list_for_each_entry_prev_continue_safe0)
{
   list_body_t a1,b1,c1, *p1,*t1;
   a1.z = 'a';
   b1.z = 'b';
   c1.z = 'c';
   list_init(&r);

   list_add(&c1.link, &r);
   list_add(&b1.link, &r);
   list_add(&a1.link, &r);

   p1 = &c1;
   buffei = 0;
   list_for_each_entry_prev_continue_safe(p1,t1,&r,list_body_t,link) {
      buffer[buffei++] = p1->z;
      list_del(&p1->link);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("ba", buffer);
}


H2CASE(listt, list_for_each_entry_prev_from0)
{
   list_body_t a1,b1,c1, *p1;
   a1.z = 'a';
   b1.z = 'b';
   c1.z = 'c';
   list_init(&r);

   list_add(&c1.link, &r);
   list_add(&b1.link, &r);
   list_add(&a1.link, &r);

   p1 = &c1;
   buffei = 0;
   list_for_each_entry_prev_from(p1,&r,list_body_t,link) {
      buffer[buffei++] = p1->z;
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("cba", buffer);
}

H2CASE(listt, list_for_each_entry_prev_from_safe0)
{
   list_body_t a1,b1,c1, *p1,*t1;
   a1.z = 'a';
   b1.z = 'b';
   c1.z = 'c';
   list_init(&r);

   list_add(&c1.link, &r);
   list_add(&b1.link, &r);
   list_add(&a1.link, &r);

   p1 = &c1;
   buffei = 0;
   list_for_each_entry_prev_from_safe(p1,t1,&r,list_body_t,link) {
      buffer[buffei++] = p1->z;
      list_del(&p1->link);
   }
   buffer[buffei] = '\0';
   H2EQUAL_STRCMP("cba", buffer);
}


/******************************************/


static int list_body_cmp(void *priv, list_head *a, list_head *b)
{
	return list_entry(a, list_body_t, link)->value - list_entry(b, list_body_t, link)->value;
}


H2SUITE(listz_sort)
{   
	void setup()
  	{ 
  	   slab_initialize();
  	}

  	void teardown()
  	{  
  	   slab_uninitialize();
  	}  	  	
} ;

H2CASE(listz_sort, list_sort0)
{
   const int LIST_SORT_H2CASE_LENGTH = (512+128+2);
   int i, g = 1, count;
   list_head *head = (list_head*)slab_malloc(sizeof(*head));
   list_head *p;
   list_body_t *el;

   list_init(head);
	
   for (i = 0; i < LIST_SORT_H2CASE_LENGTH; i++) {
      el = (list_body_t*)slab_malloc(sizeof(*el));
      el->value = (g = (g * 725861) % 6599) % (LIST_SORT_H2CASE_LENGTH/3);
      el->serial = i;
      list_add_next(&el->link, head);
   }
	
   list_sort(NULL, head, list_body_cmp);

   count = 1;
   for (p = head->next; p->next != head; p = p->next) {
      el = list_entry(p, list_body_t, link);
      int cmp_result = list_body_cmp(NULL, p, p->next);
      if (p->next->prev != p) {
         H2EQUAL_STRCMP("", "list_sort() returned a corrupted list!\n");
      } 
      if (cmp_result > 0) {
         H2EQUAL_STRCMP("", "list_sort() failed to sort!\n");
      } 
      count++;
   }
   if (count != LIST_SORT_H2CASE_LENGTH) {
      H2EQUAL_STRCMP("", "list_sort() returned list of different length!\n");
   }
}

H2SUITE(linez)
{ 

	void setup()
  	{ 
  	}

  	void teardown()
  	{  		
  	}  	  	
} ;

H2CASE(linez, LINE_HEAD_NULL)
{
   line_head a = LINE_HEAD_NULL;

   H2EQUAL_INTEGER((void*)NULL, a.next);
}

H2CASE(linez, LINE_HEAD_INIT)
{
   line_head a = LINE_HEAD_INIT(a);

   H2EQUAL_INTEGER(&a, a.next);
}

H2CASE(linez, line_null)
{
   line_head a;
   line_null(&a);

   H2EQUAL_INTEGER(0, a.next);
}

H2CASE(linez, line_init)
{
   line_head a;
   line_init(&a);

   H2EQUAL_INTEGER(&a, a.next);
}

H2CASE(linez, line_add)
{
   line_head a = LINE_HEAD_INIT(a),
   			 b = LINE_HEAD_INIT(b),
   			 c = LINE_HEAD_INIT(c);

   line_add(&b, &a);
   line_add(&c, &b);

   H2CHECK(a.next == &b); 
   H2CHECK(b.next == &c);
   H2CHECK(c.next == &a);
}

H2CASE(linez, line_del_next)
{
   line_head a = LINE_HEAD_INIT(a),
   			 b = LINE_HEAD_INIT(b),
   			 c = LINE_HEAD_INIT(c);

   line_add(&b, &a);
   line_add(&c, &b);

   line_del_next(&b);

   H2CHECK(a.next == &b); 
   H2CHECK(b.next == &a); 
}

H2CASE(linez, line_del)
{
   line_head a = LINE_HEAD_INIT(a),
   			 b = LINE_HEAD_INIT(b),
   			 c = LINE_HEAD_INIT(c);

   line_add(&b, &a);
   line_add(&c, &b);

   line_del(&c);

   H2CHECK(a.next == &b); 
   H2CHECK(b.next == &a); 
}

H2CASE(linez, line_empty)
{
   line_head a,b,c;
   line_init(&a);

   H2CHECK(line_empty(&a));

   line_add(&b, &a);
   line_add(&c, &b);

   H2CHECK(!line_empty(&a));   
}


H2CASE(linez, line_stack)
{
   line_head r,a,b,c, *p;
   line_init(&r);

   line_push(&a, &r);
   line_push(&b, &r);
   line_push(&c, &r);

   H2EQUAL_INTEGER(&c, line_get_top(&r));
   line_pop(p, &r);
   H2EQUAL_INTEGER(&c, p);
   line_pop(p, &r);
   H2EQUAL_INTEGER(&b, p);
   line_pop(p, &r);
   H2EQUAL_INTEGER(&a, p);
   line_pop(p, &r);
   H2EQUAL_INTEGER(0, p);
   H2EQUAL_INTEGER(0, line_get_top(&r));
}

typedef struct line_body_t
{
   int x;
   line_head link;
   int y;
}line_body_t;

H2CASE(linez, line_entry)
{
   line_body_t body, * p;

   body.x = 88;
   body.y = 99;

   p = list_entry(&body.link, line_body_t, link);

   H2EQUAL_INTEGER(88, p->x);
   H2EQUAL_INTEGER(99, p->y);
}

H2CASE(linez, line_for_each)
{
   int i=0;
   line_head h, a,b,c, *p;
   line_init(&h);

   line_add(&a, &h);
   line_add(&b, &a);
   line_add(&c, &b);

   list_for_each(p,&h) {
      i++;
      switch(i) {
      case 1:
         H2EQUAL_INTEGER(&a, p);
         break;
      case 2:
         H2EQUAL_INTEGER(&b, p);
         break;
      case 3:
         H2EQUAL_INTEGER(&c, p);
         break;
      }
   }
}


H2CASE(linez, line_for_each_safe)
{
   int i=0;
   line_head h, a,b,c, *p,*t;
   line_init(&h);

   line_add(&a, &h);
   line_add(&b, &a);
   line_add(&c, &b);

   list_for_each_safe(p,t,&h) {
      i++;
      if (a.next == p)
        line_del_next(&a);
      switch(i) {
      case 1:
         H2CHECK(p==&a);
         break;
      case 2:
         H2CHECK(p==&b);
         break;
      case 3:
         H2CHECK(p==&c);
         break;
      }
   }
}


H2CASE(linez, line_for_each_entry)
{
   int i=0;
   line_head h;
   line_body_t a,b,c, *p;
   a.x = 11;
   b.x = 22;
   c.x = 33;
   line_init(&h);

   line_add(&c.link, &h);
   line_add(&b.link, &h);
   line_add(&a.link, &h);

   list_for_each_entry(p,&h,line_body_t,link) {
      i++;
      switch(i) {
      case 1:
         H2EQUAL_INTEGER(11, p->x);
         break;
      case 2:
         H2EQUAL_INTEGER(22, p->x);
         break;
      case 3:
         H2EQUAL_INTEGER(33, p->x);
         break;
      }
   }
}


H2CASE(linez, line_for_each_entry_safe)
{
   int i=0;
   line_head h;
   line_body_t a,b,c, *p,*t;
   a.x = 11;
   b.x = 22;
   c.x = 33;
   line_init(&h);

   line_add(&c.link, &h);
   line_add(&b.link, &h);
   line_add(&a.link, &h);

   list_for_each_entry_safe(p,t,&h,line_body_t,link) {
      i++;
      if (a.link.next == &p->link) 
         line_del_next(&a.link);
      switch(i) {
      case 1:
         H2CHECK(p->x == 11);
         break;
      case 2:
         H2CHECK(p->x == 22);
         break;
      case 3:
         H2CHECK(p->x == 33);
         break;
      }
   }
}


