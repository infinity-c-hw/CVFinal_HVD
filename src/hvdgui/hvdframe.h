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

/** @brief A simple instance count tracker for wxCvDrawEvent.
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

/** @brief Drawing event that propagates image frame.
 *
 * FIXME:
 * This function looks like a hack-around somehow.  The reason it becomes so
 * complex is that we need thread-safety, so all immediate event processing
 * methods will not work.  But if we let event queue deal with event
 * processing, the incoming video frame will pour in too fast that main
 * program does not have the ability to digest the events.  So we need some
 * sort of synchronization between event issuer and event dispatcher, thus
 * the evtsync, realevt, and tracker member.
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
class wxCvDrawEvent : public wxEvent {
	private:
		cv::Mat frame; /**< Frame to be propagated. */
		bool realevt; /**< Flag to indicate cloned instance, see class comment. */
		ICutils::Semaphore *evtsync; /**< Semaphore to synchronize event completion. */
		InstTracker *tracker; /**< Instance tracker. */

	public:
		wxCvDrawEvent(wxEventType eventType, int winid, const cv::Mat &img);
		~wxCvDrawEvent(void);

		void GetFrame(cv::Mat &img) const;
		bool WaitEventHandle(unsigned long msecs);

		virtual wxEvent *Clone(void) const;
};

wxDECLARE_EVENT(wxEVT_CV_DRAW, wxCvDrawEvent);

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
