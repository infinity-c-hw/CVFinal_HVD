#include "icv.h"
#include <cstdio>

namespace ICv {

VidReader::VidReader(ICutils::DataSync<ICv::CVShm> *in_fmem)
{
	fmem = in_fmem;
}

VidReader::VidReader(const int camindex, ICutils::DataSync<ICv::CVShm> *in_fmem)
	: cap(camindex)
{
	fmem = in_fmem;
	fps = cap.get(CV_CAP_PROP_FPS);
}

VidReader::VidReader(const std::string &fname, ICutils::DataSync<ICv::CVShm> *in_fmem)
	: cap(fname)
{
	fmem = in_fmem;
	fps = cap.get(CV_CAP_PROP_FPS);
}

VidReader::~VidReader(void)
{
}

void VidReader::th_worker(void)
{
	ICv::CVShm framebuf;

	while (run_flag) {
		cv::Mat frame;

		if (!cap.read(frame))
			break;
		framebuf.SetImg(frame);
		fmem->SetData(framebuf);
		fmem->Signal();

		/* Obey frame rate. */
		/* FIXME: osdep */
		usleep(1000000 / fps);
	}

	if (run_flag) {
		/* Not interrupted by parent thread.  Video stream ends. */
		run_flag = false;
		framebuf.SetEndOfStream(true);
		fmem->SetData(framebuf);
		fmem->Signal();
	}
}

bool VidReader::Open(const int camindex)
{
	if (run_flag)
		Stop();

	if (cap.isOpened())
		cap.release();

	if (!cap.open(camindex))
		return false;

	fps = cap.get(CV_CAP_PROP_FPS);
	return true;
}

bool VidReader::Open(const std::string &fname)
{
	if (run_flag)
		Stop();

	if (cap.isOpened())
		cap.release();

	if (!cap.open(fname))
		return false;

	fps = cap.get(CV_CAP_PROP_FPS);
	return true;
}

double VidReader::GetFPS(void) const
{
	return fps;
}

} /* namespace ICv */
