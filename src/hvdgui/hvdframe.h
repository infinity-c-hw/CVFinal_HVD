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

/** @brief A simple instance count tracker for HVDEvent.
 *
 * It is essentially a mutex lock protected integer data type.
 *
 * After construction, the counter will be one.  The return value of Increase
 * and Decrease is the value after counter change.
 */
class InstTracker {
	private:
		int inst_count; /**< Counter. */
		pthread_mutex_t m; /**< Mutex lock. */

	public:
		InstTracker(void); /**< Constructor. */
		~InstTracker(void); /**< Destructor. */

		int Increase(void); /**< Increase counter and return the new counter value. */
		int Decrease(void); /**< Decrease counter and return the new counter value. */
		int Get(void); /**< Get current counter value. */
};

/** @brief Generic HVD event data propagation class.
 *
 * HVD event is used to propagate GUI changes from worker thread into main
 * thread.
 *
 * FIXME:
 * This function looks like a hack-around somehow.  The reason it becomes so
 * complex is that we need thread-safety, so all immediate event processing
 * methods will not work.  But if we let event queue deal with event
 * processing, events like incoming video frame will pour in too fast that
 * main program does not have the ability to digest the events.  So we need
 * some sort of synchronization between event issuer and event dispatcher,
 * thus the evtsync, realevt, and tracker member.
 *
 * Basically it work on the assumption that a cloned copy of event object
 * will be queued into event queue.  When the copy in the event queue got
 * released, the event has been processed.  So we set a flag on cloned
 * instance, and if the destructor found the flag is set, the event is
 * processed, and it will wake up the event issuer.  In this way, we always
 * guarantee the event is completed before the next comes in, but the
 * assumption can be broken easily if the library behavior changes.
 *
 * The tracker member is used to track instance count.  Because the cloned
 * instance shares semaphore object, we need to consider two cases:
 *
 * 1. Object is deleted by the original instance.  This will happen if the
 * 		event is processed before semaphore timed out.
 * 2. Object is deleted by the cloned instance.  This will happen if time out
 * 		happens before event processing.
 *
 * In short, tracker is a reference counter for evtsync member.  If reference
 * counter drops to 0, the destructor is responsible for destroying all
 * dynamically allocated members.
 */
class HVDEvent : public wxEvent {
	private:
		wxEventType evttype; /**< Event type. */
		bool realevt; /**< Flag to indicate cloned instance, see class comment. */

		/** @brief Shared data between cloned HVDEvent instances.
		 */
		class HVDEventShared {
			public:
				HVDEventShared(void) : evtsync(0, 1) {} /**< Constructor. */
				~HVDEventShared(void) {} /**< Destructor. */

				ICutils::Semaphore evtsync; /**< Semaphore to synchronize event completion. */
				InstTracker tracker; /**< Instance tracker. */
		} *shared;

		/** @brief Frame to be propagated.
		 *
		 * Useful in: HVDEVT_CV_DRAW.
		 */
		cv::Mat frame;

		/** @brief Log message.
		 *
		 * Useful in: HVDEVT_LOG_INFO, HVDEVT_LOG_WARNING, HVDEVT_LOG_ERROR.
		 */
		std::string msg;

	public:
		/** @brief Constructor.
		 */
		HVDEvent(wxEventType eventType, int winid);

		/** @brief Destructor.
		 */
		~HVDEvent(void);

		/** @brief Implement Clone method.
		 */
		virtual wxEvent *Clone(void) const;

		/** @brief Wait for event to be handled.
		 *
		 * You can specify a timeout to avoid forever wait, which sometimes
		 * might lead to deadlock (see class explanation).
		 *
		 * @param[in] msecs wait timeout
		 * @return If event completes, it returns true.  If event wait times out,
		 * 			it returns false.
		 */
		bool WaitEventHandle(unsigned long msecs);

		/** @brief Set frame data to be propagated.
		 *
		 * Useful in HVDEVT_CV_DRAW.
		 */
		void SetFrame(const cv::Mat &img);

		/** @brief Get frame data.
		 *
		 * Useful in HVDEVT_CV_DRAW.
		 */
		void GetFrame(cv::Mat &img) const;

		/** @brief Set message to be propagated.
		 *
		 * FIXME: This function currently only deliver strings less than
		 * 300 characters long.  Anything beyond that will be truncated.
		 *
		 * Useful in: HVDEVT_LOG_INFO, HVDEVT_LOG_WARNING, HVDEVT_LOG_ERROR.
		 */
		void SetMessage(const char *format, va_list vl);

		/** @brief Get message.
		 *
		 * Useful in: HVDEVT_LOG_INFO, HVDEVT_LOG_WARNING, HVDEVT_LOG_ERROR.
		 */
		const char *GetMessage(void) const;
};

wxDECLARE_EVENT(HVDEVT_CV_DRAW, HVDEvent);
wxDECLARE_EVENT(HVDEVT_LOG_INFO, HVDEvent);
wxDECLARE_EVENT(HVDEVT_LOG_WARNING, HVDEvent);
wxDECLARE_EVENT(HVDEVT_LOG_ERROR, HVDEvent);

/** @brief Main GUI class.
 */
class HVDFrame : public HVDUI::HVDFrameBase {
	private:
		friend class ::HVDApp;

		/** @brief Core data pointer.
		 *
		 * Will be passed into each core implementation call.
		 */
		void *core_data;

		/** @brief Application quit event handler.
		 */
		void OnQuit(wxCloseEvent &event);

		/** @brief Menu exit selection event handler.
		 */
		void OnMenuExit(wxCommandEvent &event);

		/** @brief Menu about selection event handler.
		 */
		void OnMenuAbout(wxCommandEvent &event);

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
		void OnCvDraw(HVDEvent &event);

		/** @brief Log info event.
		 */
		void OnLogInfo(HVDEvent &event);

		/** @brief Log warning event.
		 */
		void OnLogWarning(HVDEvent &event);

		/** @brief Log error event.
		 */
		void OnLogError(HVDEvent &event);

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

		/** @brief Send log info event.
		 *
		 * In multi-threading scenario, these (SendLog*Event) functions should
		 * be used instead of calling Log* directly.
		 */
		void SendLogInfoEvent(const char *format, va_list args);

		/** @brief Send log warning event.
		 *
		 * @see SendLogInfoEvent(const char *format, va_list args)
		 */
		void SendLogWarningEvent(const char *format, va_list args);

		/** @brief Send log error event.
		 *
		 * @see SendLogInfoEvent(const char *format, va_list args)
		 */
		void SendLogErrorEvent(const char *format, va_list args);

#if 0
		void AddRecord();
		void DelRecord();
		void ModRecord();
#endif
};

} /* namespace HVDFrame */

#endif
