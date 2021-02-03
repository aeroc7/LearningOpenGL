#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <cstring>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

enum class LL {
    INFO,
    ERROR
};

#define LOG_MSG_HEAD(x) \
    do { \
        std::cout << '[' << __FILENAME__ << ':' << __LINE__ << "] "; \
        switch(x) { \
            case LL::INFO: \
                std::cout << "INFO: "; \
                break; \
            case LL::ERROR: \
                std::cout << "WARNING: "; \
                break; \
        }; \
    } while (0); \
    std::cout

#define LOG_MSG(x) LOG_MSG_HEAD(x)

#endif // _LOGGER_H_