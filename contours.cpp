#include <QApplication>
#include <iostream>
#include <string>   // for strings
#include <QSlider>
#include <QGridLayout>

#include <opencv2/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui.hpp>  // Video write
#include "detector.h"
//#include <stdlib.h>
using namespace std;
using namespace cv;


QSlider *sl1, *sl2;
int thresh = 200;
int max_thresh = 255;


CV_WRAP static int fcc=VideoWriter::fourcc('D','I','V','X');

void setLabel(Mat& im, const string label, vector<Point >& contour);

void CallBackFunc(int event, int x, int y, int flags, void* userdata);

int main(int argc, char *argv[])
{


    QApplication a(argc, argv);

    int clr[100][3];
    for(int i=0;i<100;i++)
        for(int j=0;j<3;j++)
            clr[i][j]=rand()%256;

    char *c1="hello";
    namedWindow( c1, CV_WINDOW_AUTOSIZE);
    setMouseCallback(c1, CallBackFunc, NULL);
    QWidget W;

    QGridLayout* GL=new QGridLayout();
    W.setLayout(GL);
    W.show();

    sl1=new QSlider();
    sl1->setGeometry(QRect(0,0,200,50));
    sl1->setOrientation(Qt::Horizontal);
    sl1->setRange(0,255);
    sl1->setValue(125);
    //    sl1->show();

    sl2=new QSlider();
    sl2->setGeometry(QRect(0,0,200,50));
    sl2->setOrientation(Qt::Horizontal);
    sl2->setRange(0,255);
    sl2->setContentsMargins(0,100,200,50);
    //    sl2->show();
    GL->addWidget(sl1,0,0);
    GL->addWidget(sl2,1,0);

    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;
    Mat src, src_gray;
    Mat dst, dst_norm, dst_norm_scaled;
    dst = Mat::zeros( src.size(), CV_32FC1 );

    cap>>src;
    VideoWriter outputVideo=VideoWriter( "C:\\Users\\chibi\\Documents\\build-MyQT_try-Desktop_Qt_5_7_1_MinGW_32bit-Release\\release\\hello.avi",
                                         fcc,30, src.size());//fcc

    Mat video_mat;
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    for(;;) //Show the image captured in the window and repeat
    {
        float krec=1;
        cap >> src; // get a new frame from camera
        cvtColor(src,dst,CV_RGB2GRAY);
        video_mat=dst;
//         cvtColor(dst,video_mat,CV_BGR2GRAY);
        blur(dst,dst,Size(3,3));
        Canny(dst, dst, sl1->value(), 240, 3);
        //        threshold(dst,dst,sl1->value(),255,CV_THRESH_BINARY);

        //         Mat drawing = Mat::zeros( dst.size(), CV_8UC1 );
        findContours( dst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
        cvtColor(dst,dst,CV_GRAY2BGR);

        vector<vector<Point>> approx;
        approx.resize(contours.size());
        for(int i=0;i<contours.size();i++)
        {
            approxPolyDP(Mat(contours[i]), approx[i], arcLength(Mat(contours[i]), true)*0.02, true);//.02
        }

        for(int i=0;i<approx.size();i++)
        {

            if (fabs(contourArea(approx[i])) < 500 || !isContourConvex(approx[i]) )
                continue;

//            drawContours(src,approx,i,Scalar(clr[i%100][0], clr[i%100][1], clr[i%100][2]),1);
            Rect r = boundingRect(approx[i]);


            if(approx[i].size()==4)
            {
                drawContours(src,approx,i,Scalar(0,0,255), 3);
                setLabel(src, "square" , approx[i]);
                rectangle(src,r,Scalar(0,255,0));
            }
        }

        //        qDebug()<<contours.size();

        outputVideo << src;

        imshow(c1,src);
        if(waitKey(30) >= 0) break;
    }

}


void setLabel(Mat& im, const string label, vector<Point >& contour)
{
    int fontface = cv::FONT_HERSHEY_SIMPLEX;
    double scale = 0.4;
    int thickness = 1;
    int baseline = 0;

    Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
    Rect r = cv::boundingRect(contour);

    Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
    rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255,255,255), CV_FILLED);
    putText(im, label, pt, fontface, scale, CV_RGB(0,0,0), thickness, 8);
}


void CallBackFunc(int event, int x, int y, int flags, void* userdata){
    if  ( event == EVENT_MBUTTONDOWN )
    {
        cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
}
