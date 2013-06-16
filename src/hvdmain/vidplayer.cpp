#include "vidplayer.h"

#include <cassert>

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
		usleep(1000000 / fps);
		fmem->GetData(framebuf);

		if (!framebuf.GetValid())
			continue;

		framebuf.GetImg(frame);
		hvdframe->SendCvDrawEvent(frame);
	}

	if (run_flag)
		run_flag = false;
}

void VidPlayer::SetFPS(double fps_set)
{
	assert(fps_set != 0);
	fps = fps_set;
}
