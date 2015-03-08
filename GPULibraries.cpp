//#include <Python.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;
using namespace cv::gpu;

enum Method
{
    FGD_STAT,
    MOG,
    MOG2,
    GMG
};
bool setupAlready = false;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

VideoCapture cap;
Mat frame;
Mat grayscaleFrame;
vector<int> data;
vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
Size ksize;
int setup() {
  cap.open(0);
  if (!cap.isOpened())
    {
        cerr << "can not open camera or video file" << endl;
        return -1;
    }
   cap >> frame;
   GpuMat d_fgmask;
   GpuMat d_fgimg;
   GpuMat d_bgimg;

   Mat fgmask;
   Mat fgimg;
   Mat bgimg;
   ksize.height = 13;
   ksize.width = 25;
   return 1;
}

int main(int argc, const char** argv){
  if (!setupAlready) {
    setup();
    setupAlready = true;
  }
  printf("In main!\n");
  GpuMat d_frame(frame);
  cap >> frame;
  if (frame.empty())
     return 0;
  inRange(frame, Scalar(0, 55, 90, 255), Scalar(70, 175, 230, 255), grayscaleFrame);
  d_frame.upload(grayscaleFrame);
	
  gpu::GpuMat dst(grayscaleFrame);
  gpu::GpuMat dst1(grayscaleFrame);
  gpu::GpuMat dst2(grayscaleFrame);

  gpu::GaussianBlur(d_frame, dst, ksize, 0);
  gpu::threshold(dst, dst1, thresh, max_thresh, THRESH_BINARY);
  dst1.download(frame);
  findContours(frame, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
  convexHull(Mat(contours[0]), data);

  int value = 0;
  int count = 0;
  for(int i =0; i < contours.size(); i++) {
     for(int j = 0; j < contours.at(i).size(); j++) {
	 value += contours[i][j].y;
	 count++;
      }
  }
  int avg = value/count;
  printf("avg: %d\n",avg);
  printf("DONE!\n");
  return avg;
}
