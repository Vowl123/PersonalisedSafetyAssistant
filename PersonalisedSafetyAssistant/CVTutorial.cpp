#include "cvtutorial.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/ml/ml.hpp"
#include <iostream>
#include <vector>
#include <fstream>
using namespace cv;
using namespace std;

void cvtutorial::read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';')
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
			cout << _labels[j].compare(uniq_label[i]);
			if (_labels[j].compare(uniq_label[i]))
				labels.push_back(i);
		}
	}
}

Rect cvtutorial::FaceDetection(Mat img)
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

Mat cvtutorial::computeHoG(Mat img)
{
	Mat resized_img;
	resize(img,resized_img,Size(64,128)); //resize image
	HOGDescriptor hog;
	vector<float>descriptors; //descriptors is used to store a HoG histogram
	hog.compute(resized_img, descriptors,Size(0,0), Size(0,0)); //compute HoG histogram
	Mat HoGHist(descriptors); //use mat structure to store HoG histogram
	return HoGHist.t();
}

void cvtutorial::TrainingData(const string& filename) //filename is the path and name of CSV file
{
	vector<Mat> images;
	vector<int> _labels;
	try {
		cvtutorial::read_csv(filename, images, _labels); //read CSV file
	} catch (Exception& e) {
		cerr << "Error opening file \"" << filename << "\". Reason: " << e.msg << endl;
		exit(1);
	}
	int filesize=images.size();
	Mat labels(_labels); //use Mat structure to store labels
	labels.copyTo(label);
	DataTrain.create(filesize, HoGHistLength, CV_32FC1);
	Mat HoGHist;
	Rect faceRect;
	for (int i = 0; i < filesize; i++)
	{
		faceRect = cvtutorial::FaceDetection(images[i]);
		HoGHist = cvtutorial::computeHoG(images[i](faceRect)); //Use HoG descriptor as facial representation
		Mat DataTrain_row = DataTrain.row(i); //Each row of DataTrain is a training sample
		HoGHist.copyTo(DataTrain_row);
	}
}

void cvtutorial::FRTrain(bool saveTrain = false)
{
	CvSVMParams paras;
	paras.svm_type = CvSVM::C_SVC;
	paras.kernel_type = CvSVM::LINEAR;
	paras.C = 500;
	paras.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100000, FLT_EPSILON );
	SVM.train(DataTrain,label,Mat(),Mat(),paras);
	if (saveTrain) //Save the SVM training model
	{
		SVM.save("C:/Users/Josh/Documents/Visual Studio 2012/Git/PersonalisedSafetyAssistant/svmFR.xml");
		FileStorage fs("ID.xml",FileStorage::WRITE); //XML file storage
		if (!fs.isOpened())
		{
			cerr<<"failed to open "<<endl;
		}
		write(fs, "IDno", uniq_label);
		fs.release();
	}
}

void cvtutorial::FR(bool loadSVM)
{
	if (loadSVM)
	{
		SVM.load("C:/Users/Josh/Documents/Visual Studio 2012/Git/PersonalisedSafetyAssistant/svmFR.xml");
		FileStorage fs("ID.xml", cv::FileStorage::READ); //read data from XML file
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
	Mat frame,HoGHist;
	Rect faceRect;
	int id;
	namedWindow("FaceRecognition", CV_WINDOW_AUTOSIZE);
	char key = 0;
	while(key != 27) //press "Esc" to stop
	{
		capture>>frame;
		faceRect = cvtutorial::FaceDetection(frame);
		if (faceRect.width == 0)
		{
			imshow("FaceRecognition",frame);
			key=waitKey(30);
			continue;
		}
		HoGHist = cvtutorial::computeHoG(frame(faceRect));
		id = (int)SVM.predict(HoGHist);
		string text = uniq_label[id];
		rectangle(frame,faceRect,Scalar(0,0,255));
		putText(frame, text, Point(faceRect.x+2,faceRect.y+12), CV_FONT_HERSHEY_COMPLEX, 0.5, cvScalar(255, 255, 0)); // overlay text
		imshow("FaceRecognition",frame);
		key=waitKey(20);
	}
}
