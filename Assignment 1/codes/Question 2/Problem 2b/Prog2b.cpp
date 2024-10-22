/*
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Jan 28th 2024
*/

#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <vector>

typedef unsigned char int8;
typedef std::vector<int8> ImageVector;
typedef std::vector<std::vector<int8> > ImageMatrix;

ImageMatrix readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrix imageData, int width, int height);
ImageMatrix getMirrorReflectedImage(ImageMatrix imageData, int width, int height, int padding);
ImageMatrix bilateralFilter(ImageMatrix mirroredImageData, int width, int height, int filterSize, int padding, double sigmaC, double sigmaS);
int8 channelRounding(double value);
int8 convolutionHelper(ImageMatrix& imageData, int filterSize, int k, int l, double sigmaC, double sigmaS);
double calculateMeanSquareError(ImageMatrix noiseFreeImageData, ImageMatrix filteredimageData, int width, int height);

int main(int argc, char* argv[]) {
  int imageWidth = 768;
  int imageHeight = 512;

  std::string noiseFreeFileName = "Flower_gray.raw";
  std::string fileName = "Flower_gray_noisy.raw";
  std::string writeFileName = "output2b.raw";
  int filterSize = 3;
  double sigmaC = 1.0;
  double sigmaS = 1.0;
  int padding = filterSize / 2;

  if (argc < 4) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name noise_free_image.raw input_image.raw output_image.raw [filter_size=3] [sigmaC=1.0] [sigmaS=1.0]" << std::endl;
    return 0;
  }

  noiseFreeFileName = argv[1];
  fileName = argv[2];
  writeFileName = argv[3];

  if (argc > 4) {
    filterSize = atoi(argv[4]);

    if (filterSize % 2 == 0) {
      std::cout << "Value Error - Incorrect Filter Size" << std::endl;
      std::cout << "Please use odd value for filter size" << std::endl;
      return 0;
    }

    padding = filterSize / 2;
  }

  if (argc > 5) {
    sigmaC = atof(argv[5]);
  }

  if (argc > 6) {
    sigmaS = atof(argv[6]);
  }

  ImageMatrix noiseFreeImageData = readFile(noiseFreeFileName, imageWidth, imageHeight);

  std::cout << "Read" << std::endl;
  ImageMatrix imageData = readFile(fileName, imageWidth, imageHeight);

  std::cout << "Mirror" << std::endl;
  ImageMatrix mirroredImageData = getMirrorReflectedImage(imageData, imageWidth, imageHeight, padding);

  std::cout << "Filter" << std::endl;
  ImageMatrix denoisedImageData = bilateralFilter(mirroredImageData, imageWidth, imageHeight, filterSize, padding, sigmaC, sigmaS);

  std::cout << "Write" << std::endl;
  writeFile(writeFileName, denoisedImageData, imageWidth, imageHeight);

  double mse = calculateMeanSquareError(noiseFreeImageData, denoisedImageData, imageWidth, imageHeight);

  double psnr = 10 * std::log10((255 * 255) / mse);

  std::cout << "PSNR: " << psnr << std::endl;

  return 0;
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

ImageMatrix getMirrorReflectedImage(ImageMatrix imageData, int width, int height, int padding) {
  int mirrorImageWidth = width + padding * 2;
  int mirrorImageHeight = height + padding * 2;

  ImageMatrix mirroredImageData(mirrorImageHeight, ImageVector(mirrorImageWidth));

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

ImageMatrix bilateralFilter(ImageMatrix mirroredImageData, int width, int height, int filterSize, int padding, double sigmaC, double sigmaS) {
  ImageMatrix filteredImageData(height, ImageVector(width));

  for (int i = padding; i <= height + (padding - 1); i++) {
    for (int j = padding; j <= width + (padding - 1); j++) {
      filteredImageData[i - padding][j - padding] = convolutionHelper(mirroredImageData, filterSize, i, j, sigmaC, sigmaS);
    }
  }

  return filteredImageData;
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

int8 convolutionHelper(ImageMatrix& imageData, int filterSize, int i, int j, double sigmaC, double sigmaS) {
  int8 finalIntensityValue;

  int coordPadding = (filterSize / 2);

  double num_weight = 0.0;
  double den_weight = 0.0;

  for (int k = i - coordPadding; k <= i + coordPadding; k++) {
    for (int l = j - coordPadding; l <= j + coordPadding; l++) {
      double left_num = (k - i) * (k - i) + (l - j) * (l - j);
      double left_den = 2 * sigmaC * sigmaC;

      double left_frac = left_num / left_den;

      double right_num_root = imageData[k][l] - imageData[i][j];
      double right_num = right_num_root * right_num_root;
      double right_den = 2 * sigmaS * sigmaS;

      double right_frac = right_num / right_den;

      double exponent = -left_frac - right_frac;
      double curr_weight = std::exp(exponent);


      num_weight += imageData[k][l] * curr_weight;
      den_weight += curr_weight;
    }
  }

  double weight = num_weight / den_weight;

  finalIntensityValue = channelRounding(weight);

  return finalIntensityValue;
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
