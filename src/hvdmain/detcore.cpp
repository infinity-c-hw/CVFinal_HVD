#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <cmath>

#include <hvdlogger.h>

static void drawSquares(cv::Mat &img, std::vector<std::vector<cv::Point> > &squares)
{
	std::vector<std::vector<cv::Point> >::iterator it;

	for (it = squares.begin(); it != squares.end(); it++)
		cv::polylines(img, (*it), true, cv::Scalar(0,255,0), 3);
}

static bool siftMatch(cv::Mat &sample, cv::Mat &match, std::vector<cv::Mat> &siftImages)
{
	cv::SiftFeatureDetector detector(0, 3, 0.04, 10, 1.6);
	cv::SiftDescriptorExtractor extractor(3.0);

	std::vector<cv::KeyPoint> keypoint1, keypoint2;

	detector.detect(sample, keypoint1);
	detector.detect(match, keypoint2);

	cv::Mat description1, description2;
	extractor.compute(sample, keypoint1, description1);
	extractor.compute(match, keypoint2, description2);

	std::vector<cv::DMatch> matches;
	cv::Mat result;

	cv::BruteForceMatcher<cv::L2<float> > matcher;
	matcher.match(description1, description2, matches);

	cv::drawMatches(sample, keypoint1, match, keypoint2, matches, result);
	siftImages.push_back(result);

	if(matches.size() >= 6 )
		return true;
	return false;
}

static double angle(cv::Point &pt1, cv::Point &pt2, cv::Point &pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

static std::vector<std::vector<cv::Point> > findSquares4(cv::Mat img)
{
	std::vector<std::vector<cv::Point> > contours;
	std::vector<std::vector<cv::Point> > squares;
	int i_contour = 0;
	int i, c, l, N = 11;
	cv::Size sz(img.size().width & -2, img.size().height & -2);
	cv::Mat timg =  img.clone(); // make a copy of input image
	cv::Mat gray(sz, CV_8UC1);
	cv::Mat pyr(sz, CV_8UC3);
	std::vector<cv::Mat> tgray(3);
	std::vector<cv::Point> result;
	double s, t;

	// select the maximum ROI in the image
	// with the width and height divisible by 2
	cv::Mat roi(timg, cv::Rect(0, 0, sz.width, sz.height));

	// down-scale and upscale the image to filter out the noise
	cv::pyrDown(timg, pyr);
	cv::pyrUp(pyr, timg);

	// extract the c-th color plane
	cv::split(timg, tgray);

	// find squares in every color plane of the image
	for (c = 0; c < 3; c++) {
		// try several threshold levels
		for (l = 0; l < N; l++) {
			// hack: use Canny instead of zero threshold level.
			// Canny helps to catch squares with gradient shading
			if(l == 0) {
				// apply Canny. Take the upper threshold from slider
				// and set the lower to 0 (which forces edges merging)
				cv::Canny( tgray[c], gray, 0, 50, 5 );
				// dilate canny output to remove potential
				// holes between edge segments
				cv::dilate( gray, gray, cv::Mat() );
			} else {
				// apply threshold if l!=0:
				//     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
				cv::threshold( tgray[c], gray, (l+1)*255/N, 255, CV_THRESH_BINARY );
			}

			// find contours and store them all as a list
			cv::findContours( gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

			// test each contour
			for (i_contour = 0; i_contour < contours.size(); i_contour++) {
				// approximate contour with accuracy proportional
				// to the contour perimeter
				cv::approxPolyDP(contours[i_contour], result, cv::arcLength(contours[i_contour], true) * 0.02, true);

				// square contours should have 4 vertices after approximation
				// relatively large area (to filter out noisy contours)
				// and be convex.
				// Note: absolute value of an area is used because
				// area may be positive or negative - in accordance with the
				// contour orientation;
				if(result.size() == 4 &&
						cv::contourArea(result) > 1000 &&
						cv::isContourConvex(result)) {
					s = 0;

					for (i = 0; i < 5; i++) {
						// find minimum angle between joint
						// edges (maximum of cosine)
						if (i >= 2) {
							t = fabs(angle(result[2], result[0], result[1]));
							s = s > t ? s : t;
						}
					}

					// if cosines of all angles are small
					// (all angles are ~90 degree) then write quandrange
					// vertices to resultant sequence
					if (s < 0.3)
						squares.push_back(result);
				}
			}
		}
	}

	return squares;
}

static void locateSign(std::vector<std::vector<cv::Point> > &squares, std::vector<cv::RotatedRect> &signBox)
{
	int i = 0;
	cv::Point point[4];

	// read 4 sequence elements at a time (all vertices of a square)
	for (i = 0; i < squares.size(); i += 4) {
		cv::Point pt[4];

		pt[0] = squares[i][0];
		pt[1] = squares[i][1];
		pt[2] = squares[i][2];
		pt[3] = squares[i][3];
			
		// find upper-left point
		if (pt[0].x < pt[2].x && pt[0].y < pt[2].y) {
			// 0 3
			// 1 2 do nothing
		} else if(pt[2].x < pt[0].x && pt[2].y < pt[0].y) {
			//2 1
			//3 0
			cv::Point temp;

			temp = pt[0];
			pt[0] = pt[2];
			pt[2] = temp;

			temp = pt[1];
			pt[1] =pt[3];
			pt[3] = temp;
		} else if(pt[1].x < pt[3].x && pt[1].y < pt[3].y) {
			// 1 0 => 0 3
			// 2 3 => 1 2
			cv::Point temp;

			temp = pt[0];
			pt[0] = pt[1];
			pt[1] = pt[2];
			pt[2] = pt[3];
			pt[3] = temp;
		} else if(pt[3].x < pt[1].x && pt[3].y < pt[1].y) {
			//3  2 => 0 3
			//0  1 => 1 2
			cv::Point temp;

			temp = pt[0];
			pt[0] = pt[3];
			pt[3] = pt[2];
			pt[2] = pt[1];
			pt[1] = temp;
		} else {
			//3  0
			//2  1
		}

		// 0 3
		// 1 2
		int dxL, dxR, dyH, dyL;  // four bounds
		dxL = pt[0].x + 0.48*(pt[3].x - pt[0].x);
		dxR = dxL + 0.12*(pt[3].x - pt[0].x);
		dyH = pt[0].y;
		dyL = pt[0].y + 0.3*(pt[1].y - pt[0].y);

		point[0].x = dxL;
		point[0].y = dyH;
		point[1].x = dxL;
		point[1].y = dyL;
		point[2].x = dxR;
		point[2].y = dyL;
		point[3].x = dxR;
		point[3].y = dyH;
	
		if (!(dxR - dxL < 2 || dyL - dyH < 2)) {
			cv::RotatedRect rectBox(cv::Point2f((point[0].x + point[2].x) / 2, (point[0].y + point[2].y) / 2),
									cv::Size2f(point[3].x - point[0].x, point[1].y - point[0].y), 0);
			signBox.push_back(rectBox);
		}
	}
}

bool detection_core(cv::Mat &io_img)
{
	cv::Mat img1 = io_img;
	cv::Mat resultImg;
	cv::Mat signImg = cv::imread("sign.jpg");
	std::vector<std::vector<cv::Point> > squares;
	std::vector<cv::RotatedRect> signBox;
	std::vector<cv::RotatedRect>::iterator it;
	std::vector<cv::Mat> siftImages;
	int matchCount = 0;

	resultImg = img1.clone();

	squares = findSquares4(img1);
	drawSquares(resultImg, squares);
	locateSign(squares, signBox);

	for (it = signBox.begin(); it != signBox.end(); it++) {
		cv::Mat boxImg;
		boxImg = img1((*it).boundingRect());

		cv::rectangle(resultImg, (*it).boundingRect(), cv::Scalar(255,0,0), 3);
		if(siftMatch(boxImg, signImg, siftImages))
			matchCount++;
	}

	if (matchCount >= 1)
		HVDLogger::ThreadLogInfo("Legal user");
	else
		HVDLogger::ThreadLogWarning("Illegal!!!!!!!!");

	io_img = resultImg;
	return (matchCount >= 1);
}
