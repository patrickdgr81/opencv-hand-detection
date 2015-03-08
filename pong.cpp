//#include <Python.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <allegro.h>
#include <cstdlib>
#include <time.h>

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

int findMove(){
  if (!setupAlready) {
    setup();
    setupAlready = true;
  }
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
  return avg;
}


int ball_x = 320;
int ball_y = 240;

int ball_tempX = 320;
int ball_tempY = 240;

int p1_x = 20;
int p1_y = 210;

int p1_tempX = 20;
int p1_tempY = 210;

int p2_x = 620;
int p2_y = 210;

int p2_tempX = 620;
int p2_tempY = 210;

time_t secs;    //The seconds on the system clock will be stored here
                //this will be used as the seed for srand()

int dir;     //This will keep track of the circles direction
            //1= up and left, 2 = down and left, 3 = up and right, 4 = down and right

BITMAP *buffer; //This will be our temporary bitmap for double buffering

void moveBall(){

    ball_tempX = ball_x;
    ball_tempY = ball_y;

    if (dir == 1 && ball_x > 5 && ball_y > 5){
     
         if( ball_x == p1_x + 15 && ball_y >= p1_y && ball_y <= p1_y + 60){
                  dir = rand()% 2 + 3;
         }else{    
                 --ball_x;
                 --ball_y;
         }    
              
    } else if (dir == 2 && ball_x > 5 && ball_y < 475){

         if( ball_x == p1_x + 15 && ball_y >= p1_y && ball_y <= p1_y + 60){
                  dir = rand()% 2 + 3;
         }else{    
                 --ball_x;
                 ++ball_y;
         }

    } else if (dir == 3 && ball_x < 635 && ball_y > 5){

         if( ball_x + 5 == p2_x && ball_y >= p2_y && ball_y <= p2_y + 60){
                  dir = rand()% 2 + 1;
         }else{    
                 ++ball_x;
                 --ball_y;
         }

    } else if (dir == 4 && ball_x < 635 && ball_y < 475){

         if( ball_x + 5 == p2_x && ball_y >= p2_y && ball_y <= p2_y + 60){
                  dir = rand()% 2 + 1;
         }else{    
                 ++ball_x;
                 ++ball_y;
         }

    } else { 

        if (dir == 1 || dir == 3)    ++dir;
        else if (dir == 2 || dir == 4)    --dir;

    }    
    
    acquire_screen();
    circlefill ( buffer, ball_tempX, ball_tempY, 5, makecol( 0, 0, 0));
    circlefill ( buffer, ball_x, ball_y, 5, makecol( 128, 255, 0));
    draw_sprite( screen, buffer, 0, 0);
    release_screen();
    
    rest(5);

}    
int prevHand = 0;
void p1Move(){
 
    p1_tempY = p1_y;
    int currentPos = findMove();
    if( currentPos > prevHand && p1_y > 0){
     
        --p1_y;
              
    } else if( currentPos > prevHand && p1_y < 420){
     
        ++p1_y;
              
    }     
    
    acquire_screen();
    rectfill( buffer, p1_tempX, p1_tempY, p1_tempX + 10, p1_tempY + 60, makecol ( 0, 0, 0));
    rectfill( buffer, p1_x, p1_y, p1_x + 10, p1_y + 60, makecol ( 0, 0, 255));
    release_screen();
          
}  

void p2Move(){
 
    p2_tempY = p2_y;
 
    if( key[KEY_UP] && p2_y > 0){
     
        --p2_y;
              
    } else if( key[KEY_DOWN] && p2_y < 420){
     
        ++p2_y;
              
    }     
    
    acquire_screen();
    rectfill( buffer, p2_tempX, p2_tempY, p2_tempX + 10, p2_tempY + 60, makecol ( 0, 0, 0));
    rectfill( buffer, p2_x, p2_y, p2_x + 10, p2_y + 60, makecol ( 0, 0, 255));
    release_screen();
          
}    

void startNew(){

    clear_keybuf();
    readkey();
    clear_to_color( buffer, makecol( 0, 0, 0));
    ball_x = 320;
    ball_y = 240;

    p1_x = 20;
    p1_y = 210;

    p2_x = 620;
    p2_y = 210;

}    

void checkWin(){

    if ( ball_x < p1_x){
        textout_ex( screen, font, "Player 2 Wins!", 320, 240, makecol( 255, 0, 0), makecol( 0, 0, 0)); 
        startNew();
    } else if ( ball_x > p2_x){
        textout_ex( screen, font, "Player 1 Wins!", 320, 240, makecol( 255, 0, 0), makecol( 0, 0, 0)); 
        startNew();
    }    
   
}    

void setupGame(){
 
    acquire_screen();
    rectfill( buffer, p1_x, p1_y, p1_x + 10, p1_y + 60, makecol ( 0, 0, 255));
    rectfill( buffer, p2_x, p2_y, p2_x + 10, p2_y + 60, makecol ( 0, 0, 255));  
    circlefill ( buffer, ball_x, ball_y, 5, makecol( 128, 255, 0));
    draw_sprite( screen, buffer, 0, 0);
    release_screen();
    
    time(&secs);
    srand( (unsigned int)secs);
    dir = rand() % 4 + 1;
            
}    

int main(){

    allegro_init();
    install_keyboard();
    set_color_depth(16);
    set_gfx_mode( GFX_AUTODETECT, 640, 480, 0, 0);
    
    buffer = create_bitmap( 640, 480); 
    
    setupGame();
    
    while( !key[KEY_ESC]){

        p1Move();
        p2Move();
        moveBall();
        checkWin();
   
    }    
    
    return 0;

}
END_OF_MAIN();
