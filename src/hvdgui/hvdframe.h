#ifndef HVDFRAME_H_
#define HVDFRAME_H_

#include <cstdarg>

#include <hvd.h>
#include <string>

#include <sem.h>

#include <cvpanel/cvpanel.h>
#include "hvdapp.h"

/** @brief Implements HVDFrame class.
 */
namespace HVDFrame {

/** @brief Drawing event that propagates image frame.
 *
 * FIXME:
 * This function looks like a hack-around somehow.  The reason it becomes so
 * complex is that we need thread-safety, so all immediate event processing
 * methods will not work.  But if we let event queue deal with event
 * processing, the incoming video frame will pour in too fast that main
 * program does not have the ability to digest the events.  So we need some
 * sort of synchronization between event issuer and event dispatcher, thus
 * the evtsync and realevt member.
 * Basically it work on the assumption that a cloned copy of event object
 * will be queued into event queue.  When the copy in the event queue got
 * released, the event has been processed.  So we set a flag on cloned
 * instance, and if the destructor found the flag is set, the event is
 * processed, and it will wake up the event issuer.  In this way, we always
 * guarantee the event is completed before the next comes in, but the
 * assumption can be broken easily if the library behavior changes.
 */
class wxCvDrawEvent : public wxEvent {
	private:
		cv::Mat frame;
		ICutils::Semaphore *evtsync; /**< semaphore to synchronize event completion */
		bool realevt; /**< flag to indicate cloned instance, see class comment */

	public:
		wxCvDrawEvent(wxEventType eventType, int winid, const cv::Mat &img, ICutils::Semaphore *in_evtsync);
		~wxCvDrawEvent(void);

		void GetFrame(cv::Mat &img) const;

		virtual wxEvent *Clone(void) const
		{
			wxCvDrawEvent *cloneptr = new wxCvDrawEvent(*this);
			cloneptr->realevt = true;
			return cloneptr;
		}
};

wxDECLARE_EVENT(wxEVT_CV_DRAW, wxCvDrawEvent);

/** @brief Main GUI class.
 */
class HVDFrame : public HVDUI::HVDFrameBase {
	private:
		friend class HVDApp;

		/** @brief Flag to ignore wxEVT_CV_DRAW.
		 *
		 * If this attribute is set, SendCvDrawEvent() will refuse to queue
		 * new event into event queue.
		 *
		 * This is essential, because if hvd core calls into functions that
		 * waits for drawing completion when some drawing event is still in
		 * event queue, it results in deadlock becuase we're unable to
		 * process event, thus unable to sem.Post() event object.
		 *
		 * So here we take a take a not-so-good but workingapproach.  That is,
		 * whenever calling into hvd call, we invalidate drawing event
		 * temporarily and delete all pending events, so we can prevent such
		 * deadlock.
		 *
		 * This attribute is managed by prepare_core() and post_core() methods.
		 */
		bool ignore_cv_draw_evt;

		/** @brief Core data pointer.
		 *
		 * Will be passed into each core implementation call.
		 */
		void *core_data;

		/** @brief Prepare hvd core call.
		 *
		 * Sets ignore_cv_draw_evt and clear event queue.
		 */
		void prepare_core(void);

		/** @brief Post hvd core call.
		 *
		 * Restart draw event processing.
		 */
		void post_core(void);

		/** @brief Application quit event handler.
		 */
		void OnQuit(wxCloseEvent &event);

		/** @brief Menu exit selection event handler.
		 */
		void OnMenuExit(wxCommandEvent &event);

		/** @brief Open video button event handler.
		 */
		void OnToolOpenVideo(wxCommandEvent &event);

		/** @brief Open camera button event handler.
		 */
		void OnToolOpenCamera(wxCommandEvent &event);

		/** @brief Open image button event handler.
		 */
		void OnToolOpenImage(wxCommandEvent &event);

		/** @brief OpenCV draw event handler.
		 */
		void OnCvDraw(wxCvDrawEvent &event);

	public:
		/** @brief Constructor.
		 *
		 * @param[in] parent specify parent of the widget
		 */
		HVDFrame(wxWindow *parent);

		/** @brief Destructor.
		 */
		~HVDFrame(void);

		/** @brief Write information into log column.
		 */
		void LogInfo(const char *format, ...);

		/** @brief Write information into log column.
		 *
		 * This variant receives va_list instead of variadic variable.
		 */
		void LogInfo(const char *format, va_list args);

		/** @brief Write warning into log column.
		 */
		void LogWarning(const char *format, ...);

		/** @brief Write warning into log column.
		 *
		 * This variant receives va_list instead of variadic variable.
		 */
		void LogWarning(const char *format, va_list args);

		/** @brief Write error into log column.
		 */
		void LogError(const char *format, ...);

		/** @brief Write error into log column.
		 *
		 * This variant receives va_list instead of variadic variable.
		 */
		void LogError(const char *format, va_list args);

		/** @brief Access CV image panel.
		 *
		 * @return wxCvPanel object.
		 */
		wxCvPanel *GetCvPanel(void);

		/** @brief Send drawing event to OpenCV panel.
		 *
		 * In multi-threading scenario, this function should be used instead of
		 * calling wxCvPanel::DrawFrame() directly.
		 */
		void SendCvDrawEvent(cv::Mat &img);

#if 0
		void AddRecord();
		void DelRecord();
		void ModRecord();
#endif
};

} /* namespace HVDFrame */

#endif
