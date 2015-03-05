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

int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

int main(int argc, const char** argv)
{
    cv::CommandLineParser cmd(argc, argv,
        "{ c | camera | false       | use camera }"
        "{ f | file   | 768x576.avi | input video file }"
        "{ m | method | mog         | method (fgd, mog, mog2, gmg) }"
        "{ h | help   | false       | print help message }");

    if (cmd.get<bool>("help"))
    {
        cout << "Usage : bgfg_segm [options]" << endl;
        cout << "Avaible options:" << endl;
        cmd.printParams();
        return 0;
    }

    bool useCamera = cmd.get<bool>("camera");
    string file = cmd.get<string>("file");
    string method = cmd.get<string>("method");

    if (method != "fgd"
        && method != "mog"
        && method != "mog2"
        && method != "gmg")
    {
        cerr << "Incorrect method" << endl;
        return -1;
    }

    Method m = method == "mog" ? MOG2 :
                                  MOG2;

    VideoCapture cap;
    cap.open(0);

    if (!cap.isOpened())
    {
        cerr << "can not open camera or video file" << endl;
        return -1;
    }

    Mat frame;
    Mat grayscaleFrame;
    cap >> frame;
    GpuMat d_frame(frame);

    FGDStatModel fgd_stat;
    MOG_GPU mog;
    MOG2_GPU mog2;
    GMG_GPU gmg;
    gmg.numInitializationFrames = 40;

    GpuMat d_fgmask;
    GpuMat d_fgimg;
    GpuMat d_bgimg;

    Mat fgmask;
    Mat fgimg;
    Mat bgimg;

    mog2(d_frame, d_fgmask);
    namedWindow("original", WINDOW_NORMAL);
    namedWindow("image", WINDOW_NORMAL);
    namedWindow("blurred", WINDOW_NORMAL);
    namedWindow("threshold", WINDOW_NORMAL);
    Size ksize;
    ksize.height = 13;
    ksize.width = 25;
    vector<int> data;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    for(;;)
    {
        cap >> frame;
        if (frame.empty())
            break;
	
	imshow("original", frame);
	
	inRange(frame, Scalar(0, 55, 90, 255), Scalar(70, 175, 230, 255), grayscaleFrame);
	d_frame.upload(grayscaleFrame);
	
	gpu::GpuMat dst(grayscaleFrame);
	gpu::GpuMat dst1(grayscaleFrame);
	gpu::GpuMat dst2(grayscaleFrame);

	gpu::GaussianBlur(d_frame, dst, ksize, 0);

	imshow("image", grayscaleFrame);	
	dst.download(grayscaleFrame);
        imshow("blurred", grayscaleFrame);
	
	gpu::threshold(dst, dst1, thresh, max_thresh, THRESH_BINARY);
	dst1.download(frame);
	imshow("threshold", frame);
	findContours(frame, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
        convexHull(Mat(contours[0]), data);
	
	//printf("x=%d y=%d\n", contours[0][data[0]].x, contours[0][data[0]].y);
	
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
	
        int key = waitKey(30);
        if (key == 27)
            break;
    }

    return 0;
}



