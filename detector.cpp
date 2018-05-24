#include "detector.h"

detector::detector()
{
    kernel_ind=0;
    max=0;
    core_s=90;
    core_mask=Mat::zeros( core_s, core_s, CV_8U );
    core_kernel = Mat::zeros( core_s, core_s, CV_8U );
    float rk=1.5;
    float rr1=2.55;
    int i,j;
    for( i=0;i<core_s;i++)
        for( j=0;j<core_s;j++)
        {
            float x=i-core_s/2;
            float y=j-core_s/2;
            if(((x*x+y*y)>(core_s*core_s/(2*2)/rk)))//extern
                core_kernel.at<char>(Point(i,j))=0;//70
            else if((x*x+y*y)<core_s*core_s/(rr1*rr1)/rk)//6//internal
                core_kernel.at<char>(Point(i,j))=0;//50
            else
                core_kernel.at<char>(Point(i,j))=255;//160
            //            qDebug()<<kernel.at<float>(Point(i,j));
        }

    for( i=0;i<core_s;i++)
        for( j=0;j<core_s;j++)
        {

            float x=i-core_s/2;
            float y=j-core_s/2;
            if((x*x+y*y)<(core_s*core_s/2/2))
                core_mask.at<char>(Point(i,j))=255;
            else
                core_mask.at<char>(Point(i,j))=0;

        }

    for (int i=0;i<det_N;i++)
    {
        s[i]=core_s*(1.-(i/(float)det_N)*.4);
        resize(core_kernel,kernel[i], Size(s[i],s[i]));
        resize(core_mask,mask[i],Size(s[i],s[i]));
    }
}

void detector::getMax(Mat& x_mat,int& ind_1, int& ind_2, float& max1)
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


void detector::multiMax(Mat& src)
{
    Mat out_std;
    max=0;
    int i1=0,i2=0;
    float max1=0;
    for (int i=0;i<det_N;i++)
    {
        matchTemplate(src, kernel[i], out_std, TM_CCORR_NORMED);//mask[i]
        getMax(out_std,i1,i2,max1);
        if(max<max1)
        {
            ind_1=i1;
            ind_2=i2;
            max=max1;
            kernel_ind=i;
            dst_s=s[i];
        }
    }
}
