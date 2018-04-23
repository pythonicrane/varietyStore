/**------------------------------------------
* @Author: Zhao Heln
* @Time: 2018年4月21日15:33:46
* @Software: Visual Studio 2017
* @Project: 计算机视觉作业
* @Problem: ORB特征匹配
* @Description：----------------------------*
*
------------------------------------------**/
#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
using namespace cv;
using namespace std;

Mat changeImage(Mat& img, double angle, double scale);
void orbMatch(Mat& img_1, Mat& img_2, int time);
void orbGoodMatch(Mat& img_1, Mat& img_2, int time);
void orbTestScale(Mat& img_1, void(*pMatch)(Mat& img_1, Mat& img_2, int time));
void orbTestRotate(Mat& img_1, void(*pMatch)(Mat& img_1, Mat& img_2, int time));
void orbTestMix(Mat& img_1, void(*pMatch)(Mat& img_1, Mat& img_2, int time));
void orbTestMixIMG(vector<Mat>& vecMat);
void goodORBScale(Mat& img_1);


int main()
{
	Mat img_1 = imread("lenaTest.jpg");
	Mat img_2 = imread("balloon.jpg");
	Mat img_3 = imread("hnu1.jpg");
	Mat img_4 = imread("catom.jpg");
	//orbTestScale(img_1, orbMatch);
	//orbTestScale(img_2, orbMatch);
	//orbTestScale(img_2, orbGoodMatch);
	//orbTestRotate(img_1, orbMatch);
	//orbTestRotate(img_2, orbMatch);
	//orbTestRotate(img_3, orbMatch);
	//orbTestRotate(img_2, orbGoodMatch);
	//orbTestMix(img_2, orbGoodMatch);
	goodORBScale(img_1);
	waitKey(0);
	return 0;
}

/* 图片旋转+缩放，内容不丢失 */
Mat changeImage(Mat& img, double angle, double scale)
{
	//图片旋转
	cv::Point2f center(img.cols / 2, img.rows / 2);
	cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1);
	cv::Rect bbox = cv::RotatedRect(center, img.size(), angle).boundingRect();
	rot.at<double>(0, 2) += bbox.width / 2.0 - center.x;
	rot.at<double>(1, 2) += bbox.height / 2.0 - center.y;
	cv::Mat dst;
	cv::warpAffine(img, dst, rot, bbox.size());

	//图片缩放
	Size ResImgSiz = Size(dst.cols*scale, dst.rows*scale);
	Mat ResImg = Mat(ResImgSiz, dst.type());
	resize(dst, ResImg, ResImgSiz, CV_INTER_CUBIC);

	return ResImg;
}

/*ORB特征提取+暴力算法匹配*/
void orbMatch(Mat& img_1, Mat& img_2, int time)
{

	//关键点检测
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	int nkeypoint = 1000;//特征点个数
	Ptr<ORB> orb = ORB::create(nkeypoint);

	orb->detect(img_1, keypoints_1);
	orb->detect(img_2, keypoints_2);

	//计算特征
	Mat descriptors_1, descriptors_2;
	orb->compute(img_1, keypoints_1, descriptors_1);
	orb->compute(img_2, keypoints_2, descriptors_2);

	//brute force matcher 匹配特征值
	BFMatcher matcher(NORM_HAMMING);
	std::vector<DMatch> mathces;
	matcher.match(descriptors_1, descriptors_2, mathces);
	//绘制匹配图
	Mat img_mathes;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, mathces, img_mathes);

	//显示输出
	ostringstream s_time;
	s_time << time;
	imshow("OPENCV_ORB_" + s_time.str(), img_mathes);
	cout << "TIME" + s_time.str() + ": (keypoints_1:" << keypoints_1.size() << ") && (keypoints_2:" << keypoints_2.size()
		<< ") = (MatchPoints: " << mathces.size() << ")" << endl;

}

/*FAST特征匹配*/
void fastMatch(Mat& img_1, Mat& img_2, int time)
{
	// vector of keyPoints
	std::vector<KeyPoint> keyPoints;
	// construction of the fast feature detector object
	Ptr<FeatureDetector> fast = FastFeatureDetector::create(40);
	fast->detect(img_1, keyPoints);
	drawKeypoints(img_1, keyPoints, img_1, Scalar::all(255), DrawMatchesFlags::DRAW_OVER_OUTIMG);
	imshow("FAST feature", img_1);
}

/*ORB特征提取+FLANN优秀点匹配*/
void orbGoodMatch(Mat& img_1, Mat& img_2, int time)
{
	//提取特征点
	Ptr<ORB> orb = ORB::create(1000);  // 在这里调整精度，值越小点越少，越精准
	vector<KeyPoint> keyPoint1, keyPoint2;
	orb->detect(img_1, keyPoint1);
	orb->detect(img_2, keyPoint2);

	//特征点描述，为下边的特征点匹配做准备
	Mat imageDesc1, imageDesc2;
	orb->compute(img_1, keyPoint1, imageDesc1);
	orb->compute(img_2, keyPoint2, imageDesc2);

	//获取特征点最近的两个关键点
	flann::Index flannIndex(imageDesc1, flann::LshIndexParams(12, 20, 2), cvflann::FLANN_DIST_HAMMING);
	vector<DMatch> GoodMatchePoints;
	Mat macthIndex(imageDesc2.rows, 2, CV_32SC1), matchDistance(imageDesc2.rows, 2, CV_32FC1);
	flannIndex.knnSearch(imageDesc2, macthIndex, matchDistance, 2, flann::SearchParams());

	// Lowe's algorithm,获取优秀匹配点
	for (int i = 0; i < matchDistance.rows; i++)
	{
		if (matchDistance.at<float>(i, 0) < 0.6 * matchDistance.at<float>(i, 1))//欧氏距离比例小于0.6
		{
			DMatch dmatches(i, macthIndex.at<int>(i, 0), matchDistance.at<float>(i, 0));
			GoodMatchePoints.push_back(dmatches);
		}
	}


	Mat first_match;
	drawMatches(img_2, keyPoint2, img_1, keyPoint1, GoodMatchePoints, first_match);

	//显示输出
	ostringstream s_time;
	s_time << time;
	imshow("OPENCV_ORB_" + s_time.str(), first_match);
	cout << "TIME" + s_time.str() + ": (keyPoint1:" << keyPoint1.size() << ") && (keyPoint2:" << keyPoint2.size()
		<< ") = (GoodMatchePoints: " << GoodMatchePoints.size() << ")" << endl;
}

/*尺度变化对比*/
void orbTestScale(Mat& img_1, void(*pMatch)(Mat& img_1, Mat& img_2, int time))
{
	Mat img_2 = changeImage(img_1, 0, 0.2);
	Mat img_3 = changeImage(img_1, 0, 0.4);
	Mat img_4 = changeImage(img_1, 0, 0.5);
	Mat img_5 = changeImage(img_1, 0, 0.6);
	Mat img_6 = changeImage(img_1, 0, 0.7);
	Mat img_7 = changeImage(img_1, 0, 1.0);
	Mat img_8 = changeImage(img_1, 0, 1.2);
	Mat img_9 = changeImage(img_1, 0, 1.5);
	Mat img_10 = changeImage(img_1, 0, 1.7);


	pMatch(img_1, img_2, 1);
	pMatch(img_1, img_3, 2);
	pMatch(img_1, img_4, 3);
	pMatch(img_1, img_5, 4);
	pMatch(img_1, img_6, 5);
	pMatch(img_1, img_7, 6);
	pMatch(img_1, img_8, 7);
	pMatch(img_1, img_9, 8);
	pMatch(img_1, img_10, 9);
}

/*旋转变化对比*/
void orbTestRotate(Mat& img_1, void(*pMatch)(Mat& img_1, Mat& img_2, int time))
{
	Mat img_2 = changeImage(img_1, -30, 1);
	Mat img_3 = changeImage(img_1, -45, 1);
	Mat img_4 = changeImage(img_1, -60, 1);
	Mat img_5 = changeImage(img_1, -70, 1);
	Mat img_6 = changeImage(img_1, -80, 1);

	pMatch(img_1, img_2, 1);
	pMatch(img_1, img_3, 2);
	pMatch(img_1, img_4, 3);
	pMatch(img_1, img_5, 4);
	pMatch(img_1, img_6, 5);
}

/*混合变化对比*/
void orbTestMix(Mat& img_1, void(*pMatch)(Mat& img_1, Mat& img_2, int time))
{
	Mat img_2 = changeImage(img_1, -30, 1);
	Mat img_3 = changeImage(img_1, -70, 1);
	Mat img_4 = changeImage(img_1, 180, 1);
	Mat img_5 = changeImage(img_1, 0, 1);
	Mat img_6 = changeImage(img_1, 0, 0.2);
	Mat img_7 = changeImage(img_1, 0, 0.6);
	Mat img_8 = changeImage(img_1, 0, 1.3);

	pMatch(img_1, img_2, 1);
	pMatch(img_1, img_3, 2);
	pMatch(img_1, img_4, 3);
	pMatch(img_1, img_5, 4);
	pMatch(img_1, img_6, 5);
	pMatch(img_1, img_7, 6);
	pMatch(img_1, img_8, 7);
}

void goodORBScale(Mat& img_1)
{
	double scaleRate = 1.0;
	int goodPointNum = 1;
	while (goodPointNum)
	{
		scaleRate -= 0.1;
		Mat img_2 = changeImage(img_1, 0, scaleRate);
		//提取特征点
		Ptr<ORB> orb = ORB::create(1000);  // 在这里调整精度，值越小点越少，越精准
		vector<KeyPoint> keyPoint1, keyPoint2;
		orb->detect(img_1, keyPoint1);
		orb->detect(img_2, keyPoint2);
		//特征点描述，为下边的特征点匹配做准备
		Mat imageDesc1, imageDesc2;
		orb->compute(img_1, keyPoint1, imageDesc1);
		orb->compute(img_2, keyPoint2, imageDesc2);

		//获取特征点最近的两个关键点
		flann::Index flannIndex(imageDesc1, flann::LshIndexParams(12, 20, 2), cvflann::FLANN_DIST_HAMMING);
		vector<DMatch> GoodMatchePoints;
		Mat macthIndex(imageDesc2.rows, 2, CV_32SC1), matchDistance(imageDesc2.rows, 2, CV_32FC1);
		flannIndex.knnSearch(imageDesc2, macthIndex, matchDistance, 2, flann::SearchParams());

		// Lowe's algorithm,获取优秀匹配点
		for (int i = 0; i < matchDistance.rows; i++)
		{
			if (matchDistance.at<float>(i, 0) < 0.6 * matchDistance.at<float>(i, 1))//欧氏距离比例小于0.6
			{
				DMatch dmatches(i, macthIndex.at<int>(i, 0), matchDistance.at<float>(i, 0));
				GoodMatchePoints.push_back(dmatches);
			}
		}

		goodPointNum = GoodMatchePoints.size();
		cout << "When Scale Rate = " << scaleRate << " the GoodMatchPoints = " << goodPointNum << endl;
		if (goodPointNum == 0)
		{
			Mat first_match;
			drawMatches(img_2, keyPoint2, img_1, keyPoint1, GoodMatchePoints, first_match);
			imshow("OPENCV_ORB_0Match", first_match);
		}
	}

}