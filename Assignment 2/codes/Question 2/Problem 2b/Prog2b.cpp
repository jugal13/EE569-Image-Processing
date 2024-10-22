/*
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Feb 19th 2024
*/

#include <iostream>
#include <vector>

typedef unsigned char int8;
typedef std::vector<int8> ImageVector;
typedef std::vector<std::vector<int8> > ImageMatrix;
typedef std::vector<double> ImageVectorDouble;
typedef std::vector<std::vector<double> > ImageMatrixDouble;

ImageMatrix readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrixDouble& imageData, int width, int height);
ImageMatrixDouble copyImageMatrixDouble(ImageMatrix& imageData, int width, int height);
ImageMatrixDouble getMirrorReflectedImage(ImageMatrixDouble& imageData, int width, int height, int padding);
ImageMatrixDouble generateDiffusionMatrix(std::string method, int normalisingFactor);
int channelRounding(double value);
ImageMatrixDouble generateDitheredImage(ImageMatrixDouble precisionImageData, ImageMatrixDouble& errorMatrix, int width, int height, int filterSize, int padding, int threshold);

int main(int argc, char* argv[]) {
  int imageWidth = 750;
  int imageHeight = 500;

  std::string fileName = "Lighthouse.raw";
  std::string writeFileName = "Fixed.raw";

  if (argc < 4) {
    std::cout << "Syntax Error - Incorrect Parameter Usage:" << std::endl;
    std::cout << "program_name input_image.raw output_image.raw method" << std::endl;
    return 0;
  }

  fileName = argv[1];
  writeFileName = argv[2];
  std::string method = argv[3];

  ImageMatrix imageData = readFile(fileName, imageWidth, imageHeight);

  ImageMatrixDouble precisionImageData = copyImageMatrixDouble(imageData, imageWidth, imageHeight);

  int normalisingFactor;
  ImageMatrixDouble diffusionMatrix;
  int filterSize;
  int padding;
  int threshold = 127;

  if (method == "fs") {
    normalisingFactor = 16;
    diffusionMatrix = generateDiffusionMatrix(method, normalisingFactor);
    filterSize = 3;
    padding = filterSize / 2;

    ImageMatrixDouble paddedImageData = getMirrorReflectedImage(precisionImageData, imageWidth, imageHeight, padding);

    ImageMatrixDouble ditheredImageData = generateDitheredImage(paddedImageData, diffusionMatrix, imageWidth, imageHeight, filterSize, padding, threshold);

    writeFile(writeFileName, ditheredImageData, imageWidth, imageHeight);

    return 0;
  }

  if (method == "jjn") {
    normalisingFactor = 48;
    diffusionMatrix = generateDiffusionMatrix(method, normalisingFactor);
    filterSize = 5;
    padding = filterSize / 2;

    ImageMatrixDouble paddedImageData = getMirrorReflectedImage(precisionImageData, imageWidth, imageHeight, padding);

    ImageMatrixDouble ditheredImageData = generateDitheredImage(paddedImageData, diffusionMatrix, imageWidth, imageHeight, filterSize, padding, threshold);

    writeFile(writeFileName, ditheredImageData, imageWidth, imageHeight);

    return 0;
  }

  if (method == "stucki") {
    normalisingFactor = 42;
    diffusionMatrix = generateDiffusionMatrix(method, normalisingFactor);
    filterSize = 5;
    padding = filterSize / 2;

    ImageMatrixDouble paddedImageData = getMirrorReflectedImage(precisionImageData, imageWidth, imageHeight, padding);

    ImageMatrixDouble ditheredImageData = generateDitheredImage(paddedImageData, diffusionMatrix, imageWidth, imageHeight, filterSize, padding, threshold);

    writeFile(writeFileName, ditheredImageData, imageWidth, imageHeight);

    return 0;
  }

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

void writeFile(std::string fileName, ImageMatrixDouble& imageData, int width, int height) {
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

ImageMatrixDouble copyImageMatrixDouble(ImageMatrix& imageData, int width, int height) {
  ImageMatrixDouble precisionImageData(height, ImageVectorDouble(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      precisionImageData[i][j] = (double)imageData[i][j];
    }
  }

  return precisionImageData;
}

ImageMatrixDouble getMirrorReflectedImage(ImageMatrixDouble& imageData, int width, int height, int padding) {
  int mirrorImageWidth = width + padding * 2;
  int mirrorImageHeight = height + padding * 2;

  ImageMatrixDouble mirroredImageData(mirrorImageHeight, ImageVectorDouble(mirrorImageWidth));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      mirroredImageData[i + padding][j + padding] = (double)imageData[i][j];
    }
  }

  for (int j = 0; j < width; j++) {
    for (int k = 0; k < padding; k++) {
      mirroredImageData[k][j + padding] = (double)imageData[padding - k][j];

      mirroredImageData[mirrorImageHeight - (k + 1)][j + padding] = (double)imageData[height - (padding + 1 - k)][j];
    }
  }

  for (int i = 0; i < mirrorImageHeight; i++) {
    for (int k = 0; k < padding; k++) {
      mirroredImageData[i][k] = (double)mirroredImageData[i][padding * 2 - k];

      mirroredImageData[i][mirrorImageWidth - (k + 1)] = (double)mirroredImageData[i][mirrorImageWidth - (padding * 2 + 1 - k)];
    }
  }

  return mirroredImageData;
}

ImageMatrixDouble generateDiffusionMatrix(std::string method, int normalisingFactor) {
  ImageMatrixDouble diffusionMatrix;
  ImageVectorDouble row;

  if (method == "fs") {
    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);

    diffusionMatrix.push_back(row);
    row.clear();

    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(7.0 / normalisingFactor);
    diffusionMatrix.push_back(row);
    row.clear();

    row.push_back(3.0 / normalisingFactor);
    row.push_back(5.0 / normalisingFactor);
    row.push_back(1.0 / normalisingFactor);
    diffusionMatrix.push_back(row);

    return diffusionMatrix;
  }

  if (method == "jjn") {
    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);

    diffusionMatrix.push_back(row);
    row.clear();

    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);

    diffusionMatrix.push_back(row);
    row.clear();

    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(7.0 / normalisingFactor);
    row.push_back(5.0 / normalisingFactor);

    diffusionMatrix.push_back(row);
    row.clear();

    row.push_back(3.0 / normalisingFactor);
    row.push_back(5.0 / normalisingFactor);
    row.push_back(7.0 / normalisingFactor);
    row.push_back(5.0 / normalisingFactor);
    row.push_back(3.0 / normalisingFactor);

    diffusionMatrix.push_back(row);
    row.clear();

    row.push_back(1.0 / normalisingFactor);
    row.push_back(3.0 / normalisingFactor);
    row.push_back(5.0 / normalisingFactor);
    row.push_back(3.0 / normalisingFactor);
    row.push_back(1.0 / normalisingFactor);

    diffusionMatrix.push_back(row);
    row.clear();

    return diffusionMatrix;
  }

  if (method == "stucki") {
    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);

    diffusionMatrix.push_back(row);
    row.clear();

    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);

    diffusionMatrix.push_back(row);
    row.clear();

    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(0.0);
    row.push_back(8.0 / normalisingFactor);
    row.push_back(4.0 / normalisingFactor);

    diffusionMatrix.push_back(row);
    row.clear();

    row.push_back(2.0 / normalisingFactor);
    row.push_back(4.0 / normalisingFactor);
    row.push_back(8.0 / normalisingFactor);
    row.push_back(4.0 / normalisingFactor);
    row.push_back(2.0 / normalisingFactor);

    diffusionMatrix.push_back(row);
    row.clear();

    row.push_back(1.0 / normalisingFactor);
    row.push_back(2.0 / normalisingFactor);
    row.push_back(4.0 / normalisingFactor);
    row.push_back(2.0 / normalisingFactor);
    row.push_back(1.0 / normalisingFactor);

    diffusionMatrix.push_back(row);
    row.clear();

    return diffusionMatrix;
  }

  return diffusionMatrix;
}

int channelRounding(double value) {
  int channelValue = (int)value;

  if (channelValue > 255) {
    return 255;
  }

  if (channelValue < 0) {
    return 0;
  }

  return (int8)channelValue;
}

ImageMatrixDouble generateDitheredImage(ImageMatrixDouble precisionImageData, ImageMatrixDouble& errorMatrix, int width, int height, int filterSize, int padding, int threshold) {
  ImageMatrixDouble ditheredImage(height, ImageVectorDouble(width));

  for (int i = padding; i <= height + (padding - 1); i += 2) {
    int row = i;

    for (int j = padding; j <= width + (padding - 1); j++) {
      double preciseData = precisionImageData[row][j];
      double binarisedData = preciseData > threshold ? 255.0 : 0.0;
      double error = preciseData - binarisedData;
      ditheredImage[row - padding][j - padding] = binarisedData;

      for (int k = row - padding, filterX = 0; k <= row + padding; k++, filterX++) {
        for (int l = j - padding, filterY = 0; l <= j + padding; l++, filterY++) {
          precisionImageData[k][l] += errorMatrix[filterX][filterY] * error;
        }
      }
    }

    row = i + 1;
    for (int j = width + (padding - 1); j >= padding; j--) {
      double preciseData = precisionImageData[i + 1][j];
      double binarisedData = preciseData > threshold ? 255.0 : 0.0;
      double error = preciseData - binarisedData;
      ditheredImage[row - padding][j - padding] = binarisedData;

      for (int k = row - padding, filterX = 0; k <= row + padding; k++, filterX++) {
        for (int l = j - padding, filterY = filterSize - 1; l <= j + padding; l++, filterY--) {
          precisionImageData[k][l] += errorMatrix[filterX][filterY] * error;
        }
      }
    }
  }

  return ditheredImage;
}
