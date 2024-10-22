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
int8 channelRounding(double value);
ImageMatrixRGB getMirrorReflectedImage(ImageMatrixRGB imageData, int width, int height, int padding);
RGB medianHelper(ImageMatrixRGB& mirroredImageData, int filterSize, int i, int j, int padding);
ImageMatrixRGB medianFilter(ImageMatrixRGB& mirroredImageData, int width, int height, int filterSize, int padding);
ImageMatrixRGB gaussianFilter(ImageMatrixRGB mirroredImageData, int width, int height, int filterSize, int padding, double sigma);
double gaussianFunction(int x, int y, double sigma);
int8 convolutionHelper(ImageMatrixRGB& imageData, std::vector<std::vector<double> > imageFilter, int x_coord, int y_coord, std::string color);
ImageMatrixRGB bilateralFilter(ImageMatrixRGB mirroredImageData, int width, int height, int filterSize, int padding, double sigmaC, double sigmaS);
RGB bilateralConvolutionHelper(ImageMatrixRGB& imageData, int filterSize, int k, int l, double sigmaC, double sigmaS);
double getBilateralIntensityWeight(int k, int l, int i, int j, int sigmaC, int8 pixelIntensityAtKL, int8 pixelIntensityAtIJ, int sigmaS);
ImageMatrixRGB linearCombination(ImageMatrixRGB imageData1, ImageMatrixRGB imageData2, double scaleFactor1, double scaleFactor2, int width, int height);

int main(int argc, char* argv[]) {
  int imageWidth = 768;
  int imageHeight = 512;

  std::string fileName = "Flower.raw";
  std::string writeFileName = "output3.raw";

  int medianFilterSize = 3;
  int K = 5;

  int bilateralFilterSize = 5;
  int sigmaC = 20;
  int sigmaS = 10;

  int gaussianFilterSize = 7;
  int gaussianSigma = 2;

  if (argc < 3) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name input_image.raw output_image.raw [median_filter_size=3] [K=5]" << std::endl;
    return 0;
  }

  fileName = argv[1];
  writeFileName = argv[2];

  if (argc > 3) {
    medianFilterSize = atoi(argv[3]);

    if (medianFilterSize % 2 == 0) {
      std::cout << "Value Error - Incorrect Filter Size" << std::endl;
      std::cout << "Please use odd value for filter size" << std::endl;
      return 0;
    }
  }

  if (argc > 4) {
    K = atoi(argv[4]);
  }

  std::cout << "Read File" << std::endl;
  ImageMatrixRGB imageData = readFile(fileName, imageWidth, imageHeight);

  std::cout << "Mirror Pad Image" << std::endl;
  ImageMatrixRGB leftMirroredImageData = getMirrorReflectedImage(imageData, imageWidth, imageHeight, medianFilterSize / 2);

  std::cout << "Median Filter" << std::endl;
  ImageMatrixRGB leftFilteredImageData = medianFilter(leftMirroredImageData, imageWidth, imageHeight, medianFilterSize, medianFilterSize / 2);

  std::cout << "Write Median Filtered Image" << std::endl;
  std::string medianFileName = "Median" + std::to_string(medianFilterSize) + ".raw";
  writeFile(medianFileName, leftFilteredImageData, imageWidth, imageHeight);

  for (int i = 0; i < K; i++) {
    std::cout << "Mirror Image: " << i + 1 << std::endl;
    leftMirroredImageData = getMirrorReflectedImage(leftFilteredImageData, imageWidth, imageHeight, bilateralFilterSize / 2);

    std::cout << "Bilateral Filter Image: " << i + 1 << std::endl;
    leftFilteredImageData = bilateralFilter(leftMirroredImageData, imageWidth, imageHeight, bilateralFilterSize, bilateralFilterSize / 2, sigmaC, sigmaS);
  }

  std::cout << "Mirror Pad Image" << std::endl;
  ImageMatrixRGB rightMirroredImageData = getMirrorReflectedImage(imageData, imageWidth, imageHeight, gaussianFilterSize / 2);

  std::cout << "Gaussian Filter" << std::endl;
  ImageMatrixRGB rightFilteredImageData = gaussianFilter(rightMirroredImageData, imageWidth, imageHeight, gaussianFilterSize, gaussianFilterSize / 2, gaussianSigma);

  std::cout << "Linear Combination" << std::endl;
  ImageMatrixRGB paintedImageData = linearCombination(leftFilteredImageData, rightFilteredImageData, 1.4, 0.4, imageWidth, imageHeight);

  std::cout << "Write File" << std::endl;
  writeFile(writeFileName, paintedImageData, imageWidth, imageHeight);

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

ImageMatrixRGB bilateralFilter(ImageMatrixRGB mirroredImageData, int width, int height, int filterSize, int padding, double sigmaC, double sigmaS) {
  ImageMatrixRGB filteredImageData(height, ImageVectorRGB(width));

  for (int i = padding; i <= height + (padding - 1); i++) {
    for (int j = padding; j <= width + (padding - 1); j++) {
      filteredImageData[i - padding][j - padding] = bilateralConvolutionHelper(mirroredImageData, filterSize, i, j, sigmaC, sigmaS);
    }
  }

  return filteredImageData;
}

RGB bilateralConvolutionHelper(ImageMatrixRGB& imageData, int filterSize, int i, int j, double sigmaC, double sigmaS) {
  int8 redIntensityValue;
  int8 greenIntensityValue;
  int8 blueIntensityValue;

  int coordPadding = (filterSize / 2);

  double red_num_weight = 0.0;
  double red_den_weight = 0.0;

  double green_num_weight = 0.0;
  double green_den_weight = 0.0;

  double blue_num_weight = 0.0;
  double blue_den_weight = 0.0;

  for (int k = i - coordPadding; k <= i + coordPadding; k++) {
    for (int l = j - coordPadding; l <= j + coordPadding; l++) {
      double red_curr_weight = getBilateralIntensityWeight(k, l, i, j, sigmaC, imageData[k][l].r, imageData[i][j].r, sigmaS);
      red_num_weight += imageData[k][l].r * red_curr_weight;
      red_den_weight += red_curr_weight;

      double green_curr_weight = getBilateralIntensityWeight(k, l, i, j, sigmaC, imageData[k][l].g, imageData[i][j].g, sigmaS);
      green_num_weight += imageData[k][l].g * green_curr_weight;
      green_den_weight += green_curr_weight;

      double blue_curr_weight = getBilateralIntensityWeight(k, l, i, j, sigmaC, imageData[k][l].b, imageData[i][j].b, sigmaS);
      blue_num_weight += imageData[k][l].b * blue_curr_weight;
      blue_den_weight += blue_curr_weight;
    }
  }

  double red_weight = red_num_weight / red_den_weight;
  double green_weight = green_num_weight / green_den_weight;
  double blue_weight = blue_num_weight / blue_den_weight;

  redIntensityValue = channelRounding(red_weight);
  greenIntensityValue = channelRounding(green_weight);
  blueIntensityValue = channelRounding(blue_weight);

  return RGB(redIntensityValue, greenIntensityValue, blueIntensityValue);
}

double getBilateralIntensityWeight(int k, int l, int i, int j, int sigmaC, int8 pixelIntensityAtKL, int8 pixelIntensityAtIJ, int sigmaS) {
  double left_num = (k - i) * (k - i) + (l - j) * (l - j);
  double left_den = 2 * sigmaC * sigmaC;

  double left_frac = left_num / left_den;

  double right_num_root = pixelIntensityAtKL - pixelIntensityAtIJ;
  double right_num = right_num_root * right_num_root;
  double right_den = 2 * sigmaS * sigmaS;

  double right_frac = right_num / right_den;

  double exponent = -left_frac - right_frac;
  double curr_weight = std::exp(exponent);

  return curr_weight;
}

ImageMatrixRGB linearCombination(ImageMatrixRGB imageData1, ImageMatrixRGB imageData2, double scaleFactor1, double scaleFactor2, int width, int height) {
  ImageMatrixRGB linearCombinedImageData(height, ImageVectorRGB(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int8 red = channelRounding(scaleFactor1 * imageData1[i][j].r - scaleFactor2 * imageData2[i][j].r);
      int8 green = channelRounding(scaleFactor1 * imageData1[i][j].g - scaleFactor2 * imageData2[i][j].g);
      int8 blue = channelRounding(scaleFactor1 * imageData1[i][j].b - scaleFactor2 * imageData2[i][j].b);

      linearCombinedImageData[i][j] = RGB(red, green, blue);
    }
  }

  return linearCombinedImageData;
}
