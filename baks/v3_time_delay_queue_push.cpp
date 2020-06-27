// queue push only when time is up(no pop?)
#include <opencv2/core/utility.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <ctime>
#include <queue> 
#include <cassert>
#include <chrono>
#include <unistd.h>
#include <stack>
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
float closed(Mat src, Mat container){
    Mat dst_diff; // abs difference
    bool pp_state = false;
    float ratio;
    if(src.size() == container.size()){ // size should be equal
      absdiff(src, container, dst_diff); // a-b=c
      cvtColor(dst_diff, dst_diff, COLOR_BGR2GRAY);
      threshold(dst_diff, dst_diff, 20, 255, THRESH_BINARY); // a binary will be easier to understand the difference
      float nBlackCount = pixel_counter(dst_diff, 2); // count the blacks
      float area = dst_diff.cols * dst_diff.rows; // area of the ROI
      ratio = nBlackCount / area; // judge ratio to decide pianpian state
      // cout << nBlackCount << "/" << area << "=" << ratio;
      resize(src, src, Size(200, 200));
      resize(dst_diff, dst_diff, Size(200, 200));
      resize(container, container, Size(200, 200));
      imshow("dst", dst_diff);
      imshow("crop of source", src);
      imshow("front of queue", container);
      waitKey(30);
    }
    
    // aPair.first = ratio;
    // aPair.second = 
    return ratio;
}
bool my_cmp(pair<Mat, float> a, pair<Mat, float> b){
  if(a.second < b.second){
    return true;
  }
  else{
    return false;
  }
}
int main( int argc, char** argv )
{
    auto start = chrono::system_clock::now();
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
    bool state; // pianpian state
    float ret_ratio; // distance between src crop and crop in queue
    vector<pair<Mat, float> > find_max; // compare ratio to decide which to push to queue
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
        // resize(img_crop, img_crop, Size(200,200)); // better view 
        // imshow("Image", img_crop); 
      }
      // --------------------------- video src ROI compare ----------------------------
      if(!r.empty()){
        Mat src_crop; // corresponding ROI in src
        // if(frame_counter % 2 && frame_counter < 1001){
        // frame_counter += 1;
        src_crop = frame(r);
        ret_ratio = closed(src_crop, ROIs_queue.front());
        state = ret_ratio< 0.90?false:true;
        if(state){
          find_max.push_back(make_pair(src_crop, ret_ratio));
          auto end = chrono::system_clock::now();
          chrono::duration<double> elapsed_seconds = end-start;
          cout << "time elapsed:" << elapsed_seconds.count() << endl;
          // 2 seconds passed, actual time can be 2 hours
          if(elapsed_seconds.count() > 2){
            start = end; // refresh
            ROIs_queue.push(src_crop); 
            time_t end_time = chrono::system_clock::to_time_t(end);   
            cout << "push at " << ctime(&end_time)
                      << "elapsed time: " << elapsed_seconds.count() << "s\n";
            pair<Mat, float> max_one = *max_element(find_max.begin(), find_max.end(), my_cmp);
            // duplicate data, erase first front, may be a bug according to y`s algorithm
            ROIs_queue.front() = max_one.first;
          }
        }
        cout << " queue size:" << ROIs_queue.size() << endl;
        //}
        //else{
        //  frame_counter = 0;
        //}
      }
      //while(ROIs_queue.size()>5)
      //  ROIs_queue.pop();

      // --------------------------- put text ---------------------------------------
      msg = state?"CLOSED":"OPENED";
      Scalar color = state?Scalar(0, 255, 0):Scalar(0, 0, 255);
      putText(frame, msg, Point(20, 100), FONT_HERSHEY_COMPLEX, 1, color, 2, 8, 0);
      imshow("video source",frame);
      waitKey(30);

    }
    return 0;
}
