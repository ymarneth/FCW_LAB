/* Timer.h                                                HDO, 1998-2020
   -------
   Simple utility to measure run-times for C and C++.
======================================================================*/

#ifndef Timer_h
#define Timer_h


#ifdef __cplusplus
  extern "C" {
#endif

void startTimer();
void stopTimer();
double elapsed();    /*returns elapsed time in seconds*/

#ifdef __cplusplus
  } // C
#endif


#endif

/*end of Timer.h
======================================================================*/
