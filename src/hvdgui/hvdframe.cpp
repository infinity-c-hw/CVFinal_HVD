#include "hvdframe.h"
#include "hvdlogger.h"
#include "hvdcore_extfunc.h"

#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/numdlg.h>
#include <wx/aboutdlg.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


namespace HVDFrame {

InstTracker::InstTracker(void)
{
	inst_count = 1;
	pthread_mutex_init(&m, NULL);
}

InstTracker::~InstTracker(void)
{
	pthread_mutex_destroy(&m);
}

int InstTracker::Increase(void)
{
	int val = 0;
	pthread_mutex_lock(&m);
	inst_count++;
	val = inst_count;
	pthread_mutex_unlock(&m);
	return val;
}

int InstTracker::Decrease(void)
{
	int val = 0;
	pthread_mutex_lock(&m);
	inst_count--;
	val = inst_count;
	pthread_mutex_unlock(&m);
	return val;
}

int InstTracker::Get(void)
{
	int val = 0;
	pthread_mutex_lock(&m);
	val = inst_count;
	pthread_mutex_unlock(&m);
	return val;
}

HVDEvent::HVDEvent(wxEventType eventType, int winid)
	: wxEvent(winid, eventType)
{
	shared = new HVDEventShared;

	evttype = eventType;
	realevt = false;
}

HVDEvent::~HVDEvent(void)
{
	/* The cloned instance should notify event process completion through
	 * semaphore. */
	if (realevt)
		shared->evtsync.Post();

	/* The last instance is responsible for deleting dynamically allocated fields. */
	if (shared->tracker.Decrease() == 0)
		delete shared;
}

wxEvent *HVDEvent::Clone(void) const
{
	HVDEvent *cloneptr = new HVDEvent(*this);
	cloneptr->realevt = true;
	cloneptr->shared->tracker.Increase();
	return cloneptr;
}

bool HVDEvent::WaitEventHandle(unsigned long msecs)
{
	return shared->evtsync.TimedWait(msecs);
}

void HVDEvent::SetFrame(const cv::Mat &img)
{
	frame = img.clone();
}

void HVDEvent::GetFrame(cv::Mat &img) const
{
	img = frame;
}

void HVDEvent::SetMessage(const char *format, va_list vl)
{
	/* FIXME: Strings over 300 characters long will be truncated. */
	char buffer[301] = {0};
	vsnprintf(buffer, sizeof(buffer), format, vl);
	msg = buffer;
}

const char *HVDEvent::GetMessage(void) const
{
	return msg.c_str();
}

wxDEFINE_EVENT(HVDEVT_CV_DRAW, HVDEvent);
wxDEFINE_EVENT(HVDEVT_LOG_INFO, HVDEvent);
wxDEFINE_EVENT(HVDEVT_LOG_WARNING, HVDEvent);
wxDEFINE_EVENT(HVDEVT_LOG_ERROR, HVDEvent);
typedef void (wxEvtHandler::*HVDEventFunction)(HVDEvent&);
#define HVDEventHandler(func) wxEVENT_HANDLER_CAST(HVDEventFunction, func)

HVDFrame::HVDFrame(wxWindow *parent)
	: HVDFrameBase(parent)
{
	this->Connect(HVDEVT_CV_DRAW, HVDEventHandler(HVDFrame::OnCvDraw));
	this->Connect(HVDEVT_LOG_INFO, HVDEventHandler(HVDFrame::OnLogInfo));
	this->Connect(HVDEVT_LOG_WARNING, HVDEventHandler(HVDFrame::OnLogWarning));
	this->Connect(HVDEVT_LOG_ERROR, HVDEventHandler(HVDFrame::OnLogError));
}

HVDFrame::~HVDFrame(void)
{
	this->Disconnect(HVDEVT_CV_DRAW, HVDEventHandler(HVDFrame::OnCvDraw));
	this->Disconnect(HVDEVT_LOG_INFO, HVDEventHandler(HVDFrame::OnLogInfo));
	this->Disconnect(HVDEVT_LOG_WARNING, HVDEventHandler(HVDFrame::OnLogWarning));
	this->Disconnect(HVDEVT_LOG_ERROR, HVDEventHandler(HVDFrame::OnLogError));
}

void HVDFrame::OnQuit(wxCloseEvent &event)
{
	/* Call core cleanup routine.
	 * There's no need to restore event processing as we're shutting down the
	 * program anyway. */
	hvdmain_cleanup(&core_data);

	Destroy();
}

void HVDFrame::OnMenuExit(wxCommandEvent &event)
{
	Close(true);
}

void HVDFrame::OnMenuAbout(wxCommandEvent &event)
{
	wxAboutDialogInfo info;
	info.SetName(wxT("HVD"));
	info.SetVersion(wxT("Preview"));
	info.SetDescription(wxT("HVD indicates Handicapped Vehicle Detection, but it does\n"
								"little thing, and has many bugs."));

	wxAboutBox(info, this);
}

void HVDFrame::OnToolOpenVideo(wxCommandEvent &event)
{
	wxString fname = wxFileSelector(wxT("Open Video"));
	if (fname.empty())
		return;

	hvdmain_open_video(core_data, fname.ToStdString());
}

void HVDFrame::OnToolOpenCamera(wxCommandEvent &event)
{
	long cam_index = wxGetNumberFromUser(wxT("Enter camera index."),
											wxT("Index"), wxT("Open Camera"),
											0, 0, 100);

	if (cam_index == -1)
		return;

	hvdmain_open_camera(core_data, cam_index);
}

void HVDFrame::OnToolOpenImage(wxCommandEvent &event)
{
	wxString fname = wxFileSelector(wxT("Open Image"));
	if (fname.empty())
		return;

	hvdmain_open_image(core_data, fname.ToStdString());
}

void HVDFrame::OnCvDraw(HVDEvent &event)
{
	cv::Mat frame;
	event.GetFrame(frame);
	cvp_video->DrawFrame(frame);
}

void HVDFrame::OnLogInfo(HVDEvent &event)
{
	LogInfo(event.GetMessage());
}

void HVDFrame::OnLogWarning(HVDEvent &event)
{
	LogWarning(event.GetMessage());
}

void HVDFrame::OnLogError(HVDEvent &event)
{
	LogError(event.GetMessage());
}

void HVDFrame::LogInfo(const char *format, ...)
{
	va_list vl;
	va_start(vl, format);
	LogInfo(format, vl);
	va_end(vl);
}

void HVDFrame::LogInfo(const char *format, va_list args)
{
	wxString wxmsg;
	wxTextAttr ta;

	wxmsg.PrintfV(wxString::FromUTF8(format), args);

	ta.SetFontWeight(wxFONTWEIGHT_NORMAL);
	ta.SetTextColour(wxColour(*wxBLACK));

	tc_dbglog->SetDefaultStyle(ta);
	tc_dbglog->AppendText(wxmsg + wxT("\n"));
}

void HVDFrame::LogWarning(const char *format, ...)
{
	va_list vl;
	va_start(vl, format);
	LogWarning(format, vl);
	va_end(vl);
}

void HVDFrame::LogWarning(const char *format, va_list args)
{
	wxString wxmsg;
	wxTextAttr ta;

	wxmsg.PrintfV(wxString::FromUTF8(format), args);

	ta.SetFontWeight(wxFONTWEIGHT_BOLD);
	ta.SetTextColour(wxColour(*wxYELLOW));

	tc_dbglog->SetDefaultStyle(ta);
	tc_dbglog->AppendText(wxmsg + wxT("\n"));
}

void HVDFrame::LogError(const char *format, ...)
{
	va_list vl;
	va_start(vl, format);
	LogError(format, vl);
	va_end(vl);
}

void HVDFrame::LogError(const char *format, va_list args)
{
	wxString wxmsg;
	wxTextAttr ta;

	wxmsg.PrintfV(wxString::FromUTF8(format), args);

	ta.SetFontWeight(wxFONTWEIGHT_BOLD);
	ta.SetTextColour(wxColour(*wxRED));

	tc_dbglog->SetDefaultStyle(ta);
	tc_dbglog->AppendText(wxmsg + wxT("\n"));
}

wxCvPanel *HVDFrame::GetCvPanel(void)
{
	return cvp_video;
}

void HVDFrame::SendCvDrawEvent(cv::Mat &img)
{
	HVDEvent event(HVDEVT_CV_DRAW, GetId());

	event.SetEventObject(this);
	event.SetFrame(img);
	AddPendingEvent(event);

	/* Wait for event to be processed. */
	event.WaitEventHandle(1000);
}

void HVDFrame::SendLogInfoEvent(const char *format, va_list args)
{
	HVDEvent event(HVDEVT_LOG_INFO, GetId());

	event.SetEventObject(this);
	event.SetMessage(format, args);
	AddPendingEvent(event);

	event.WaitEventHandle(1000);
}

void HVDFrame::SendLogWarningEvent(const char *format, va_list args)
{
	HVDEvent event(HVDEVT_LOG_WARNING, GetId());

	event.SetEventObject(this);
	event.SetMessage(format, args);
	AddPendingEvent(event);

	event.WaitEventHandle(1000);
}

void HVDFrame::SendLogErrorEvent(const char *format, va_list args)
{
	HVDEvent event(HVDEVT_LOG_ERROR, GetId());

	event.SetEventObject(this);
	event.SetMessage(format, args);
	AddPendingEvent(event);

	event.WaitEventHandle(1000);
}


} /* namespace HVDFrame */
