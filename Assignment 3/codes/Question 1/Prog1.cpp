/*
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Mar 10th 2024
*/

#include <iostream>
#include <vector>

typedef unsigned char int8;

class RGB {
public:
  double r;
  double g;
  double b;

  RGB() {
    r = g = b = 0;
  }

  RGB(double r_value, double g_value, double b_value) : r(r_value), g(g_value), b(b_value) {}

  RGB(const RGB& rgb) {
    r = rgb.r;
    g = rgb.g;
    b = rgb.b;
  }

  RGB(double rgb[3]) {
    r = rgb[0];
    g = rgb[1];
    b = rgb[2];
  }

  std::vector<double> rgbString() {
    std::vector<double> rgb(3, 0);

    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;

    return rgb;
  }
};

typedef std::vector<RGB> ImageVectorRGB;
typedef std::vector<std::vector<RGB> > ImageMatrixRGB;

ImageMatrixRGB readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrixRGB imageData, int width, int height);
ImageMatrixRGB mapTriangles(ImageMatrixRGB imageData, int width, int height);
ImageMatrixRGB reverseMappingTriangle(ImageMatrixRGB& mappedImageData, int width, int height);

int main(int argc, char* argv[]) {
  int imageWidth = 328;
  int imageHeight = 328;

  std::string fileName = "dog.raw";
  std::string writeFileName = "output_dog.raw";
  std::string recreateFileName = "output_recreate.raw";

  if (argc < 4) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name input_image.raw output_image.raw reverse_output_image.raw" << std::endl;
    return 0;
  }

  fileName = argv[1];
  writeFileName = argv[2];
  recreateFileName = argv[3];

  ImageMatrixRGB imageData = readFile(fileName, imageWidth, imageHeight);

  ImageMatrixRGB mappedImageData = mapTriangles(imageData, imageWidth, imageHeight);

  ImageMatrixRGB reverseMappedImageData = reverseMappingTriangle(mappedImageData, imageWidth, imageHeight);

  writeFile(writeFileName, mappedImageData, imageWidth, imageHeight);

  writeFile(recreateFileName, reverseMappedImageData, imageWidth, imageHeight);

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

      imageData[i][j] = RGB(data[0], data[1], data[2]);
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
      std::vector<double> data = imageData[i][j].rgbString();

      for (int k = 0; k < data.size(); k++) {
        int8 bits = round(data[k]);

        fwrite(&bits, sizeof(int8), 1, outfile);
      }
    }
  }

  fclose(outfile);
}

ImageMatrixRGB mapTriangles(ImageMatrixRGB imageData, int width, int height) {
  ImageMatrixRGB mappedImageData(height, ImageVectorRGB(width));

  // Top Triangle
  for (int i = 0; i < height / 2; i++) {
    for (int j = i; j < width - i; j++) {
      int u = round(0.2171180608 * i + 0.7828819392 * j + 0.002423691585 * std::pow(i, 2) - 0.00002955721445 * (i * j) - 0.002394134371 * std::pow(j, 2));
      int v = j;

      mappedImageData[u][v] = imageData[i][j];
    }
  }

  // Bottom Triangle
  for (int i = height / 2, row = 0; i < height; i++, row++) {
    for (int j = width / 2 - row; j <= width / 2 + row; j++) {
      int u = round(1.773334599 * i - 0.7733345985 * j - 0.00236493761 * std::pow(i, 2) - 0.00002919676062 * (i * j) + 0.002394134371 * std::pow(j, 2));
      int v = j;

      mappedImageData[u][v] = imageData[i][j];
    }
  }

  // Left Trignle Top Half
  for (int i = 0; i < height / 2; i++) {
    for (int j = 0; j <= i; j++) {
      int u = i;
      int v = round(0.7828819392 * i + 0.2171180608 * j - 0.002394134371 * std::pow(i, 2) - 0.00002955721445 * (i * j) + 0.002423691585 * std::pow(j, 2));

      mappedImageData[u][v] = imageData[i][j];
    }
  }

  // Left Triangle Bottom half
  for (int i = height / 2, row = 0; i < height; i++, row++) {
    for (int j = 0; j <= width / 2 - row; j++) {
      int u = i;
      int v = round(0.7828819392 * i + 0.2171180608 * j - 0.002394134371 * std::pow(i, 2) - 0.00005911442891 * (i * j) + 0.0024532488 * std::pow(j, 2));

      mappedImageData[u][v] = imageData[i][j];
    }
  }

  // Right Triangle Top Half
  for (int i = 0; i < height / 2; i++) {
    for (int j = width - i - 1; j < width; j++) {
      int u = i;
      int v = round(-0.7733345985 * i + 1.773334599 * j + 0.002394134371 * std::pow(i, 2) - 0.00002919676062 * (i * j) - 0.00236493761 * std::pow(j, 2));

      mappedImageData[u][v] = imageData[i][j];
    }
  }

  // Right triangle Bottom Half
  for (int i = height / 2, row = 0; i < height; i++, row++) {
    for (int j = width / 2 + row; j < width; j++) {
      int u = i;
      int v = round(-0.7733345985 * i + 1.773334599 * j + 0.002394134371 * std::pow(i, 2) - 0.00002919676062 * (i * j) - 0.00236493761 * std::pow(j, 2));

      mappedImageData[u][v] = imageData[i][j];
    }
  }

  return mappedImageData;
}

ImageMatrixRGB reverseMappingTriangle(ImageMatrixRGB& mappedImageData, int width, int height) {
  ImageMatrixRGB imageData(height, ImageVectorRGB(width));

  // Top Triangle
  for (int i = 0; i < height / 2; i++) {
    for (int j = i; j < width - i; j++) {
      int u = round(0.2171180608 * i + 0.7828819392 * j + 0.002423691585 * std::pow(i, 2) - 0.00002955721445 * (i * j) - 0.002394134371 * std::pow(j, 2));
      int v = j;

      imageData[i][j] = mappedImageData[u][v];
    }
  }

  // Bottom Triangle
  for (int i = height / 2, row = 0; i < height; i++, row++) {
    for (int j = width / 2 - row; j <= width / 2 + row; j++) {
      int u = round(1.773334599 * i - 0.7733345985 * j - 0.00236493761 * std::pow(i, 2) - 0.00002919676062 * (i * j) + 0.002394134371 * std::pow(j, 2));
      int v = j;

      imageData[i][j] = mappedImageData[u][v];
    }
  }

  // Left Trignle Top Half
  for (int i = 0; i < height / 2; i++) {
    for (int j = 0; j <= i; j++) {
      int u = i;
      int v = round(0.7828819392 * i + 0.2171180608 * j - 0.002394134371 * std::pow(i, 2) - 0.00002955721445 * (i * j) + 0.002423691585 * std::pow(j, 2));

      imageData[i][j] = mappedImageData[u][v];
    }
  }

  // Left Triangle Bottom half
  for (int i = height / 2, row = 0; i < height; i++, row++) {
    for (int j = 0; j <= width / 2 - row; j++) {
      int u = i;
      int v = round(0.7828819392 * i + 0.2171180608 * j - 0.002394134371 * std::pow(i, 2) - 0.00005911442891 * (i * j) + 0.0024532488 * std::pow(j, 2));

      imageData[i][j] = mappedImageData[u][v];
    }
  }

  // Right Triangle Top Half
  for (int i = 0; i < height / 2; i++) {
    for (int j = width - i - 1; j < width; j++) {
      int u = i;
      int v = round(-0.7733345985 * i + 1.773334599 * j + 0.002394134371 * std::pow(i, 2) - 0.00002919676062 * (i * j) - 0.00236493761 * std::pow(j, 2));

      imageData[i][j] = mappedImageData[u][v];
    }
  }

  // Right triangle Bottom Half
  for (int i = height / 2, row = 0; i < height; i++, row++) {
    for (int j = width / 2 + row; j < width; j++) {
      int u = i;
      int v = round(-0.7733345985 * i + 1.773334599 * j + 0.002394134371 * std::pow(i, 2) - 0.00002919676062 * (i * j) - 0.00236493761 * std::pow(j, 2));

      imageData[i][j] = mappedImageData[u][v];
    }
  }

  return imageData;
}
