//
//  cv_util.h
//  
//
//  Created by Maurizio Giordano on 22/01/13.
//
//

#ifndef _cv_util_h
#define _cv_util_h
#include <opencv2/highgui/highgui.hpp>

#include <string>
using namespace std;
using namespace cv;

void cvShowManyImages(string title, string framename, int ptr1, int ptr2, int wait, int status, unsigned int tp, unsigned int fp, unsigned int fn,  unsigned int tn, unsigned int nbShadowErrors, double recall, double precision, double specificity, double fmeasure, float fps, std::vector<cv::Mat> imglist);
#endif
