#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <cstring>
#include <string>

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

#define UNUSED(a) (void)(a)

#define COND_UNLIKELY(x) __builtin_expect(x, 0)

#define ASSERT(x) \
	do { \
		if (COND_UNLIKELY(!(x))) { \
			LOG_MSG(LL::ERROR) << "Assertion " + std::string(#x) + " failed" << '\n'; \
			abort(); \
		} \
	} while (0)

#define QUICK_EXIT(x) \
	do { \
		LOG_MSG(LL::ERROR) << "Aborting program; " << x << '\n'; \
		abort(); \
	} while (0)

#endif // _LOGGER_H_