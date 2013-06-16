#include "thread_runner.h"

namespace ICutils {

ThreadRunner::ThreadRunner(void)
{
	run_flag = false;
}

ThreadRunner::~ThreadRunner(void)
{
	Stop();
}

struct th_exchg {
	pthread_mutex_t m;
	pthread_cond_t cv;
	ThreadRunner *tr;
};

void *ThreadRunner::th_entry(void *args)
{
	struct th_exchg *exchg = (struct th_exchg *)args;
	ThreadRunner *tr = NULL;

	pthread_mutex_lock(&exchg->m);
	tr = exchg->tr;
	tr->run_flag = true;
	pthread_cond_signal(&exchg->cv);
	pthread_mutex_unlock(&exchg->m);

	tr->th_worker();
	pthread_exit(NULL);
	return NULL;
}

bool ThreadRunner::Start(void)
{
	struct th_exchg exchg;

	if (run_flag)
		return false;

	pthread_mutex_init(&exchg.m, NULL);
	pthread_cond_init(&exchg.cv, NULL);
	exchg.tr = this;

	pthread_mutex_lock(&exchg.m);
	pthread_create(&thid, NULL, th_entry, (void *)&exchg);
	pthread_cond_wait(&exchg.cv, &exchg.m);
	pthread_mutex_unlock(&exchg.m);

	pthread_mutex_destroy(&exchg.m);
	pthread_cond_destroy(&exchg.cv);

	if (!run_flag)
		return false;
	return true;
}

bool ThreadRunner::Stop(void)
{
	if (!run_flag)
		return false;

	run_flag = false;
	pthread_join(thid, NULL);
	return true;
}

} /* namespace ICutils */
