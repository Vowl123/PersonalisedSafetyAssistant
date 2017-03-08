#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "personalisedsafetyassistant/cvtutorial.h"
#include <iostream>
#include <vector>
#include <fstream>
using namespace cv;	
using namespace std;
bool compareRect(cv::Rect r1, cv::Rect r2) { return r1.height < r2.height; }
Mat captureFrame (VideoCapture capture)
{
	Mat frame;
	
	capture.read(frame);

	return frame;

}

int main()
{
	cvtutorial a;
	/*string face_cascade_name = "C:/opencv/sources/data/haarcascades/haarcascade_frontalface_default.xml";
	CascadeClassifier face_cascade;
	if( !face_cascade.load( face_cascade_name ) )
	{ 
		cerr << "Error loading face detection model." << endl;
		exit(0);
	}
	vector<Rect> faces;

	VideoCapture capture(0); // open the video file for reading
	if ( !capture.isOpened() )  // if not success, exit program
	{
		cout << "Cannot open the video file" << endl;
		return -1;
	}
	Mat frame;
	string name,fullfile,data_record;
	cout<<"Please input your name: ";
	cin>>name;
	string path = "C:/database/face/";
	namedWindow("collection", CV_WINDOW_AUTOSIZE);
	char key = 0;
	int count = 0;
	ofstream FR_CSV;
	FR_CSV.open(path + "FaceRecognition.txt",ios::app); //Open file

	while(key != 27) // press "Esc" to stop
	{
		capture>>frame;
		face_cascade.detectMultiScale(frame, faces, 1.2, 2, 0, Size(50,50)); //detect faces
		if (faces.empty()) { //no faces are detected
			key = waitKey(30);
		} else {
			if (count < 5){
				stringstream name_count;
				name_count<<++count;
				fullfile = path + name + name_count.str() + ".jpg";
				data_record = fullfile + ";" + name;
				imwrite(fullfile,frame); //save the picture
				FR_CSV << data_record << endl;
				imshow("collection",frame);
				cout << name_count.str() << endl;
				key = waitKey(60000);
			}
			key = waitKey(30);
		}
	}
	FR_CSV.close();*/
	a.TrainingData("C:/database/face/FaceRecognition.txt");
	a.FRTrain(true);
	a.FR(false);
	return 0;
}