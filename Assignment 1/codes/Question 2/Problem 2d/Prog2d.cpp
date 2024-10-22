/*
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Jan 28th 2024
*/

#include <iostream>
#include <fstream>
#include <vector>

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

  int8 getColor(std::string colorName) {
    if (colorName == "r") {
      return r;
    }

    if (colorName == "g") {
      return g;
    }

    if (colorName == "b") {
      return b;
    }

    return 0;
  }
};

typedef std::vector<RGB> ImageVectorRGB;
typedef std::vector<std::vector<RGB> > ImageMatrixRGB;

ImageMatrixRGB readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrixRGB imageData, int width, int height);
std::vector<int> calculateHistogram(ImageMatrixRGB& imageData, int width, int height, std::string color);
void writeFileSingleChannel(std::string fileName, ImageMatrixRGB imageData, int width, int height, int channel);
int8 channelRounding(double value);
ImageMatrixRGB getMirrorReflectedImage(ImageMatrixRGB imageData, int width, int height, int padding);
RGB medianHelper(ImageMatrixRGB& mirroredImageData, int filterSize, int i, int j, int padding);
ImageMatrixRGB medianFilter(ImageMatrixRGB& mirroredImageData, int width, int height, int filterSize, int padding);
ImageMatrixRGB gaussianFilter(ImageMatrixRGB mirroredImageData, int width, int height, int filterSize, int padding, double sigma);
double gaussianFunction(int x, int y, double sigma);
int8 convolutionHelper(ImageMatrixRGB& imageData, std::vector<std::vector<double> > imageFilter, int x_coord, int y_coord, std::string color);
double calculateMeanSquareError(ImageMatrixRGB noiseFreeImageData, ImageMatrixRGB filteredimageData, int width, int height);

int main(int argc, char* argv[]) {
  int imageWidth = 768;
  int imageHeight = 512;

  std::string noiseFreeFileName = "Flower.raw";
  std::string fileName = "Flower_noisy.raw";
  std::string writeFileName = "output2c.raw";

  int filterSize = 3;
  double sigma = 1.0;
  int padding = filterSize / 2;

  if (argc < 4) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name noise_free_image.raw noisy_input_image.raw output_image.raw [filter_size=3] [sigma=1.0]" << std::endl;
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
    sigma = atof(argv[5]);
  }

  ImageMatrixRGB noiseFreeImageData = readFile(noiseFreeFileName, imageWidth, imageHeight);

  ImageMatrixRGB imageData = readFile(fileName, imageWidth, imageHeight);

  std::vector<int> redHistogram = calculateHistogram(imageData, imageWidth, imageHeight, "r");
  std::vector<int> greenHistogram = calculateHistogram(imageData, imageWidth, imageHeight, "g");
  std::vector<int> blueHistogram = calculateHistogram(imageData, imageWidth, imageHeight, "b");

  std::ofstream outfile1("red_histogram");
  std::ofstream outfile2("green_histogram");
  std::ofstream outfile3("blue_histogram");

  for (int i = 0; i < redHistogram.size(); i++) {
    outfile1 << redHistogram[i] << std::endl;
    outfile2 << greenHistogram[i] << std::endl;
    outfile3 << blueHistogram[i] << std::endl;
  }

  outfile1.close();
  outfile2.close();
  outfile3.close();

  writeFileSingleChannel("red_channel.raw", imageData, imageWidth, imageHeight, 0);
  writeFileSingleChannel("green_channel.raw", imageData, imageWidth, imageHeight, 1);
  writeFileSingleChannel("blue_channel.raw", imageData, imageWidth, imageHeight, 2);

  ImageMatrixRGB mirroredImageData = getMirrorReflectedImage(imageData, imageWidth, imageHeight, padding);

  ImageMatrixRGB denoisedImage = medianFilter(mirroredImageData, imageWidth, imageHeight, filterSize, padding);

  mirroredImageData = getMirrorReflectedImage(denoisedImage, imageWidth, imageHeight, padding);

  denoisedImage = gaussianFilter(mirroredImageData, imageWidth, imageHeight, filterSize, padding, sigma);

  writeFile(writeFileName, denoisedImage, imageWidth, imageHeight);

  double mse = calculateMeanSquareError(noiseFreeImageData, denoisedImage, imageWidth, imageHeight);

  double psnr = 10 * std::log10((255 * 255) / mse);

  std::cout << "PSNR: " << psnr << std::endl;

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
        int8 bits = data[k];

        fwrite(&bits, sizeof(int8), 1, outfile);
      }
    }
  }

  fclose(outfile);
}

void writeFileSingleChannel(std::string fileName, ImageMatrixRGB imageData, int width, int height, int channel) {
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
        int8 bits = k == channel ? data[k] : 0;

        fwrite(&bits, sizeof(int8), 1, outfile);
      }
    }
  }

  fclose(outfile);
}

std::vector<int> calculateHistogram(ImageMatrixRGB& imageData, int width, int height, std::string color) {
  std::vector<int> pixelHistogram(256, 0);
  int imageSize = width * height;

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      pixelHistogram[imageData[i][j].getColor(color)] += 1;
    }
  }

  return pixelHistogram;
}

ImageMatrixRGB getMirrorReflectedImage(ImageMatrixRGB imageData, int width, int height, int padding) {
  int mirrorImageWidth = width + padding * 2;
  int mirrorImageHeight = height + padding * 2;

  ImageMatrixRGB mirroredImageData(mirrorImageHeight, ImageVectorRGB(mirrorImageWidth));

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

ImageMatrixRGB medianFilter(ImageMatrixRGB& mirroredImageData, int width, int height, int filterSize, int padding) {
  ImageMatrixRGB filteredImageData(height, ImageVectorRGB(width));

  for (int i = padding; i <= height + (padding - 1); i++) {
    for (int j = padding; j <= width + (padding - 1); j++) {
      filteredImageData[i - padding][j - padding] = medianHelper(mirroredImageData, filterSize, i, j, padding);
    }
  }

  return filteredImageData;
}

RGB medianHelper(ImageMatrixRGB& mirroredImageData, int filterSize, int i, int j, int padding) {
  std::vector<int8> redVector;
  std::vector<int8> greenVector;
  std::vector<int8> blueVector;

  for (int k = i - padding; k <= i + padding; k++) {
    for (int l = j - padding; l <= j + padding; l++) {
      redVector.push_back(mirroredImageData[k][l].r);
      greenVector.push_back(mirroredImageData[k][l].g);
      blueVector.push_back(mirroredImageData[k][l].b);
    }
  }

  std::sort(redVector.begin(), redVector.end());
  std::sort(greenVector.begin(), greenVector.end());
  std::sort(blueVector.begin(), blueVector.end());

  int center = (filterSize * filterSize) / 2;

  int8 red = redVector[center];
  int8 green = greenVector[center];
  int8 blue = blueVector[center];

  return RGB(red, green, blue);
}

ImageMatrixRGB gaussianFilter(ImageMatrixRGB mirroredImageData, int width, int height, int filterSize, int padding, double sigma) {
  ImageMatrixRGB interpolatedImage(height, ImageVectorRGB(width));

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
      int8 r = convolutionHelper(mirroredImageData, filter, i, j, "r");
      int8 g = convolutionHelper(mirroredImageData, filter, i, j, "g");
      int8 b = convolutionHelper(mirroredImageData, filter, i, j, "b");

      interpolatedImage[i - padding][j - padding] = RGB(r, g, b);
    }
  }

  return interpolatedImage;
}

double gaussianFunction(int x, int y, double sigma) {
  return std::exp(-(pow(x, 2) + std::pow(y, 2)) / (2 * (std::pow(sigma, 2)))) / (2 * M_PI * std::pow(sigma, 2));
}

int8 convolutionHelper(ImageMatrixRGB& imageData, std::vector<std::vector<double> > imageFilter, int x_coord, int y_coord, std::string color) {
  double finalIntensityValue = 0;

  int initialCoord = -(imageFilter.size() / 2);
  int filterLength = imageFilter.size();

  for (int i = 0, x = initialCoord; i < filterLength; i++, x++) {
    for (int j = 0, y = initialCoord; j < filterLength; j++, y++) {
      int imageX = x_coord + x;
      int imageY = y_coord + y;

      int8 colorValue = imageData[imageX][imageY].getColor(color);
      double filterValue = imageFilter[i][j];

      finalIntensityValue += colorValue * filterValue;
    }
  }

  return channelRounding(finalIntensityValue);
}

double calculateMeanSquareError(ImageMatrixRGB noiseFreeImageData, ImageMatrixRGB filteredimageData, int width, int height) {
  double total = 0.0;

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      double diff_r = noiseFreeImageData[i][j].r - filteredimageData[i][j].r;
      double diff_sq_r = diff_r * diff_r;

      total += diff_sq_r;

      double diff_g = noiseFreeImageData[i][j].g - filteredimageData[i][j].g;
      double diff_sq_g = diff_g * diff_g;

      total += diff_sq_g;

      double diff_b = noiseFreeImageData[i][j].b - filteredimageData[i][j].b;
      double diff_sq_b = diff_b * diff_b;

      total += diff_sq_b;
    }
  }

  double mse = total / (width * height * 3);

  return mse;
}
