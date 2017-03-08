#ifndef _CVTUTORIAL_H_
#define _CVTOTURIAL_H_
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/ml/ml.hpp"
#include <vector>
using namespace std;
using namespace cv;
#define HoGHistLength 3780
class cvtutorial
{
public:
	void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator);
	Rect FaceDetection(Mat img);
	Mat computeHoG(Mat img);
	void TrainingData(const string& filename);
	void FRTrain(bool saveTrain);
	void FR(bool loadSVM);
public:
	vector<string> uniq_label;
	Mat DataTrain;
	Mat label;
	CvSVM SVM;
};
#endif 