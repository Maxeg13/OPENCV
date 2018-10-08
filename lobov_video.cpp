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
#define KERN_N 12
//#define WRITING

using namespace std;
using namespace cv;

float sum1,sum2;
int nest_ind=0,nest_size=0,nest_max=0;
QSlider *sl1, *sl2;
int thresh = 200;
int max_thresh = 255;
Point wp[2];
bool go_on=1;
int size_div=1;
QVector<Point> centrH;
vector<int> areas;

vector<Vec4i> hierarchy;
vector<vector<Point> > contours;
Mat mask[KERN_N],mask_g;
Mat src2;
Mat dst;
Mat predst;
Mat video_mat;
Mat kernel[KERN_N];
Mat prekernel;
Mat trace_mat;
Mat quest_mat, quest_kill_mat;
int indi,indj,ind2i,ind2j,ind_k;
bool alg_on=0;

CV_WRAP static int fcc=VideoWriter::fourcc('D','I','V','X');
void getMax(Mat& x_mat,int& ind_1, int& ind_2, float& max1);
bool ratio(float x,float limit);
void multiMax(Mat& src, float& max, int& ind_1, int& ind_2, int &kernel_ind);

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

void nested(int);

Point getCentroid(vector<Point>& cont);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    mask_g=Mat::ones( 8, 8, CV_8U );
    for(int i=0;i<KERN_N;i++)
    {
        kernel[i]=Mat::zeros( 8, 8, CV_8U );

    }

    char *c1="cam";
    char *c2="process";
    char *c3="trace_mat";

    namedWindow( c3, CV_WINDOW_AUTOSIZE);
    namedWindow( c2, CV_WINDOW_AUTOSIZE);
    namedWindow( c1, CV_WINDOW_AUTOSIZE);

    //    setMouseCallback(c1, CallBackFunc, NULL);
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

    VideoCapture cap;
    cap.open("C://Users//chibi//Downloads//IMG_0447.MOV");//cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    VideoWriter outputVideo=VideoWriter( "C:\\Users\\chibi\\Documents\\build-MyQT_try-Desktop_Qt_5_7_1_MinGW_32bit-Release\\release\\hello.avi",
                                         fcc,30, Size(380,374));//fcc

    //    qDebug()<<
    //    qDebug()<<cap.get(CAP_PROP_FPS);

    Mat src, src_gray;

    dst = Mat::zeros( Size(8,8), CV_8U );

    //    qDebug()<<cap.get(CAP_PROP_FRAME_WIDTH );

    //    cap.set(CAP_PROP_FPS,9);
    //    cap.set(CAP_PROP_EXPOSURE ,4);

    //    cap.set(CV_CAP_PROP_FRAME_WIDTH,1024);/*1280*/
    //    cap.set(CV_CAP_PROP_FRAME_HEIGHT,720);/*720*/
    //    qDebug()<<cap.get(CAP_PROP_FRAME_WIDTH );
    //    qDebug()<<cap.get(CAP_PROP_FRAME_HEIGHT );
    //    qDebug()<<cap.get(CAP_PROP_EXPOSURE );
    cap>>src;
    //    cap.set(CAP_PROP_POS_FRAMES,1000)/*;*/

    Size S = Size((int) 640,    // Acquire input size
                  (int) 480);
#ifdef WRITING
    VideoWriter outputVideo=VideoWriter( "C:\\Users\\chibi\\Documents\\build-MyQT_try-Desktop_Qt_5_7_1_MinGW_32bit-Release\\release\\video.avi",
                                         fcc,30, S);//fcc
#endif

    bool done=0;

    setMouseCallback(c1, CallBackFunc, NULL);

    qDebug()<<cap.get(CAP_PROP_FPS);

    for(;;) //Show the image captured in the window and repeat
    {
        static int frame_i=29.91*398;
        if(go_on)
        {
            frame_i+=20;
            cap.set(CAP_PROP_POS_FRAMES,frame_i );
            qDebug()<<"frame_i: "<<frame_i<<",    time, s: "<<(frame_i)/cap.get(CAP_PROP_FPS);//full interval: 1230, //376// 3.27
            cap >> src; // get a new frame from camera
            int indent=65*2;
            Size sh=Size(640/size_div,480/size_div/1.28);
            resize(src,src,sh);
            Mat src1=src(Rect(Point(indent,0),Point(sh.width-indent,sh.height)));
            resize(src1,src2,src1.size());
            if(trace_mat.size()==Size(0,0))
            {
                quest_mat=Mat::zeros(src1.size().height, src1.size().width,CV_8UC3);
                quest_kill_mat=Mat::ones(src1.size().height, src1.size().width,CV_8UC3);
                trace_mat=Mat::ones(src1.size().height, src1.size().width,CV_8UC3);
                for(int i=0;i<trace_mat.size().width;i++)
                    for(int j=0;j<trace_mat.size().height;j++)
                    {
                        trace_mat.at<Vec3b>(Point(i,j))[0]=255;
                        trace_mat.at<Vec3b>(Point(i,j))[1]=255;
                        trace_mat.at<Vec3b>(Point(i,j))[2]=255;
                        quest_kill_mat.at<Vec3b>(Point(i,j))[0]=255;
                        quest_kill_mat.at<Vec3b>(Point(i,j))[1]=255;
                        quest_kill_mat.at<Vec3b>(Point(i,j))[2]=255;
                    }


            }
            src2=trace_mat&src2;
            src2&=quest_kill_mat;
            src2|=quest_mat;
            //            GaussianBlur(src1, src, Size(7,7), 5, 5);
            //            resize(src1,video_mat,src1.size());
            int a=11,c1=81*2,c2=161*2;
            float rec_k=2.3;
            rectangle(src1,Rect(Point(c1-a,c2-rec_k*a),Point(c1+a,c2+rec_k*a)),Scalar(0,0,0),19);
            c1=86*2,c2=131*2;
            rectangle(src1,Rect(Point(c1-a,c2-rec_k*a),Point(c1+a,c2+rec_k*a)),Scalar(0,0,0),19);
            circle(src1,Point(99,64)*2,6,Scalar(0,0,0),16);
            cvtColor(src1,predst,CV_RGB2GRAY);

            sum2=sum1;
            sum1=sum(predst(Rect(Point(indi,indj),Point(ind2i,ind2j))))[0];
            //            qDebug()<<sum1-sum2;
            //            cout<<src2.size();
            Canny(predst, predst, 130, 240, 3);

            findContours( predst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

            nest_max=0;
            float mas=0;
            Point2f weighted_v=Point2f(0,0);
            float h;
            for(int i=0;i<contours.size();i++)
            {
                nest_size=0;
                //                nested(i);
                //                if(isContourConvex(contours[i]))
                //                {
                //                    h=1/(contourArea(contours[i])+0.001)/(contourArea(contours[i])+0.001);
                //                    mas+=h;
                //                    Point2f hh=h*getCentroid(contours[i]);
                //                    weighted_v=weighted_v+hh;
                //                }
                drawContours(predst,contours,i,Scalar(255,255,255), 5);


            }
            //                            cout<<mas<<endl;
            //            weighted_v=weighted_v*(1/mas);
            //            cout<<weighted_v;
            //            rectangle(predst,Rect(weighted_v-Point2f(10,10),weighted_v+Point2f(10,10)),Scalar(0,0,255),10);
            //            rectangle(video_mat,Rect(weighted_v-Point2f(10,10),weighted_v+Point2f(10,10)),Scalar(0,0,255),10);
            //                qDebug()<<nest_ind;
            //                drawContours(predst,contours,nest_ind,Scalar(255,255,255), 5);

            //            video_matpredst
            video_mat=predst;
            //        cout<<video_mat.type();
            if(alg_on)
            {
                float max;

                multiMax(predst,max,indi,indj,ind_k);
                ind2i=indi+prekernel.size().width;
                ind2j=indj+prekernel.size().height;
                //            float max1=0;
                //            getMax(dst,i,j,max1);
                rectangle(video_mat,Rect(Point(indi,indj),Point(ind2i,ind2j)),Scalar(255,255,255),5);
                static Point p[2];
                p[0]=p[1];
                p[1]=Point(indi+prekernel.size().width/2,indj+
                           prekernel.size().height/2);
                if((p[1]!=Point(0,0))&&(p[0]!=Point(0,0)))
                    line(trace_mat,p[0],p[1],Scalar(0,0,0),2);
                //                qDebug()<<ind_k;
                if((sum1-sum2)<-146000)
                {
                    putText(quest_mat, string("!"), Point((indi+ind2i)/2,(indj+ind2j)/2), FONT_HERSHEY_SIMPLEX, 1, CV_RGB(255,0,0), 3, 8);
                    putText(quest_kill_mat, string("!"), Point((indi+ind2i)/2,(indj+ind2j)/2), FONT_HERSHEY_SIMPLEX, 1, CV_RGB(255,0,0), 3, 8);
                }
                outputVideo<<src2;
            }
        }
#ifdef WRITING
        outputVideo << video_mat;
#endif
        imshow(c2,predst);
        imshow(c1,src2);

        imshow(c3,trace_mat);
        //        imshow(c3,dst);
        if(waitKey(42) >= 0) break;//42
    }
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

void nested(int i)
{
    if(hierarchy[i][2]!=-1)
    {

        nest_size++;
        if(nest_size>nest_max)
        {
            nest_max=nest_size;
            nest_ind=hierarchy[i][2];
        }
        nested(nest_ind);
    }
}

bool mouse_on=0;

void CallBackFunc(int event, int x, int y, int flags, void* userdata){
    if  ( event == EVENT_LBUTTONDOWN )
    {
        indi=x;
        indj=y;
        mouse_on=1;
        wp[0]=Point(x,y);
    }
    if(event==EVENT_LBUTTONUP)
    {
        mouse_on=0;
        wp[1]=Point(x,y);
        Mat mm=predst(Rect(wp[0],wp[1]));
        resize(mm,prekernel,mm.size());
        //        prekernel=predst(Rect(wp[0],wp[1]));
        mask_g=Mat::ones( prekernel.size().height,prekernel.size().width, CV_8U )*255;

        Mat matr;

        //        cout<<matr;
        for(int i=0;i<KERN_N;i++)
        {
            matr=getRotationMatrix2D(Point(mask_g.size().width/2,mask_g.size().height/2),360/KERN_N*i,1);
            warpAffine(prekernel,kernel[i],matr,prekernel.size());
            warpAffine(mask_g,mask[i],matr,prekernel.size());
        }

        alg_on=1;
        go_on=1;

        //        mm.copyTo(kernel);
    }

    if(event==EVENT_MBUTTONDOWN)
    {

        go_on=0;
        alg_on=0;
    }
}

void multiMax(Mat& src, float& max,int& ind_1,int& ind_2, int& kernel_ind)
{
    int inert_ind1=ind_1,inert_ind2=ind_2;
    //    Rect r(Point(inert_ind1,inert_ind2),Point(inert_ind1,inert_ind2));
    Mat out_std;
    max=0;
    int i1=0,i2=0;
    float max1=0;
    for (int i=0;i<KERN_N;i++)
    {
        matchTemplate(src, kernel[i], out_std, TM_CCORR_NORMED, mask[i]);//mask[i]
        getMax(out_std,i1,i2,max1);
        if((abs(inert_ind1-i1)+abs(inert_ind2-i2))<(640/size_div/5))
            if(max<max1)
            {
                ind_1=i1;
                ind_2=i2;
                max=max1;
                kernel_ind=i;
                //            dst_s=s[i];
            }
    }
}

void getMax(Mat& x_mat,int& ind_1, int& ind_2, float& max1)
{
    int i=0,j=0,bar=0;
    for( i=bar;i<x_mat.cols-bar;i++)
        for( j=bar;j<x_mat.rows-bar;j++)
        {
            if(x_mat.at<float>(Point(i,j))>max1)
            {
                ind_1=i;
                ind_2=j;
                max1=x_mat.at<float>(Point(i,j));
                //                qDebug()<<max1;
            }
        }
}
