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

#include "../include/util.hpp"

int main(int argc, char *argv[])
{
    cv::VideoCapture camera(0);
    std::cout << "Bar Code Reader\nPress 'q' to quit\n";

    // Create windows
    cv::namedWindow("Original", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Processed", cv::WINDOW_AUTOSIZE);

    // Get image from command line argument, load in color mode
    cv::Mat image_original, image_processed;
    image_original = cv::imread(argv[1], 1);

    while (true)
    {
        // camera >> image_original;
        if (image_original.empty())
        {
            std::cout << "Could not access image...\n";
            break;
        }

        // Convert image from BGR colorspace to grayscale
        cv::cvtColor(image_original, image_processed, cv::COLOR_BGR2GRAY);
        // Convert image from grayscale to binary
        cv::threshold(image_processed, image_processed, 100, 255, cv::THRESH_BINARY);

        // Extract middle of image, takes exactly 1 row
        cv::Range row_indices(image_processed.rows / 2, image_processed.rows / 2 + 1);
        cv::Range column_indices(0, image_processed.cols);
        cv::Mat image_cropped = image_processed(row_indices, column_indices);

        // Determine width in pixels for a single 1-bit "bar"
        int switch_count = 0, start_col = 0, end_col = 0;
        for (int i = 0; i < image_cropped.rows; i++)
        {
            // Search from LHS of image
            for (int j = 1; j < image_cropped.cols / 4; j++)
            {
                if (image_cropped.at<unsigned char>(i, j) != image_cropped.at<unsigned char>(i, j - 1))
                {
                    // Find index of first black bar
                    if (switch_count == 0)
                    {
                        start_col = j - 1;
                    }
                    switch_count++;
                    // Find index of last black bar
                    if (switch_count == 4)
                    {
                        end_col = j;
                        break;
                    }
                }
            }
        }

        int bar_width = (end_col - start_col) / 3;
        std::string zero, one;
        for (int i = 0; i < bar_width; i++)
        {
            zero.append("0");
            one.append("1");
        }

        std::string bar_code_original;
        // Store bar code in binary form into string
        for (int i = 0; i < image_cropped.rows; i++)
        {
            // Store only left 6 digits in bar code
            for (int j = 0; j < image_cropped.cols; j++)
            {
                // If pixel is black, use "1" to represent
                if (image_cropped.at<unsigned char>(i, j) == 0)
                {
                    bar_code_original.append("1");
                } else
                {
                    bar_code_original.append("0");
                }
            }
        }

        // Remove first trail of zeros
        while (bar_code_original[0] != '1')
        {
            bar_code_original.erase(bar_code_original.begin());
        }
        // Remove first guard bar
        for (int idx = 0; idx < 24; idx++)
        {
            bar_code_original.erase(bar_code_original.begin());
        }
        // Remove last trail of zeros
        while (bar_code_original[bar_code_original.size() - 1] != '1')
        {
            bar_code_original = bar_code_original.substr(0, bar_code_original.size() - 1);
        }

        std::string bar_code_processed;
        // Actual bar code representation regardless of scale / bar thickness
        for (int i = 0; i < bar_code_original.size(); i += bar_width)
        {
            std::string current_digit = bar_code_original.substr(i, 8);
            if (current_digit == zero)
            {
                bar_code_processed.append("0");
            } else if (current_digit == one)
            {
                bar_code_processed.append("1");
            }
        }
        
        // number of digits you want to detect
        int n_bits = 7, n_digits = 6;
        std::string bar_code_result, parity;
        std::map<std::string, std::string>::iterator map_iterator;
        using namespace BarCodeReader;
        LeftDigitMap left_digits_even("even");
        LeftDigitMap left_digits_odd("odd");
        ParityMap parity_table;
        for (int i = 0; i < n_bits * n_digits; i += n_bits)
        {
            std::string digit_in_binary = bar_code_processed.substr(i, n_bits);
            map_iterator                = left_digits_even.ld_map.find(digit_in_binary);
            // Search even parity table, and search odd parity table if need be
            if (map_iterator == left_digits_even.ld_map.end())
            {
                map_iterator = left_digits_odd.ld_map.find(digit_in_binary);
            } else if (map_iterator == left_digits_odd.ld_map.end())
            {
                std::cout << "Left digit not found\n";
            }
            
            // Determine first 7 digits of bar code
            parity.push_back(parity_check(digit_in_binary));
            bar_code_result.append(map_iterator->second);
        }

        // Search parity
        map_iterator = parity_table.p_map.find(parity);
        if (map_iterator == parity_table.p_map.end())
        {
            std::cout << "Parity not found\n";
        } else
        {
            std::cout << map_iterator->second;
        }
        std::cout << bar_code_result << std::endl;

        // Display image on window
        cv::imshow("Original", image_original);
        cv::imshow("Processed", image_processed);

        // Part 2
        // Exit program via 'q'
        char quit_key = (char)cv::waitKey(30);
        if (quit_key == 'q' || quit_key == 27)
        {
            break;
        }
    }
    return 0;
}