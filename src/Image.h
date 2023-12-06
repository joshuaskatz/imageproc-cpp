#include <stdint.h>
#include <tiffio.h>

#include <cstdio>
#include <opencv2/opencv.hpp>
using namespace cv;

enum ImageType { PNG, JPG, TIF };

struct Image {
  Mat data;
  int w, h, channels;
  size_t size;

  Image(const char *filename);
  Image(int w, int h, int channels);
  Image(const Image &img);

  ~Image();

  bool read(const char *filename);

  bool write(const char *filename);

  ImageType get_file_type(const char *filename);
};