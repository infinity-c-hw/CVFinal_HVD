#include "cvpanel.h"

#include <cstdio>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <hvdlogger.h>

wxCvPanel::wxCvPanel(wxWindow *parent)
	: wxPanel(parent)
{
	this->Connect(wxEVT_SIZE, wxSizeEventHandler(wxCvPanel::OnSize));
	this->Connect(wxEVT_PAINT, wxPaintEventHandler(wxCvPanel::OnPaint));

	this->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Connect(wxEVT_LEFT_UP, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Connect(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Connect(wxEVT_MIDDLE_UP, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Connect(wxEVT_MIDDLE_DCLICK, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Connect(wxEVT_RIGHT_DCLICK, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Connect(wxEVT_MOTION, wxMouseEventHandler(wxCvPanel::OnMouse));

	this->SetBackgroundColour(wxColour(*wxBLACK));

	cf.valid = false;
	mousecb = NULL;
	userdata = NULL;
}

wxCvPanel::~wxCvPanel(void)
{
	this->Disconnect(wxEVT_SIZE, wxSizeEventHandler(wxCvPanel::OnSize));
	this->Disconnect(wxEVT_PAINT, wxPaintEventHandler(wxCvPanel::OnPaint));

	this->Disconnect(wxEVT_LEFT_DOWN, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Disconnect(wxEVT_LEFT_UP, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Disconnect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Disconnect(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Disconnect(wxEVT_MIDDLE_UP, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Disconnect(wxEVT_MIDDLE_DCLICK, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Disconnect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Disconnect(wxEVT_RIGHT_UP, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Disconnect(wxEVT_RIGHT_DCLICK, wxMouseEventHandler(wxCvPanel::OnMouse));
	this->Disconnect(wxEVT_MOTION, wxMouseEventHandler(wxCvPanel::OnMouse));
}

void wxCvPanel::OnSize(wxSizeEvent &event)
{
	cf.panel_width = event.GetSize().GetWidth();
	cf.panel_height = event.GetSize().GetHeight();

	Refresh(false);
	Update();
}

void wxCvPanel::OnPaint(wxPaintEvent &event)
{
	wxClientDC dc(this);

	if (!cf.valid) {
		dc.SetBackground(*wxBLACK_BRUSH);
		dc.Clear();
	} else {
		render(dc);
	}
}

void wxCvPanel::OnMouse(wxMouseEvent &event)
{
	int wxkeymods = 0;
	int cvevent = 0;
	int cvflags = 0;
	wxPoint pos, img_pos;
	int rb_x = 0;
	int rb_y = 0;

	if (mousecb == NULL)
		return;

	/* Scale back coordination. */
	pos = event.GetPosition();
	rb_x = cf.lt_x + (cf.image_width * cf.scale);
	rb_y = cf.lt_y + (cf.image_height * cf.scale);

	if (pos.x < cf.lt_x || pos.y < cf.lt_y ||
			pos.x >= rb_x || pos.y >= rb_y)
		return;
	img_pos.x = (pos.x - cf.lt_x) / cf.scale;
	img_pos.y = (pos.y - cf.lt_y) / cf.scale;

	/* Convert wxMouseEvent into OpenCV event. */
	if (event.Moving() || event.Dragging())
		cvevent = cv::EVENT_MOUSEMOVE;
	else if (event.LeftDown())
		cvevent = cv::EVENT_LBUTTONDOWN;
	else if (event.RightDown())
		cvevent = cv::EVENT_RBUTTONDOWN;
	else if (event.MiddleDown())
		cvevent = cv::EVENT_MBUTTONDOWN;
	else if (event.LeftUp())
		cvevent = cv::EVENT_LBUTTONUP;
	else if (event.RightUp())
		cvevent = cv::EVENT_RBUTTONUP;
	else if (event.MiddleUp())
		cvevent = cv::EVENT_MBUTTONUP;
	else if (event.LeftDClick())
		cvevent = cv::EVENT_LBUTTONDBLCLK;
	else if (event.RightDClick())
		cvevent = cv::EVENT_RBUTTONDBLCLK;
	else if (event.MiddleDClick())
		cvevent = cv::EVENT_MBUTTONDBLCLK;

	/* Convert wxMouseEvent into OpenCV flags. */
	if (event.LeftIsDown())
		cvflags |= cv::EVENT_FLAG_LBUTTON;
	if (event.RightIsDown())
		cvflags |= cv::EVENT_FLAG_RBUTTON;
	if (event.MiddleIsDown())
		cvflags |= cv::EVENT_FLAG_MBUTTON;

	wxkeymods = event.GetModifiers();
	if (wxkeymods & wxMOD_CONTROL)
		cvflags |= cv::EVENT_FLAG_CTRLKEY;
	if (wxkeymods & wxMOD_SHIFT)
		cvflags |= cv::EVENT_FLAG_SHIFTKEY;
	if (wxkeymods & wxMOD_ALT)
		cvflags |= cv::EVENT_FLAG_ALTKEY;

	/* Call mouse callback. */
	mousecb(cvevent, img_pos.x, img_pos.y, cvflags, userdata);
}

void wxCvPanel::update_frame_info(void)
{
	/* Update scale info. */
	/* Test resize W.R.T. width first. */
	cf.scale = (float)(cf.panel_width - 1) / cf.image_width;
	if (cf.image_height * cf.scale > cf.panel_height) {
		/* Height does not fit, resize W.R.T. height. */
		cf.scale = (float)(cf.panel_height - 1) / cf.image_height;
	}

	int scaled_width = cf.image_width * cf.scale;
	int scaled_height = cf.image_height * cf.scale;

	/* Update position info. */
	if (cf.panel_width - scaled_width <= 1) {
		cf.lt_x = 0;
		cf.lt_y = (cf.panel_height - scaled_height) / 2;
	} else {
		cf.lt_x = (cf.panel_width - scaled_width) / 2;
		cf.lt_y = 0;
	}
}

void wxCvPanel::render(wxDC &dc)
{
	update_frame_info();
	wxBitmap f_bitmap(cf.wx_img.Scale(cf.image_width * cf.scale, cf.image_height * cf.scale));
	dc.DrawBitmap(f_bitmap, wxPoint(cf.lt_x, cf.lt_y), false);
}

void wxCvPanel::DrawFrame(const cv::Mat &frame)
{
	cv::cvtColor(frame, cf.cv_img, CV_BGR2RGB);

	cf.image_width = cf.cv_img.size().width;
	cf.image_height = cf.cv_img.size().height;

	cf.wx_img = wxImage(cf.image_width, cf.image_height, cf.cv_img.data, true);
	cf.valid = true;

	Refresh(false);
	Update();
}

void wxCvPanel::Clear(void)
{
	cf.valid = false;
	Refresh(false);
	Update();
}

void wxCvPanel::RegisterMouseCallback(CVPMouseCallback cb, void *userdata)
{
	this->mousecb = cb;
	this->userdata = userdata;
}
