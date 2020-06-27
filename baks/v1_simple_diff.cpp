// simple difference, real time, process each frame
#include <opencv2/core/utility.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/xfeatures2d.hpp"
#include <iostream>
#include <ctime>
using namespace std;
using namespace cv;
//using namespace saliency;
using namespace cv::xfeatures2d;
int PixelCounter(Mat src, int nflag){
	int nCount_White = 0;
	int nCount_Black = 0;
	Mat_<uchar>::iterator itor = src.begin<uchar>();
	Mat_<uchar>::iterator itorEnd = src.end<uchar>();
	for (; itor != itorEnd; ++itor){
		if ((*itor) > 0){//white
			nCount_White += 1;
		}
		else{//black
			nCount_Black += 1;
		}
	}
  int &ret = nflag == 1?nCount_White:nCount_Black;
  return ret;
}

int main( int argc, char** argv )
{
    namedWindow("video source", WINDOW_AUTOSIZE);
    VideoCapture cap;
    string video_path = "/home/hurton/Desktop/tkvideo20200608/video4.mp4";
    cap.open(video_path);
    Rect2d r; // ROI rect
    Mat frame; // store src
    string msg = ""; // gone or back
    Mat img_crop; // target pianpian
    bool gone = false; // pianpian gone or back flag
    while (1){
        cap >> frame;
        if(frame.empty()){
          break;
        }
        resize(frame, frame, Size(640,480)); // depends on ur fenbianlv, src is too large for me
        if (waitKey(33) >= 0) { // clicked keyboard, any key
          r = selectROI(frame, false); // false means no grid 
          img_crop = frame(r); 
          resize(img_crop, img_crop, Size(200,200)); // broad view 
          imshow("Image", img_crop); 
        }
        Mat src_crop; // corresponding ROI in src
        if(!r.empty()){
          src_crop = frame(r);
          resize(src_crop, src_crop, Size(200,200));
          Mat dst_diff; // abs difference
          if(src_crop.size() == img_crop.size()){ // size should be equal
            absdiff(src_crop, img_crop, dst_diff); // a-b=c
            threshold(dst_diff, dst_diff, 50, 255, THRESH_BINARY); // a binary will be easier to understand the difference
            float nBlackCount = PixelCounter(dst_diff, 2); // count the blacks
            float area = dst_diff.cols * dst_diff.rows; // area of the ROI
            float ratio = nBlackCount / area; // judge ratio to decide pianpian state
            cout << nBlackCount << "/" << area << "=" << ratio << endl;
            gone = ratio < 0.999?true:false; // make 0.7 an argument rather than a hard-coding 1:0.999 2:0.999 3:
            imshow("dst", dst_diff);
            imshow("crop of source", src_crop);
            waitKey(30);
          }
          else{
            cerr << "Crops not same size" ;
          }
        }
        msg = gone?"OPENED":"CLOSED";
        Scalar color = gone?Scalar(0, 0, 255):Scalar(0, 255, 0);
        putText(frame, msg, Point(20, 100), FONT_HERSHEY_COMPLEX, 1, color, 2, 8, 0);
        imshow("video source",frame);
        waitKey(30);
    }
 
  return 0;
}
