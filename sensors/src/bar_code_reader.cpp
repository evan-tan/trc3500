#include "../include/util.hpp"
#include <iostream>
#include <map>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {

  std::map<std::string, std::string> left_digits_even{
      {"0100111", "0"}, {"0110011", "1"}, {"0011011", "2"}, {"0100001", "3"},
      {"0011101", "4"}, {"0111001", "5"}, {"0000101", "6"}, {"0010001", "7"},
      {"0001001", "8"}, {"0010111", "9"}};

  std::map<std::string, std::string> left_digits_odd{
      {"0001101", "0"}, {"0011001", "1"}, {"0010011", "2"}, {"0111101", "3"},
      {"0100011", "4"}, {"0110001", "5"}, {"0101111", "6"}, {"0111011", "7"},
      {"0110111", "8"}, {"0001011", "9"}};

  std::map<std::string, std::string> right_digits_even{
      {"1110010", "0"}, {"1100110", "1"}, {"1101100", "2"}, {"1000010", "3"},
      {"1011100", "4"}, {"1001110", "5"}, {"1010000", "6"}, {"1000100", "7"},
      {"1001000", "8"}, {"1110100", "9"}};

  std::map<std::string, std::string> parity_table;
  parity_table["OOOOOO"] = "0";
  parity_table["OOEOEE"] = "1";
  parity_table["OOEEOE"] = "2";
  parity_table["OOEEEO"] = "3";
  parity_table["OEOOEE"] = "4";
  parity_table["OEEOOE"] = "5";
  parity_table["OEEEOO"] = "6";
  parity_table["OEOEOE"] = "7";
  parity_table["OEOEEO"] = "8";
  parity_table["OEEOEO"] = "9";

  // Begin video capture
  // cv::VideoCapture camera(0);
  std::cout
      << "Bar Code Reader\nPress 'f' to freeze frame\nPress 'q' to quit\n";

  // Create windows
  cv::namedWindow("Original", cv::WINDOW_AUTOSIZE);
  cv::namedWindow("Processed", cv::WINDOW_AUTOSIZE);

  // Create image containers
  cv::Mat image_original, image_processed;
  image_original = cv::imread(argv[1], 1);

  while (true) {
    // Part 1
    // Continuously grab image from camera
    // camera >> image_original;

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

    // Determine width in pixels for a single 1-bit "bar"
    int switch_count = 0, start_col = 0, end_col = 0;
    for (int i = 0; i < image_cropped.rows; i++) {
      for (int j = 1; j < image_cropped.cols / 4; j++) {
        if (image_cropped.at<unsigned char>(i, j) !=
            image_cropped.at<unsigned char>(i, j - 1)) {
          // Find index of first black bar
          if (switch_count == 0) {
            start_col = j - 1;
          }
          switch_count++;
          // Find
          if (switch_count == 4) {
            end_col = j;
            break;
          }
        }
      }
    }

    std::string zero, one;
    int bar_width = (end_col - start_col) / 3;
    for (int i = 0; i < bar_width; i++) {
      zero.append("0");
      one.append("1");
    }
    std::cout << "'Bar' width: " << bar_width << std::endl;

    // Store bar code in binary form into string
    std::string bar_code_original;
    for (int i = 0; i < image_cropped.rows; i++) {
      // Store only left 6 digits in bar code
      for (int j = 0; j < image_cropped.cols; j++) {
        // If pixel is black
        if (image_cropped.at<unsigned char>(i, j) == 0) {
          bar_code_original.append("1");
        } else {
          bar_code_original.append("0");
        }
      }
    }

    // Remove first trail of zeros
    while (bar_code_original[0] != '1') {
      bar_code_original.erase(bar_code_original.begin());
    }
    // Remove first guard bar
    for (int idx = 0; idx < 24; idx++) {
      bar_code_original.erase(bar_code_original.begin());
    }
    // Remove last trail of zeros
    while (bar_code_original[bar_code_original.size() - 1] != '1') {
      bar_code_original =
          bar_code_original.substr(0, bar_code_original.size() - 1);
    }

    // Decode regardless of thickness
    std::string bar_code_processed;
    for (int i = 0; i < bar_code_original.size(); i += bar_width) {
      std::string current_digit = bar_code_original.substr(i, 8);
      if (current_digit == zero) {
        bar_code_processed.append("0");
      } else if (current_digit == one) {
        bar_code_processed.append("1");
      }
    }

    int n_bits = 7;
    std::string bar_code_result, parity;
    std::map<std::string, std::string>::iterator map_iterator;
    for (int i = 0; i < 42; i += n_bits) {
      std::string curr_digit_bin = bar_code_processed.substr(i, 7);
      map_iterator = left_digits_even.find(curr_digit_bin);

      // If not found in even parity table
      if (map_iterator == left_digits_even.end()) {
        // Search odd parity table
        map_iterator = left_digits_odd.find(curr_digit_bin);
      } else if (map_iterator == left_digits_odd.end()) {
        std::cout << "Not found\n";
      }
      bar_code_result.append(map_iterator->second);
      parity.append(parity_check(curr_digit_bin));
    }
    map_iterator = parity_table.find(parity);
    if (map_iterator == parity_table.end()) {
      std::cout << "Fuck yo parity\n";
    } else {
      std::cout << map_iterator->second;
    }
    std::cout << bar_code_result << std::endl;

    // Display image on window
    cv::imshow("Original", image_original);
    cv::imshow("Processed", image_cropped);

    // Part 2
    // Exit program via 'q'
    char quit_key = (char)cv::waitKey(30);
    if (quit_key == 'q' || quit_key == 27) {
      break;
    }
  }
  return 0;
}