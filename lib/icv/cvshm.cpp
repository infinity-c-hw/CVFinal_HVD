#include "cvshm.h"

namespace ICv {

CVShm::CVShm(void)
{
	valid = false;
}

CVShm::~CVShm(void)
{
}

void CVShm::SetImg(const cv::Mat &in_img)
{
	img = in_img.clone();
	valid = true;
}

void CVShm::GetImg(cv::Mat &out_img)
{
	out_img = img.clone();
}

bool CVShm::GetValid(void)
{
	return valid;
}

void CVShm::SetEndOfStream(bool val)
{
	end_of_stream = val;
}

bool CVShm::GetEndOfStream(void)
{
	return end_of_stream;
}

CVShm &CVShm::operator=(const CVShm &cvshm)
{
	if (this == &cvshm)
		return *this;

	img = cvshm.img.clone();
	valid = cvshm.valid;
	end_of_stream = cvshm.end_of_stream;
	return *this;
}

} /* namespace ICv */
