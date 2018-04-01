#include <pxcsensemanager.h>
#include <pxcsession.h>
#include "util_render.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <opencv2\opencv.hpp>
using namespace cv;
using namespace std;
int main()
{
	UtilRender *renderColor = new UtilRender(L"COLOR_STREAM");
	PXCSenseManager *psm = PXCSenseManager::CreateInstance();
	if (!psm)
	{
		wprintf_s(L"Unabel to create the PXCSenseManager\n");
		return 1;
	}
	psm->EnableStream(PXCCapture::STREAM_TYPE_COLOR);
	psm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH);

	if (psm->Init() != PXC_STATUS_NO_ERROR)
	{
		wprintf_s(L"Unable to Init the PXCSenseManager\n");
		return 2;
	}
	PXCImage *colorIm;
	PXCImage::ImageData color_data;
	PXCImage::ImageInfo color_information;
	while (waitKey(1))
	{
		if (psm->AcquireFrame(true) < PXC_STATUS_NO_ERROR) break;

		PXCCapture::Sample *sample = psm->QuerySample();
		colorIm = sample->color;
		colorIm->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB24, &color_data);
		color_information = sample->color->QueryInfo();
		Mat color_img = Mat(color_information.height, color_information.width, CV_8UC3);
		uchar * dp = (uchar*)color_data.planes[0];
		int cnt = 0;
		for (int y = 0; y < color_information.height; y++)
			for (int x = 0; x < color_information.width; x++)
			{
				color_img.at<Vec3b>(y, x)[0] = dp[cnt++];
				color_img.at<Vec3b>(y, x)[1] = dp[cnt++];
				color_img.at<Vec3b>(y, x)[2] = dp[cnt++];
			}
		flip(color_img, color_img, 1);
		pyrDown(color_img, color_img);
		imshow("a", color_img);
		colorIm->ReleaseAccess(&color_data);
		psm->ReleaseFrame();
	}
	psm->Release();
}