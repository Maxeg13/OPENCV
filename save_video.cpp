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



    Size S = Size((int) 640,    // Acquire input size
                  (int) 480);

                                            // Open the output

    VideoWriter outputVideo=VideoWriter( "C:\\Users\\chibi\\Documents\\build-MyQT_try-Desktop_Qt_5_7_1_MinGW_32bit-Release\\release\\hello.avi",
                                         fcc,30, S);//fcc




    Mat src, res;
    vector<Mat> spl;
    VideoCapture cap(0);
    if(!cap.isOpened())  // check if we succeeded
return -1;
    char *c1="hello";
    namedWindow( c1, CV_WINDOW_AUTOSIZE);

     cap >> src;
    cout<<src.size().width;
    cout<<src.size().height;
    for(;;) //Show the image captured in the window and repeat
    {
        cap >> src;              // read


        split(src, spl);                // process - extract only the correct channel
        for (int i =0; i < 3; ++i)
            if (i != 0)
                spl[i] = Mat::zeros(S, spl[0].type());
       merge(spl, res);

       outputVideo << src;
        imshow(c1,src);
        if(waitKey(30) >= 0) break;
    }

    cout << "Finished writing" << endl;
    return 0;
}
