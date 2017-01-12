#include "opencv2/highgui/highgui.hpp"
#include <iostream>
using namespace cv;	
using namespace std;

Mat captureFrame (VideoCapture capture)
{
	Mat frame;
	
	capture.read(frame);

	return frame;

}

int main()
{
	VideoCapture capture(0); // open the video file for reading
	if ( !capture.isOpened() )  // if not success, exit program
	{
		cout << "Cannot open the video file" << endl;
		return -1;
	}

	double fps = 30; //get the frames rate of the video
	int delay = (int)(1000/fps); //delay between two frames
	Mat frame;
	namedWindow("video",CV_WINDOW_AUTOSIZE);

	int key = 0;
	while(key != 27) // press "Esc" to stop
	{ //Pipeline to go here
		

		Mat matrix = captureFrame(capture);

		// imshow("video", matrix); Commented for benchmarking

		key=waitKey(delay);
	}


	return 0;
}


