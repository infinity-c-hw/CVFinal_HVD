#ifndef CVSHM_H_
#define CVSHM_H_

#include <pthread.h>
#include <opencv2/core/core.hpp>

namespace ICv {

/** @brief OpenCV image information shared memory wrapper.
 *
 * It wraps information needed when exchanging data between worker threads,
 * including image itself, valid bit, and end-of-stream bit.
 *
 * The valid bit indicates whether the image is initialized or not. If valid
 * bit is false, content of image is undefined and should not be used.
 *
 * The end-of-stream bit indicates whether upstream source encounters
 * end-of-stream or not.  For example, the reader thread finished a video
 * file.  It sets end-of-stream bit to true, and propagate it to downstream
 * threads.  Upon receiving e-o-s bit, downstream threads know that the stream
 * has ended, so they can end themselves as well.
 */
class CVShm {
	private:
		cv::Mat img; /**< OpenCV image object. */
		bool valid; /**< OpenCV image object valid bit. */
		bool end_of_stream; /**< Stream end bit. */

	public:
		/** @brief Constructor.
		 */
		CVShm(void);

		/** @brief Destructor.
		 */
		~CVShm(void);

		/** @brief Store image into object.
		 *
		 * This function call also sets valid bit to true.
		 *
		 * @param[in] in_img image to store
		 */
		void SetImg(const cv::Mat &in_img);

		/** @brief Get image out of object.
		 *
		 * @param[out] out_img space to store image
		 */
		void GetImg(cv::Mat &out_img);

		/** @brief Get image valid bit.
		 *
		 * @return Value of valid bit.
		 */
		bool GetValid(void);

		/** @brief Set end-of-stream bit.
		 *
		 * @param[in] val new value of end-of-stream bit
		 */
		void SetEndOfStream(bool val);

		/** @brief Get end-of-stream bit.
		 *
		 * @return Value of end-of-stream bit.
		 */
		bool GetEndOfStream(void);

		/** @brief Implement assignment operator.
		 *
		 * Properly copies all internal data.
		 */
		CVShm &operator=(const CVShm &cvshm);
};

} /* namespace ICv */

#endif
