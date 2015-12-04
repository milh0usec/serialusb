/*
 Copyright (c) 2015 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>
#include <unistd.h>

#include <proxy.h>
#include <GE.h>

volatile int done = 0;

static void terminate(int sig) {
  done = 1;
}

#ifdef WIN32
BOOL WINAPI ConsoleHandler(DWORD dwType) {
  switch(dwType) {
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:

    done = 1; //signal the main thread to terminate

    //Returning would make the process exit!
    //We just make the handler sleep until the main thread exits,
    //or until the maximum execution time for this handler is reached.
    Sleep(10000);

    return TRUE;
    default:
    break;
  }
  return FALSE;
}
#endif

int main(int argc, char * argv[]) {

  char * port = NULL;

  if (!GE_initialize()) {
    fprintf(stderr, "GE_initialize failed\n");
    return -1;
  }

  (void) signal(SIGINT, terminate);
  (void) signal(SIGTERM, terminate);

  if (argc > 1) {
    port = argv[1];
  }

  int ret = proxy_init(port);
  if (port != NULL && ret == 0) {

    GE_TimerStart(10000);

    while (!done) {

      GE_PumpEvents();
    }

    GE_TimerClose();
  }

  proxy_stop();

  GE_quit();

  printf("Exiting\n");

  return ret;
}
