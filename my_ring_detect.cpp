#include <QApplication>
#include <iostream>
#include <string>   // for strings

#include <opencv2/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui.hpp>  // Video write
#include "detector.h"
using namespace std;
using namespace cv;


CV_WRAP static int fcc=VideoWriter::fourcc('D','I','V','X');



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    detector det=detector();

    Mat dst, src, src2;
    char* c1="first";
    char* c2="second";
    char* c3="third";


    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    std::vector<Rect> faces;
    vector<Rect> r;
    r.resize(1);

    //    cvtColor(kernel,kernel, CV_8UC4);

    namedWindow( c1, CV_WINDOW_AUTOSIZE);
    //      namedWindow( c2, CV_WINDOW_AUTOSIZE);
    namedWindow( c3, CV_WINDOW_AUTOSIZE);
    imshow(c3, det.kernel[1]);
    //    imshow(c1, src);
    //    imshow(c1, kernel*sum);

    Mat mean, out_std;


    Size S = Size((int) 640,    // Acquire input size
                  (int) 480);

    // Open the output

    VideoWriter outputVideo=VideoWriter( "C:\\Users\\chibi\\Documents\\build-MyQT_try-Desktop_Qt_5_7_1_MinGW_32bit-Release\\release\\hello.avi",
                                         fcc,15, S);//fcc



    cout<<src.size().width<<" ";
    cout<<src.size().height<<"\n";
    Mat video_mat;

    for(;;) //Show the image captured in the window and repeat
    {
        float krec=1;
        cap >> src; // get a new frame from camera
        cvtColor(src, dst, CV_BGR2GRAY);

        det.multiMax(dst);
        qDebug()<<det.max;
        //         qDebug()<<dst.size().width;
        if(det.max>0.59)
            rectangle(dst,Rect(det.ind_1,det.ind_2,det.dst_s*krec,det.dst_s*krec),Scalar(255,255,255),5);
        //          imshow(c2, src);

        cvtColor(dst, video_mat, CV_GRAY2BGR);
        outputVideo << video_mat;

        imshow(c1,dst);
        if(waitKey(30) >= 0) break;
    }

    cout << "Finished writing" << endl;
    return 0;
}
