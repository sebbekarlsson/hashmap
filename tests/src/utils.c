#include "include/utils.h"
#include "include/colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define MSG() printf(BOLDWHITE "> %s\n" RESET, msg)

#define OK() printf(GREEN "\t[OK]\n" RESET)

#define FAIL() printf(RED "\t[FAIL]\n" RESET)

unsigned int _assert(unsigned int condition, const char* msg)
{
  MSG();

  if (condition) {
    OK();
    return 1;
  }

  FAIL();
  exit(1);
  return 0;
}

unsigned long now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}
