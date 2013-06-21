#include "vidprocessor.h"

#include <opencv2/imgproc/imgproc.hpp>

VidProcessor::VidProcessor(ICutils::DataSync<ICv::CVShm> *in_fmem_vr, ICutils::DataSync<ICv::CVShm> *in_fmem_vp)
{
	fmem_vr = in_fmem_vr;
	fmem_vp = in_fmem_vp;
	run_flag = false;
}

VidProcessor::~VidProcessor(void)
{
}

void VidProcessor::th_worker(void)
{
	ICv::CVShm framebuf;

	while (run_flag) {
		if (!fmem_vr->TimedWait(2000))
			continue;
		fmem_vr->GetData(framebuf);

		if (framebuf.GetEndOfStream())
			run_flag = false;
		if (!framebuf.GetValid())
			continue;

		/* TODO: call into detection core */
		fmem_vp->SetData(framebuf);
	}
}
