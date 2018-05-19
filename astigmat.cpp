#include <QApplication>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <QDebug>
//#include <opencv/>
using namespace std;
using namespace cv;
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);


    //    Mat image, grey;
    //    image=imread("C://Users\\chibi\\Documents\\Lightshot\\2057921811589a1.jpg");


    /// Declare variables
    Mat src, dst;

    Mat kernel;
    Point anchor;
    double delta;
    int ddepth;
    int kernel_size;
    char* window_name = "filter2D Demo";

    int c;

    /// Load an image
//    src = imread("C://Users\\chibi\\Documents\\Lightshot\\2057921811589a1.jpg");

    src = imread("C:\\Users\\chibi\\Pictures\\Camera Roll//Screenshot_1.png");
    if( !src.data )
    { return -1; }

    /// Create window
    namedWindow( window_name, CV_WINDOW_AUTOSIZE );

    /// Initialize arguments for the filter
    anchor = Point( -1, -1 );
    delta = 0;
    ddepth = -1;

    /// Loop - Will filter the image with different kernel sizes each 0.5 seconds
    int ind = 0;
    //    while( true )
    {
        c = waitKey(500);
        /// Press 'ESC' to exit the program
        //        if( (char)c == 27 )
        //        { break; }

        /// Update kernel size for a normalized box filter
        //        kernel_size = 3 + 2*( ind%5 );

        int s=19;
        kernel = Mat::zeros( s, s, CV_32F );

        kernel.at<float>(Point(0,s-1))=1;
        blur(kernel,kernel,Size(13,13));
        kernel.at<float>(Point(s/2,s-1))=0.03;
        kernel.at<float>(Point(s/2,s/2))=0.03;

        float sum=0;
        for(int i=0;i<s;i++)
            for(int j=0;j<s;j++)
                sum+=kernel.at<float>(Point(i,j));

        qDebug()<<sum;

        kernel/=sum;

        for(int j=0;j<3;j++)
        {
            //            for(int i=0;i<2;i++)
            //                qDebug()<<kernel.at<float>(Point(j,j));
            //            qDebug()<<"\n";
        }

        //        kernel/=2;
        /// Apply filter
        filter2D(src, dst, ddepth , kernel, anchor, delta, BORDER_DEFAULT );
        imshow( "hello",dst );
        imshow( window_name, src );
        ind++;
    }

    //    return 0;



    return a.exec();
}
