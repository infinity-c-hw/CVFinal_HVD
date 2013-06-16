#include <cstring>
#include <errno.h>

#include "sem.h"

namespace ICutils {

Semaphore::Semaphore(unsigned long sem_initval, unsigned long sem_maxval)
{
	resources = sem_initval;
	max = sem_maxval;
	pthread_mutex_init(&m_cv, NULL);
	pthread_cond_init(&cv, NULL);
}

Semaphore::~Semaphore(void)
{
	pthread_mutex_destroy(&m_cv);
	pthread_cond_destroy(&cv);
}

void Semaphore::Wait(void)
{
	pthread_mutex_lock(&m_cv);

	while (resources == 0)
		pthread_cond_wait(&cv, &m_cv);

	resources--;
	pthread_mutex_unlock(&m_cv);
}

bool Semaphore::TryWait(void)
{
	bool success = true;

	pthread_mutex_lock(&m_cv);

	if (resources == 0) {
		success = false;
	} else {
		resources--;
		success = true;
	}

	pthread_mutex_unlock(&m_cv);
	return success;
}

bool Semaphore::TimedWait(unsigned long to_msecs)
{
	int ret = 0;
	bool success = true;
	unsigned long to_nsecs = to_msecs * 1000000;
	struct timespec ts_to;

	memset(&ts_to, 0, sizeof(ts_to));

	clock_gettime(CLOCK_REALTIME, &ts_to);
	if (ts_to.tv_nsec + to_nsecs >= 1000000000) {
		ts_to.tv_sec += 1;
		to_nsecs -= 1000000000;
	}
	ts_to.tv_nsec += to_nsecs;

	pthread_mutex_lock(&m_cv);

	while (resources == 0) {
		ret = pthread_cond_timedwait(&cv, &m_cv, &ts_to);
		if (ret == ETIMEDOUT) {
			success = false;
			break;
		} else {
			resources--;
			success = true;
			break;
		}
	}

	pthread_mutex_unlock(&m_cv);
	return success;
}

void Semaphore::Post(void)
{
	pthread_mutex_lock(&m_cv);

	if (resources == max)
		goto out;

	if (resources == 0)
		pthread_cond_signal(&cv);
	resources++;

out:
	pthread_mutex_unlock(&m_cv);
}

} /* namespace ICutils */
