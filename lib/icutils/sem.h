#ifndef BINSEM_H_
#define BINSEM_H_

#include <climits>
#include <pthread.h>

namespace ICutils {

/** @brief Semaphore implementation.
 *
 * I took wxWidgets wxSemaphore class as an example when designing this class.
 */
class Semaphore {
	private:
		unsigned long max; /**< Maximum resource availability. */
		unsigned long resources; /**< Current resource count. */

		pthread_mutex_t m_cv; /**< Underlying synchronization tool. */
		pthread_cond_t cv; /**< Underlying synchronization tool. */

	public:
		/** @brief Constructor.
		 *
		 * @param[in] sem_initval initial resource availability
		 * @param[in] sem_maxval maximum resource availability
		 */
		Semaphore(unsigned long sem_initval = 0, unsigned long sem_maxval = ULONG_MAX);

		/** @brief Destructor.
		 */
		~Semaphore(void);

		/** @brief Wait for one resource.
		 *
		 * This variant will wait forever.
		 */
		void Wait(void);

		/** @brief Wait for one resource.
		 *
		 * If one resource is not available, it returns false immediately.
		 *
		 * @return Return true if one resource is available.  Otherwise, it
		 * 			returns false.
		 */
		bool TryWait(void);

		/** @brief Wait for one resource.
		 *
		 * It waits for one resource for to_msecs milliseconds.  If one
		 * resource is not available and the wait timed out, it returns false.
		 *
		 * @return Return true is one resource is available.  Otherwise, it
		 * 			returns false.
		 */
		bool TimedWait(unsigned long to_msecs);

		/** @brief Release one resource.
		 */
		void Post(void);
};

} /* namespace ICutils */

#endif
