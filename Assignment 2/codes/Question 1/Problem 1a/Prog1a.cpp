/*
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Feb 19th 2024
*/

#include <iostream>
#include <vector>

typedef unsigned char int8;

class RGB
{
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
typedef std::pair<double, double> MinMaxPair;

ImageMatrixRGB readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrixDouble imageData, int width, int height);
int8 channelRounding(double value);
ImageMatrixDouble convertRGBtoGrayScale(ImageMatrixRGB& imageMatrix, int width, int height);
ImageMatrixDouble getMirrorReflectedImage(ImageMatrixDouble& imageData, int width, int height, int padding);
ImageMatrixDouble getSobelFilter(int size, std::string direction);
ImageMatrixDouble gradientConvolution(ImageMatrixDouble& imageData, ImageMatrixDouble& filter, int width, int height, int padding);
double convolutionHelper(ImageMatrixDouble& imageData, ImageMatrixDouble& filter, int i, int j, int padding);
ImageMatrixDouble getGradientMagnitude(ImageMatrixDouble& xGradientImageData, ImageMatrixDouble& yGradientImageData, int width, int height);
ImageMatrixDouble generateEdgeMap(ImageMatrixDouble& gradientImageData, int threshold, int width, int height);
MinMaxPair getMinMax(ImageMatrixDouble& imageData, int width, int height);
ImageMatrixDouble getNormalisedImageData(ImageMatrixDouble& imageData, MinMaxPair minmax, int width, int height);
int getThresholdIntensity(ImageMatrixDouble& imageData, double threshold, int width, int height);

int main(int argc, char* argv[]) {
  int imageWidth = 481;
  int imageHeight = 321;

  std::string fileName = "Tiger.raw";
  std::string writeFileName = "output1a.raw";

  int filterSize = 3;
  int padding = filterSize / 2;

  if (argc < 4) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name input_image.raw output_image.raw threshold[0-1]" << std::endl;
    return 0;
  }

  fileName = argv[1];
  writeFileName = argv[2];

  double threshold = atof(argv[3]);

  ImageMatrixRGB imageData = readFile(fileName, imageWidth, imageHeight);

  ImageMatrixDouble grayImageData = convertRGBtoGrayScale(imageData, imageWidth, imageHeight);

  ImageMatrixDouble mirrorGrayImageData = getMirrorReflectedImage(grayImageData, imageWidth, imageHeight, padding);

  ImageMatrixDouble xSobelFilter = getSobelFilter(3, "x");

  ImageMatrixDouble xGradientImageData = gradientConvolution(mirrorGrayImageData, xSobelFilter, imageWidth, imageHeight, padding);

  MinMaxPair xMinMax = getMinMax(xGradientImageData, imageWidth, imageHeight);

  ImageMatrixDouble xNormalisedGradientImageData = getNormalisedImageData(xGradientImageData, xMinMax, imageWidth, imageHeight);

  writeFile("x_gradient.raw", xNormalisedGradientImageData, imageWidth, imageHeight);

  ImageMatrixDouble ySobelFilter = getSobelFilter(3, "y");

  ImageMatrixDouble yGradientImageData = gradientConvolution(mirrorGrayImageData, ySobelFilter, imageWidth, imageHeight, padding);

  MinMaxPair yMinMax = getMinMax(yGradientImageData, imageWidth, imageHeight);

  ImageMatrixDouble yNormalisedGradientImageData = getNormalisedImageData(yGradientImageData, yMinMax, imageWidth, imageHeight);

  writeFile("y_gradient.raw", yNormalisedGradientImageData, imageWidth, imageHeight);

  ImageMatrixDouble magnitudeImageData = getGradientMagnitude(xGradientImageData, yGradientImageData, imageWidth, imageHeight);

  MinMaxPair magnitudeMinMax = getMinMax(magnitudeImageData, imageWidth, imageHeight);

  ImageMatrixDouble normalisedMagnitudeImageData = getNormalisedImageData(magnitudeImageData, magnitudeMinMax, imageWidth, imageHeight);

  writeFile("normalised_gradient.raw", normalisedMagnitudeImageData, imageWidth, imageHeight);

  int thresholdIntensity = getThresholdIntensity(normalisedMagnitudeImageData, threshold, imageWidth, imageHeight);

  ImageMatrixDouble thresholdImageData = generateEdgeMap(normalisedMagnitudeImageData, thresholdIntensity, imageWidth, imageHeight);

  writeFile(writeFileName, thresholdImageData, imageWidth, imageHeight);

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
      int8 data = channelRounding(imageData[i][j]);

      fwrite(&data, sizeof(int8), 1, outfile);
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

ImageMatrixDouble convertRGBtoGrayScale(ImageMatrixRGB& imageData, int width, int height) {
  ImageMatrixDouble grayImageData(height, ImageVectorDouble(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      RGB rgb = imageData[i][j];

      grayImageData[i][j] = 0.2989 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b;
    }
  }

  return grayImageData;
}

ImageMatrixDouble getMirrorReflectedImage(ImageMatrixDouble& imageData, int width, int height, int padding) {
  int mirrorImageWidth = width + padding * 2;
  int mirrorImageHeight = height + padding * 2;

  ImageMatrixDouble mirroredImageData(mirrorImageHeight, ImageVectorDouble(mirrorImageWidth));

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

ImageMatrixDouble getSobelFilter(int size, std::string direction) {
  ImageMatrixDouble filter;

  if (direction == "x") {
    ImageVectorDouble filterRow;

    filterRow.push_back(-1);
    filterRow.push_back(-0);
    filterRow.push_back(1);
    filter.push_back(filterRow);
    filterRow.clear();

    filterRow.push_back(-2);
    filterRow.push_back(0);
    filterRow.push_back(2);
    filter.push_back(filterRow);
    filterRow.clear();

    filterRow.push_back(-1);
    filterRow.push_back(-0);
    filterRow.push_back(1);
    filter.push_back(filterRow);
    filterRow.clear();

    return filter;
  }

  if (direction == "y") {
    ImageVectorDouble filterRow;

    filterRow.push_back(1);
    filterRow.push_back(2);
    filterRow.push_back(1);
    filter.push_back(filterRow);
    filterRow.clear();

    filterRow.push_back(0);
    filterRow.push_back(0);
    filterRow.push_back(0);
    filter.push_back(filterRow);
    filterRow.clear();

    filterRow.push_back(-1);
    filterRow.push_back(-2);
    filterRow.push_back(-1);
    filter.push_back(filterRow);
    filterRow.clear();

    return filter;
  }

  return ImageMatrixDouble();
}

ImageMatrixDouble gradientConvolution(ImageMatrixDouble& imageData, ImageMatrixDouble& filter, int width, int height, int padding) {
  ImageMatrixDouble gradientImageData(height, ImageVectorDouble(width));

  for (int i = padding; i <= height + (padding - 1); i++) {
    for (int j = padding; j <= width + (padding - 1); j++) {
      gradientImageData[i - padding][j - padding] = convolutionHelper(imageData, filter, i, j, padding);
    }
  }

  return gradientImageData;
}

double convolutionHelper(ImageMatrixDouble& imageData, ImageMatrixDouble& filter, int i, int j, int padding) {
  double sum = 0;

  for (int k = i - padding, filterX = 0; k <= i + padding; k++, filterX++) {
    for (int l = j - padding, filterY = 0; l <= j + padding; l++, filterY++) {
      double imageValue = imageData[k][l];
      double filterValue = (double)filter[filterX][filterY];

      sum += imageValue * filterValue;
    }
  }

  return sum;
}

ImageMatrixDouble getGradientMagnitude(ImageMatrixDouble& xGradientImageData, ImageMatrixDouble& yGradientImageData, int width, int height) {
  ImageMatrixDouble gradientImageData(height, ImageVectorDouble(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int gx = xGradientImageData[i][j];
      int gy = yGradientImageData[i][j];

      gradientImageData[i][j] = std::sqrt(((gx * gx) + (gy * gy)));
    }
  }

  return gradientImageData;
}

ImageMatrixDouble generateEdgeMap(ImageMatrixDouble& gradientImageData, int thresholdIntensity, int width, int height) {
  ImageMatrixDouble imageData(height, ImageVectorDouble(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      imageData[i][j] = gradientImageData[i][j] > thresholdIntensity ? 255 : 0;
    }
  }

  return imageData;
}

MinMaxPair getMinMax(ImageMatrixDouble& imageData, int width, int height) {
  MinMaxPair minmax(imageData[0][0], imageData[0][0]);

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (imageData[i][j] < minmax.first) {
        minmax.first = imageData[i][j];
      }

      if (imageData[i][j] > minmax.second) {
        minmax.second = imageData[i][j];
      }
    }
  }

  return minmax;
}

ImageMatrixDouble getNormalisedImageData(ImageMatrixDouble& imageData, MinMaxPair minmax, int width, int height) {
  ImageMatrixDouble normalisedImageData(height, ImageVectorDouble(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      normalisedImageData[i][j] = ((imageData[i][j] - minmax.first) / (minmax.second - minmax.first)) * 255;
    }
  }

  return normalisedImageData;
}

int getThresholdIntensity(ImageMatrixDouble& imageData, double threshold, int width, int height) {
  std::vector<int> pixelHistogram(256);

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int pixelIntensity = channelRounding(imageData[i][j]);

      pixelHistogram[pixelIntensity]++;
    }
  }

  double imageSize = width * height * 1.0;

  std::vector<double> normalisedPixelHistogram(256);

  for (int i = 0; i < pixelHistogram.size(); i++) {
    normalisedPixelHistogram[i] = pixelHistogram[i] / imageSize;
  }

  std::vector<double> cdfHistogram(256);

  cdfHistogram[0] = normalisedPixelHistogram[0];

  for (int i = 1; i < cdfHistogram.size(); i++) {
    cdfHistogram[i] = cdfHistogram[i - 1] + normalisedPixelHistogram[i];
  }

  for (int i = 0; i < cdfHistogram.size(); i++) {
    std::cout << cdfHistogram[i] << std::endl;
  }

  for (int i = 0; i < cdfHistogram.size(); i++) {
    if (cdfHistogram[i] >= threshold) {
      return i;
    }
  }

  return 0;
}
