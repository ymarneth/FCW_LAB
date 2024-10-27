/* SignalHandling.h:                                    HDO, 2019-02-13
   ----------------
   Simple utility to catch all signals.
======================================================================*/

#ifndef SignalHandling_h
#define SignalHandling_h


#ifdef __cplusplus
extern "C" {
#endif

/*installs signal handlers for following signals:
    SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV and SIGTERM*/
void installSignalHandlers();

#ifdef __cplusplus
} // extern
#endif


#endif

/*end of SignalHandling.h
======================================================================*/
