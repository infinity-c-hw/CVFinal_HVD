#ifndef DATASYNC_H_
#define DATASYNC_H_

#include <pthread.h>
#include "sem.h"

/** @brief Custom utility namespace.
 */
namespace ICutils {

/** @brief Data exchanging and synchronization.
 *
 * A shared buffer implementation with simple wait/signal mechanism.
 *
 * Data will be protected with mutex lock while being read/written.
 *
 * You can use the simple wait/signal mechanism to notify about data
 * availability.  The underlying synchronization tool is semaphore, so if
 * Signal() goes faster than Wait(), Wait() will return immediately.
 *
 * If you're going to feed a class into template, you must implement
 * assignment operator, as DataSync will assign data directly without trying
 * to understand the underlying data structure (it is impossible anyway).
 * Otherwise, you'll need to specialize the SetData() and GetData() method
 * by yourself.
 *
 * Also, if you want to specialize your own data access functions, please
 * remember to wrap your data access with mutex lock, or the object losts
 * its meaning.
 */
template <typename T>
class DataSync {
	private:
		T data; /**< Data field. */
		pthread_mutex_t m_data; /**< Data field mutex lock. */

		Semaphore sem; /**< Semaphore used to perform synchronization. */

	public:
		/** @brief Constructor.
		 */
		DataSync(void);

		/** @brief Destructor.
		 */
		~DataSync(void);

		/** @brief Set data field.
		 *
		 * Note that it directly assigns data field.  If your object needs
		 * deep copy, please properly implement your assignment operator.
		 * Or if you need complex operation, feel free to specialize the
		 * function to your like.
		 *
		 * @param[in] in_data data to be stored into object
		 */
		void SetData(const T &in_data);

		/** @brief Get data field.
		 *
		 * Note that out_data is a reference.
		 *
		 * @param[out] out_data space to retrieve data
		 */
		void GetData(T &out_data);

		/** @brief Wait for synchronization event.
		 *
		 * It will block forever until event comes.
		 */
		void Wait(void);

		/** @brief Wait for synchronization event.
		 *
		 * If it does not detect incoming event, it will return immediately
		 * with return value false.
		 *
		 * @return True if event received.  Otherwise, false is returned.
		 */
		bool TryWait(void);

		/** @brief Wait for synchronization event for a time period.
		 *
		 * If event does not come within to_msecs milliseconds, it returns
		 * false.
		 *
		 * @return True if event received.  Otherwise, false is returned.
		 */
		bool TimedWait(unsigned long to_msecs);

		/** @brief Send synchronization event.
		 *
		 * It will cause existing or following call to Wait() series functions
		 * to return with success.
		 */
		void Signal(void);
};

template <typename T>
DataSync<T>::DataSync(void)
	: sem(0, 1)
{
	pthread_mutex_init(&m_data, NULL);
}

template <typename T>
DataSync<T>::~DataSync(void)
{
	pthread_mutex_destroy(&m_data);
}

template <typename T>
void DataSync<T>::SetData(const T &in_data)
{
	pthread_mutex_lock(&m_data);
	data = in_data;
	pthread_mutex_unlock(&m_data);
}

template <typename T>
void DataSync<T>::GetData(T &out_data)
{
	pthread_mutex_lock(&m_data);
	out_data = data;
	pthread_mutex_unlock(&m_data);
}

template <typename T>
void DataSync<T>::Wait(void)
{
	sem.Wait();
}

template <typename T>
bool DataSync<T>::TryWait(void)
{
	return sem.TryWait();
}

template <typename T>
bool DataSync<T>::TimedWait(unsigned long to_msecs)
{
	return sem.TimedWait(to_msecs);
}

template <typename T>
void DataSync<T>::Signal(void)
{
	sem.Post();
}

} /* namespace ICutils */

#endif
