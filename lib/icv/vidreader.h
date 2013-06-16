#ifndef VIDREADER_H_
#define VIDREADER_H_

#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <pthread.h>

#include <icutils.h>
#include "cvshm.h"

/** @brief Custom OpenCV utility namespace.
 */
namespace ICv {

/** @brief Wrapper for OpenCV VideoCapture class.
 *
 * It receives data captured by VideoCapture, and transfer the image into a
 * shared memory.
 */
class VidReader : public ICutils::ThreadRunner {
	private:
		cv::VideoCapture cap; /**< OpenCV video capture. */
		ICutils::DataSync<ICv::CVShm> *fmem; /**< Shared frame buffer. */

		double fps; /**< Video frame rate.  Updated on each Open(). */

		virtual void th_worker(void); /**< Thread worker function. */

		/* Disable some default methods. */
		VidReader(void) {}
		VidReader &operator=(const VidReader &vr) { return *this; }

	public:
		/** @brief Constructor.
		 *
		 * Set a shared buffer into VidReader object.
		 *
		 * @param[in] in_fmem shared frame buffer
		 */
		VidReader(ICutils::DataSync<ICv::CVShm> *in_fmem);

		/** @brief Constructor.
		 *
		 * It additionally receives a camera index as parameter if you want to
		 * open camera capture directly.
		 *
		 * @param[in] camindex camera index, see OpenCV VideoCapture explanation
		 * @param[in] in_fmem shared frame buffer
		 */
		VidReader(const int camindex, ICutils::DataSync<ICv::CVShm> *in_fmem);

		/** @brief Constructor.
		 *
		 * It additionally receives a file name as parameter if you want to open
		 * video file directly.
		 *
		 * @param[in] fname video file name
		 * @param[in] in_fmem shared frame buffer
		 */
		VidReader(const std::string &fname, ICutils::DataSync<ICv::CVShm> *in_fmem);

		/** @brief Destructor.
		 */
		virtual ~VidReader(void);

		/** @brief Open camera.
		 *
		 * You can open camera with this method if you didn't open anything on
		 * object construction.
		 *
		 * @param[in] camindex camera index, see OpenCV VideoCapture explanation
		 */
		bool Open(const int camindex);

		/** @brief Open video file.
		 *
		 * You can open video file with this method if you didn't open anything on
		 * object construction.
		 *
		 * @param[in] fname video file name
		 */
		bool Open(const std::string &fname);

		/** @brief Get frame rate of currently reading video in fps.
		 *
		 * @return Frame rate.
		 */
		double GetFPS(void) const;
};

} /* namespace ICv */

#endif
