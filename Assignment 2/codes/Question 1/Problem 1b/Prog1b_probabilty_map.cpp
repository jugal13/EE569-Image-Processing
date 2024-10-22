/*
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Feb 19th 2024
*/

#include <iostream>
#include <vector>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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
};

typedef std::vector<RGB> ImageVectorRGB;
typedef std::vector<std::vector<RGB> > ImageMatrixRGB;
typedef std::vector<int8> ImageVectorInt8;
typedef std::vector<std::vector<int8> > ImageMatrixInt8;
typedef std::vector<double> ImageVectorDouble;
typedef std::vector<std::vector<double> > ImageMatrixDouble;

ImageMatrixRGB readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrixDouble imageData, int width, int height);
int8 channelRounding(double value);
ImageMatrixInt8 convertRGBtoGrayScale(ImageMatrixRGB& imageMatrix, int width, int height);
ImageMatrixInt8 getMirrorReflectedImage(ImageMatrixInt8& imageData, int width, int height, int padding);
ImageMatrixInt8 gaussianFilter(ImageMatrixInt8 mirroredImageData, int width, int height, int filterSize, int padding, double sigma);
double gaussianFunction(int x, int y, double sigma);
double convolutionHelper(ImageMatrixInt8& imageData, ImageMatrixDouble& filter, int i, int j, int padding);

int main(int argc, char* argv[]) {
  int imageWidth = 481;
  int imageHeight = 321;

  std::string fileName = "Tiger.raw";
  std::string writeFileName = "output1b.raw";

  int filterSize = 3;
  int padding = filterSize / 2;
  int sigma = 1;
  std::vector<std::pair<int,int>> thresholds = {
    {50, 100},
    {60, 120},
    {70, 140},
    {90, 180},
    {100, 200},
    {120, 240},
    {50, 125},
    {60, 150},
    {70, 175},
    {90, 225},
    {100, 250},
    {50, 150},
    {60, 180},
    {70, 210},
    {80, 240}
  };

  if (argc < 3) {
    std::cout << "Syntax Error - Incorrect Parameter Usage:" << std::endl;
    std::cout << "program_name input_image.raw output_image.raw" << std::endl;
    return 0;
  }

  fileName = argv[1];
  writeFileName = argv[2];

  ImageMatrixRGB imageData = readFile(fileName, imageWidth, imageHeight);

  ImageMatrixInt8 grayImageData = convertRGBtoGrayScale(imageData, imageWidth, imageHeight);

  ImageMatrixInt8 mirrorImageData = getMirrorReflectedImage(grayImageData, imageWidth, imageHeight, padding);

  ImageMatrixInt8 filteredImageData = gaussianFilter(mirrorImageData, imageWidth, imageHeight, filterSize, padding, sigma);

  cv::Mat cvImageData(imageHeight, imageWidth, CV_8UC1);

  for (int i = 0; i < imageHeight; i++) {
    for (int j = 0; j < imageWidth; j++) {
      cvImageData.at<int8>(i, j) = filteredImageData[i][j];
    }
  }

  cv::Mat totalEdgeMap(cvImageData.size(), CV_64FC1);

  for (int i = 0; i < thresholds.size(); i++) {
    cv::Mat cvEdgeDetectedImageData(cvImageData.size(), cvImageData.type());

    int lowerThreshold = thresholds[i].first;
    int higherThreshold = thresholds[i].second;

    cv::Canny(cvImageData, cvEdgeDetectedImageData, lowerThreshold, higherThreshold, 3, true);

    totalEdgeMap += cvEdgeDetectedImageData;
  }

  totalEdgeMap = totalEdgeMap / (double)thresholds.size();

  ImageMatrixDouble edgeDetectedImageData(imageHeight, ImageVectorDouble(imageWidth));

  for (int i = 0; i < imageHeight; i++) {
    for (int j = 0; j < imageWidth; j++) {
      edgeDetectedImageData[i][j] = totalEdgeMap.at<double>(i, j);
    }
  }

  writeFile(writeFileName, edgeDetectedImageData, imageWidth, imageHeight);

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

      imageData[i][j] = RGB(data);
    }
  }

  fclose(infile);

  return imageData;
}

void writeFile(std::string fileName, ImageMatrixDouble imageData, int width, int height) {
  FILE* outfile;

  outfile = fopen(&fileName[0], "wb");

  if (!outfile) {
    std::cout << "Cannot open file: " << fileName << std::endl;
    exit(1);
  }

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      double data = imageData[i][j];

      fwrite(&data, sizeof(double), 1, outfile);
    }
  }

  fclose(outfile);
}

int8 channelRounding(double value) {
  int channelValue = (int)value;

  if (channelValue > 255) {
    return 255;
  }

  if (channelValue < 0) {
    return 0;
  }

  return (int8)channelValue;
}

ImageMatrixInt8 convertRGBtoGrayScale(ImageMatrixRGB& imageData, int width, int height) {
  ImageMatrixInt8 grayImageData(height, ImageVectorInt8(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      RGB rgb = imageData[i][j];

      grayImageData[i][j] = channelRounding(0.2989 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b);
    }
  }

  return grayImageData;
}

ImageMatrixInt8 getMirrorReflectedImage(ImageMatrixInt8& imageData, int width, int height, int padding) {
  int mirrorImageWidth = width + padding * 2;
  int mirrorImageHeight = height + padding * 2;

  ImageMatrixInt8 mirroredImageData(mirrorImageHeight, ImageVectorInt8(mirrorImageWidth));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      mirroredImageData[i + padding][j + padding] = imageData[i][j];
    }
  }

  for (int j = 0; j < width; j++) {
    for (int k = 0; k < padding; k++) {
      mirroredImageData[k][j + padding] = imageData[padding - k][j];

      mirroredImageData[mirrorImageHeight - (k + 1)][j + padding] = imageData[height - (padding + 1 - k)][j];
    }
  }

  for (int i = 0; i < mirrorImageHeight; i++) {
    for (int k = 0; k < padding; k++) {
      mirroredImageData[i][k] = mirroredImageData[i][padding * 2 - k];

      mirroredImageData[i][mirrorImageWidth - (k + 1)] = mirroredImageData[i][mirrorImageWidth - (padding * 2 + 1 - k)];
    }
  }

  return mirroredImageData;
}

ImageMatrixInt8 gaussianFilter(ImageMatrixInt8 mirroredImageData, int width, int height, int filterSize, int padding, double sigma) {
  ImageMatrixInt8 interpolatedImage(height, ImageVectorInt8(width));

  std::vector<std::vector<double> > filter(filterSize, std::vector<double>(filterSize, 0.0));

  int index = filterSize / 2;
  int normalisingFactor = 1;

  for (int i = -index; i <= index; i++) {
    for (int j = -index; j <= index; j++) {
      double value = gaussianFunction(i, j, sigma);

      int x = index + i;
      int y = index + j;

      filter[x][y] = value;
    }
  }

  for (int i = padding; i <= height + (padding - 1); i++) {
    for (int j = padding; j <= width + (padding - 1); j++) {
      interpolatedImage[i - padding][j - padding] = convolutionHelper(mirroredImageData, filter, i, j, padding);
    }
  }

  return interpolatedImage;
}

double gaussianFunction(int x, int y, double sigma) {
  return std::exp(-(pow(x, 2) + std::pow(y, 2)) / (2 * (std::pow(sigma, 2)))) / (2 * M_PI * std::pow(sigma, 2));
}

double convolutionHelper(ImageMatrixInt8& imageData, ImageMatrixDouble& filter, int i, int j, int padding) {
  double sum = 0;

  for (int k = i - padding, filterX = 0; k <= i + padding; k++, filterX++) {
    for (int l = j - padding, filterY = 0; l <= j + padding; l++, filterY++) {
      double imageValue = (double)imageData[k][l];
      double filterValue = filter[filterX][filterY];

      sum += imageValue * filterValue;
    }
  }

  return channelRounding(sum);
}
