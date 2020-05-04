#include <cmath>
#include <iostream>
#include <map>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <vector>

int main(int argc, char *argv[]) {

// Even parity, then odd
  std::vector<std::vector<int>> left_digits{
      {0b0100111, 0b0110011, 0b0011011, 0b0100001, 0b0011101, 0b0111001,
       0b0000101, 0b0010001, 0b0001001, 0b0010111},
      {0b0001101, 0b0011001, 0b0010011, 0b0111101, 0b0100011, 0b0110001,
       0b0101111, 0b0111011, 0b0110111, 0b0001011}};
  // Even parity RIGHT digits
  std::vector<int> right_digits = {0b1110010, 0b1100110, 0b1101100, 0b1000010,
                                   0b1011100, 0b1001110, 0b1010000, 0b1000100,
                                   0b1001000, 0b1110100};

  std::map<std::string, int> left_digits_even{
      {"0100111", 0}, {"0110011", 1}, {"0011011", 2}, {"0100001", 3},
      {"0011101", 4}, {"0111001", 5}, {"0000101", 6}, {"0010001", 7},
      {"0001001", 8}, {"0010111", 9}};
  std::map<std::string, int> left_digits_odd{
      {"0001101", 0}, {"0011001", 1}, {"0010011", 2}, {"0111101", 3},
      {"0100011", 4}, {"0110001", 5}, {"0101111", 6}, {"0111011", 7},
      {"0110111", 8}, {"0001011", 9}};
  std::map<std::string, int> right_digits_even{
      {"1110010", 0}, {"1100110", 1}, {"1101100", 2}, {"1000010", 3},
      {"1011100", 4}, {"1001110", 5}, {"1010000", 6}, {"1000100", 7},
      {"1001000", 8}, {"1110100", 9}};

  // Begin video capture
  cv::VideoCapture camera(0);
  std::cout
      << "Bar Code Reader\nPress 'f' to freeze frame\nPress 'q' to quit\n";

  // Create windows
  cv::namedWindow("Original", cv::WINDOW_AUTOSIZE);
  cv::namedWindow("Processed", cv::WINDOW_AUTOSIZE);

  // Create image containers
  cv::Mat image_original, image_processed;

  while (true) {
    // Part 1
    // Continuously grab image from camera
    camera >> image_original;

    if (image_original.empty()) {
      std::cout << "Could not access camera...\n";
      break;
    }

    // Part 3
    // Convert image from BGR colorspace to grayscale
    cv::cvtColor(image_original, image_processed, cv::COLOR_BGR2GRAY);

    // Part 4
    // Convert image from grayscale to binary
    cv::threshold(image_processed, image_processed, 100, 255,
                  cv::THRESH_BINARY);

    // Extract middle of image, takes exactly 1 row
    cv::Range row_indices(image_processed.rows / 2,
                          image_processed.rows / 2 + 1);
    cv::Range column_indices(0, image_processed.cols);
    cv::Mat image_cropped = image_processed(row_indices, column_indices);

    // Find left guard bar
    // Rows should be 1 tho
    int switch_count = 0, start_col = 0, end_col = 0;
    for (int i = 0; i < image_cropped.rows; i++) {
      for (int j = 0; j < image_cropped.cols / 2; j++) {
        if (image_cropped.at<unsigned char>(i, j) !=
            image_cropped.at<unsigned char>(i, j - 1)) {
          if (switch_count == 0) {
            start_col = j;
          }
          switch_count++;
          if (switch_count == 3) {
            end_col = j;
            break;
          }
        }
      }
    }

    int bar_width = (end_col - start_col) / 3;
    std::cout << bar_width << std::endl;

    int num_bits = 0;
    std::vector<std::string> bar_code;
    bar_code.reserve(6);

    for (int i = 0; i < image_cropped.rows; i++) {
      for (int j = end_col; j < image_cropped.cols / 2; j = j + bar_width) {
        if (image_cropped.at<unsigned char>(i, j) == 0) {
        }

        if (num_bits == 7) {
          num_bits = 0;
        } else {
          num_bits++;
        }
      }
    }

    // Display image on window
    cv::imshow("Original", image_original);
    cv::imshow("Processed", image_processed);

    // Part 2
    // Exit program via 'q'
    char quit_key = (char)cv::waitKey(30);
    if (quit_key == 'q' || quit_key == 27) {
      break;
    }
  }
  return 0;
}