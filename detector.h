#ifndef DETECTOR_H
#define DETECTOR_H
#include <opencv2/opencv.hpp>
#include <QDebug>
#define det_N 3

using namespace std;
using namespace cv;
class detector
{
public:
    detector();
    int s[det_N], core_s;
    int dst_s;
    int ind_1, ind_2, kernel_ind;
    float max;
    Mat mask[det_N];
    Mat core_mask;
    Mat out_std;
    Mat core_kernel;
    Mat kernel[det_N];
    void multiMax(Mat& src);
    void getMax(Mat& x_mat,int& ind_1, int& ind_2, float& max1);
};

#endif // DETECTOR_H
