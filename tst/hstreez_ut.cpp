#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "listz.h"
#include "hstreez.h"

extern "C" {
}

struct _test_item
{
   list_head link;
   unsigned char *key;
   int key_length;
   unsigned char *value;
   int value_length;
};

unsigned long _key_hash(list_head *item)
{
   struct _test_item *ti = (struct _test_item *) list_entry(item, struct _test_item, link);
   return (unsigned long) ti->key;
}

unsigned long _value_hash(list_head *item)
{
   struct _test_item *ti = (struct _test_item *) list_entry(item, struct _test_item, link);
   return (unsigned long) ti->value;
}

int _key_cmp(list_head *a, list_head *b)
{
   return (unsigned long) _key_hash(a) - (unsigned long) _key_hash(b);
}

struct _test_args
{
   hstree_tree *local_tree;
   hstree_tree *remote_tree;
   int equals, notequals;
};
int _visitor(void *args, unsigned long startkey, unsigned long endkey, unsigned long hash, list_head *items)
{
   _test_args *ta = (_test_args *) args;
   unsigned long hs = hstree_gethash(ta->remote_tree, startkey, endkey);
   if (hs == hash) {
      ta->equals++;
      return 1;
   }
   ta->notequals++;
   return 0;
}

H2SUITE(hstreez)
{
   void setup()
   {
   }

   void teardown()
   {
   }
};

H2CASE(hstreez, _create_destroy)
{
   hstree_tree tree, *res;
   res = hstree_create(&tree, 2, _key_hash, _value_hash, _key_cmp);
   H2CHECK(res != NULL);
   H2EQUAL_STRCMP("width=2,height=32,NI=1,0,0-0-0",hstree_tostring(&tree, 9));
   hstree_destroy(&tree);
}

H2CASE(hstreez, _insert)
{
   hstree_tree tree, *res;
   res = hstree_create(&tree, 4, _key_hash, _value_hash, _key_cmp);
   H2CHECK(res != NULL);
   H2EQUAL_STRCMP("width=4,height=16,NI=1,0,0-0-0",hstree_tostring(&tree, 9));

   struct _test_item item1 = { { }, (unsigned char*) 0x1, 11, (unsigned char*) 0xA, 17 };
   hstree_insert(&tree, &item1.link);
   H2EQUAL_STRCMP("width=4,height=16,NI=17,1,0-1-A,1-1-A,2-1-A,3-1-A,4-1-A,5-1-A,6-1-A,7-1-A,8-1-A,9-1-A,10-1-A,11-1-A,12-1-A,13-1-A,14-1-A,15-1-A,16-1-A",
               hstree_tostring(&tree, 9));

   struct _test_item item2 = { { }, (unsigned char*) 0x2, 11, (unsigned char*) 0xB0, 17 };
   hstree_insert(&tree, &item2.link);
   H2EQUAL_STRCMP("width=4,height=16,NI=18,2,0-2-BA,1-2-BA,2-2-BA,3-2-BA,4-2-BA,5-2-BA,6-2-BA,7-2-BA,8-2-BA,9-2-BA,10-2-BA,11-2-BA,12-2-BA,13-2-BA,14-2-BA,15-2-BA,16-1-A,16-1-B0",
               hstree_tostring(&tree, 9));

   struct _test_item item3 = { { }, (unsigned char*) 0x4, 11, (unsigned char*) 0xC00, 17 };
   hstree_insert(&tree, &item3.link);
   H2EQUAL_STRCMP("width=4,height=16,NI=20,3,0-3-CBA,1-3-CBA,2-3-CBA,3-3-CBA,4-3-CBA,5-3-CBA,6-3-CBA,7-3-CBA,8-3-CBA,9-3-CBA,10-3-CBA,11-3-CBA,12-3-CBA,13-3-CBA,14-3-CBA,15-2-BA,16-1-A,16-1-B0,15-1-C00,16-1-C00",
               hstree_tostring(&tree, 9));

   struct _test_item item4 = { { }, (unsigned char*) 0x7, 11, (unsigned char*) 0xD000, 17 };
   hstree_insert(&tree, &item4.link);
   H2EQUAL_STRCMP("width=4,height=16,NI=21,4,0-4-DCBA,1-4-DCBA,2-4-DCBA,3-4-DCBA,4-4-DCBA,5-4-DCBA,6-4-DCBA,7-4-DCBA,8-4-DCBA,9-4-DCBA,10-4-DCBA,11-4-DCBA,12-4-DCBA,13-4-DCBA,14-4-DCBA,15-2-BA,16-1-A,16-1-B0,15-2-DC00,16-1-C00,16-1-D000",
               hstree_tostring(&tree, 9));
   hstree_destroy(&tree);
}

H2CASE(hstreez, _insert_bound_0x0)
{
   hstree_tree tree, *res;
   res = hstree_create(&tree, 16, _key_hash, _value_hash, _key_cmp);

   struct _test_item item = { { }, (unsigned char*) 0x0, 11, (unsigned char*) 0xA, 17 };
   hstree_insert(&tree, &item.link);
   H2EQUAL_STRCMP("width=16,height=8,NI=9,1,0-1-A,1-1-A,2-1-A,3-1-A,4-1-A,5-1-A,6-1-A,7-1-A,8-1-A",
               hstree_tostring(&tree, 9));
   hstree_destroy(&tree);
}

H2CASE(hstreez, _insert_bound_0xF)
{
   hstree_tree tree, *res;
   res = hstree_create(&tree, 16, _key_hash, _value_hash, _key_cmp);

   struct _test_item item = { { }, (unsigned char*) 0xF, 11, (unsigned char*) 0xA, 17 };
   hstree_insert(&tree, &item.link);
   H2EQUAL_STRCMP("width=16,height=8,NI=9,1,0-1-A,1-1-A,2-1-A,3-1-A,4-1-A,5-1-A,6-1-A,7-1-A,8-1-A",
               hstree_tostring(&tree, 9));
   hstree_destroy(&tree);
}

H2CASE(hstreez, _insert_bound_0x10)
{
   hstree_tree tree, *res;
   res = hstree_create(&tree, 16, _key_hash, _value_hash, _key_cmp);

   struct _test_item item = { { }, (unsigned char*) 0x10, 11, (unsigned char*) 0xA, 17 };
   hstree_insert(&tree, &item.link);
   H2EQUAL_STRCMP("width=16,height=8,NI=9,1,0-1-A,1-1-A,2-1-A,3-1-A,4-1-A,5-1-A,6-1-A,7-1-A,8-1-A",
               hstree_tostring(&tree, 9));
   hstree_destroy(&tree);
}

H2CASE(hstreez, _insert_bound_0xffffffff)
{
   hstree_tree tree, *res;
   res = hstree_create(&tree, 16, _key_hash, _value_hash, _key_cmp);

   struct _test_item item = { { }, (unsigned char*) 0xFFFFFFFF, 11, (unsigned char*) 0xA, 17 };
   hstree_insert(&tree, &item.link);
   H2EQUAL_STRCMP("width=16,height=8,NI=9,1,0-1-A,1-1-A,2-1-A,3-1-A,4-1-A,5-1-A,6-1-A,7-1-A,8-1-A",
               hstree_tostring(&tree, 9));
   hstree_destroy(&tree);
}

H2CASE(hstreez, _remove)
{
   hstree_tree tree, *res;
   res = hstree_create(&tree, 16, _key_hash, _value_hash, _key_cmp);
   H2CHECK(res != NULL);

   struct _test_item item1 = { { }, (unsigned char*) 0x0, 11, (unsigned char*) 0xA, 17 };
   hstree_insert(&tree, &item1.link);
   struct _test_item item2 = { { }, (unsigned char*) 0x1, 11, (unsigned char*) 0xB0, 17 };
   hstree_insert(&tree, &item2.link);
   struct _test_item item3 = { { }, (unsigned char*) 0xF, 11, (unsigned char*) 0xC00, 17 };
   hstree_insert(&tree, &item3.link);
   struct _test_item item4 = { { }, (unsigned char*) 0x10, 11, (unsigned char*) 0xD000, 17 };
   hstree_insert(&tree, &item4.link);
   struct _test_item item5 = { { }, (unsigned char*) 0xFFFFFFFF, 11, (unsigned char*) 0xE0000, 17 };
   hstree_insert(&tree, &item5.link);

   H2EQUAL_STRCMP("width=16,height=8,NI=21,5,0-5-EDCBA,1-4-DCBA,2-4-DCBA,3-4-DCBA,4-4-DCBA,5-4-DCBA,6-4-DCBA,7-3-CBA,8-1-A,8-1-B0,8-1-C00,7-1-D000,8-1-D000,1-1-E0000,2-1-E0000,3-1-E0000,4-1-E0000,5-1-E0000,6-1-E0000,7-1-E0000,8-1-E0000",
               hstree_tostring(&tree, 9));

   hstree_remove(&tree, &item2.link);
   H2EQUAL_STRCMP("width=16,height=8,NI=20,4,0-4-EDC0A,1-3-DC0A,2-3-DC0A,3-3-DC0A,4-3-DC0A,5-3-DC0A,6-3-DC0A,7-2-C0A,8-1-A,8-1-C00,7-1-D000,8-1-D000,1-1-E0000,2-1-E0000,3-1-E0000,4-1-E0000,5-1-E0000,6-1-E0000,7-1-E0000,8-1-E0000",
               hstree_tostring(&tree, 9));
   hstree_remove(&tree, &item4.link);
   H2EQUAL_STRCMP("width=16,height=8,NI=18,3,0-3-E0C0A,1-2-C0A,2-2-C0A,3-2-C0A,4-2-C0A,5-2-C0A,6-2-C0A,7-2-C0A,8-1-A,8-1-C00,1-1-E0000,2-1-E0000,3-1-E0000,4-1-E0000,5-1-E0000,6-1-E0000,7-1-E0000,8-1-E0000",
               hstree_tostring(&tree, 9));
   hstree_remove(&tree, &item5.link);
   H2EQUAL_STRCMP("width=16,height=8,NI=10,2,0-2-C0A,1-2-C0A,2-2-C0A,3-2-C0A,4-2-C0A,5-2-C0A,6-2-C0A,7-2-C0A,8-1-A,8-1-C00",
               hstree_tostring(&tree, 9));
   hstree_remove(&tree, &item1.link);
   H2EQUAL_STRCMP("width=16,height=8,NI=9,1,0-1-C00,1-1-C00,2-1-C00,3-1-C00,4-1-C00,5-1-C00,6-1-C00,7-1-C00,8-1-C00",
               hstree_tostring(&tree, 9));
   hstree_remove(&tree, &item3.link);
   H2EQUAL_STRCMP("width=16,height=8,NI=1,0,0-0-0",
               hstree_tostring(&tree, 9));
   hstree_destroy(&tree);
}

H2CASE(hstreez, _gethash)
{
   hstree_tree tree, *res;
   res = hstree_create(&tree, 16, _key_hash, _value_hash, _key_cmp);
   H2CHECK(res != NULL);

   struct _test_item item1 = { { }, (unsigned char*) 0x1, 11, (unsigned char*) 0xA, 17 };
   hstree_insert(&tree, &item1.link);
   struct _test_item item2 = { { }, (unsigned char*) 0xF, 11, (unsigned char*) 0xB0, 17 };
   hstree_insert(&tree, &item2.link);
   struct _test_item item3 = { { }, (unsigned char*) 0x10, 11, (unsigned char*) 0xC00, 17 };
   hstree_insert(&tree, &item3.link);
   struct _test_item item4 = { { }, (unsigned char*) 0x17, 11, (unsigned char*) 0xD000, 17 };
   hstree_insert(&tree, &item4.link);
   struct _test_item item5 = { { }, (unsigned char*) 0xC1C2C3, 11, (unsigned char*) 0xE0000, 17 };
   hstree_insert(&tree, &item5.link);
   struct _test_item item6 = { { }, (unsigned char*) 0xFFFFFFFF, 11, (unsigned char*) 0xF00000, 17 };
   hstree_insert(&tree, &item6.link);

   /*8*/H2EQUAL_INTEGER(0x0, hstree_gethash(&tree, 0x0, 0x0));
   H2EQUAL_INTEGER(0xA, hstree_gethash(&tree, 0x1, 0x1));
   H2EQUAL_INTEGER(0xB0, hstree_gethash(&tree, 0xF, 0xF));
   H2EQUAL_INTEGER(0xC00, hstree_gethash(&tree, 0x10, 0x10));
   H2EQUAL_INTEGER(0xD000, hstree_gethash(&tree, 0x17, 0x17));
   H2EQUAL_INTEGER(0xE0000, hstree_gethash(&tree, 0xC1C2C3, 0xC1C2C3));
   H2EQUAL_INTEGER(0xF00000, hstree_gethash(&tree, 0xFFFFFFFF, 0xFFFFFFFF));
   /*7*/H2EQUAL_INTEGER(0xBA, hstree_gethash(&tree, 0x0, 0xF));
   H2EQUAL_INTEGER(0xF00000, hstree_gethash(&tree, 0xFFFFFFF0, 0xFFFFFFFF));
   /*6*/H2EQUAL_INTEGER(0xDCBA, hstree_gethash(&tree, 0x0, 0xFF));
   H2EQUAL_INTEGER(0xF00000, hstree_gethash(&tree, 0xFFFFFF00, 0xFFFFFFFF));
   /*5*/H2EQUAL_INTEGER(0xDCBA, hstree_gethash(&tree, 0x0, 0xFFF));
   H2EQUAL_INTEGER(0xF00000, hstree_gethash(&tree, 0xFFFFF000, 0xFFFFFFFF));
   /*4*/H2EQUAL_INTEGER(0xDCBA, hstree_gethash(&tree, 0x0, 0xFFFF));
   H2EQUAL_INTEGER(0xF00000, hstree_gethash(&tree, 0xFFFF0000, 0xFFFFFFFF));
   /*3*/H2EQUAL_INTEGER(0xDCBA, hstree_gethash(&tree, 0x0, 0xFFFFF));
   H2EQUAL_INTEGER(0xF00000, hstree_gethash(&tree, 0xFFF00000, 0xFFFFFFFF));
   /*2*/H2EQUAL_INTEGER(0xEDCBA, hstree_gethash(&tree, 0x0, 0xFFFFFF));
   H2EQUAL_INTEGER(0xF00000, hstree_gethash(&tree, 0xFF000000, 0xFFFFFFFF));
   /*1*/H2EQUAL_INTEGER(0xEDCBA, hstree_gethash(&tree, 0x0, 0xFFFFFFF));
   H2EQUAL_INTEGER(0xF00000, hstree_gethash(&tree, 0xF0000000, 0xFFFFFFFF));
   /*0*/H2EQUAL_INTEGER(0xFEDCBA, hstree_gethash(&tree, 0x0, 0xFFFFFFFF));

   hstree_destroy(&tree);
}

H2CASE(hstreez, _synchronize)
{
   struct _test_args ta = { 0, 0, 0 };
   hstree_tree remote_tree, local_tree;
   ta.local_tree = hstree_create(&local_tree, 4, _key_hash, _value_hash, _key_cmp);
   H2CHECK(ta.local_tree != NULL);
   ta.remote_tree = hstree_create(&remote_tree, 4, _key_hash, _value_hash, _key_cmp);
   H2CHECK(ta.remote_tree != NULL);

   struct _test_item item1 = { { }, (unsigned char*) 0x1, 11, (unsigned char*) 0xA, 17 };
   hstree_insert(&remote_tree, &item1.link);
   struct _test_item item2 = { { }, (unsigned char*) 0x2, 11, (unsigned char*) 0xB0, 17 };
   hstree_insert(&remote_tree, &item2.link);
   struct _test_item item3 = { { }, (unsigned char*) 0x4, 11, (unsigned char*) 0xC00, 17 };
   hstree_insert(&remote_tree, &item3.link);
   struct _test_item item4 = { { }, (unsigned char*) 0x7, 11, (unsigned char*) 0xD000, 17 };
   hstree_insert(&remote_tree, &item4.link);
   struct _test_item item5 = { { }, (unsigned char*) 0xFFFFFFFF, 11, (unsigned char*) 0xE0000, 17 };
   hstree_insert(&remote_tree, &item5.link);

   hstree_synchronize(&local_tree, _visitor, &ta);
   H2EQUAL_INTEGER(37, ta.notequals);
   hstree_destroy(&remote_tree);
   hstree_destroy(&local_tree);
}
