#ifndef FORTH_INTER_H
#define FORTH_INTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"

char* forth_inter_token(char** buf);

#ifdef __cplusplus
}
#endif
#endif