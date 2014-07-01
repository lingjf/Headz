#include <string.h>
#include <setjmp.h>
#include <time.h>
#include "h2unit.h"

class h2unit_task *__h2unit_instance;

static jmp_buf _jmp_env;
static char _fail_message[2048];

h2unit_task::h2unit_task()
{
   case_list = NULL;
   case_here = NULL;
   case_count = 0;
   case_failed = 0;
   case_passed = 0;
   check_failed = 0;
   check_passed = 0;
}

void h2unit_task::install(class h2unit_case *c)
{
   c->next_case = case_list;
   case_list = c;
   case_count++;
}

void h2unit_task::run(bool verbose)
{
   case_here = case_list;
   long total_starts = time(NULL);
   printf("\n");
   while (case_here) {
      bool failed;
      long case_starts = time(NULL);
      case_here->setup();
      if (!setjmp(_jmp_env)) {
         case_here->testcase();
         case_passed++;
         failed = 0;
      }
      else {
         case_failed++;
         failed = 1;
      }
      case_here->teardown();
      if (failed) {
         printf("\033[1;35m");
         printf("\rH2CASE(%s, %s): Failed at %s:%d\n", case_here->suite_name, case_here->case_name,
                source_file, source_line);
         printf("\033[1;31m");
         printf("%s\n", _fail_message);
      }
      else {
         if (verbose) {
            printf("\033[0;34m");
            printf("\rH2CASE(%s, %s): Passed - %ld seconds                            \n",
                   case_here->suite_name, case_here->case_name, time(NULL) - case_starts);
         }
      }
      printf("\033[1;34m");
      printf("\rH2UNIT running ... %d%% completed.", (int) (100.0 * (case_passed + case_failed) / case_count));
      case_here = case_here->next_case;
   }
   if (case_failed > 0) {
      printf("\033[1;31m");
      printf("\rH2UNIT Error <%d failure, %d cases, %d checks, %ld seconds>\n", case_failed, case_count,
             check_failed + check_passed, time(NULL) - total_starts);
   }
   else {
      printf("\033[1;32m");
      printf("\rH2UNIT Passed <%d cases, %d checks, %ld seconds>\n", case_count, check_failed + check_passed,
             time(NULL) - total_starts);
   }
   printf("\033[0m");
   printf("\n");
}

void h2unit_task::position(char *file, int line)
{
   source_file = file;
   source_line = line;
}

bool h2unit_task::real(bool r)
{
   if (r) {
      check_passed++;
   }
   else {
      check_failed++;
   }
   return r;
}

void h2unit_task::check(char *condition, bool result)
{
   if (!real(result)) {
      sprintf(_fail_message, "   H2CHECK(%s) failed", condition);
      longjmp(_jmp_env, 1);
   }
}

void h2unit_task::equal(int expected, int actual)
{
   if (!real(expected == actual)) {
      sprintf(_fail_message, "   expected is <%d 0x%x>\n   actually is <%d 0x%x>", expected, expected, actual,
              actual);
      longjmp(_jmp_env, 1);
   }
}

void h2unit_task::equal(double expected, double actual, double threshold)
{
   double delta = expected - actual;
   if (delta < 0) delta = -delta;
   if (!real(delta < threshold)) {
      sprintf(_fail_message, "   expected is <%f>\n   actually is <%f>", expected, actual);
      longjmp(_jmp_env, 1);
   }
}

void h2unit_task::equal(char *expected, char *actual)
{
   if (!real(strcmp(expected, actual) == 0)) {
      sprintf(_fail_message, "   expected is <%s>\n   actually is <%s>", expected, actual);
      longjmp(_jmp_env, 1);
   }
}

void h2unit_task::equal_strcmp_nocase(char *expected, char *actual)
{
   if (!real(strcasecmp(expected, actual) == 0)) {
      sprintf(_fail_message, "   expected is <%s>\n   actually is <%s>", expected, actual);
      longjmp(_jmp_env, 1);
   }
}

void h2unit_task::regex(void *express, void *actual)
{
   sprintf(_fail_message, "   Regular Expression is not supported!");
   longjmp(_jmp_env, 1);
}

void h2unit_task::equal(unsigned char *expected, unsigned char *actual, int length)
{
   if (!real(memcmp(expected, actual, length) == 0)) {
      sprintf(_fail_message, "   expected is <>\n   actually is <>");
      longjmp(_jmp_env, 1);
   }
}

h2unit_case::h2unit_case()
{
   suite_name = NULL;
   case_name = NULL;
   if (!__h2unit_instance) {
      __h2unit_instance = new h2unit_task();
   }
   __h2unit_instance->install(this);
}

int main(int argc, char **argv)
{
   bool verbose = 0;
   if (argc > 1) {
      if (strstr(argv[1], "-v")) verbose = 1;
   }
   __h2unit_instance->run(verbose);
}

