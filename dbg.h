#ifndef __dbg__h
#define __dbg__h 1

#include <stdio.h>
#include <errno.h>

#define  check_mem(A) if(!(A)) { fprintf(stderr, "Memory error!\n"); return NULL; }

#define check_file(A) if(!(A)) { fprintf(stderr, "Couldn't open file\n"); return NULL; }

#endif
