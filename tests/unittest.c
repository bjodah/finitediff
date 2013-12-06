#include <stdio.h>
#include "unittest.h"

int run_tests(int ntests, const TestCase** test_cases, char * suite_name){
  int exit_status=0, result, i;
  TestCase cur_test;
  for (i=0; i<ntests; ++i){
    cur_test = *test_cases[i];
    result = (*cur_test.func)();
    if (result == 1){
      printf("In %s, %s%c%s passed %c%s\n", suite_name,
	     cur_test.name, 0x1B, "[32;1m",
	     0x1B, "[30;0m");
    } 
    else{
      exit_status = 1;
      printf("In %s, %s%c%s failed %c%s with result %d\n", suite_name, 
	     cur_test.name, 0x1B, "[31;1m",
	     0x1B, "[30;0m", result);
    }
  }
  return exit_status;
}
