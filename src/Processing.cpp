#include <iostream>
using namespace std;
#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;

#include "Processing.h"

void Processing::invert_image() {
  if (!image->data.empty()) {
    bitwise_not(image->data, image->data);
  }
}

void Processing::white_balance() {
  if (!image->data.empty() && image->channels >= 3) {
    std::vector<Mat> channels;
    split(image->data, channels);

    for (auto& channel : channels) {
      equalizeHist(channel, channel);
    }

    merge(channels, image->data);
  }
}
