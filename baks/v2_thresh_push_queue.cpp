// deque stores the ROI, short time memory match
#include <opencv2/core/utility.hpp>
#include <opencv2/core.hpp>
//#include <opencv2/saliency.hpp> // contrib/modules/saliency
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/xfeatures2d.hpp"
#include <iostream>
#include <ctime>
#include <queue> 
#include <cassert>
using namespace std;
using namespace cv;
//using namespace saliency;
//using namespace cv::xfeatures2d;
int pixel_counter(Mat src, int nflag){
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
bool closed(Mat src, Mat container){
    Mat dst_diff; // abs difference
    bool pp_state = false;
    if(src.size() == container.size()){ // size should be equal
      absdiff(src, container, dst_diff); // a-b=c
      cvtColor(dst_diff, dst_diff, COLOR_BGR2GRAY);
      threshold(dst_diff, dst_diff, 20, 255, THRESH_BINARY); // a binary will be easier to understand the difference
      float nBlackCount = pixel_counter(dst_diff, 2); // count the blacks
      float area = dst_diff.cols * dst_diff.rows; // area of the ROI
      float ratio = nBlackCount / area; // judge ratio to decide pianpian state
      cout << nBlackCount << "/" << area << "=" << ratio;
      pp_state = ratio < 0.90?false:true; // make 0.7 an argument rather than a hard-coding 1:0.999 2:0.999 3:
      resize(src, src, Size(200, 200));
      resize(dst_diff, dst_diff, Size(200, 200));
      resize(container, container, Size(200, 200));
      imshow("dst", dst_diff);
      imshow("crop of source", src);
      imshow("front of queue", container);
      waitKey(30);
    }
    return pp_state;
}

int main( int argc, char** argv )
{
    namedWindow("video source", WINDOW_AUTOSIZE);
    VideoCapture cap;
    string video_path = "/home/hurton/Desktop/play/pianpian_match/tkvideo20200608/2020-06-08/172.16.40.213_01_20200608195720578.mp4";
    cap.open(video_path);
    Rect2d r; // ROI rect
    Mat frame; // store src
    string msg = ""; // gone or back
    Mat img_crop; // target pianpian
    queue<Mat> ROIs_queue;
    int frame_counter = 0;
    bool state = false; 
    while (1){
      cap >> frame;
      if(frame.empty()){
        cerr << "input empty!";
        break;
      }
      frame_counter += 1;
      resize(frame, frame, Size(640,480)); // depends on ur fenbianlv, src is too large for me
      // --------------------------- select ROI ---------------------------------------
      if (waitKey(33) >= 0) { // clicked keyboard, any key
        r = selectROI(frame, false); // false means no grid 
        img_crop = frame(r); // gotcha nigger
        assert(!img_crop.empty());
        cout << r.x << "," << r.y << endl;
        ROIs_queue.push(img_crop);
        //resize(img_crop, img_crop, Size(200,200)); // better view 
        //imshow("Image", img_crop); 
      }
      if(!r.empty()){
        Mat src_crop; // corresponding ROI in src
        //if(frame_counter % 2 && frame_counter < 1001){
        frame_counter += 1;
          
        src_crop = frame(r);
        state = closed(src_crop, ROIs_queue.front());
        if(state){
          ROIs_queue.push(src_crop);
        }
        cout << " queue size:" << ROIs_queue.size() << endl;
        //}
        //else{
        //  frame_counter = 0;
        //}
      }
      while(ROIs_queue.size()>5)
        ROIs_queue.pop();
      msg = state?"CLOSED":"OPENED";
      Scalar color = state?Scalar(0, 255, 0):Scalar(0, 0, 255);
      putText(frame, msg, Point(20, 100), FONT_HERSHEY_COMPLEX, 1, color, 2, 8, 0);
      imshow("video source",frame);
      waitKey(30);

    }
    return 0;
}
