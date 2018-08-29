#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "BasicServer"
#endif


#include <limits>
#include <ctime>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "exsystemutil.h"
#include "exlog.h"

namespace vh1981lib {
	namespace timeUtils {

		unsigned int tick() {
			struct timespec tp;
			clock_gettime(CLOCK_MONOTONIC, &tp);
			return static_cast<unsigned int>(tp.tv_sec) * 1000 + static_cast<unsigned int>(tp.tv_nsec) / 1000000;
		}

		unsigned int diff(unsigned int previous, unsigned int later)
		{
			return (previous <= later) ? later - previous : (std::numeric_limits<unsigned int>::max() - previous) + later + 1;
		}
	}
}
