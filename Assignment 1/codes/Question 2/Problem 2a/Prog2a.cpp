/*
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Jan 28th 2024
*/

#include <iostream>
#include <string>
#include <cmath>
#include <vector>

typedef unsigned char int8;
typedef std::vector<int8> ImageVector;
typedef std::vector<std::vector<int8> > ImageMatrix;

ImageMatrix readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrix imageData, int width, int height);
ImageMatrix getMirrorReflectedImage(ImageMatrix imageData, int width, int height, int padding);
ImageMatrix lowPassFilter(ImageMatrix mirroredImageData, int width, int height, int filterSize, int padding);
ImageMatrix gaussianFilter(ImageMatrix mirroredImageData, int width, int height, int filterSize, int padding, double sigma);
double gaussianFunction(int x, int y, double sigma);
int8 channelRounding(double value);
int8 convolutionHelper(ImageMatrix& imageData, std::vector<std::vector<double> > imageFilter, int x_coord, int y_coord, int filterNormalisingFactor);
double calculateMeanSquareError(ImageMatrix noiseFreeImageData, ImageMatrix filteredimageData, int width, int height);

int main(int argc, char* argv[]) {
  int imageWidth = 768;
  int imageHeight = 512;

  std::string noiseFreeFileName = "Flower_gray.raw";
  std::string fileName = "Flower_gray_noisy.raw";
  std::string writeFileName = "output2a.raw";
  int filterSize = 3;
  double sigma = 1.0;
  int padding = filterSize / 2;

  if (argc < 4) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name noise_free_input.raw noisy_input_image.raw output_image.raw filter [filter_size=3] [sigma=1.0]" << std::endl;
    return 0;
  }

  noiseFreeFileName = argv[1];
  fileName = argv[2];
  writeFileName = argv[3];
  std::string filterType = argv[4];

  if (argc > 5) {
    filterSize = atoi(argv[5]);

    if (filterSize % 2 == 0) {
      std::cout << "Value Error - Incorrect Filter Size" << std::endl;
      std::cout << "Please use odd value for filter size" << std::endl;
      return 0;
    }

    padding = filterSize / 2;
  }

  if (argc > 6) {
    sigma = atof(argv[6]);
  }

  ImageMatrix noiseFreeImageData = readFile(noiseFreeFileName, imageWidth, imageHeight);

  ImageMatrix imageData = readFile(fileName, imageWidth, imageHeight);

  if (filterType == "mean") {

    ImageMatrix mirroredImageData = getMirrorReflectedImage(imageData, imageWidth, imageHeight, padding);

    ImageMatrix denoisedImageData = lowPassFilter(mirroredImageData, imageWidth, imageHeight, filterSize, padding);

    writeFile(writeFileName, denoisedImageData, imageWidth, imageHeight);

    double mse = calculateMeanSquareError(noiseFreeImageData, denoisedImageData, imageWidth, imageHeight);

    double psnr = 10 * std::log10((255 * 255) / mse);

    std::cout << "PSNR: " << psnr << std::endl;

    return 0;
  }

  if (filterType == "gaussian") {
    ImageMatrix mirroredImageData = getMirrorReflectedImage(imageData, imageWidth, imageHeight, padding);

    ImageMatrix denoisedImageData = gaussianFilter(mirroredImageData, imageWidth, imageHeight, filterSize, padding, sigma);

    writeFile(writeFileName, denoisedImageData, imageWidth, imageHeight);

    double mse = calculateMeanSquareError(noiseFreeImageData, denoisedImageData, imageWidth, imageHeight);

    double psnr = 10 * std::log10((255 * 255) / mse);

    std::cout << "PSNR: " << psnr << std::endl;

    return 0;
  }

  std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
  std::cout << "program_name input_image.raw output_image.raw filter [filter_size=3] [sigma=1.0]" << std::endl;
  std::cout << "Incorrect filter type" << std::endl;
  std::cout << "Pleas use either lowpass or gaussian" << std::endl;

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

ImageMatrix lowPassFilter(ImageMatrix mirroredImageData, int width, int height, int filterSize, int padding) {

  ImageMatrix interpolatedImage(height, ImageVector(width));

  std::vector<std::vector<double> > filter(filterSize, std::vector<double>(filterSize, 1.0));

  int normalisingFactor = filterSize * filterSize;

  for (int i = padding; i <= height + (padding - 1); i++) {
    for (int j = padding; j <= width + (padding - 1); j++) {
      interpolatedImage[i - padding][j - padding] = convolutionHelper(mirroredImageData, filter, i, j, normalisingFactor);
    }
  }

  return interpolatedImage;
}

ImageMatrix gaussianFilter(ImageMatrix mirroredImageData, int width, int height, int filterSize, int padding, double sigma) {
  ImageMatrix interpolatedImage(height, ImageVector(width));

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
      interpolatedImage[i - padding][j - padding] = convolutionHelper(mirroredImageData, filter, i, j, normalisingFactor);
    }
  }

  return interpolatedImage;
}

double gaussianFunction(int x, int y, double sigma) {
  return std::exp(-(pow(x, 2) + std::pow(y, 2)) / (2 * (std::pow(sigma, 2)))) / (2 * M_PI * std::pow(sigma, 2));
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

int8 convolutionHelper(ImageMatrix& imageData, std::vector<std::vector<double> > imageFilter, int x_coord, int y_coord, int filterNormalisingFactor) {
  double finalIntensityValue = 0;

  int initialCoord = -(imageFilter.size() / 2);
  int filterLength = imageFilter.size();

  for (int i = 0, x = initialCoord; i < filterLength; i++, x++) {
    for (int j = 0, y = initialCoord; j < filterLength; j++, y++) {
      int imageX = x_coord + x;
      int imageY = y_coord + y;

      int8 colorValue = imageData[imageX][imageY];
      double filterValue = imageFilter[i][j];

      finalIntensityValue += colorValue * filterValue;
    }
  }

  finalIntensityValue = finalIntensityValue / filterNormalisingFactor;

  return channelRounding(finalIntensityValue);
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
