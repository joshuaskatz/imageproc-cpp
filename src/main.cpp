#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <iostream>

#include "Processing.h"

int main(int argc, char **argv) {
  Image img("../src/assets/init-2.jpg");
  Processing proc;
  proc.image = &img;
  proc.invert_image();
  img.write("inverted.jpg");
  proc.white_balance();
  img.write("wb.jpg");
  if (img.data.empty()) {
    std::cout << "Could not open or find the image." << std::endl;
    return -1;
  }

  // Splitting the image into individual color channels (BGR format)
  std::vector<Mat> channels;
  split(img.data, channels);

  // Simulating adjustments to cyan, yellow, and magenta channels
  // Simulating Cyan: Reduce Red channel
  channels[2] += 15;  // Decrease Red for Cyan

  // Simulating Yellow: Reduce Blue and Green channels
  channels[0] -= 25;  // Decrease Blue for Yellow

  // Merge the modified channels back together
  Mat adjusted_image;
  merge(channels, adjusted_image);

  // Display the original image and adjusted image
  imshow("Original Image", img.data);
  imshow("Adjusted Image", adjusted_image);

  waitKey(0);
  return 0;
}