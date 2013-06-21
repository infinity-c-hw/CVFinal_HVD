#include "vidplayer.h"

#include <cassert>

#include <osdep.h>
#include <hvdframe.h>

VidPlayer::VidPlayer(HVDFrame::HVDFrame *in_hvdframe, ICutils::DataSync<ICv::CVShm> *in_fmem)
{
	hvdframe = in_hvdframe;
	fmem = in_fmem;
	fps = 30;
}

VidPlayer::~VidPlayer(void)
{
}

void VidPlayer::th_worker(void)
{
	ICv::CVShm framebuf;

	while (run_flag) {
		cv::Mat frame;
		OSDep::msleep(1000 / fps);
		fmem->GetData(framebuf);

		if (!framebuf.GetValid())
			continue;

		framebuf.GetImg(frame);
		hvdframe->SendCvDrawEvent(frame);
	}
}

void VidPlayer::SetFPS(double fps_set)
{
	assert(fps_set != 0);
	fps = fps_set;
}
