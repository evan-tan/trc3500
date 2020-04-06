#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
    cv::VideoCapture camera(0);
    cv::Mat image_original, image_processed;
    cv::Mat image_blob, image_hsv;

    std::cout << "Blob Statistics\nPress 'q' to quit\n";

    cv::namedWindow("Original", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Processed", cv::WINDOW_AUTOSIZE);

    cv::SimpleBlobDetector::Params params;
    params.minThreshold = 150;          // oh shit
    params.maxThreshold = 500;
    params.filterByArea = false;        // Filter by Area.
    params.minArea = 500;
    params.filterByCircularity = true;  // Filter by Circularity
    params.minCircularity = 0.1;
    params.filterByConvexity = true;    // Filter by Convexity
    params.minConvexity = 0.5;
    params.filterByInertia = false;     // Filter by Inertia
    params.minInertiaRatio = 0.01;
    params.blobColor = 0;
    std::vector<cv::KeyPoint> keypoints;    // Storage for blobs
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params); // Set up detector with params

    int erosion_dilation_size = 1;
    cv::Mat element_erosion = getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosion_dilation_size + 1, 2 * erosion_dilation_size + 1), cv::Point(erosion_dilation_size, erosion_dilation_size)); 
    cv::Mat element_dilation = getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosion_dilation_size + 1, 2 * erosion_dilation_size + 1), cv::Point(erosion_dilation_size, erosion_dilation_size));

    while (true)
    {

        // Part 1
        camera >> image_original;
        if (image_original.empty())
        {
            
            std::cout << "Could not access camera...\n";
            break;
        }

        // Part 3
        image_hsv = cv::Mat::zeros(cv::Size(image_original.rows, image_original.cols), CV_8UC3);
        cv::cvtColor(image_original, image_hsv, cv::COLOR_BGR2HSV);
        cv::inRange(image_hsv, cv::Scalar(20, 80, 130), cv::Scalar(50, 225, 225), image_processed);
        
        std::vector<cv::Mat> hsv_channels;
        hsv_channels.clear();
        hsv_channels.resize(image_processed.channels());
        cv::split(image_processed, &hsv_channels[0]);
        image_blob = hsv_channels[2];
            
        // Part 4
        // Remove background noise by eroding and dilating
        cv::erode(image_blob, image_blob, element_erosion);
        cv::dilate(image_blob, image_blob, element_dilation);
        detector->detect(image_blob, keypoints);
        drawKeypoints(image_blob, keypoints, image_blob, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

        int m00;

        // Part 6
        for (int i = 0; i < image_blob.rows; i++)
        {
            for (int j = 0; j < image_blob.cols; j++)
            {
                // std::cout << image_blob.at<unsigned char>(i,j);
            }
        }


        cv::imshow("Original", image_original);
        cv::imshow("Processed", image_processed);

        // Part 2
        char key = (char)cv::waitKey(30);
        if (key == 'q' || key == 27)
        {
            // key == 27 for ESC
            break;
        }
    }
    return 0;
}

// // Convert half of the image to gray
// for (int y = 0; y < frame->height / 2; y++) {
//   for (int x = 0; x < frame->width; x++) {
//     // This is a pointer to the start of the current row.
//     //  Note: The image is stored as a 1-D array which is mapped back
//     //  into 2-space by multiplying the widthStep (the image width rounded
//     //  to a "nice" value, eg a multiple of 4 or 8 depending on the OS and
//     //  CPU) by the row number.
//     uchar *row = (uchar *)(frame->imageData + frame->widthStep * y);

//     int gray = (row[x * 3] + row[x * 3 + 1] + row[x * 3 + 2]) / 3;

//     row[x * 3] = gray;
//     row[x * 3 + 1] = gray;
//     row[x * 3 + 2] = gray;
//   }