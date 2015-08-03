#ifndef _UNITTEST_H_
#define _UNITTEST_H_

typedef struct testcase_t {
  int (* func)();
  char* name;
} TestCase;

int run_tests(int ntests, const TestCase** test_cases, char * suite_name);

#endif
