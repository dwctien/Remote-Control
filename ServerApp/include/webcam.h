#ifndef _WEBCAM_H_
#define _WEBCAM_H_

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <Windows.h>
#include <fstream>
#include "html_generator.h"

using namespace std;

cv::Mat capturePhoto();

Response getImage(string filename);

void recordVideo(string filename, int duration);

Response getVideo(string filename, int duration);

#endif // !_WEBCAM_H_