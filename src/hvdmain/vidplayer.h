#ifndef VIDPLAYER_H_
#define VIDPLAYER_H_

#include <icv.h>
#include <icutils.h>

#include <opencv2/core/core.hpp>

#include <hvdframe.h>

/** @brief Video player.
 *
 * It controls video playback.  It will keep emitting draw event regardless
 * of whether new frames comes in or not.  This enables easy integration of
 * built-in OSD into VidPlayer.
 */
class VidPlayer : public ICutils::ThreadRunner {
	private:
		double fps; /**< Frame rate of video playback. */
		HVDFrame::HVDFrame *hvdframe; /**< Target HVDFrame to put video frame. */
		ICutils::DataSync<ICv::CVShm> *fmem; /**< Source of video frame. */

		virtual void th_worker(void); /**< Thread worker function. */

		/* Disable some default methods. */
		VidPlayer(void) {}
		VidPlayer &operator=(const VidPlayer &vp) { return *this; }

	public:
		/** @brief Constructor.
		 *
		 * @param[in] in_hvdframe HVDFrame to associate
		 * @param[in] in_fmem video source
		 */
		VidPlayer(HVDFrame::HVDFrame *in_hvdframe, ICutils::DataSync<ICv::CVShm> *in_fmem);

		/** @brief Destructor.
		 */
		virtual ~VidPlayer(void);

		/** @brief Set frame rate of video playback.
		 *
		 * @param[in] fps_set new frame rate
		 */
		void SetFPS(double fps_set);
};

#endif
