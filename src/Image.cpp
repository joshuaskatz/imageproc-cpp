#include <iostream>
using namespace std;
#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;
#include <tiffio.h>

#include "Image.h"

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
  data = Mat(h, w, CV_MAKETYPE(CV_8U, channels));
}

Image::Image(const Image &img)
    : w(img.w), h(img.h), channels(img.channels), size(img.size) {
  img.data.copyTo(data);
}

Image::~Image() { data.release(); }

bool Image::read(const char *filename) {
  ImageType type = get_file_type(filename);
  bool success = false;

  if (type == TIF) {
    TIFF *tif = TIFFOpen(filename, "r");

    if (tif) {
      uint32_t imageWidth, imageLength;
      TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
      TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);

      w = static_cast<int>(imageWidth);
      h = static_cast<int>(imageLength);
      TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &channels);

      std::vector<cv::Mat> planes;
      uint32_t rowStride = TIFFScanlineSize(tif);

      cv::Mat img(h, w, CV_MAKETYPE(CV_8U, channels));

      for (int row = 0; row < h; row++) {
        TIFFReadScanline(tif, img.data + row * rowStride, row);
      }

      data = img.clone();  // Clone the data to keep it persistent

      success = true;
      TIFFClose(tif);
    }
  } else {
    data = cv::imread(filename, IMREAD_UNCHANGED);

    if (!data.empty()) {
      w = data.cols;
      h = data.rows;
      channels = data.channels();
      success = true;
    }
  }

  return success;
}
bool Image::write(const char *filename) {
  ImageType type = get_file_type(filename);
  bool success = false;

  switch (type) {
    case PNG:
    case JPG:
    case TIF:

      success = imwrite(filename, data);
      break;

      TIFF *tif = TIFFOpen(filename, "w");

      if (tif) {
        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, w);
        TIFFSetField(tif, TIFFTAG_IMAGELENGTH, h);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, channels);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
        TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
        TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

        // Calculate the number of bytes per row
        size_t row_bytes = w * channels * sizeof(uint8_t);

        // Write image data
        for (uint32_t row = 0; row < h; row++) {
          if (TIFFWriteEncodedStrip(tif, row, data.ptr(row), row_bytes) == -1) {
            success = false;
            break;
          }
        }

        success = true;
        TIFFClose(tif);
      }
      break;
  }

  return success;
}

ImageType Image::get_file_type(const char *filename) {
  const char *ext = strrchr(filename, '.');
  if (ext != nullptr) {
    if (strcmp(ext, ".png") == 0) {
      return PNG;
    } else if (strcmp(ext, ".jpg") == 0) {
      return JPG;
    } else if (strcmp(ext, ".tif") == 0) {
      return TIF;
    }
  }
  return PNG;
}
