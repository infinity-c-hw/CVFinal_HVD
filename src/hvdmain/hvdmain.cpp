#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <hvdframe.h>
#include <hvdlogger.h>
#include <icutils.h>
#include <icv.h>

#include <new>
#include <string>

#include "vidprocessor.h"
#include "vidplayer.h"

/** @brief Structure that contains things useful throughout program session.
 */
struct hvdmain {
	HVDFrame::HVDFrame *hvdf;

	ICutils::DataSync<ICv::CVShm> *fmem_vr;
	ICutils::DataSync<ICv::CVShm> *fmem_vp;
	ICv::VidReader *vr;
	VidProcessor *vc;
	VidPlayer *vp;
};

void cv_mouse_callback(int event, int x, int y, int flags, void *userdata)
{
//	struct hvdmain *hm = (struct hvdmain *)userdata;
	const char *eventstr = NULL;

	printf("(%3d, %3d), ", x, y);

	if (event == cv::EVENT_MOUSEMOVE)
		eventstr = "EVENT_MOUSEMOVE";
	else if (event == cv::EVENT_LBUTTONDOWN)
		eventstr = "EVENT_LBUTTONDOWN";
	else if (event == cv::EVENT_RBUTTONDOWN)
		eventstr = "EVENT_RBUTTONDOWN";
	else if (event == cv::EVENT_MBUTTONDOWN)
		eventstr = "EVENT_MBUTTONDOWN";
	else if (event == cv::EVENT_LBUTTONUP)
		eventstr = "EVENT_LBUTTONUP";
	else if (event == cv::EVENT_RBUTTONUP)
		eventstr = "EVENT_RBUTTONUP";
	else if (event == cv::EVENT_MBUTTONUP)
		eventstr = "EVENT_MBUTTONUP";
	else if (event == cv::EVENT_LBUTTONDBLCLK)
		eventstr = "EVENT_LBUTTONDBLCLK";
	else if (event == cv::EVENT_RBUTTONDBLCLK)
		eventstr = "EVENT_RBUTTONDBLCLK";
	else if (event == cv::EVENT_MBUTTONDBLCLK)
		eventstr = "EVENT_MBUTTONDBLCLK";

	printf("%-19s, ", eventstr);

	if (flags & cv::EVENT_FLAG_LBUTTON)
		printf("EF_LBUTTON|");
	if (flags & cv::EVENT_FLAG_RBUTTON)
		printf("EF_RBUTTON|");
	if (flags & cv::EVENT_FLAG_MBUTTON)
		printf("EF_MBUTTON|");
	if (flags & cv::EVENT_FLAG_CTRLKEY)
		printf("EF_CTRLKEY|");
	if (flags & cv::EVENT_FLAG_SHIFTKEY)
		printf("EF_SHIFTKEY|");
	if (flags & cv::EVENT_FLAG_ALTKEY)
		printf("EF_ALTKEY|");

	printf("\n");
}

/* ********* Implement HVD core function. **********/

bool hvdmain_init(HVDFrame::HVDFrame *hvdf, void **data)
{
	struct hvdmain *hm = NULL;

	try {
		hm = new struct hvdmain;
		hm->hvdf = hvdf;
		hm->fmem_vr = new ICutils::DataSync<ICv::CVShm>;
		hm->fmem_vp = new ICutils::DataSync<ICv::CVShm>;

		hm->vr = new ICv::VidReader(hm->fmem_vr);
		hm->vc = new VidProcessor(hm->fmem_vr, hm->fmem_vp);
		hm->vp = new VidPlayer(hm->hvdf, hm->fmem_vp);
	} catch (std::bad_alloc &e) {
		return false;
	}

	*data = hm;

//	hm->hvdf->GetCvPanel()->RegisterMouseCallback(cv_mouse_callback, hm);
	return true;
}

void hvdmain_cleanup(void **data)
{
	struct hvdmain *hm = (struct hvdmain *)*data;
	hm->vc->Stop();
	hm->vr->Stop();
	hm->vp->Stop();
	delete hm;
}

bool hvdmain_open_video(void *data, const std::string &fname)
{
	struct hvdmain *hm = (struct hvdmain *)data;
	ICv::CVShm framebuf;

	hm->vc->Stop();
	hm->vr->Stop();
	hm->vp->Stop();

	/* Reset shared frame data.
	 * This is necessary because end-of-stream might be set on previous
	 * playback, and it will cause VidProcessor to get the old state and stop
	 * at the moment it starts. */
	hm->fmem_vr->SetData(framebuf);

	if (!hm->vr->Open(fname)) {
		wxString msg;
		msg.Printf("Cannot open file \"%s\" as video.", fname.c_str());
		HVDLogger::LogError(msg.mb_str());
		wxMessageBox(msg, wxT("Error"), wxOK);
		return false;
	}

	if (!hm->vr->Start()) {
		wxString msg;
		msg.Printf("Cannot start video reader.");
		HVDLogger::LogError(msg.mb_str());
		wxMessageBox(msg, wxT("Error"), wxOK);
		return false;
	}

	if (!hm->vc->Start()) {
		wxString msg;
		msg.Printf("Cannot start video processor.");
		HVDLogger::LogError(msg.mb_str());
		wxMessageBox(msg, wxT("Error"), wxOK);
		return false;
	}

	/* Video player needs FPS, get one from video reader. */
	hm->vp->SetFPS(hm->vr->GetFPS());

	if (!hm->vp->Start()) {
		wxString msg;
		msg.Printf("Cannot start video player.");
		HVDLogger::LogError(msg.mb_str());
		wxMessageBox(msg, wxT("Error"), wxOK);
		return false;
	}

	return true;
}

bool hvdmain_open_camera(void *data, const int dev_no)
{
	/* TODO: As in the warning message. */
	HVDLogger::LogWarning("Opening camera is not implemented yet.");
	HVDLogger::LogWarning("Camera: %d", dev_no);
	return false;
}

bool hvdmain_open_image(void *data, const std::string &fname)
{
	/* Somehow VideoCapture handles image also, so we reuse code from
	 * open_video here. */
	return hvdmain_open_video(data, fname);

	/* The following should be the true image handling code.  But it currently
	 * has a bug in SendCvDrawEvent (HVDFrame stops receiving event in HVD
	 * core) which we have not yet come up with a solution, so we disabled the
	 * code and use alternative solution above.
	 */
#if 0
	struct hvdmain *hm = (struct hvdmain *)data;
	cv::Mat img;

	hm->vc->Stop();
	hm->vr->Stop();
	hm->vp->Stop();

	img = cv::imread(fname.c_str());
	if (img.data == NULL) {
		wxString msg;
		msg.Printf(wxT("Cannot open file \"%s\" as image."), fname.c_str());
		HVDLogger::LogError(msg.mb_str());
		wxMessageBox(msg, wxT("Error"), wxOK);
		return false;
	}

	hm->hvdf->SendCvDrawEvent(img);
	return true;
#endif
}
