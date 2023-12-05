#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"

Image::Image(const char *filename) {
  if (read(filename)) {
    printf("Read %s\n", filename);
    size = w * h * channels;
  } else {
    printf("Failed to read %s\n", filename);
  }
}

Image::Image(int w, int h, int channels) : w(w), h(h), channels(channels) {
  size = w * h * channels;
  data = new uint8_t[size];
}

Image::Image(const Image &img) : Image(img.w, img.h, img.channels) {
  memcpy(data, img.data, size);
}

Image::~Image() { stbi_image_free(data); }

bool Image::read(const char *filename) {
  data = stbi_load(filename, &w, &h, &channels, 3);
  if (stbi_failure_reason()) cout << stbi_failure_reason();
  return data != NULL;
}

bool Image::write(const char *filename) {
  ImageType type = get_file_type(filename);
  int success;
  switch (type) {
    case PNG:
      success = stbi_write_png(filename, w, h, channels, data, w * channels);
      break;
    case BMP:
      success = stbi_write_bmp(filename, w, h, channels, data);
      break;
    case JPG:
      success = stbi_write_jpg(filename, w, h, channels, data, 100);
      break;
    case TGA:
      success = stbi_write_tga(filename, w, h, channels, data);
      break;
  }
  if (success != 0) {
    printf("\e[32mWrote \e[36m%s\e[0m, %d, %d, %d, %zu\n", filename, w, h,
           channels, size);
    return true;
  } else {
    printf("\e[31;1m Failed to write \e[36m%s\e[0m, %d, %d, %d, %zu\n",
           filename, w, h, channels, size);
    return false;
  }
}

ImageType Image::get_file_type(const char *filename) {
  const char *ext = strrchr(filename, '.');
  if (ext != nullptr) {
    if (strcmp(ext, ".png") == 0) {
      return PNG;
    } else if (strcmp(ext, ".jpg") == 0) {
      return JPG;
    } else if (strcmp(ext, ".bmp") == 0) {
      return BMP;
    } else if (strcmp(ext, ".tga") == 0) {
      return TGA;
    }
  }
  return PNG;
}

void Image::invert_image() {
  if (data != nullptr) {
    cv::Mat imageMat(h, w, CV_MAKETYPE(CV_8U, channels), data);

    // Invert the image using OpenCV
    cv::bitwise_not(imageMat, imageMat);

    // Copy the inverted data back to the image object
    memcpy(data, imageMat.data, size);
  }
}

void Image::white_balance() {
  if (data != nullptr && channels >= 3) {
    cv::Mat imageMat(h, w, CV_MAKETYPE(CV_8U, channels), data);

    // Split the image into individual BGR channels
    std::vector<cv::Mat> channels;
    cv::split(imageMat, channels);

    // Perform histogram equalization on each color channel separately
    for (auto &channel : channels) {
      cv::equalizeHist(channel, channel);
    }

    // Merge the equalized channels back into the image
    cv::merge(channels, imageMat);

    // Copy the adjusted data back to the image object
    memcpy(data, imageMat.data, size);
  }
}

void Image::adjust_by_channel(float red_scale, float green_scale,
                              float blue_scale) {
  if (data != nullptr && channels >= 3) {
    cv::Mat imageMat(h, w, CV_MAKETYPE(CV_8U, channels), data);

    // Split the image into individual BGR channels
    std::vector<cv::Mat> channels;
    cv::split(imageMat, channels);

    // Adjust each color channel separately by scaling the intensities
    channels[2] *= red_scale;    // Red channel
    channels[1] *= green_scale;  // Green channel
    channels[0] *= blue_scale;   // Blue channel

    // Merge the adjusted channels back into the image
    cv::merge(channels, imageMat);

    // Copy the adjusted data back to the image object
    memcpy(data, imageMat.data, size);
  }
}

void Image::warm_up(float strength) {
  if (data != nullptr && channels >= 3) {
    // Increase blue channel intensity and decrease red channel intensity
    float red_scale =
        1.0 - (strength * 0.5);         // Decrease red channel less than blue
    float blue_scale = 1.0 + strength;  // Increase blue channel

    // Apply color balance adjustments
    adjust_by_channel(red_scale, 1.0, blue_scale);
  }
}
