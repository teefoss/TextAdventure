#ifndef utility_h
#define utility_h

#define WRAP(n, min, max)   \
     if ( n < min) {        \
          n = max;          \
     } else if ( n > max) { \
          n = min;          \
     }

void Error(char * message, ...);
void StripExtension(char * path);

#endif /* utility_h */
