#ifndef THREAD_RUNNER_H_
#define THREAD_RUNNER_H_

#include <pthread.h>

namespace ICutils {

/** @brief Thread runner class to help simplify thread implementation.
 *
 * It abstracts away complexity between pthread and class.
 *
 * It is an abstract base class.  You must inherite it and implement the
 * worker function.
 *
 * Call Start() starts thread.  In high level view, when you called Start(),
 * th_worker() is executed and put into another thread.
 *
 * If you want the thread to be stoppable, you need to periodically check for
 * run_flag value.  Stop() call only sets run_flag to false, and it waits
 * silently for thread to end itself.  So if you ignore run_flag, you'll not
 * be able to stop the thread using Stop().
 */
class ThreadRunner {
	private:
		/** @brief Thread entry function.
		 *
		 * This is the real pthread entry point.  It will call th_worker()
		 * after initialization, and end when th_worker() returns.
		 */
		static void *th_entry(void *args);
		pthread_t thid; /**< Thread ID. */

		/** @brief Thread worker function.
		 *
		 * This is what programmer expects to be run in another thread.
		 *
		 * You must implement this function.
		 */
		virtual void th_worker(void) = 0;

	protected:
		/** @brief Thread running flag.
		 *
		 * It will be set to false when Stop() is called.
		 */
		bool run_flag;

	public:
		/** @brief Constructor.
		 */
		ThreadRunner(void);

		/** @brief Destructor.
		 */
		virtual ~ThreadRunner(void);

		/** @brief Start thread execution.
		 */
		bool Start(void);

		/** @brief Stop thread execution.
		 */
		bool Stop(void);
};

} /* namespace ICutils */

#endif
