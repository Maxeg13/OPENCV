//read about contours hierarchy.
//calculated on the 3 contours images. canny leads to 3 additional contours
//

#include <QApplication>
#include <iostream>
#include <string>   // for strings
#include <QSlider>
#include <QGridLayout>

#include <opencv2/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui.hpp>  // Video write
#include "detector.h"
//#include <stdlib.h>

//#define WRITING

using namespace std;
using namespace cv;



QSlider *sl1, *sl2;
int thresh = 200;
int max_thresh = 255;
Point centrs[3], centrs_buf[3];
QVector<Point> centrH;
vector<int> areas;

CV_WRAP static int fcc=VideoWriter::fourcc('D','I','V','X');

bool ratio(float x,float limit);

float angle(Point p)
{
    float a=atan(p.y/(p.x+0.0000001234))*180/3.14;
    if(p.x<0)
        a-=180;
    return(a);
}
static double sinus(Point pt1, Point pt2, Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

Point getCentroid(vector<Point>& cont);

void checkNestRatio(vector<vector<Point>>& contours, vector<Vec4i>& hierarchy, int i, int* k);

bool nested(vector<vector<Point>>& contours, vector<Vec4i>& hierarchy, int k[], int v[]);

void setLabel(Mat& im, const string label, vector<Point >& contour);

void CallBackFunc(int event, int x, int y, int flags, void* userdata);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    areas.resize(3);
    for(int i=0;i<3;i++)
        areas[i]=2400;

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

#ifdef WRITING
    Size S = Size((int) 640,    // Acquire input size
                  (int) 480);
    VideoWriter outputVideo=VideoWriter( "C:\\Users\\chibi\\Documents\\build-MyQT_try-Desktop_Qt_5_7_1_MinGW_32bit-Release\\release\\video.avi",
                                         fcc,30, S);//fcc
#endif

    bool done=0;
    Mat video_mat;
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    //    int k=0;
    //    if(k=(1==1))
    //       true;
    //    qDebug()<<k;
    int cnt=0;

    setMouseCallback(c1, CallBackFunc, NULL);
    for(;;) //Show the image captured in the window and repeat
    {
        //        qDebug()<<"hey";
        cnt=0;
        float krec=1;
        cap >> src; // get a new frame from camera
        cvtColor(src,src_gray,CV_RGB2GRAY);
        cvtColor(src_gray, video_mat, CV_GRAY2BGR);
        //         cvtColor(dst,video_mat,CV_BGR2GRAY);
        //        blur(src_gray,dst,Size(3, 3));
        dst=src_gray;
        Canny(dst, dst, 130, 240, 3);
        //        threshold(dst,dst,sl1->value(),255,CV_THRESH_BINARY);

        //         Mat drawing = Mat::zeros( dst.size(), CV_8UC1 );
        findContours( dst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
        cvtColor(dst,dst,CV_GRAY2BGR);

        vector<vector<Point>> approx;
        approx.resize(contours.size());

        for(int i=0;i<contours.size();i++)
        {
            approxPolyDP(Mat(contours[i]), approx[i], arcLength(Mat(contours[i]), true)*sl1->value()/2500., true);//.026
        }

        centrH.resize(0);
        for(int i=0;i<approx.size();i++)
        {

            //            if (fabs(contourArea(approx[i])) < 200 || !isContourConvex(approx[i]) )
            //                continue;
            int k[6]={i,0,0,0,0,0};
//            int v1[3]={4,4,6};
            int v2[3]={4,3,3};
            int v3[3]={4,4,5};

//                         drawContours(video_mat,approx,i,Scalar(0,0,255), 1);

            bool square_label, triangle_label1, triangle_label2;
            //skip the seldom bad situation
            if(/*!(square_label=nested(approx, hierarchy, k, v1))&&*/
                    !(triangle_label1=nested(approx, hierarchy, k, v2))&&
                    !(triangle_label2=nested(approx, hierarchy, k, v3)))
                continue;

            cnt++;
            Rect r = boundingRect(approx[k[0]]);

            //            drawContours(video_mat,approx,k[0],Scalar(0,0,255), 2);
            //            drawContours(video_mat,approx,k[2],Scalar(255,0,0), 2);
            //            drawContours(video_mat,approx,k[4],Scalar(0,0,255), 2);
            Point centroid=getCentroid(approx[k[4]]);
            Rect rr=Rect(centroid-Point(2,2),centroid+Point(2,2));
            rectangle(src,rr,CV_RGB(255,255,255), CV_FILLED);

            float limit=1.07;
            float area=contourArea(approx[k[0]]);


            if(square_label)
            {
//                if((ratio(area/areas[0],limit)))
                {
                    areas[0]=area;
                    centrs[0]=centroid;
                }

            }

            //                setLabel(src, "square" , approx[i]);

            if(triangle_label1)
            {
//                if((ratio(area/areas[1],limit)))
                {
                    areas[1]=area;
                    centrs[1]=centroid;
                }
            }
            //                setLabel(src, "triangle1" , approx[i]);

            if(triangle_label2)
            {
//                if((ratio(area/areas[2],limit)))
                {
                    areas[2]=area;
                    centrs[2]=centroid;
                }
            }

            //                setLabel(src, "triangle2" , approx[i]);

            //            rectangle(src,r,Scalar(0,255,0));
        }
        //        line(src,centrs[0],centrs[1],Scalar(0,0,0),2);
        //        line(src,centrs[1],centrs[2],Scalar(0,0,0),2);

        line(video_mat,centrs[0],centrs[1],Scalar(0,0,0),2);
        line(video_mat,centrs[1],centrs[2],Scalar(0,0,0),2);
        line(video_mat,centrs[0],centrs[0]-Point(0,50),Scalar(0,0,200),2);

        float ang1=angle(centrs[0]-centrs[1]);
        float ang2=angle(centrs[2]-centrs[1]);
        float ang3=angle(centrs[1]-centrs[0]);
        ellipse(video_mat,centrs[1],Size(30,30),ang2-ang2,ang1,ang2,Scalar(0,0,200),3);
        ellipse(video_mat,centrs[0],Size(30,30),0,ang3,-90,Scalar(0,0,200),3);

        //        ellipse(video_mat,centrs[1],Size(30,30),ang2-ang2,ang1,ang2,Scalar(0,0,200),3);
        //        ellipse(src,centrs[0],Size(30,30),0,ang3,-90,Scalar(0,0,200),3);
        //        qDebug()<<contours.size();

#ifdef WRITING
        outputVideo << video_mat;
#endif

        imshow(c1,video_mat);
        if(waitKey(42) >= 0) break;
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
    if  ( event == EVENT_LBUTTONDOWN )
    {
        centrs[0]=Point(x,y);
    }
}


//k[] - indexes of contours, v[] - criterias (numbers of vertices)
bool nested(vector<vector<Point>>& contours, vector<Vec4i>& hierarchy, int* k, int* v)
{
    static int ca=0;
    Point p[6],ph;
    for(int i=0;i<5;i++)
    {
        if(contours[k[i]].size()==v[i/2])
        {
            if(contours[k[i]].size()==4)
            {
                p[i]=getCentroid(contours[k[i]]);
            }
            //            if(contours[k[i]].size()==4)
            //            {
            //                float a=angle(contours[k[i]][0]-contours[k[i]][1]);
            //                float b=angle(contours[k[i]][3]-contours[k[i]][2]);
            //                if(fabs(a-b)>7)
            //                    return 0;
            //            }

            k[i+1]=hierarchy[k[i]][2];
            if(k[i+1]==-1)
                return 0;
        }
        else
            return 0;
    }


    return 1;
}



//just pushes an indexes, nothing more
void checkNestRatio(vector<vector<Point>>& contours, vector<Vec4i>& hierarchy, int i, int* k)
{
    //    qDebug()<<contourArea(contours[k[i-1]]);
    //    qDebug()<<contourArea(contours[k[i]]);
    if(contourArea(contours[k[i-1]])/contourArea(contours[k[i]])>1.4)
    {

    }
    else
    {
        k[i]=hierarchy[k[i]][2];
    }

}

bool ratio(float x,float limit)
{
    if(x>1)
    {
        if(x<limit)
            return 1;
    }
    else
    {
        if(x>(1/limit))
            return 1;
    }
    return 0;
}

Point getCentroid(vector<Point>& cont)
{
    if(cont.size())
    {
        Point ac=Point(0,0);
        for(int i=0;i<cont.size();i++)
            ac+=cont[i];
        ac*=1./cont.size();
        return ac;
    }
}
