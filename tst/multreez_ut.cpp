#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "multreez.h"

extern "C" {

}



H2SUITE(multreet)
{ 
	void setup()
  	{ 
  	}

  	void teardown()
  	{  		
  	}  	  	
} ;

H2CASE(multreet, MULTREE_HEAD_INIT)
{
   multree_head a = MULTREE_HEAD_INIT(a);

   H2CHECK(a.parent == NULL);
   H2CHECK(a.child == NULL);
   H2CHECK(a.sibling.prev == &a.sibling);
   H2CHECK(a.sibling.next == &a.sibling);   
}

H2CASE(multreet, multree_zero)
{
   multree_head a;
   multree_zero(&a);

   H2CHECK(a.parent == NULL);
   H2CHECK(a.child == NULL);
   H2CHECK(a.sibling.prev == &a.sibling);
   H2CHECK(a.sibling.next == &a.sibling);   
}

H2CASE(multreet, multree_add_child)
{
   multree_head a,b,c;
   multree_zero(&a);
   multree_zero(&b);
   multree_zero(&c);
   
   multree_add_child(&b, &a);
   multree_add_child(&c, &a);

   H2CHECK(b.parent == &a);
   H2CHECK(a.child == &b);
   
   H2CHECK(c.parent == &a);
   H2CHECK(b.sibling.next == &c.sibling);
   H2CHECK(c.sibling.prev == &b.sibling);

   H2CHECK(!multree_add_child(0, &b));
   H2CHECK(!multree_add_child(&c, 0));   
}

H2CASE(multreet, multree_is)
{
   multree_head a,b,c;
   multree_zero(&a);
   multree_zero(&b);
   multree_zero(&c);
   
   multree_add_child(&b, &a);
   multree_add_child(&c, &a);


   H2CHECK(multree_is_root(&a));
   H2CHECK(!multree_is_leaf(&a));

   H2CHECK(!multree_is_root(&b));
   H2CHECK(multree_is_leaf(&b));

   H2CHECK(multree_is_first_child(&b));
   H2CHECK(!multree_is_first_child(&c));   

   H2CHECK(multree_is_singleton(&a));
   H2CHECK(!multree_is_singleton(&b));
   H2CHECK(!multree_is_singleton(&c));
}

H2CASE(multreet, multree_add_sibling_left)
{
   multree_head a,b,c,d;
   multree_zero(&a);
   multree_zero(&b);
   multree_zero(&c);   
   multree_zero(&d);
   
   multree_add_child(&b, &a);
   multree_add_child(&c, &a);

   multree_add_sibling_left(&d, &c);

   H2CHECK(d.parent == &a);
   H2CHECK(b.sibling.next == &d.sibling);
   H2CHECK(d.sibling.next == &c.sibling);

   H2CHECK(!multree_add_sibling_left(&b, &a));
}

H2CASE(multreet, multree_add_sibling_right)
{
   multree_head a,b,c,d;
   multree_zero(&a);
   multree_zero(&b);
   multree_zero(&c);   
   multree_zero(&d);
   
   multree_add_child(&b, &a);
   multree_add_child(&c, &a);

   multree_add_sibling_right(&d, &c);

   H2CHECK(d.parent == &a);
   H2CHECK(c.sibling.next == &d.sibling);
   H2CHECK(d.sibling.next == &b.sibling);

   H2CHECK(!multree_add_sibling_right(&b, &a));
}


H2CASE(multreet, multree_depth)
{
   multree_head a,b,c,d,e,f,g;
   multree_zero(&a);
   multree_zero(&b);
   multree_zero(&c);   
   multree_zero(&d);
   multree_zero(&e);
   multree_zero(&f);   
   multree_zero(&g);   
   
   multree_add_child(&b, &a);
   multree_add_child(&c, &a);

   multree_add_child(&d, &b);
   multree_add_child(&e, &b);
   multree_add_child(&f, &c);
   multree_add_child(&g, &f);

/* ---------------------------
            a
           /  \
          b    c
         / \   |
        d   e  f
               |
               g
   --------------------------- */               
   H2CHECK(multree_depth(NULL) == -1);

   H2CHECK(multree_depth(&a) == 0);
   H2CHECK(multree_depth(&b) == 1);
   H2CHECK(multree_depth(&c) == 1);
   H2CHECK(multree_depth(&d) == 2);
   H2CHECK(multree_depth(&e) == 2);
   H2CHECK(multree_depth(&f) == 2);
   H2CHECK(multree_depth(&g) == 3);
}


H2CASE(multreet, multree_root_and_left_right)
{
   multree_head a,b,c,d,e,f,g;
   multree_zero(&a);
   multree_zero(&b);
   multree_zero(&c);   
   multree_zero(&d);
   multree_zero(&e);
   multree_zero(&f);   
   multree_zero(&g);   
   
   multree_add_child(&b, &a);
   multree_add_child(&c, &a);

   multree_add_child(&d, &b);
   multree_add_child(&e, &b);
   multree_add_child(&f, &c);
   multree_add_child(&g, &f);

/* ---------------------------
            a
           /  \
          b    c
         / \   |
        d   e  f
               |
               g
   --------------------------- */               

   H2CHECK(multree_root(&a) == &a);
   H2CHECK(multree_root(&b) == &a);
   H2CHECK(multree_root(&c) == &a);
   H2CHECK(multree_root(&d) == &a);
   H2CHECK(multree_root(&e) == &a);
   H2CHECK(multree_root(&f) == &a);
   H2CHECK(multree_root(&g) == &a);

   H2CHECK(multree_left_sibling(&e) == &d);
   H2CHECK(multree_right_sibling(&b) == &c);
   H2CHECK(multree_left_sibling(&f) == &f);
   H2CHECK(multree_right_sibling(&f) == &f);
   H2CHECK(multree_left_sibling(&g) == &g);   
}


H2CASE(multreet, multree_del)
{
   multree_head a,b,c,d;
   multree_zero(&a);
   multree_zero(&b);
   multree_zero(&c);   
   multree_zero(&d);
   
   multree_add_child(&b, &a);
   multree_add_child(&c, &a);
   multree_add_child(&d, &a);

   multree_del(&b);

   H2CHECK(a.child == &c);
   H2CHECK(c.sibling.next == &d.sibling);
   H2CHECK(d.sibling.next == &c.sibling);

   multree_del(&d);
   H2CHECK(a.child == &c);
   H2CHECK(multree_is_singleton(&c));

   multree_del(&a);
}


