#include <iostream>

#include "Image.h"

int main(int argc, char **argv) {
  Image img("../src/init.jpg");

  img.invert_image();
  img.write("inverted.jpg");

  img.white_balance();
  img.write("wb.jpg");

  img.warm_up(0.1);
  img.write("warm.jpg");

  return 0;
}