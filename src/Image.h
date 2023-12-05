#include <stdint.h>

#include <cstdio>

enum ImageType { PNG, JPG, BMP, TGA };

struct Image {
 public:
  uint8_t *data = NULL;
  size_t size = 0;
  int w;
  int h;
  int channels;

  Image(const char *filename);
  Image(int w, int h, int channels);
  Image(const Image &img);

  ~Image();

  bool read(const char *filename);
  bool write(const char *filename);

  ImageType get_file_type(const char *filename);

  void invert_image();
  void white_balance();
  void adjust_by_channel(float red_scale, float green_scale, float blue_scale);
  void warm_up(float strength);
};