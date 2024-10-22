/*
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Jan 28th 2024
*/

#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

typedef unsigned char int8;
typedef std::vector<int8> ImageVector;
typedef std::vector<std::vector<int8> > ImageMatrix;

ImageMatrix readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrix imageData, int width, int height);
double calculateMeanSquareError(ImageMatrix noiseFreeImageData, ImageMatrix filteredimageData, int width, int height);

int main(int argc, char* argv[]) {
  int imageWidth = 768;
  int imageHeight = 512;

  std::string noiseFreeFileName = "Flower_gray.raw";
  std::string fileName = "Flower_gray_noisy.raw";
  std::string writeFileName = "output2c.raw";

  if (argc < 3) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name noise_free_image.raw noisy_input_image.raw output_image.raw" << std::endl;
    return 0;
  }

  noiseFreeFileName = argv[1];
  fileName = argv[2];
  writeFileName = argv[3];

  ImageMatrix noiseFreeImageData = readFile(noiseFreeFileName, imageWidth, imageHeight);

  ImageMatrix imageData = readFile(fileName, imageWidth, imageHeight);

  cv::Mat cvImageData(imageHeight, imageWidth, CV_8UC3);

  for (int i = 0; i < imageHeight; i++) {
    for (int j = 0; j < imageWidth; j++) {
      for (int k = 0; k < 3; k++) {
        cvImageData.at<int8>(i, j) = imageData[i][j];
      }
    }
  }

  cv::Mat cvDenoisedImage;
  cv::fastNlMeansDenoising(cvImageData, cvDenoisedImage, 10, 7, 14);

  ImageMatrix denoisedImageData(imageHeight, ImageVector(imageWidth));

  for (int i = 0; i < imageHeight; i++) {
    for (int j = 0; j < imageWidth; j++) {
      for (int k = 0; k < 3; k++) {
        denoisedImageData[i][j] = (int8)cvDenoisedImage.at<int8>(i, j);
      }
    }
  }

  writeFile(writeFileName, denoisedImageData, imageWidth, imageHeight);

  double mse = calculateMeanSquareError(noiseFreeImageData, denoisedImageData, imageWidth, imageHeight);

  double psnr = 10 * std::log10((255 * 255) / mse);

  std::cout << "PSNR: " << psnr << std::endl;
}

ImageMatrix readFile(std::string fileName, int width, int height) {
  FILE* infile;

  infile = fopen(&fileName[0], "rb");

  if (!infile) {
    std::cout << "Cannot open file: " << fileName << std::endl;
    exit(1);
  }

  ImageMatrix imageData(height, ImageVector(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int8 data[1];

      fread(data, sizeof(int8), 1, infile);

      imageData[i][j] = data[0];
    }
  }

  fclose(infile);

  return imageData;
}

void writeFile(std::string fileName, ImageMatrix imageData, int width, int height) {
  FILE* outfile;

  outfile = fopen(&fileName[0], "wb");

  if (!outfile) {
    std::cout << "Cannot open file: " << fileName << std::endl;
    exit(1);
  }

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int8* data = &imageData[i][j];

      fwrite(data, sizeof(int8), 1, outfile);
    }
  }

  fclose(outfile);
}

double calculateMeanSquareError(ImageMatrix noiseFreeImageData, ImageMatrix filteredimageData, int width, int height) {
  double total = 0.0;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      double diff = noiseFreeImageData[i][j] - filteredimageData[i][j];
      double diff_sq = diff * diff;

      total += diff_sq;
    }
  }

  double mse = total / (width * height);

  return mse;
}
