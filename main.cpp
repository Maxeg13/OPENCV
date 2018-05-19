#include <QApplication>
#include <iostream>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <QDebug>
//#include <opencv/>
using namespace std;
using namespace cv;

int ddepth;
int kernel_size;
double delta;

int thresh(int x, int y)
{
    if(x<0)
        return(0);
    if(x>(y-1))
        return(y-1);
    return(x);

}

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    Mat dst, src, src2;
    char* c1="first";
    char* c2="second";
    Point anchor;
    anchor = Point( -1, -1 );
    delta = 0;
    ddepth = -1;
    int s=140;
    Mat kernel = Mat::zeros( s, s, CV_32F );
    Mat sum_k=Mat::ones(s, s, CV_32F );
    sum_k/=s*s;
    Mat mat_norm;
    int i,j, ind_1, ind_2;

	
    for( i=0;i<s;i++)
        for( j=0;j<s;j++)
        {
            float x=i-s/2;
            float y=j-s/2;
            if(((x*x+y*y)<(s*s/4.)))
                kernel.at<float>(Point(i,j))=1.;
            if((x*x+y*y)<s*s/4.9)
                kernel.at<float>(Point(i,j))=0.;
            //            qDebug()<<kernel.at<float>(Point(i,j));
        }

    float sum=0;

    for(int i=0;i<s;i++)
        for(int j=0;j<s;j++)
            sum+=kernel.at<float>(Point(i,j));

//    qDebug()<<sum;

    kernel/=sum;
    float sc=0.3;

    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    std::vector<Rect> faces;
    vector<Rect> r;
    r.resize(1);

    namedWindow( c1, CV_WINDOW_AUTOSIZE);
    namedWindow( c2, CV_WINDOW_AUTOSIZE);
    //    imshow(c1, kernel*sum);
    for(;;)
    {

        cap >> src; // get a new frame from camera
        src2=src;
//        src/=;
                Canny(src, dst, 20, 20*3, 3);
                dst/=5;
//        cvtColor(dst, dst, CV_BGR2GRAY);
        filter2D(dst, mat_norm, ddepth , sum_k, anchor, delta, BORDER_DEFAULT );
        filter2D(dst, dst, ddepth , kernel, anchor, delta, BORDER_DEFAULT );
        mat_norm.convertTo(mat_norm,CV_32F);
//        mat_norm.convertTo(mat_norm,CV_8UC1);


//        qDebug()<<mat_norm.at<float>(Point(0,0));

                imshow(c1, kernel*sum);
        int a;
        mat_norm/=80.;
        int bar=0;
        for( i=bar;i<dst.cols-bar;i++)
            for( j=bar;j<dst.rows-bar;j++)
            {
//                if(dst.at<uint8_t>(Point(i,j))>max1)
                {
//                    a=mat_norm.at<uint8_t>(Point(0,0));
//                    qDebug()<<(uint8_t)(1./(1+mat_norm.at<uint8_t>(Point(i,j))));
//                    dst.at<uint8_t>(Point(i,j))=dst.at<uint8_t>(Point(i,j))/(mat_norm.at<float>(Point(i,j)));
//                    if((1.+mat_norm.at<uint8_t>(Point(i,j)))<1)
//                        qDebug()<<(1.+mat_norm.at<uint8_t>(Point(i,j)));
                }
            }
//        qDebug()<<a;


        float max1=0;
        bar=30;
        for( i=bar;i<dst.cols-bar;i++)
            for( j=bar;j<dst.rows-bar;j++)
            {
                if(dst.at<uint8_t>(Point(i,j))>max1)
                {
                    ind_1=i;
                    ind_2=j;
                    max1=dst.at<uint8_t>(Point(i,j));
//                    0.1*mat_norm.at<uint8_t>(Point(i,j))+
                    //                    qDebug()<<(uint8_t)dst.at<uint8_t>(Point(i,j));
                }
            }
        rectangle(dst,Rect(ind_1,ind_2,30,20),Scalar(0,255,0),5);
        imshow(c2, dst);
        //        for()
        if(waitKey(30) >= 0) break;

    }


    return a.exec();
}
