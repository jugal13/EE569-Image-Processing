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
typedef std::vector<unsigned int> DitheringVector;
typedef std::vector<std::vector<unsigned int> > DitheringMatrix;

ImageMatrix readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrix imageData, int width, int height);
ImageMatrix fixedThresholdDithering(ImageMatrix& imageData, int width, int height, int threshold);
ImageMatrix randomThresholdDithering(ImageMatrix& imageData, int width, int height);
int generateRandomThreshold(int i, int j);
DitheringMatrix generateDitheringMatrix(int size);
int ditheringMatrixHelper(DitheringMatrix& ditheringMatrix, int size, int i, int j);
int getQuad(int i, int j, int size);
int getRow(int i, int j, int size);
int getCol(int i, int j, int size);
int8 channelRounding(double value);
ImageMatrix generateThresholdMatrix(DitheringMatrix& ditherMatrix, int size);
ImageMatrix ditherMatrixThresholding(ImageMatrix& imageMatrix, ImageMatrix& thresholdMatrix, int width, int height, int size);

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

  if (method == "matrix" && argc < 5) {
    std::cout << "Syntax Error - Incorrect Parameter Usage:" << std::endl;
    std::cout << "program_name input_image.raw output_image.raw method index" << std::endl;
    return 0;
  }

  ImageMatrix imageData = readFile(fileName, imageWidth, imageHeight);

  if (method == "fixed") {
    ImageMatrix fixedImageData = fixedThresholdDithering(imageData, imageWidth, imageHeight, 127);

    writeFile(writeFileName, fixedImageData, imageWidth, imageHeight);

    return 0;
  }

  if (method == "random") {
    ImageMatrix randomImageData = randomThresholdDithering(imageData, imageWidth, imageHeight);

    writeFile(writeFileName, randomImageData, imageWidth, imageHeight);
  }

  if (method == "matrix") {
    int size = atoi(argv[4]);

    DitheringMatrix ditheringMatrix = generateDitheringMatrix(size);

    std::cout << "Dithering Matrix" << std::endl;
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        std::cout << ditheringMatrix[i][j] << " ";
      }
      std::cout << std::endl;
    }

    ImageMatrix thresholdMatrix = generateThresholdMatrix(ditheringMatrix, size);

    ImageMatrix ditheredImageData = ditherMatrixThresholding(imageData, thresholdMatrix, imageWidth, imageHeight, size);

    writeFile(writeFileName, ditheredImageData, imageWidth, imageHeight);
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

void writeFile(std::string fileName, ImageMatrix imageData, int width, int height) {
  FILE* outfile;

  outfile = fopen(&fileName[0], "wb");

  if (!outfile) {
    std::cout << "Cannot open file: " << fileName << std::endl;
    exit(1);
  }

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int8 data = imageData[i][j];

      fwrite(&data, sizeof(int8), 1, outfile);
    }
  }

  fclose(outfile);
}

ImageMatrix fixedThresholdDithering(ImageMatrix& imageData, int width, int height, int threshold) {
  ImageMatrix ditheredImageData(height, ImageVector(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      ditheredImageData[i][j] = imageData[i][j] >= threshold ? 255 : 0;
    }
  }

  return ditheredImageData;
}

ImageMatrix randomThresholdDithering(ImageMatrix& imageData, int width, int height) {
  ImageMatrix ditheredImageData(height, ImageVector(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int threshold = generateRandomThreshold(i, j);

      ditheredImageData[i][j] = imageData[i][j] >= threshold ? 255 : 0;
    }
  }

  return ditheredImageData;
}

int generateRandomThreshold(int i, int j) {
  std::srand(std::rand() + i + j);

  return std::rand() % 256;
}

DitheringMatrix generateDitheringMatrix(int size) {
  DitheringMatrix ditheringMatrix(size, DitheringVector(size));

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      ditheringMatrix[i][j] = ditheringMatrixHelper(ditheringMatrix, size, i, j);
    }
  }

  return ditheringMatrix;
}

int ditheringMatrixHelper(DitheringMatrix& ditheringMatrix, int size, int x, int y) {
  if (size == 2) {
    return getQuad(x, y, size);
  }

  int quad = getQuad(x, y, size);
  int p = getRow(x, y, size);
  int q = getCol(x, y, size);

  return 4 * ditheringMatrixHelper(ditheringMatrix, size / 2, p, q) + quad;
}

int getQuad(int i, int j, int size) {
  if (i < size / 2 && j < size / 2) {
    return 1;
  }

  if (i < size / 2 && j >= size / 2) {
    return 2;
  }

  if (i >= size / 2 && j < size / 2) {
    return 3;
  }

  return 0;
}

int getRow(int i, int j, int size) {
  if (i < size / 2) {
    return i;
  }

  return i - size / 2;
}

int getCol(int i, int j, int size) {
  if (j < size / 2) {
    return j;
  }

  return j - size / 2;
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

ImageMatrix generateThresholdMatrix(DitheringMatrix& ditherMatrix, int size) {
  ImageMatrix filter(size, ImageVector(size));

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      filter[i][j] = channelRounding(((ditherMatrix[i][j] + 0.5) / (size * size)) * 255);
    }
  }

  return filter;
}

ImageMatrix ditherMatrixThresholding(ImageMatrix& imageMatrix, ImageMatrix& thresholdMatrix, int width, int height, int size) {
  ImageMatrix ditheredImageMatrix(height, ImageVector(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      ditheredImageMatrix[i][j] = imageMatrix[i][j] > thresholdMatrix[i % size][j % size] ? 255 : 0;
    }
  }

  return ditheredImageMatrix;
}
