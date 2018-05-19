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
Mat frame;
int thresh(int x, int y)
{
    if(x<0)
        return(0);
    if(x>(y-1))
        return(y-1);
    return(x);

}
IplImage* im, *small_im;
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    int s=100;
    Mat mask = Mat::zeros( s, s, CV_8UC1 );
    for(int i=0;i<s;i++)
        for(int j=0;j<s;j++)
        {
            float x=i-s/2+2;
            float y=j-s/2+10;
            if(((x*x+y*y)<(s*s/4.5)))
                mask.at<uint8_t>(Point(i,j))=255;
            else
                mask.at<uint8_t>(Point(i,j))=0;
            //            qDebug()<<mask.at<float>(Point(i,j));
        }


    CascadeClassifier face_cascade;
    face_cascade.load( "C:\\opencv\\opencv\\sources\\data\\haarcascades//haarcascade_frontalface_alt2.xml" );

    //    Mat image, grey;
    //    image=imread("C://Users\\chibi\\Documents\\Lightshot\\2057921811589a1.jpg");

    Mat face_src;
    face_src=imread("C://Users\\chibi\\Documents\\Lightshot\\Screenshot_3.png");
    //   qDebug()<<face_src.size().width;


    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    std::vector<Rect> faces;
    std::vector<int> levels;
    std::vector<double> weights;
    vector<Rect> r;
    //    Mat edges;
    //    resize(face_src,face_src,Size(20,20));
    //    namedWindow("edges",1);
    float sc=-0.05;
    for(;;)
    {
        cap >> frame; // get a new frame from camera
        im=new IplImage(frame);
        //        cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        int right=frame.cols;
        int down=frame.rows;
        //        qDebug()<<up;
        //        imshow("edges", frame);

        face_cascade.detectMultiScale( frame, faces, levels, weights, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(40, 40),
                                       Size(200,200),true);

        r.resize(faces.size());
        for(int i=0;i<faces.size();i++)
            r[i]=Rect(Point(thresh(faces[i].x-faces[i].width*sc,right),thresh(faces[i].y-faces[i].height*sc,down)),
                      Point(thresh(faces[i].x+faces[i].width*(1+sc),right),thresh(faces[i].y+faces[i].height*(1+sc)*1.3,down)));

        //i=
        for(int i=0;i<r.size();i++)
        {
            if(weights[i]>55.5)
            {
                float size_k=0.8;
                //            resize(r[i],r[i],Size(30,30));
                Size sz= Size(r[i].size().width,r[i].size().height);
                //            r[i].size().width+3;
                //            Size sz=Size
//                            qDebug()<<i<<" levels="<<levels[i];

                Mat mm=face_src;
                Mat msk=mask;
                resize(msk,msk,sz);
                resize(mm,mm,sz);
                mm.copyTo(frame(r[i]),msk);
            }
            //            Mat m1=frame(r[i]);
            //            resize(m1,m1,Size(800,800));
            //            imshow("edges", m1);
            //            CvRect rr=cvRect(3,3,5,5);
            //            Mat m2;


            //            cvSetImageROI( im, r[i] );
            //            cvCopy(im,im);
            //            small_im->
            //            cvAddS(im, cvScalar(255,255,255), im);

            //            face_src.copyTo(frame);
            //            rectangle(frame, r[i], Scalar(255, 255, 255), 2, 6, 0);
            imshow("edges", frame);
            //            //            face_src.copyTo(frame(r[i]));
            //        }
            //        //        cvtColor(frame, edges, CV_BGR2GRAY);
            //        //        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
            //        //        Canny(edges, edges, 0, 30, 3);

            //            imshow("edges", face_src);
            if(waitKey(30) >= 0) break;
        }
        //    // the camera will be deinitialized automatically in VideoCapture destructor
        //    //    return 0;
    }


    return a.exec();
}
