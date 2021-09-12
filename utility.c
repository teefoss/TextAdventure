#include "utility.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void Error(char * message, ...)
{
    va_list argptr;
    
    va_start(argptr, message);
    vfprintf(stderr, message, argptr);
    va_end(argptr);
    printf("\n");
    exit(EXIT_FAILURE);
}


void StripExtension(char * path)
{
    int length = strlen(path) - 1;
    
    while ( length > 0 && path[length] != '.' )
        length--;
    
    if ( length )
        path[length] = '\0';
}
