#ifndef _ARBITRATOR_LOGGING_H
#define _ARBITRATOR_LOGGING_H

#include <stdio.h>

// Set to log level of arbitrator.
// When 0, no logging is captured.
extern int logging_level;

// Set to stdout, stderr, or capture file.
extern FILE *stream;


typedef enum {
    DEBUG = 0,
    INFO
} loglevel_e;


//performance hit?
//fflush(stream);

#define INFO(...)                       \
do {                                    \
    if (logging_level <= INFO) {        \
        fprintf(stream, __VA_ARGS__);   \
        fprintf(stream, "\n");          \
    }                                   \
} while(0)


#define DEBUG(...)                      \
do {                                    \
    if (logging_level <= DEBUG) {       \
        fprintf(stream, "%s:%d:\t",     \
                __FILE__,               \
                __LINE__);              \
        fprintf(stream, __VA_ARGS__);   \
        fprintf(stream, "\n");          \
    }                                   \
} while(0)


#endif /* _ARBITRATOR_LOGGING_H*/
