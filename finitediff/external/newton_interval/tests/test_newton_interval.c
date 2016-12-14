#include "newton_interval.h"
#include "unittest.h"

#include <stdio.h>

int test1(){
  const int nt = 4;
  double t[] = {0.0,1.0,2.0,3.0};
  double treq = 0.5;
  int i=-2;
  i=(int)get_interval(t, nt, treq);
  return (i == 0);
}

int test2(){
  const int nt = 2;
  double t[] = {0.0,1.0};
  double treq = 0.5;
  int i=-2;
  i=(int)get_interval(t, nt, treq);
  return (i == 0);
}

int test3(){
  const int nt = 3;
  double t[] = {0.0,1.0,2.0};
  double treq = 0.9;
  int i=-2;
  i=(int)get_interval(t, nt, treq);
  return (i == 0);
}

int test4(){
  const int nt = 3;
  double t[] = {0.0,1.0,2.0};
  double treq = -0.9;
  int i=-2;
  i=(int)get_interval(t, nt, treq);
  return (i == -1);
}

int test5(){
  const int nt = 50;
  double t[nt];
  double treq = 10.0;
  int j;
  for(j = 0; j<nt; ++j)
    t[j] = j*treq/(nt-1);
  int i = -2;
  int ti = nt-2;
  i=get_interval_from_guess(t, nt, treq, ti);
  return (i==49);
}

#define NTESTS 5
static const TestCase t1 = {test1, "test1"};
static const TestCase t2 = {test2, "test2"};
static const TestCase t3 = {test3, "test3"};
static const TestCase t4 = {test4, "test4"};
static const TestCase t5 = {test5, "test5"};
static const TestCase* test_cases[NTESTS] = {&t1, &t2, &t3, &t4, &t5};

int main(int argc, char ** argv){
  return run_tests(NTESTS, test_cases, argv[0]);
}
