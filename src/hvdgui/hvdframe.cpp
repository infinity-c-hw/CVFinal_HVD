#include "hvdframe.h"
#include "hvdlogger.h"
#include "hvdcore_extfunc.h"

#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/numdlg.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


namespace HVDFrame {

wxCvDrawEvent::wxCvDrawEvent(wxEventType eventType, int winid, const cv::Mat &img, ICutils::Semaphore *in_evtsync)
	: wxEvent(winid, eventType)
{
	frame = img.clone();
	evtsync = in_evtsync;
	realevt = false;
}

wxCvDrawEvent::~wxCvDrawEvent(void)
{
	if (realevt)
		evtsync->Post();
}

void wxCvDrawEvent::GetFrame(cv::Mat &img) const
{
	img = frame;
}

wxDEFINE_EVENT(wxEVT_CV_DRAW, wxCvDrawEvent);
typedef void (wxEvtHandler::*wxCvDrawEventFunction)(wxCvDrawEvent&);
#define wxCvDrawEventHandler(func) wxEVENT_HANDLER_CAST(wxCvDrawEventFunction, func)

HVDFrame::HVDFrame(wxWindow *parent)
	: HVDFrameBase(parent)
{
	this->Connect(wxEVT_CV_DRAW, wxCvDrawEventHandler(HVDFrame::OnCvDraw));

	ignore_cv_draw_evt = false;
}

HVDFrame::~HVDFrame(void)
{
	this->Disconnect(wxEVT_CV_DRAW, wxCvDrawEventHandler(HVDFrame::OnCvDraw));
}

void HVDFrame::prepare_core(void)
{
	ignore_cv_draw_evt = true;
	DeletePendingEvents();
}

void HVDFrame::post_core(void)
{
	ignore_cv_draw_evt = false;
}

void HVDFrame::OnQuit(wxCloseEvent &event)
{
	/* Call core cleanup routine.
	 * There's no need to restore event processing as we're shutting down the
	 * program anyway. */
	prepare_core();
	hvdmain_cleanup(&core_data);

	Destroy();
}

void HVDFrame::OnMenuExit(wxCommandEvent &event)
{
	Close(true);
}

void HVDFrame::OnToolOpenVideo(wxCommandEvent &event)
{
	wxString fname = wxFileSelector(wxT("Open Video"));
	if (fname.empty())
		return;

	prepare_core();
	hvdmain_open_video(core_data, fname.ToStdString());
	post_core();
}

void HVDFrame::OnToolOpenCamera(wxCommandEvent &event)
{
	long cam_index = wxGetNumberFromUser(wxT("Enter camera index."),
											wxT("Index"), wxT("Open Camera"),
											0, 0, 100);

	if (cam_index == -1)
		return;

	prepare_core();
	hvdmain_open_camera(core_data, cam_index);
	post_core();
}

void HVDFrame::OnToolOpenImage(wxCommandEvent &event)
{
	wxString fname = wxFileSelector(wxT("Open Image"));
	if (fname.empty())
		return;

	prepare_core();
	hvdmain_open_image(core_data, fname.ToStdString());
	post_core();
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
	if (ignore_cv_draw_evt)
		return;

	ICutils::Semaphore sem(0, 1);
	wxCvDrawEvent event(wxEVT_CV_DRAW, GetId(), img, &sem);

	event.SetEventObject(this);
	AddPendingEvent(event);

	/* Wait for event to be processed. */
	sem.Wait();
}

} /* namespace HVDFrame */
