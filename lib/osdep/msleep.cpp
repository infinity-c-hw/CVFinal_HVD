#include "osdep.h"

#ifdef __linux__
#include <unistd.h>
#elif defined _WIN32
#include <windows.h>
#endif

namespace OSDep {

void msleep(unsigned long msec)
{
#ifdef __linux__
	usleep(msec * 1000);
#elif defined _WIN32
	Sleep(msec);
#endif
}

} /* namespace OSDep */
