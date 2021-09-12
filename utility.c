#include "utility.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

void Error(char * message, ...)
{
    va_list argptr;
    
    va_start(argptr, message);
    vfprintf(stderr, message, argptr);
    va_end(argptr);
    printf("\n");
    exit(EXIT_FAILURE);
}
