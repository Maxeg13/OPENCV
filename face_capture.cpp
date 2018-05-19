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

    float sc=0.3;
    CascadeClassifier face_cascade;
    face_cascade.load( "C:\\opencv\\opencv\\sources\\data\\haarcascades//haarcascade_frontalface_alt2.xml" );

    //    Mat image, grey;
    //    image=imread("C://Users\\chibi\\Documents\\Lightshot\\2057921811589a1.jpg");

    //    Mat small_image;
    //    small_image=imread("C://Users\\chibi\\Documents\\Lightshot\\2057921811589a1.jpg");

    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    std::vector<Rect> faces;
    vector<Rect> r;
    //    Mat edges;

    //    namedWindow("edges",1);
    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        int right=frame.cols;
        int down=frame.rows;
        //        qDebug()<<up;
//        imshow("edges", frame);

        face_cascade.detectMultiScale( frame, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

        r.resize(faces.size());
        for(int i=0;i<faces.size();i++)
            r[i]=Rect(Point(thresh(faces[i].x-faces[i].width*sc,right),thresh(faces[i].y-faces[i].height*sc,down)),
                      Point(thresh(faces[i].x+faces[i].width*(1+sc),right),thresh(faces[i].y+faces[i].height*(1+sc)*1.4,down)));

//        for(int i=0;i<r.size();i++)
        int i=r.size()-1;
        if(r.size())
        {
            Mat m1=frame(r[i]);
            resize(m1,m1,Size(800,800));
            imshow("edges", m1);
            //            //                        rectangle(frame, r[i], Scalar(255, 255, 255), 2, 8, 0);
            //            //                        imshow("edges", frame);
            //            //            small_image.copyTo(frame(r[i]));
            //        }
            //        //        cvtColor(frame, edges, CV_BGR2GRAY);
            //        //        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
            //        //        Canny(edges, edges, 0, 30, 3);

            //            imshow("edges", small_image);
            if(waitKey(30) >= 0) break;
        }
        //    // the camera will be deinitialized automatically in VideoCapture destructor
        //    //    return 0;
    }


    return a.exec();
}
