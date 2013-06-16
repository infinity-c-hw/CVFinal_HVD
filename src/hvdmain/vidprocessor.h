#ifndef VIDPROCESSOR_H_
#define VIDPROCESSOR_H_

#include <icutils.h>
#include <icv.h>

/** @brief Video processor.
 *
 * This component does video frame processing.  It catches frames from
 * VidReader, process the frame, and put the frame into VidPlayer.
 */
class VidProcessor : public ICutils::ThreadRunner {
	private:
		ICutils::DataSync<ICv::CVShm> *fmem_vr; /**< Frame buffer shared with VidReader. */
		ICutils::DataSync<ICv::CVShm> *fmem_vp; /**< Frame buffer shared with VidPlayer. */

		virtual void th_worker(void); /**< Thread worker function. */

		/* Disable some default methods. */
		VidProcessor(void) {}
		VidProcessor &operator=(const VidProcessor &vc) { return *this; }

	public:
		/** @brief Constructor.
		 *
		 * @param[in] fstor shared frame buffer
		 */
		VidProcessor(ICutils::DataSync<ICv::CVShm> *in_fmem_vr, ICutils::DataSync<ICv::CVShm> *in_fmem_vp);

		/** @brief Destructor.
		 */
		virtual ~VidProcessor(void);
};

#endif
