#include "newton_interval.h"
#include "unittest.h"

#include <stdio.h>

int test1(){
  int i;
  const int n = 4;
  double arr[] = {0.0, 1.0, 2.0, 3.0};
  i = check_nan(arr, n);
  return (i == -1);
}

int test2(){
  int i;
  const int n = 4;
  const double nan = (double)0.0/(double)0.0;
  double arr[] = {0.0, 1.0, nan, 3.0};
  i = check_nan(arr, n);
  return (i == 2);
}



#define NTESTS 2
static const TestCase t1 = {test1, "test1"};
static const TestCase t2 = {test1, "test2"};
static const TestCase* test_cases[NTESTS] = {&t1, &t2};

int main(int argc, char ** argv){
  return run_tests(NTESTS, test_cases, argv[0]);
}
