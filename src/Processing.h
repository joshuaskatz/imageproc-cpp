#include <opencv2/opencv.hpp>

#include "Image.h"
using namespace cv;

struct Processing {
  Image* image;

  void invert_image();
  void white_balance();
};
