#if 0
#include "opencv2/opencv.hpp"
#include <iostream>
#include "opencv2/core/utils/logger.hpp"
using namespace std;
using namespace cv;
void filter1();
Mat calcGrayHist(const Mat& img);
Mat getGrayHistImage(const Mat& hist);
int main()
{
	utils::logging::setLogLevel(utils::logging::LOG_LEVEL_SILENT);
	filter1();
	return 0;
}
Mat getGrayHistImage(const Mat& hist)
{
	CV_Assert(hist.type() == CV_32FC1);
	CV_Assert(hist.size() == Size(1, 256));
	double histMax;
	minMaxLoc(hist, 0, &histMax);
	Mat imgHist(100, 256, CV_8UC1, Scalar(255));
	for (int i = 0; i < 256; i++) {
		line(imgHist, Point(i, 100),
			Point(i, 100 - cvRound(hist.at<float>(i, 0) * 100 / histMax)), Scalar(0));
	}
	return imgHist;
}
Mat calcGrayHist(const Mat& img)
{
	CV_Assert(img.type() == CV_8UC1);
	Mat hist;
	int channels[] = { 0 };
	int dims = 1;
	const int histSize[] = { 256 };
	float graylevel[] = { 0, 256 };
	const float* ranges[] = { graylevel };
	calcHist(&img, 1, channels, noArray(), hist, dims, histSize, ranges);
	return hist;
}
void filter1()
{
	Mat src = imread("rose.bmp", IMREAD_GRAYSCALE);
	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}
	float data[] = { -1,-1,0, -1,0,1, 0,1,1 };
	Mat emboss(3, 3, CV_32FC1, data);
	Mat dst1, dst2;
	filter2D(src, dst1, -1, emboss, Point(-1, -1));
	filter2D(src, dst2, -1, emboss, Point(-1, -1), 128);
	imshow("src", src);
	imshow("dst1", dst1);
	imshow("dst2", dst2);

	waitKey();
	destroyAllWindows();
}
#endif


// gaussian 필터를 활용해서 선명하게 보기 or 노이즈를 만들어서 보기
#if 1
#include "opencv2/opencv.hpp"
#include <iostream>
#include "opencv2/core/utils/logger.hpp"

using namespace cv;
using namespace std;

void blurring_mean();
void blurring_gaussian();
void unsharp_mask();
void noise_gaussian();
void filter_bilateral();
void filter_median();

int main(void)
{
	utils::logging::setLogLevel(utils::logging::LOG_LEVEL_SILENT);
	//blurring_mean();
	//blurring_gaussian();
	//unsharp_mask();
	//noise_gaussian();
	//filter_bilateral();
	filter_median();

	return 0;
}

void blurring_mean()
{
	Mat src = imread("rose.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	imshow("src", src);

	Mat dst;
	for (int ksize = 3; ksize <= 7; ksize += 2) {
		blur(src, dst, Size(ksize, ksize));

		String desc = format("Mean: %dx%d", ksize, ksize);
		putText(dst, desc, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255), 1, LINE_AA);

		imshow("dst", dst);
		waitKey();
	}

	destroyAllWindows();
}

void blurring_gaussian()
{
	Mat src = imread("rose.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	imshow("src", src);

	Mat dst;
	for (int sigma = 1; sigma <= 5; sigma++) {
		GaussianBlur(src, dst, Size(0, 0), (double)sigma);

		String desc = format("Gaussian: sigma = %d", sigma);
		putText(dst, desc, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1.0,
			Scalar(255), 1, LINE_AA);

		imshow("dst", dst);
		waitKey();
	}

	destroyAllWindows();
}

void unsharp_mask()
{
	Mat src = imread("rose.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	imshow("src", src);

	for (int sigma = 1; sigma <= 5; sigma++) {
		Mat blurred;
		GaussianBlur(src, blurred, Size(), sigma);

		float alpha = 1.5f;
		Mat dst = (1 + alpha) * src - alpha * blurred;

		String desc = format("sigma: %d", sigma);
		putText(dst, desc, Point(10, 30), FONT_HERSHEY_COMPLEX, 1.0, Scalar(255), 1, LINE_AA);

		imshow("dst", dst);
		waitKey();
	}
	destroyAllWindows();
}

void noise_gaussian()
{
	Mat src = imread("lenna.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	imshow("src", src);

	for (int stddev = 10; stddev <= 30; stddev += 10) {
		Mat noise(src.size(), CV_32SC1);
		randn(noise, 0, stddev);

		Mat dst;
		add(src, noise, dst, Mat(), CV_8U);

		String desc = format("stddev = %d", stddev);
		putText(dst, desc, Point(10, 30), FONT_HERSHEY_COMPLEX, 1.0, Scalar(255), 1, LINE_AA);

		imshow("dst", dst);
		waitKey();
	}
	destroyAllWindows();
}

void filter_bilateral()
{
	Mat src = imread("lenna.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}
	Mat noise(src.size(), CV_32SC1);

	imshow("src", src);

	randn(noise, 0, 5);
	add(src, noise, src, Mat(), CV_8U);

	imshow("src_noise", src);
	waitKey();

	Mat dst2;
	GaussianBlur(src, dst2, Size(), 5);

	Mat dst3;
	bilateralFilter(src, dst3, -1, 10, 5);  // 잡음 제거 
	imshow("dst2", dst2);
	imshow("dst3", dst3);
	waitKey();
	destroyAllWindows();
}

void filter_median()
{
	Mat src = imread("lenna.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	int num = (int)(src.total() * 0.1);
	for (int i = 0; i < num; i++) {
		int x = rand() % src.cols;
		int y = rand() % src.rows;
		src.at<uchar>(y, x) = (i % 2) * 255;
	}
	Mat dst1;
	GaussianBlur(src, dst1, Size(), 1);
	Mat dst2;
	medianBlur(src, dst2, 3);

	imshow("src", src);
	imshow("dst1", dst1);
	imshow("dst2", dst2);
	waitKey();
	destroyAllWindows();
}

#endif