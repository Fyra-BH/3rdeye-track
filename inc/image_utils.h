#ifndef __IMAGE_UTILS_H__
#define __IMAGE_UTILS_H__

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

void letterbox(cv::Mat& img, int new_width, int new_height, int &offset_x, int &offset_y, float &scale);

#endif //__IMAGE_UTILS_H__