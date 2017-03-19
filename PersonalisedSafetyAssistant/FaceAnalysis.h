#ifndef _FACE_ANALYSIS_H_
#define _FACE_ANALYSIS_H_
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/ml/ml.hpp"
#include<vector>
#include <string>
using namespace cv;
using namespace std;

#define block_x 8
#define block_y 8

class FaceAnalysis
{
public:
	void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels);
	void FRTrain(vector<Mat> images, vector<int> labels, bool saveTrain);
	void FR(bool loadFFR, int imWidth, int imHeight);
public:
	Ptr<FaceRecognizer> FFR;
};
#endif
