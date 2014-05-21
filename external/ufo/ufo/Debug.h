#ifndef UFO_DEBUG_H
#define UFO_DEBUG_H

#if defined(DEBUG) 

#include <stdio.h>
#include <assert.h>

#define PRINT(...) fprintf(stderr,__VA_ARGS__)

#else /* DEBUG not defined */

#define PRINT(...)

#if not defined(assert)
#define assert(c)
#endif

#endif /* if defined DEBUG */

#endif // UFO_DEBUG_H

