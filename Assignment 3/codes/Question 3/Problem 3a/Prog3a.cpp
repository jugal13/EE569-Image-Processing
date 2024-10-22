/*
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Mar 10th 2024
*/

#include <iostream>
#include <vector>

typedef unsigned char int8;

typedef std::vector<int8> ImageVector;
typedef std::vector<std::vector<int8> > ImageMatrix;

ImageMatrix readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrix imageData, int width, int height);
int findMaxIntensity(ImageMatrix imageData, int width, int height);
ImageMatrix binariseImageData(ImageMatrix imageData, int width, int height, int threshold);
ImageMatrix normaliseImageData(ImageMatrix imageData, int width, int height);
ImageMatrix denormaliseImageData(ImageMatrix imageData, int width, int height);
ImageMatrix zeroPadding(ImageMatrix imageData, int width, int height, int padding);
ImageMatrix morphologicalThinning(ImageMatrix paddedImageData, int width, int height, int padding, std::string fileName);
bool checkConditionalMarkPattern(ImageMatrix& imageData, int i, int j);
bool checkUnconditionalMarkPattern(ImageMatrix& imageData, int i, int j);
ImageMatrix getMatrix(ImageMatrix paddedImageData, int width, int height, int padding);

int main(int argc, char* argv[]) {
  int imageWidth = 252;
  int imageHeight = 252;

  std::string fileName = "spring.raw";
  std::string writeFileName = "output3.raw";

  if (argc < 5) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name imageWidth imageHeight input.raw output.raw" << std::endl;
    return 0;
  }

  imageWidth = atoi(argv[1]);
  imageHeight = atoi(argv[2]);
  fileName = argv[3];
  writeFileName = argv[4];
  int padding = 1;

  ImageMatrix imageData = readFile(fileName, imageWidth, imageHeight);

  int maxIntensity = findMaxIntensity(imageData, imageWidth, imageHeight);

  ImageMatrix binarisedImageData = binariseImageData(imageData, imageWidth, imageHeight, maxIntensity * 0.5);

  ImageMatrix normalisedImageData = normaliseImageData(binarisedImageData, imageWidth, imageHeight);

  ImageMatrix paddedImageData = zeroPadding(normalisedImageData, imageWidth, imageHeight, padding);

  ImageMatrix thinnedImageData = morphologicalThinning(paddedImageData, imageWidth, imageHeight, padding, writeFileName);

  ImageMatrix denormalisedImageData = denormaliseImageData(thinnedImageData, imageWidth, imageHeight);

  writeFile(writeFileName, denormalisedImageData, imageWidth, imageHeight);

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

      fread(&data[0], sizeof(int8), 1, infile);

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
      int8 bits = imageData[i][j];

      fwrite(&bits, sizeof(int8), 1, outfile);
    }
  }

  fclose(outfile);
}

int findMaxIntensity(ImageMatrix imageData, int width, int height) {
  int maxIntensity = imageData[0][0];

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (imageData[i][j] > maxIntensity) {
        maxIntensity = imageData[i][j];
      }
    }
  }

  return maxIntensity;
}

ImageMatrix binariseImageData(ImageMatrix imageData, int width, int height, int threshold) {
  ImageMatrix binarisedImageData(height, ImageVector(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      binarisedImageData[i][j] = imageData[i][j] > threshold ? 255 : 0;
    }
  }

  return binarisedImageData;
}

ImageMatrix normaliseImageData(ImageMatrix imageData, int width, int height) {
  ImageMatrix normalisedImageData(height, ImageVector(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      normalisedImageData[i][j] = imageData[i][j] / 255;
    }
  }

  return normalisedImageData;
}

ImageMatrix denormaliseImageData(ImageMatrix imageData, int width, int height) {
  ImageMatrix normalisedImageData(height, ImageVector(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      normalisedImageData[i][j] = imageData[i][j] * 255;
    }
  }

  return normalisedImageData;
}

ImageMatrix zeroPadding(ImageMatrix imageData, int width, int height, int padding) {
  int paddedHeight = height + 2 * padding;
  int paddedWidth = width + 2 * padding;

  ImageMatrix paddedImageData(paddedHeight, ImageVector(paddedWidth));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      paddedImageData[i + padding][j + padding] = imageData[i][j];
    }
  }

  for (int j = 0; j < width; j++) {
    for (int k = 0; k < padding; k++) {
      paddedImageData[k][j + padding] = 0;

      paddedImageData[paddedHeight - (k + 1)][j + padding] = 0;
    }
  }

  for (int i = 0; i < paddedHeight; i++) {
    for (int k = 0; k < padding; k++) {
      paddedImageData[i][k] = 0;

      paddedImageData[i][paddedWidth - (k + 1)] = 0;
    }
  }

  return paddedImageData;
}

ImageMatrix morphologicalThinning(ImageMatrix paddedImageData, int width, int height, int padding, std::string fileName) {
  ImageMatrix imageData = paddedImageData;

  int paddedHeight = height + 2 * padding;
  int paddedWidth = width + 2 * padding;

  bool changed = true;
  int count = 0;

  while (changed) {
    count++;
    changed = false;

    std::cout << "Iteration: " << count << std::endl;
    // if (count > 1) {
      // return getMatrix(imageData, width, height, padding);
    // }

    ImageMatrix intermediateData(paddedHeight, ImageVector(paddedWidth, 0));

    // First pass
    for (int i = padding; i < height + padding; i++) {
      for (int j = padding; j < width + padding; j++) {
        if (checkConditionalMarkPattern(imageData, i, j)) {
          intermediateData[i][j] = 1;
          changed = true;
        }
      }
    }

    // Second pass
    ImageMatrix outputData(paddedHeight, ImageVector(paddedWidth));
    for (int i = padding; i < height + padding; i++) {
      for (int j = padding; j < width + padding; j++) {
        if (intermediateData[i][j]) {
          bool match = checkUnconditionalMarkPattern(intermediateData, i, j);
          if (match) {
            outputData[i][j] = imageData[i][j];
          }
          else {
            outputData[i][j] = 0;
            changed = true;
          }
        }
        else {
          outputData[i][j] = imageData[i][j];
        }
      }
    }

    if (count % 5 == 0) {
      std::string outfileName = fileName.substr(0, fileName.size() - 4);
      ImageMatrix writeThinned = denormaliseImageData(getMatrix(outputData, width, height, padding), width, height);
      std::string outfileT = "Thinned_" + outfileName + "_" + std::to_string(count) + ".raw";
      writeFile(outfileT, writeThinned, width, height);
    }

    imageData = outputData;
  }

  return getMatrix(imageData, width, height, padding);
}

ImageMatrix getMatrix(ImageMatrix paddedImageData, int width, int height, int padding) {
  ImageMatrix imageData(height, ImageVector(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      imageData[i][j] = paddedImageData[i + padding][j + padding];
    }
  }

  return imageData;
}

bool checkConditionalMarkPattern(ImageMatrix& imageData, int i, int j) {
  if (
    !imageData[i - 1][j - 1] && imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && !imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && !imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && !imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && !imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && !imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && !imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && !imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && !imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && !imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && !imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && !imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && !imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && !imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && !imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && !imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && !imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] && !imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && !imageData[i - 1][j] && !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] && !imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && !imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && !imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] && !imageData[i - 1][j] && imageData[i - 1][j + 1] &&
    imageData[i][j - 1] && imageData[i][j] && imageData[i][j + 1] &&
    imageData[i + 1][j - 1] && imageData[i + 1][j] && imageData[i + 1][j + 1]
    ) {
    return true;
  }

  return false;
}

bool checkUnconditionalMarkPattern(ImageMatrix& imageData, int i, int j) {
  std::vector<ImageMatrix> patterns = {
    {
      { 0, 0, 1 },
      { 0, 1, 0 },
      { 0, 0, 0 }
    },
    {
      { 1, 0, 0 },
      { 0, 1, 0 },
      { 0, 0, 0 }
    },
    {
      { 0, 0, 0 },
      { 0, 1, 0 },
      { 0, 1, 0 }
    },
    {
      { 0, 0, 0 },
      { 0, 1, 1 },
      { 0, 0, 0 }
    },
    {
      { 0, 0, 1 },
      { 0, 1, 1 },
      { 0, 0, 0 }
    },
    {
      { 0, 1, 1 },
      { 0, 1, 0 },
      { 0, 0, 0 }
    },
    {
      { 1, 1, 0 },
      { 0, 1, 0 },
      { 0, 0, 0 }
    },
    {
      { 1, 0, 0 },
      { 1, 1, 0 },
      { 0, 0, 0 }
    },
    {
      { 0, 0, 0 },
      { 1, 1, 0 },
      { 1, 0, 0 }
    },
    {
      { 0, 0, 0 },
      { 0, 1, 0 },
      { 1, 1, 0 }
    },
    {
      { 0, 0, 0 },
      { 0, 1, 0 },
      { 0, 1, 1 }
    },
    {
      { 0, 0, 0 },
      { 0, 1, 1 },
      { 0, 0, 1 }
    },
    {
      { 0, 1, 1 },
      { 1, 1, 0 },
      { 0, 0, 0 }
    },
    {
      { 1, 1, 0 },
      { 0, 1, 1 },
      { 0, 0, 0 }
    },
    {
      { 0, 1, 0 },
      { 0, 1, 1 },
      { 0, 0, 1 }
    },
    {
      { 0, 0, 1 },
      { 0, 1, 1 },
      { 0, 1, 0 }
    }
  };

  ImageMatrix neighbourhood = {
    { imageData[i - 1][j - 1], imageData[i - 1][j], imageData[i - 1][j + 1] },
    { imageData[i][j - 1], imageData[i][j], imageData[i][j + 1] },
    { imageData[i + 1][j - 1], imageData[i + 1][j], imageData[i + 1][j + 1] }
  };

  for (int k = 0; k < patterns.size(); k++) {
    if (neighbourhood == patterns[k]) {
      return true;
    }
  }

  if (
    !imageData[i - 1][j - 1] &&
    imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] &&
    imageData[i][j] &&
    imageData[i + 1][j - 1] &&
    !imageData[i + 1][j] &&
    !imageData[i + 1][j + 1] &&
    (imageData[i - 1][j] || imageData[i][j + 1])
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] &&
    !imageData[i - 1][j + 1] &&
    imageData[i][j] &&
    !imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] &&
    !imageData[i + 1][j] &&
    imageData[i + 1][j + 1] &&
    (imageData[i - 1][j] || imageData[i][j - 1])
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] &&
    !imageData[i - 1][j] &&
    imageData[i - 1][j + 1] &&
    imageData[i][j] &&
    !imageData[i][j + 1] &&
    imageData[i + 1][j - 1] &&
    !imageData[i + 1][j + 1] &&
    (imageData[i][j - 1] || imageData[i + 1][j])
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] &&
    !imageData[i - 1][j] &&
    !imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] &&
    imageData[i][j] &&
    !imageData[i + 1][j - 1] &&
    imageData[i + 1][j + 1] &&
    (imageData[i][j + 1] || imageData[i + 1][j])
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] &&
    imageData[i - 1][j] &&
    imageData[i][j - 1] &&
    imageData[i][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j] &&
    !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] &&
    imageData[i][j] &&
    imageData[i][j + 1] &&
    !imageData[i + 1][j] &&
    !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] &&
    imageData[i - 1][j] &&
    imageData[i][j - 1] &&
    imageData[i][j] &&
    imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] &&
    !imageData[i + 1][j]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] &&
    !imageData[i - 1][j] &&
    imageData[i][j - 1] &&
    imageData[i][j] &&
    imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] &&
    imageData[i + 1][j]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j] &&
    !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] &&
    imageData[i][j] &&
    imageData[i][j + 1] &&
    imageData[i + 1][j] &&
    !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j] &&
    imageData[i][j - 1] &&
    imageData[i][j] &&
    !imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] &&
    imageData[i + 1][j] &&
    !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] &&
    imageData[i - 1][j] &&
    !imageData[i - 1][j + 1] &&
    imageData[i][j - 1] &&
    imageData[i][j] &&
    !imageData[i][j + 1] &&
    imageData[i + 1][j]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] &&
    imageData[i - 1][j] &&
    !imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] &&
    imageData[i][j] &&
    imageData[i][j + 1] &&
    imageData[i + 1][j]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j] &&
    !imageData[i][j - 1] &&
    imageData[i][j] &&
    imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] &&
    imageData[i + 1][j] &&
    !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] &&
    imageData[i - 1][j + 1] &&
    imageData[i][j] &&
    (imageData[i + 1][j - 1] || imageData[i + 1][j] || imageData[i + 1][j + 1])
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] &&
    imageData[i][j] &&
    imageData[i + 1][j - 1] &&
    (imageData[i - 1][j + 1] || imageData[i][j + 1] || imageData[i + 1][j + 1])
    ) {
    return true;
  }

  if (
    imageData[i][j] &&
    imageData[i + 1][j - 1] &&
    imageData[i + 1][j + 1] &&
    (imageData[i - 1][j - 1] || imageData[i - 1][j] || imageData[i - 1][j + 1])
    ) {
    return true;
  }

  if (
    imageData[i - 1][j + 1] &&
    imageData[i][j] &&
    imageData[i + 1][j + 1] &&
    (imageData[i - 1][j - 1] || imageData[i][j - 1] || imageData[i + 1][j - 1])
    ) {
    return true;
  }

  if (
    imageData[i - 1][j] &&
    !imageData[i - 1][j + 1] &&
    !imageData[i][j - 1] &&
    imageData[i][j] &&
    imageData[i][j + 1] &&
    imageData[i + 1][j - 1] &&
    !imageData[i + 1][j]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j - 1] &&
    imageData[i - 1][j] &&
    imageData[i][j - 1] &&
    imageData[i][j] &&
    !imageData[i][j + 1] &&
    !imageData[i + 1][j] &&
    imageData[i + 1][j + 1]
    ) {
    return true;
  }

  if (
    !imageData[i - 1][j] &&
    imageData[i - 1][j + 1] &&
    imageData[i][j - 1] &&
    imageData[i][j] &&
    !imageData[i][j + 1] &&
    !imageData[i + 1][j - 1] &&
    imageData[i + 1][j]
    ) {
    return true;
  }

  if (
    imageData[i - 1][j - 1] &&
    !imageData[i - 1][j] &&
    !imageData[i][j - 1] &&
    imageData[i][j] &&
    imageData[i][j + 1] &&
    imageData[i + 1][j] &&
    !imageData[i + 1][j + 1]
    ) {
    return true;
  }

  return false;
}
