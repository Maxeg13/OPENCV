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


    Mat image, grey;
    image=imread("C://Users\\chibi\\Documents\\Lightshot\\2057921811589a1.jpg");


    Rect r(100,100,300,300);
    //    Vec3b x=image.at<Vec3b>(Point(400, 400));
    //    image.at<Vec3b>(Point(400, 400))=Vec3b();
    cvtColor(image, grey, CV_BGR2GRAY);
    //    image.convertTo(grey,CV_32F);
    //    grey=grey()
    //    image=Scalar(255);


    //    qDebug()<<x[0];
    //    qDebug()<<x[1];
    //    qDebug()<<x[2];
    Mat sobelx=grey;
    Scharr(sobelx, sobelx, CV_32F, 1, 0);
    //        imwrite("release//secfile.jpg", m);
    imshow("grey", grey);
    imshow("sobelx", sobelx);
    return a.exec();
}
