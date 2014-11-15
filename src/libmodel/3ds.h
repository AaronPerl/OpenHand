#ifndef H_3DS
#define H_3DS

#include "model.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct chunk
{
	int ID;
	long dataLength;
	long start;
} chunk;

model read3DSFile(const char *dir);

#ifdef __cplusplus
}
#endif


#endif
