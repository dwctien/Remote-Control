#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <opencv2/opencv.hpp>
#include <iostream>
#include <windows.h>
#include <fstream>
#include "html_generator.h"

cv::Mat captureScreen();

Response getScreenshot(string filename);

#endif // !_SCREEN_H_