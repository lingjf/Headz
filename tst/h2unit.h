#ifndef __H2UNIT_H__
#define __H2UNIT_H__

#include <stdio.h>

class h2unit_case;
class h2unit_task;

extern class h2unit_task *__h2unit_instance;

class h2unit_task
{
public:
   class h2unit_case *case_list;
   class h2unit_case *case_here;
   int case_count;
   int case_failed;
   int case_passed;
   int check_failed;
   int check_passed;
   char *source_file;
   int source_line;
public:
   h2unit_task();
   void install(class h2unit_case *c);
   void run(bool verbose);
   bool real(bool result);
   void position(char *file, int line);
   void check(char *condition, bool result);
   void equal(int expected, int actual);
   void equal(double expected, double actual, double threshold);
   void equal(char *expected, char *actual);
   void equal(unsigned char *expected, unsigned char *actual, int length);
   void equal_strcmp_nocase(char *expected, char *actual);
   void regex(void *express, void *actual);
};

class h2unit_case
{
public:
   class h2unit_case *next_case;
   const char *suite_name;
   const char *case_name;
public:
   h2unit_case();
   virtual void setup()
   {
   }
   virtual void teardown()
   {
   }
   virtual void testcase() = 0;
};

#define H2SUITE(suitename)  \
   struct h2unit_##suitename##_suite: public h2unit_case

#define H2CASE(suitename, casename)  \
   class h2unit_##suitename##_suite_##casename##_case:         \
         public h2unit_##suitename##_suite                     \
   {                                                           \
   public:                                                     \
      h2unit_##suitename##_suite_##casename##_case()           \
      {                                                        \
         this->suite_name  = #suitename;                       \
         this->case_name = #casename;                          \
      }                                                        \
      void testcase();                                         \
   };                                                          \
   class h2unit_##suitename##_suite_##casename##_case          \
         __##suitename##_suite_##casename##_case_instance_;    \
   void h2unit_##suitename##_suite_##casename##_case::testcase()


#define H2CHECK(condition)                                     \
   do {                                                        \
      __h2unit_instance->position(__FILE__, __LINE__);         \
      __h2unit_instance->check(#condition, (condition));       \
   } while(0)

#define H2EQUAL(...)                                           \
   do {                                                        \
      __h2unit_instance->position(__FILE__, __LINE__);         \
      __h2unit_instance->equal(__VA_ARGS__);                   \
   } while(0)

#define H2EQUAL_INTEGER(expected, actual)                      \
   do {                                                        \
      __h2unit_instance->position(__FILE__, __LINE__);         \
      __h2unit_instance->equal((int)expected, (int)actual);    \
   } while(0)

#define H2EQUAL_DOUBLE(expected, actual, threshold)            \
   do {                                                        \
      __h2unit_instance->position(__FILE__, __LINE__);         \
      __h2unit_instance->equal((double)expected, (double)actual, (double)threshold); \
   } while(0)

#define H2EQUAL_STRCMP(expected, actual)                       \
   do {                                                        \
      __h2unit_instance->position(__FILE__, __LINE__);         \
      __h2unit_instance->equal((char*)expected, (char*)actual);\
   } while(0)

#define H2EQUAL_STRCMP_NOCASE(expected, actual)                               \
   do {                                                                       \
      __h2unit_instance->position(__FILE__, __LINE__);                        \
      __h2unit_instance->equal_strcmp_nocase((char*)expected, (char*)actual); \
   } while(0)

#define H2EQUAL_MEMCMP(expected, actual, length)                                 \
   do {                                                                          \
      __h2unit_instance->position(__FILE__, __LINE__);                           \
      __h2unit_instance->equal((unsigned char*)expected, (unsigned char*)actual, (int)length);   \
   } while(0)

#define H2REGEX(expected, actual)                                 \
   do {                                                           \
      __h2unit_instance->position(__FILE__, __LINE__);            \
      __h2unit_instance->regex((char*)expected, (char*)actual);   \
   } while(0)


//  CppUTest adaptation
#define TEST_GROUP H2SUITE
#define TEST_SETUP() void setup()
#define TEST_TEARDOWN() void teardown()
#define TEST H2CASE
#define CHECK H2CHECK
#define CHECK_TRUE H2CHECK
#define CHECK_FALSE(condition) H2CHECK(!(condition))
#define CHECK_EQUAL H2EQUAL_INTEGER
#define STRCMP_EQUAL H2EQUAL_STRCMP
#define STRCMP_NOCASE_EQUAL H2EQUAL_STRCMP_NOCASE
#define LONGS_EQUAL H2EQUAL_INTEGER
#define BYTES_EQUAL H2EQUAL_INTEGER
#define POINTERS_EQUAL H2EQUAL_INTEGER
#define DOUBLES_EQUAL H2EQUAL_DOUBLE

#endif
