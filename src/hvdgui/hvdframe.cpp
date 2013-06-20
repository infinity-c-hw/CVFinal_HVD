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

wxCvDrawEvent::wxCvDrawEvent(wxEventType eventType, int winid, const cv::Mat &img)
	: wxEvent(winid, eventType)
{
	evtsync = new ICutils::Semaphore(0, 1);
	tracker = new InstTracker;

	frame = img.clone();
	realevt = false;
}

wxCvDrawEvent::~wxCvDrawEvent(void)
{
	/* The cloned instance should notify event process completion through
	 * semaphore. */
	if (realevt)
		evtsync->Post();

	/* The last instance is responsible for deleting dynamically allocated fields. */
	if (tracker->Decrease() == 0) {
		delete evtsync;
		delete tracker;
	}
}

wxEvent *wxCvDrawEvent::Clone(void) const
{
	wxCvDrawEvent *cloneptr = new wxCvDrawEvent(*this);
	cloneptr->realevt = true;
	cloneptr->tracker->Increase();
	return cloneptr;
}

void wxCvDrawEvent::GetFrame(cv::Mat &img) const
{
	img = frame;
}

bool wxCvDrawEvent::WaitEventHandle(unsigned long msecs)
{
	return evtsync->TimedWait(msecs);
}

wxDEFINE_EVENT(wxEVT_CV_DRAW, wxCvDrawEvent);
typedef void (wxEvtHandler::*wxCvDrawEventFunction)(wxCvDrawEvent&);
#define wxCvDrawEventHandler(func) wxEVENT_HANDLER_CAST(wxCvDrawEventFunction, func)

HVDFrame::HVDFrame(wxWindow *parent)
	: HVDFrameBase(parent)
{
	this->Connect(wxEVT_CV_DRAW, wxCvDrawEventHandler(HVDFrame::OnCvDraw));
}

HVDFrame::~HVDFrame(void)
{
	this->Disconnect(wxEVT_CV_DRAW, wxCvDrawEventHandler(HVDFrame::OnCvDraw));
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

void HVDFrame::OnCvDraw(wxCvDrawEvent &event)
{
	cv::Mat frame;
	event.GetFrame(frame);
	cvp_video->DrawFrame(frame);
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
	wxCvDrawEvent event(wxEVT_CV_DRAW, GetId(), img);

	event.SetEventObject(this);
	AddPendingEvent(event);

	/* Wait for event to be processed. */
	event.WaitEventHandle(1000);
}

} /* namespace HVDFrame */
