/* SignalHandling.c(pp):                               HDO, 2019-02-13
   --------------------
   Simple utility to catch all signals.
======================================================================*/

#ifndef __cplusplus
  #include <stdio.h>
  #include <stdlib.h>
  #include <signal.h>
#else
  #include <cstdio>
  #include <cstdlib>
  #include <csignal>
  #include <iostream>
  using namespace std;
#endif

#include "SignalHandling.h"

#ifdef __cplusplus
extern "C" {
#endif


void printMessageAndExit(int sig, const char *sigName) {
#ifndef __cplusplus
  fprintf(stderr, "ERROR: signal %d (%s) raised\n", sig, sigName);
#else
  cerr << "ERROR: signal " << sig << " (" << sigName << ") raised" << endl;
#endif
  exit(sig); /*may call any function installed by atexit*/
} /*printMessageAndExit*/

void signalHandler(int sig) { /*see catch (…) in C++*/
  switch (sig) {            /*breaks below to suppress warnings*/
    case SIGABRT: printMessageAndExit(SIGABRT, "SIGABRT"); break;
    case SIGFPE:  printMessageAndExit(SIGFPE,  "SIGFPE");  break;
    case SIGILL:  printMessageAndExit(SIGILL,  "SIGILL");  break;
    case SIGINT:  printMessageAndExit(SIGINT,  "SIGINT");  break;
    case SIGSEGV: printMessageAndExit(SIGSEGV, "SIGSEGV"); break;
    case SIGTERM: printMessageAndExit(SIGTERM, "SIGTERM"); break;
    default:      printMessageAndExit(sig,     "UNKNOWN SIGNAL");
  } /*switch*/
} /*signalHandler*/

void installSignalHandlers() {
  signal(SIGABRT, signalHandler); /*abort: abnormal termination*/
  signal(SIGFPE,  signalHandler); /*floating point error*/
  signal(SIGILL,  signalHandler); /*illegal instruction*/
  signal(SIGINT,  signalHandler); /*external interrupt*/
  signal(SIGSEGV, signalHandler); /*segmentation violation*/
  signal(SIGTERM, signalHandler); /*termination request sent*/
} /*installSignalHandlers*/


#ifdef __cplusplus
} // extern
#endif


/* === test ==========================================================*/

#if 0

int main(int argc, char *argv[]) {

  int  signalCase;
  int  x;
  int* p;

  installSignalHandlers();

  signalCase = 0;
  switch (signalCase) {
    case 0:
      abort(); /*same as raise(SIGABRT);*/
      break;
    case 1:
      x = 0;
      x = (x + 1) / x; /*division by zero raises SIGFPE with gcc, but cl does not ..*/
      break;
    case 2:
      /*SIGILL is typically raised when incorrect code is executed,
          but correct compilers don't generate illegal code*/
      break;
    case 3:
      while (1) { /*terminating endless loop with CTRL-C raises SIGINT*/
        x = x + 1;
        x = x - 1;
      } /*while*/
      break;
    case 4:
      p = NULL;
      *p = 0; /*dereferencing null pointer raises SIGSEGV*/
      break;
    default:
#ifndef __cplusplus
     printf("invalid signal case\n");
#else
     cout << "invalid signal case" << endl;
#endif
  } /*switch*/

#ifndef __cplusplus
  printf("no signal raised\n");
#else
  cout << "no signal raised" << endl;
#endif

  return 0;

} /*main*/

#endif


/*end of SignalHandling.cpp
======================================================================*/
