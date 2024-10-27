/* Timer.c(pp)                                          HDO, 1998-2020
   ----------
   Simple utility to measure run-times for C and C++.
======================================================================*/

#ifndef __cplusplus
  #include <time.h>
#else
  #include <chrono>
  #include <ctime>
  using namespace std;
#endif

#include "Timer.h"

#ifdef __cplusplus
  #define HIGH_RESOLUTION_TIMING  /*high resolution timing?*/
#endif

/*definition of globals used in all functions*/
#ifdef HIGH_RESOLUTION_TIMING
  static chrono::high_resolution_clock::time_point start_tp;
  static chrono::high_resolution_clock::time_point stop_tp;
#else
  static clock_t ticks; /*typically 1000 ticks per second*/
#endif


void startTimer() {
  #ifdef HIGH_RESOLUTION_TIMING
    start_tp = chrono::high_resolution_clock::now();
  #else
    ticks = clock();
  #endif
} /*startTimer*/

void stopTimer() {
  #ifdef HIGH_RESOLUTION_TIMING
    stop_tp = chrono::high_resolution_clock::now();
  #else
    ticks = clock() - ticks;
  #endif
} /*stopTimer*/

double elapsed() {
  #ifdef HIGH_RESOLUTION_TIMING
    return
      (double)chrono::duration_cast<chrono::milliseconds>(
        stop_tp - start_tp).count() / 1000.0;
  #else
    return (double)ticks / CLOCKS_PER_SEC;
  #endif
} /*elapsed*/


/* === test ==========================================================*/

#if 0

#ifndef __cplusplus
  #include <stdio.h>
#else
  #include <iostream>
  using namespace std;
#endif

int main(int argc, char *argv[]) {

  int runs, r, n, i, j;
  runs = 5; /*number of runs with ...*/
  n = 5000; /*... n*n comutations each*/

#ifndef __cplusplus
    printf("C version, ");
#else
    cout << "C++ version, ";
#endif
#ifdef HIGH_RESOLUTION_TIMING
  cout << "results for HIGH resolution timing:" << endl;
#else
  #ifndef __cplusplus
    printf("results for LOW resolution timing:\n");
  #else
    cout << "results for LOW resolution timing:" << endl;
  #endif
#endif
  for (r = 0; r < 5; r++) {
    startTimer();
    for (i = 0; i < n; i++)
      for (j = 0; j < n; j++) {
        i = i + 17;
        j = j + 4;
        i = i - 17;
        j = j - 4;
      } /*for*/
    stopTimer();
#ifndef __cplusplus
    printf("%f\n", elapsed());
#else
    cout << elapsed() << std::endl;
#endif
  } /*for*/
  return 0;

} /*main*/

#endif


/*end of Timer.c(pp)
======================================================*/
