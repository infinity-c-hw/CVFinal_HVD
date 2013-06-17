#include "osdep.h"

#include <cstring>

#ifdef _WIN32
#include <sys/timeb.h>
#endif

namespace OSDep {

void gettime(struct timespec *ts)
{
	memset(ts, 0, sizeof(*ts));
#ifdef __linux__
	clock_gettime(CLOCK_REALTIME, ts);
#elif _WIN32
	struct _timeb tb;
	_ftime(&tb);
	ts->tv_sec = tb.time;
	ts->tv_nsec = tb.millitm * 1000000;
#endif
}

} /* namespace OSDep */
