#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>

int main(int argc, char *argv[])
{
    // Begin video capture
    cv::VideoCapture camera(0);
    std::cout << "Blob Statistics\nPress 'q' to quit\n";

    // Create windows
    cv::namedWindow("Original", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Processed", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Drawing", cv::WINDOW_AUTOSIZE);

    // Create image containers
    cv::Mat image_original, image_processed, image_drawing;

    // Set to 2 d.p. though not necessary
    std::cout << std::setprecision(2) << std::fixed;

    while (true)
    {
        // Part 1
        // Continuously grab image from camera
        camera >> image_original;
        if (image_original.empty())
        {
            std::cout << "Could not access camera...\n";
            break;
        }

        // Part 3
        // Convert image from BGR colorspace to grayscale
        cv::cvtColor(image_original, image_processed, cv::COLOR_BGR2GRAY);
        // Part 4
        // Conert image from grayscale to binary
        cv::threshold(image_processed, image_processed, 100, 255, cv::THRESH_BINARY);

        // Part 6 & 7
        // Calculations
        float m00, m10, m01, m20, m02, m11, theta;
        m00 = 0.f, m10 = 0.f, m01 = 0.f, m20 = 0.f, m02 = 0.f, m11 = 0.f, theta = 0.f;
        for (int i = 0; i < image_processed.rows; i++)
        {
            for (int j = 0; j < image_processed.cols; j++)
            {
                if (image_processed.at<unsigned char>(i, j) == 0)
                {
                    m00++;
                    m10 += i;
                    m01 += j;
                    m20 += pow(i, 2);
                    m02 += pow(j, 2);
                    m11 += i * j;
                }
            }
        }
        
        theta = 0.5 * atan2(2 * (m00 * m11 - m10 * m01), (m00 * m20 - pow(m10, 2) - (m00 * m02 - pow(m01, 2))));

        std::cout << "\n";
        std::cout << "m00: " << m00 << std::setprecision(2) << "\n";
        std::cout << "m10: " << m10 << std::setprecision(2) << "\t";
        std::cout << "m01: " << m01 << std::setprecision(2) << "\n";
        std::cout << "m20: " << m20 << std::setprecision(2) << "\t";
        std::cout << "m02: " << m02 << std::setprecision(2) << "\n";
        std::cout << "\n";

        // Coordinate of centre of area
        cv::Point2f center(m01 / m00, m10 / m00);

        // Drawing functions
        // Convert CV_8UC1 to CV_8UC3 image so we can draw on image
        cv::cvtColor(image_processed, image_drawing, cv::COLOR_GRAY2BGR);

        // Draw stuff
        cv::circle(image_original, center, 20, cv::Scalar(0, 0, 255), 2);
        cv::circle(image_processed, center, 20, cv::Scalar(255, 255, 255), 2);
        cv::circle(image_drawing, center, 5, cv::Scalar(0, 0, 255), 10);

        // Draw cross for centre
        cv::line(image_drawing, cv::Point2f(center.x, center.y + 50), 
                                cv::Point2f(center.x, center.y - 50), 
                                cv::Scalar(0, 0, 255), 1);
        cv::line(image_drawing, cv::Point2f(center.x + 50, center.y), 
                                cv::Point2f(center.x - 50, center.y), 
                                cv::Scalar(0, 0, 255), 1);
        
        // Draw line for axis of rotation
        for (int k = 0; k < 25; k++)
        {
            cv::line(image_drawing, cv::Point2f(center.x + k * std::sin(theta), center.y + k * std::cos(theta)), 
                                    cv::Point2f(center.x - k * std::sin(theta), center.y - k * std::cos(theta)), 
                                    cv::Scalar(0, 255, 0), 1);
        }
        
        // Display image on window
        cv::imshow("Original", image_original);
        cv::imshow("Processed", image_processed);
        cv::imshow("Drawing", image_drawing);

        // Part 2
        // Exit program via 'q'
        char key = (char)cv::waitKey(30);
        if (key == 'q' || key == 27)
        {
            break;
        }
    }
    return 0;
}