#ifndef OSDEP_H_
#define OSDEP_H_

#ifdef __linux__
#include <time.h>
#elif defined _WIN32 /* #ifdef __linux__ */
#include <pthread.h>
#endif /* #ifdef __linux__ */

namespace OSDep{

void gettime(struct timespec *ts);
void msleep(unsigned long msec);

} /* namespace OSDep */

#endif
