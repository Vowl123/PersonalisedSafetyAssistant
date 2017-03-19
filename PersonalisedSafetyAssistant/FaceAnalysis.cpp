#include "FaceAnalysis.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/ml/ml.hpp"
#include <iostream>
#include <fstream>
#include <vector>
using namespace cv;
using namespace std;

bool compareRect(cv::Rect r1, cv::Rect r2) { return r1.height < r2.height; }

void FaceAnalysis::read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';')
{
	ifstream file(filename.c_str(), ifstream::in);
	if (!file) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path, classlabel;
	vector<string> _labels;
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		if(!path.empty() && !classlabel.empty()) {
			images.push_back(imread(path, 0));
			_labels.push_back(classlabel.c_str());
			uniq_label.push_back(classlabel.c_str());
		}
	}
	int label_len = _labels.size();
	vector<string>::iterator iter = unique(uniq_label.begin(),uniq_label.end());
	uniq_label.erase(iter,uniq_label.end()); 
	int uniq_len = uniq_label.size();
	for (int i = 0; i < uniq_len; i++)
	{
		for (int j = 0; j < label_len; j++)
		{ //The format of labels must be int
			if (_labels[j].compare(uniq_label[i]))
				labels.push_back(i);
		}
	}
}

Rect FaceAnalysis::FaceDetection(Mat img)
{
	string face_cascade_name = "C:/opencv/sources/data/haarcascades/haarcascade_frontalface_default.xml";
	CascadeClassifier face_cascade;
	if( !face_cascade.load( face_cascade_name ) )
	{ 
		cerr << "Error loading face detection model." << endl;
		exit(0);
	}
	vector<Rect> faces;
	face_cascade.detectMultiScale(img, faces, 1.2, 2, 0, Size(50,50)); //detect faces
	if (faces.empty())//no faces are detected
		return Rect();
	Rect faceRect=*max_element(faces.begin(),faces.end(),[](Rect r1, Rect r2) { return r1.height < r2.height; }); //only the largest face bounding box are maintained
	return faceRect;
}

template <typename _Tp> inline
	void FaceAnalysis::elbp_(InputArray _src, OutputArray _dst, int radius, int neighbors)
{
	//get matrices
	Mat src = _src.getMat();
	// allocate memory for result
	_dst.create(src.rows-2*radius, src.cols-2*radius, CV_32SC1);
	Mat dst = _dst.getMat();
	// zero
	dst.setTo(0);
	for(int n=0; n<neighbors; n++) {
		float x = static_cast<float>(radius * cos(2.0*CV_PI*n/static_cast<float>(neighbors)));
		float y = static_cast<float>(-radius * sin(2.0*CV_PI*n/static_cast<float>(neighbors)));
		int fx = static_cast<int>(floor(x));
		int fy = static_cast<int>(floor(y));
		int cx = static_cast<int>(ceil(x));
		int cy = static_cast<int>(ceil(y));
		float ty = y - fy;
		float tx = x - fx;
		float w1 = (1 - tx) * (1 - ty);
		float w2 =      tx  * (1 - ty);
		float w3 = (1 - tx) *      ty;
		float w4 =      tx  *      ty;
		// iterate through your data
		for(int i=radius; i < src.rows-radius;i++) {
			for(int j=radius;j < src.cols-radius;j++) {
				// calculate interpolated value
				float t = static_cast<float>(w1*src.at<_Tp>(i+fy,j+fx) + w2*src.at<_Tp>(i+fy,j+cx) + w3*src.at<_Tp>(i+cy,j+fx) + w4*src.at<_Tp>(i+cy,j+cx));
				// floating point precision, so check some machine-dependent epsilon
				dst.at<int>(i-radius,j-radius) += ((t > src.at<_Tp>(i,j)) || (std::abs(t-src.at<_Tp>(i,j)) < std::numeric_limits<float>::epsilon())) << n;
			}
		}
	}
}

void FaceAnalysis::elbp(InputArray src, OutputArray dst, int radius, int neighbors)
{
	int type = src.type();
	switch (type) {
	case CV_8SC1:   elbp_<char>(src,dst, radius, neighbors); break;
	case CV_8UC1:   elbp_<unsigned char>(src, dst, radius, neighbors); break;
	case CV_16SC1:  elbp_<short>(src,dst, radius, neighbors); break;
	case CV_16UC1:  elbp_<unsigned short>(src,dst, radius, neighbors); break;
	case CV_32SC1:  elbp_<int>(src,dst, radius, neighbors); break;
	case CV_32FC1:  elbp_<float>(src,dst, radius, neighbors); break;
	case CV_64FC1:  elbp_<double>(src,dst, radius, neighbors); break;
	default:
		string error_msg = format("Using Original Local Binary Patterns for feature extraction only works on single-channel images (given %d). Please pass the image data as a grayscale image!", type);
		CV_Error(CV_StsNotImplemented, error_msg);
		break;
	}
}

Mat FaceAnalysis::elbp(InputArray src, int radius, int neighbors) {
	Mat dst;
	elbp(src, dst, radius, neighbors);
	return dst;
}

Mat FaceAnalysis::histc(InputArray src)
{
	static int uniform[256] = {
		0,1,2,3,4,58,5,6,7,58,58,58,8,58,9,10,11,58,58,58,58,58,58,58,12,58,58,58,13,58,
		14,15,16,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,17,58,58,58,58,58,58,58,18,
		58,58,58,19,58,20,21,22,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,
		58,58,58,58,58,58,58,58,58,58,58,58,23,58,58,58,58,58,58,58,58,58,58,58,58,58,
		58,58,24,58,58,58,58,58,58,58,25,58,58,58,26,58,27,28,29,30,58,31,58,58,58,32,58,
		58,58,58,58,58,58,33,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,34,58,58,58,58,
		58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,
		58,35,36,37,58,38,58,58,58,39,58,58,58,58,58,58,58,40,58,58,58,58,58,58,58,58,58,
		58,58,58,58,58,58,41,42,43,58,44,58,58,58,45,58,58,58,58,58,58,58,46,47,48,58,49,
		58,58,58,50,51,52,58,53,54,55,56,57};
	Mat _src=src.getMat();
	int width = _src.cols;
	int height = _src.rows;
	Mat result = Mat::zeros(59,1,CV_32SC1);
	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
		{
			uchar lbp_value = _src.at<int>(i,j);
			result.at<int>(uniform[lbp_value])++;
		}
	}
	return result;
}

Mat FaceAnalysis::spatial_histogram(Mat src, int numPatterns,
	int grid_x, int grid_y)
{
	int width = src.cols/grid_x;
	int height = src.rows/grid_y;
	Mat result = Mat::zeros(grid_x * grid_y, numPatterns, CV_32FC1);
	if(src.empty())
	{
		cout<<"empty"<<endl;
		return result.reshape(1,1);
	}

	int resultRowIdx = 0;
	for(int i = 0; i < grid_y; i++) {
		for(int j = 0; j < grid_x; j++) {
			Mat src_cell = Mat(src, Range(i*height,(i+1)*height), Range(j*width,(j+1)*width));
			//Mat cell_hist = histc_(src_cell, 0, (numPatterns-1), true);
			Mat cell_hist = histc(src_cell);
			Mat result_row = result.row(resultRowIdx);
			cell_hist.reshape(1,1).convertTo(result_row, CV_32FC1);
			resultRowIdx++;
		}
	}
	return result.reshape(1,1);
}

void FaceAnalysis::TrainingData(const string& filename) //filename is the path and name of CSV file
{
	vector<Mat> images;
	vector<int> _labels;
	try {
		FaceAnalysis::read_csv(filename, images, _labels); //read CSV file
	} catch (Exception& e) {
		cerr << "Error opening file \"" << filename << "\". Reason: " << e.msg << endl;
		exit(1);
	}
	int filesize=images.size();
	Mat labels(_labels); //use Mat structure to store labels
	labels.copyTo(label);
	trainSample.create(filesize, 59*block_x*block_y, CV_32FC1);
	Rect faceRect;
	for (int i=0;i<filesize;i++)
	{
		faceRect = FaceAnalysis::FaceDetection(images[i]);
		Mat face = images[i](faceRect);
		Mat normFace;
		resize(face,normFace,Size(80,80));
		Mat lbp_image = elbp(normFace,1,8);
		Mat hist = spatial_histogram(lbp_image,59,block_x,block_y); //Use LBP descriptor as facial representation
		Mat trainSample_row = trainSample.row(i);
		hist.copyTo(trainSample_row);
	}
}

void FaceAnalysis::FRTrain(bool saveTrain = false)
{
	CvSVMParams paras;
	paras.svm_type = CvSVM::C_SVC;
	paras.kernel_type = CvSVM::LINEAR;
	paras.C = 500;
	paras.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100000, FLT_EPSILON );
	SVM.train_auto(trainSample,label,Mat(),Mat(),paras);
	if (saveTrain) //Save the SVM training model
	{
		SVM.save("C:/Users/Josh/Documents/Visual Studio 2012/Git/PersonalisedSafetyAssistant/svmFR.xml");
		FileStorage fs("C:/Users/Josh/Documents/Visual Studio 2012/Git/PersonalisedSafetyAssistant/ID.xml",FileStorage::WRITE); //XML file storage
		if (!fs.isOpened())
		{
			cerr<<"failed to open "<<endl;
		}
		write(fs, "IDno", uniq_label);
		fs.release();
	}
}

void FaceAnalysis::FR(bool loadSVM)
{
	if (loadSVM)
	{
		SVM.load("C:/Users/Josh/Documents/Visual Studio 2012/Git/PersonalisedSafetyAssistant/svmFR.xml");
		FileStorage fs("C:/Users/Josh/Documents/Visual Studio 2012/Git/PersonalisedSafetyAssistant/ID.xml", cv::FileStorage::READ); //read data from XML file
		if (!fs.isOpened())
		{
			cerr<<"failed to open "<<endl;  
		}
		FileNode IDnode = fs["IDno"];
		read(IDnode, uniq_label);
		fs.release();
	}
	VideoCapture capture(0);
	if ( !capture.isOpened() )  // if not success, exit program
	{
		cout << "Cannot open the camera" << endl;
		exit(0);
	}
	Mat frame,Gray_img;
	Rect faceRect;
	int id;
	namedWindow("FaceRecognition", CV_WINDOW_AUTOSIZE);
	char key = 0;
	while(key != 27) //press "Esc" to stop
	{
		capture>>frame;
		faceRect = FaceAnalysis::FaceDetection(frame);
		if (faceRect.width == 0)
		{
			imshow("FaceRecognition",frame);
			key=waitKey(30);
			continue;
		}
		cvtColor(frame(faceRect),Gray_img, CV_BGR2GRAY); //change color image to its gray scale
		Mat normFace;
		resize(Gray_img,normFace,Size(80,80));
		Mat lbp_image = elbp(normFace,1,8);
		Mat hist = spatial_histogram(lbp_image,59,block_x,block_y);
		id = (int)SVM.predict(hist);
		string text = uniq_label[id];
		rectangle(frame,faceRect,Scalar(0,0,255));
		putText(frame, text, Point(faceRect.x+2,faceRect.y+12), CV_FONT_HERSHEY_COMPLEX, 0.5, cvScalar(255, 255, 0)); // overlay text
		imshow("FaceRecognition",frame);
		key=waitKey(10);
	}
} 