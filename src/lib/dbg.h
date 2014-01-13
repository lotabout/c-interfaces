#ifndef DBG_H
#define DBG_H

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
    #define debug_print(fmt, args...) 
#else
    #define debug_print(fmt, args...) \
        fprintf(stderr, "[DEBUG] %s:%d: "fmt, __FILE__, __LINE__, ##args);
#endif

#define log_print(fmt, args...) \
    fprintf(stderr, "(%s:%d:) " fmt, __FILE__, __LINE__, ##args);

#define log_err(fmt, args...) \
    fprintf(stderr, "[ERROR] (%s:%d:) " fmt, __FILE__, __LINE__, ##args);

#define log_warn(fmt, args...) \
    fprintf(stderr, "[WARN] (%s:%d:) " fmt, __FILE__, __LINE__, ##args);

#define log_info(fmt, args...) \
    fprintf(stderr, "[INFO] (%s:%d:) " fmt, __FILE__, __LINE__, ##args);

#endif /* end of include guard: DBG_H */
