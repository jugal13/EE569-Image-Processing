/*
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Mar 10th 2024
*/

#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/highgui.hpp>

typedef unsigned char int8;

class RGB {
public:
  int8 r;
  int8 g;
  int8 b;

  RGB() {
    r = g = b = 0;
  }

  RGB(int8 r_value, int8 g_value, int8 b_value) : r(r_value), g(g_value), b(b_value) {}

  RGB(const RGB& rgb) {
    r = rgb.r;
    g = rgb.g;
    b = rgb.b;
  }

  RGB(int8 rgb[3]) {
    r = rgb[0];
    g = rgb[1];
    b = rgb[2];
  }

  std::vector<int8> rgbString() {
    std::vector<int8> rgb(3, 0);

    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;

    return rgb;
  }

  std::vector<int8> bgrString() {
    std::vector<int8> bgr(3, 0);

    bgr[0] = b;
    bgr[1] = g;
    bgr[2] = r;

    return bgr;
  }
};

typedef std::vector<RGB> ImageVectorRGB;
typedef std::vector<std::vector<RGB>> ImageMatrixRGB;

ImageMatrixRGB readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrixRGB imageData, int width, int height);

int main(int argc, char* argv[]) {
  int imageWidth = 605;
  int imageHeight = 454;
  int middleOffset = imageWidth + imageWidth / 2;

  std::string fileName1 = "toy_left.raw";
  std::string fileName2 = "toy_middle.raw";
  std::string fileName3 = "toy_right.raw";
  std::string writeFileName = "output.raw";

  if (argc < 5) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name input_image1.raw input_image2.raw input_image3.raw output.raw" << std::endl;
    return 0;
  }

  fileName1 = argv[1];
  fileName2 = argv[2];
  fileName3 = argv[3];
  writeFileName = argv[4];

  ImageMatrixRGB leftImageData = readFile(fileName1, imageWidth, imageHeight);
  ImageMatrixRGB middleImageData = readFile(fileName2, imageWidth, imageHeight);
  ImageMatrixRGB rightImageData = readFile(fileName3, imageWidth, imageHeight);

  cv::Mat cvLeftImageData(imageHeight, imageWidth, CV_8UC3);
  cv::Mat cvMiddleImageData(imageHeight, imageWidth, CV_8UC3);
  cv::Mat cvRightImageData(imageHeight, imageWidth, CV_8UC3);


  for (int i = 0; i < imageHeight; i++) {
    for (int j = 0; j < imageWidth; j++) {
      std::vector<int8> leftBGRValues = leftImageData[i][j].bgrString();
      std::vector<int8> middleBGRValues = middleImageData[i][j].bgrString();
      std::vector<int8> rightBGRValues = rightImageData[i][j].bgrString();

      for (int k = 0; k < 3; k++) {
        cvLeftImageData.at<cv::Vec3b>(i, j)[k] = leftBGRValues[k];
        cvMiddleImageData.at<cv::Vec3b>(i, j)[k] = middleBGRValues[k];
        cvRightImageData.at<cv::Vec3b>(i, j)[k] = rightBGRValues[k];
      }
    }
  }

  cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create(400);

  std::vector<cv::KeyPoint> leftKeypoints, middleKeypoints, rightKeypoints;
  cv::Mat leftDescriptors, middleDescriptors, rightDescriptors;

  surf->detectAndCompute(cvLeftImageData, cv::noArray(), leftKeypoints, leftDescriptors);
  surf->detectAndCompute(cvMiddleImageData, cv::noArray(), middleKeypoints, middleDescriptors);
  surf->detectAndCompute(cvRightImageData, cv::noArray(), rightKeypoints, rightDescriptors);

  cv::FlannBasedMatcher flann_matcher;
  std::vector<std::vector<cv::DMatch>> left_knn_matches, right_knn_matches;
  flann_matcher.knnMatch(leftDescriptors, middleDescriptors, left_knn_matches, 2);
  flann_matcher.knnMatch(middleDescriptors, rightDescriptors, right_knn_matches, 2);

  const float ratio_thresh = 0.7f;
  // const float ratio_thresh = 0.75f;
  std::vector<cv::DMatch> left_good_matches;
  for (size_t i = 0; i < left_knn_matches.size(); i++) {
    if (left_knn_matches[i][0].distance < ratio_thresh * left_knn_matches[i][1].distance) {
      left_good_matches.push_back(left_knn_matches[i][0]);
    }
  }

  std::vector<cv::DMatch> right_good_matches;
  for (size_t i = 0; i < right_knn_matches.size(); i++) {
    if (right_knn_matches[i][0].distance < ratio_thresh * right_knn_matches[i][1].distance) {
      right_good_matches.push_back(right_knn_matches[i][0]);
    }
  }


  std::vector<cv::DMatch> final_left_good_matches = {
    left_good_matches[2],
    left_good_matches[23],
    left_good_matches[31],
    left_good_matches[45],
  };

  std::cout << "Left - Middle" << std::endl;
  for (int i = 0; i < final_left_good_matches.size(); i++) {
    int queryIdx = final_left_good_matches[i].queryIdx;
    cv::KeyPoint leftKeypoint = leftKeypoints[queryIdx];


    double row = leftKeypoint.pt.y + imageHeight;
    double col = leftKeypoint.pt.x;

    std::cout << "{" << row << "," << col << "}," << std::endl;
  }

  std::cout << "Left - Middle" << std::endl;
  for (int i = 0; i < final_left_good_matches.size(); i++) {
    int trainIdx = final_left_good_matches[i].trainIdx;
    cv::KeyPoint middleKeypoint = middleKeypoints[trainIdx];

    double row = middleKeypoint.pt.y + imageHeight;
    double col = middleKeypoint.pt.x + middleOffset;

    std::cout << "{" << row << "," << col << "}," << std::endl;
  }

  cv::Mat left_img_matches;
  cv::drawMatches(
    cvLeftImageData, leftKeypoints,
    cvMiddleImageData, middleKeypoints,
    final_left_good_matches, left_img_matches,
    cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
  );
  //-- Show detected matches
  cv::imshow("Good Matches", left_img_matches);

  std::vector<cv::DMatch> final_right_good_matches = {
   right_good_matches[2],
   right_good_matches[14],
   right_good_matches[45],
   right_good_matches[30],
  };

  std::cout << std::endl << std::endl << "Right - Middle" << std::endl;
  for (int i = 0; i < final_right_good_matches.size(); i++) {
    int queryIdx = final_right_good_matches[i].queryIdx;
    cv::KeyPoint middleKeypoint = middleKeypoints[queryIdx];

    double row = middleKeypoint.pt.y + imageHeight;
    double col = middleKeypoint.pt.x + middleOffset;

    std::cout << "{" << row << "," << col << "}," << std::endl;
  }

  std::cout << "Right - Middle" << std::endl;
  for (int i = 0; i < final_right_good_matches.size(); i++) {
    int trainIdx = final_right_good_matches[i].trainIdx;
    cv::KeyPoint rightKeypoint = rightKeypoints[trainIdx];

    double row = rightKeypoint.pt.y + imageHeight;
    double col = rightKeypoint.pt.x + (3 * imageWidth);

    std::cout << "{" << row << "," << col << "}," << std::endl;
  }

  cv::Mat right_img_matches;
  cv::drawMatches(
    cvMiddleImageData, middleKeypoints,
    cvRightImageData, rightKeypoints,
    final_right_good_matches, right_img_matches,
    cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
  );
  cv::imshow("Good matches 2", right_img_matches);

  cv::waitKey();
  return 0;
}

ImageMatrixRGB readFile(std::string fileName, int width, int height) {
  FILE* infile;

  infile = fopen(&fileName[0], "rb");

  if (!infile) {
    std::cout << "Cannot open file: " << fileName << std::endl;
    exit(1);
  }

  ImageMatrixRGB imageData(height, ImageVectorRGB(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int8 data[3];

      fread(&data[0], sizeof(int8), 1, infile);
      fread(&data[1], sizeof(int8), 1, infile);
      fread(&data[2], sizeof(int8), 1, infile);

      imageData[i][j] = RGB(data[0], data[1], data[2]);
    }
  }

  fclose(infile);

  return imageData;
}

void writeFile(std::string fileName, ImageMatrixRGB imageData, int width, int height) {
  FILE* outfile;

  outfile = fopen(&fileName[0], "wb");

  if (!outfile) {
    std::cout << "Cannot open file: " << fileName << std::endl;
    exit(1);
  }

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      std::vector<int8> data = imageData[i][j].rgbString();

      for (int k = 0; k < data.size(); k++) {
        int8 bits = round(data[k]);

        fwrite(&bits, sizeof(int8), 1, outfile);
      }
    }
  }

  fclose(outfile);
}
