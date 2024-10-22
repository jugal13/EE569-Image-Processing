/*
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Jan 28th 2024
*/

#include <iostream>
#include <string>
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

  RGB(RGB& rgb) {
    r = rgb.r;
    g = rgb.g;
    b = rgb.b;
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

typedef std::vector<RGB> ImageVector;
typedef std::vector<std::vector<RGB> > ImageMatrix;

ImageMatrix readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrix imageData, int width, int height);
ImageMatrix getMirrorReflectedImage(ImageMatrix imageData, int width, int height);
ImageMatrix bilinearInterpolation(ImageMatrix mirroredImageData, int width, int height);
int8 convolutionHelper(ImageMatrix& imageData, float imageFilter[3][3], std::string color, int x_coord, int y_coord);

int main(int argc, char* argv[]) {
  int imageWidth = 420;
  int imageHeight = 288;

  std::string fileName = "House.raw";
  std::string writeFileName = "output1a.raw";

  if (argc < 3) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name input_image.raw output_image.raw" << std::endl;
    return 0;
  }

  fileName = argv[1];
  writeFileName = argv[2];

  ImageMatrix imageData = readFile(fileName, imageWidth, imageHeight);

  ImageMatrix mirroredImageData = getMirrorReflectedImage(imageData, imageWidth, imageHeight);

  ImageMatrix interpolatedImageData = bilinearInterpolation(mirroredImageData, imageWidth, imageHeight);

  writeFile(writeFileName, interpolatedImageData, imageWidth, imageHeight);

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

      if (i % 2 == 0 && j % 2 == 0) {
        imageData[i][j] = RGB(0, data[0], 0);
        continue;
      }

      if (i % 2 == 0 && j % 2 == 1) {
        imageData[i][j] = RGB(data[0], 0, 0);
        continue;
      }

      if (i % 2 == 1 && j % 2 == 0) {
        imageData[i][j] = RGB(0, 0, data[0]);
        continue;
      }

      if (i % 2 == 1 && j % 2 == 1) {
        imageData[i][j] = RGB(0, data[0], 0);
        continue;
      }
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
      std::vector<int8> data = imageData[i][j].rgbString();

      for (int k = 0; k < data.size(); k++) {
        int8 bits = data[k];

        fwrite(&bits, sizeof(int8), 1, outfile);
      }
    }
  }

  fclose(outfile);
}

ImageMatrix getMirrorReflectedImage(ImageMatrix imageData, int width, int height) {
  int mirrorImageWidth = width + 2;
  int mirrorImageHeight = height + 2;

  ImageMatrix mirroredImageData(mirrorImageHeight, ImageVector(mirrorImageWidth));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      mirroredImageData[i + 1][j + 1] = imageData[i][j];
    }
  }

  for (int j = 0; j < width; j++) {
    mirroredImageData[0][j + 1] = imageData[1][j];

    mirroredImageData[mirrorImageHeight - 1][j + 1] = imageData[height - 2][j];
  }

  for (int i = 0; i < mirrorImageHeight; i++) {
    mirroredImageData[i][0] = mirroredImageData[i][2];

    mirroredImageData[i][mirrorImageWidth - 1] = mirroredImageData[i][mirrorImageWidth - 3];
  }

  return mirroredImageData;
}

ImageMatrix bilinearInterpolation(ImageMatrix mirroredImageData, int imageWidth, int imageHeight) {
  float cornerFilter[3][3] = {
      {0.25, 0, 0.25},
      {0, 0, 0},
      {0.25, 0, 0.25}
  };


  float plusFilter[3][3] = {
    {0, 0.25, 0},
    {0.25, 0, 0.25},
    {0, 0.25, 0}
  };

  float rowFilter[3][3] = {
    {0, 0, 0},
    {0.5, 0, 0.5},
    {0, 0, 0}
  };

  float columnFilter[3][3] = {
    {0, 0.5, 0},
    {0, 0, 0},
    {0, 0.5, 0}
  };

  ImageMatrix interpolatedImage(imageHeight, ImageVector(imageWidth));

  for (int i = 1; i < imageHeight + 1; i++) {
    for (int j = 1; j < imageWidth + 1; j++) {
      int8 red, green, blue;

      if (i % 2 == 1 && j % 2 == 1) {
        // GREEN - RED ROW - BLUE COLUMN
        red = convolutionHelper(mirroredImageData, rowFilter, "r", i, j);
        green = mirroredImageData[i][j].g;
        blue = convolutionHelper(mirroredImageData, columnFilter, "b", i, j);
      }

      else if (i % 2 == 1 && j % 2 == 0) {
        // RED - GREEN PLUS - BLUE CORNER
        red = mirroredImageData[i][j].r;
        green = convolutionHelper(mirroredImageData, plusFilter, "g", i, j);
        blue = convolutionHelper(mirroredImageData, cornerFilter, "b", i, j);
      }

      else if (i % 2 == 0 && j % 2 == 1) {
        // BLUE - GREEN PLUS - RED CORDER
        red = convolutionHelper(mirroredImageData, cornerFilter, "r", i, j);
        green = convolutionHelper(mirroredImageData, plusFilter, "g", i, j);
        blue = mirroredImageData[i][j].b;
      }

      else if (i % 2 == 0 && j % 2 == 0) {
        // GREEN - RED COLUMN - BLUE ROW
        red = convolutionHelper(mirroredImageData, columnFilter, "r", i, j);
        green = mirroredImageData[i][j].g;
        blue = convolutionHelper(mirroredImageData, rowFilter, "b", i, j);
      }

      interpolatedImage[i - 1][j - 1] = RGB(red, green, blue);
    }
  }

  return interpolatedImage;
}

int8 convolutionHelper(ImageMatrix& imageData, float imageFilter[3][3], std::string color, int x_coord, int y_coord) {
  double finalColorValue = 0;

  for (int i = 0, x = -1; i < 3; i++, x++) {
    for (int j = 0, y = -1; j < 3; j++, y++) {
      int imageX = x_coord + x;
      int imageY = y_coord + y;

      RGB colorValue = imageData[imageX][imageY];
      float filterValue = imageFilter[i][j];

      finalColorValue += colorValue.getColor(color) * filterValue;
    }
  }

  if (finalColorValue > 255) {
    return 255;
  }

  if (finalColorValue < 0) {
    return 0;
  }

  return (int8)finalColorValue;
}
