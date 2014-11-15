#ifndef H_STL
#define H_STL

#include "model.h"

#ifdef __cplusplus
extern "C" {
#endif

void writeSTLFile(FILE *out, model m);
model readSTLFile(const char *dir);

#ifdef __cplusplus
}
#endif
 
#endif
