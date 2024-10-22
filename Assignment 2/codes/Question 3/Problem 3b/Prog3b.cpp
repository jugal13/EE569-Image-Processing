/*
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Feb 19th 2024
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

  RGB operator-(RGB const& rgb) {
    double r = this->r - rgb.r;
    double g = this->g - rgb.g;
    double b = this->b - rgb.b;

    return RGB(r, g, b);
  }
};

typedef std::vector<RGB> ImageVectorRGB;
typedef std::vector<std::vector<RGB> > ImageMatrixRGB;
typedef std::vector<double> ImageVectorDouble;
typedef std::vector<std::vector<double> > ImageMatrixDouble;

enum Pyramid { CMYW, MYGC, RGMY, KRGB, RGBM, CMGB };

ImageMatrixRGB readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrixRGB& imageData, int width, int height);
ImageMatrixRGB getMirrorReflectedImage(ImageMatrixRGB imageData, int width, int height, int padding);
ImageMatrixDouble generateDiffusionMatrix();
int channelRounding(double value);
ImageMatrixRGB generateDitheredImage(ImageMatrixRGB imageData, ImageMatrixDouble& errorMatrix, int width, int height, int filterSize, int padding);
Pyramid getPixelMBVQ(RGB rgb);
RGB getNearestVertex(Pyramid p, RGB rgb);
RGB getNearestVertexCMYW(double r, double g, double b);
RGB getNearestVertexMYGC(double r, double g, double b);
RGB getNearestVertexRGMY(double r, double g, double b);
RGB getNearestVertexKRGB(double r, double g, double b);
RGB getNearestVertexRGBM(double r, double g, double b);
RGB getNearestVertexCMGB(double r, double g, double b);

int main(int argc, char* argv[]) {
  int imageWidth = 500;
  int imageHeight = 375;

  std::string fileName = "../../images/Bird.raw";
  std::string writeFileName = "output3b.raw";

  if (argc < 3) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name input_image.raw output_image.raw" << std::endl;
    return 0;
  }

  fileName = argv[1];
  writeFileName = argv[2];

  int filterSize = 3;
  int padding = filterSize / 2;

  ImageMatrixRGB imageData = readFile(fileName, imageWidth, imageHeight);

  ImageMatrixRGB paddedImageData = getMirrorReflectedImage(imageData, imageWidth, imageHeight, padding);

  ImageMatrixDouble diffusionMatrix = generateDiffusionMatrix();

  ImageMatrixRGB ditheredImageData = generateDitheredImage(paddedImageData, diffusionMatrix, imageWidth, imageHeight, filterSize, padding);

  writeFile(writeFileName, ditheredImageData, imageWidth, imageHeight);

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
      int8 r, g, b;

      fread(&r, sizeof(int8), 1, infile);
      fread(&g, sizeof(int8), 1, infile);
      fread(&b, sizeof(int8), 1, infile);


      imageData[i][j] = RGB((double)r, (double)g, (double)b);
    }
  }

  fclose(infile);

  return imageData;
}

void writeFile(std::string fileName, ImageMatrixRGB& imageData, int width, int height) {
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
        int8 bits = channelRounding(data[k]);

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

ImageMatrixDouble generateDiffusionMatrix() {
  int normalisingFactor = 16;
  ImageMatrixDouble diffusionMatrix;
  ImageVectorDouble row;

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

ImageMatrixRGB generateDitheredImage(ImageMatrixRGB imageData, ImageMatrixDouble& errorMatrix, int width, int height, int filterSize, int padding) {
  ImageMatrixRGB ditheredImage(height, ImageVectorRGB(width));

  for (int i = padding; i <= height + (padding - 1); i += 2) {
    int row = i;

    for (int j = padding; j <= width + (padding - 1); j++) {
      RGB preciseData = imageData[row][j];
      Pyramid p = getPixelMBVQ(preciseData);
      RGB vertex = getNearestVertex(p, preciseData);
      RGB error = preciseData - vertex;
      ditheredImage[row - padding][j - padding] = vertex;

      for (int k = row - padding, filterX = 0; k <= row + padding; k++, filterX++) {
        for (int l = j - padding, filterY = 0; l <= j + padding; l++, filterY++) {
          double r = imageData[k][l].r + errorMatrix[filterX][filterY] * error.r;
          double g = imageData[k][l].g + errorMatrix[filterX][filterY] * error.g;
          double b = imageData[k][l].b + errorMatrix[filterX][filterY] * error.b;
          imageData[k][l] = RGB(r, g, b);
        }
      }
    }

    row = i + 1;
    if (row > height) {
      break;
    }
    for (int j = width + (padding - 1); j >= padding; j--) {
      RGB preciseData = imageData[row][j];
      Pyramid p = getPixelMBVQ(preciseData);
      RGB vertex = getNearestVertex(p, preciseData);
      RGB error = preciseData - vertex;
      ditheredImage[row - padding][j - padding] = vertex;

      for (int k = row - padding, filterX = 0; k <= row + padding; k++, filterX++) {
        for (int l = j - padding, filterY = filterSize - 1; l <= j + padding; l++, filterY--) {
          double r = imageData[k][l].r + errorMatrix[filterX][filterY] * error.r;
          double g = imageData[k][l].g + errorMatrix[filterX][filterY] * error.g;
          double b = imageData[k][l].b + errorMatrix[filterX][filterY] * error.b;
          imageData[k][l] = RGB(r, g, b);
        }
      }
    }


  }

  return ditheredImage;
}

// Ref: "Color Diffusion: Error-Diffusion for Color Halftones
// by Shaked, Arad, Fitzhugh, and Sobel -- HP Labs
// Hewlett-Packard Laboratories TR 96-128
// and Electronic Imaging, Proc. SPIE 3648, 1999
// Adapted from Halftoning Toolbox Version 1.2 released July 2005 (Univ. of Texas)
Pyramid getPixelMBVQ(RGB rgb) {
  if (rgb.r + rgb.g > 255) {
    if (rgb.g + rgb.b > 255) {
      if (rgb.r + rgb.g + rgb.b > 510) {
        return CMYW;
      }
      else {
        return MYGC;
      }
    }
    else {
      return RGMY;
    }
  }
  else {
    if (!(rgb.g + rgb.b > 255)) {
      if (!(rgb.r + rgb.g + rgb.b > 255)) {
        return KRGB;
      }
      else {
        return RGBM;
      }
    }
    else {
      return CMGB;
    }
  }
}

RGB getNearestVertex(Pyramid p, RGB rgb) {
  double r = rgb.r;
  double g = rgb.g;
  double b = rgb.b;

  switch (p) {
    case CMYW:
      return getNearestVertexCMYW(r, g, b);
    case MYGC:
      return getNearestVertexMYGC(r, g, b);
    case RGMY:
      return getNearestVertexRGMY(r, g, b);
    case KRGB:
      return getNearestVertexKRGB(r, g, b);
    case RGBM:
      return getNearestVertexRGBM(r, g, b);
    case CMGB:
      return getNearestVertexCMGB(r, g, b);
    default:
      return RGB();
  }
}

RGB getNearestVertexCMYW(double r, double g, double b) {
  RGB rgb(255.0, 255.0, 255.0); // White

  if (b < 128) {
    if (b <= r) {
      if (b <= g) {
        rgb = RGB(255.0, 255.0, 0.0); // Yellow
      }
    }
  }

  if (g < 128) {
    if (g <= b) {
      if (g <= r) {
        rgb = RGB(255.0, 0.0, 255.0); // Magenta
      }
    }
  }

  if (r < 128) {
    if (r <= b) {
      if (r <= g) {
        rgb = RGB(0.0, 255.0, 255.0); // Cyan
      }
    }
  }

  return rgb;
}

RGB getNearestVertexMYGC(double r, double g, double b) {
  RGB rgb(255.0, 0.0, 255.0); // Magenta

  if (g >= b) {
    if (r >= b) {
      if (r >= 128) {
        rgb = RGB(255.0, 255.0, 0.0); // Yellow
      }
      else {
        rgb = RGB(0.0, 255.0, 0.0); // Green
      }
    }
  }

  if (g >= r) {
    if (b >= r) {
      if (b >= 128) {
        rgb = RGB(0.0, 255.0, 255.0);  // Cyan
      }
      else {
        rgb = RGB(0.0, 255.0, 0.0); // Green
      }
    }
  }

  return rgb;
}

RGB getNearestVertexRGMY(double r, double g, double b) {
  RGB rgb;

  if (b > 128) {
    if (r > 128) {
      if (b >= g) {
        rgb = RGB(255.0, 0.0, 255.0); // Magenta
      }
      else {
        rgb = RGB(255.0, 255.0, 0.0); // Yellow
      }
    }
    else {
      if (g > b + r) {
        rgb = RGB(0.0, 255.0, 0.0); // Green
      }
      else {
        rgb = RGB(255.0, 0.0, 255.0); // Magenta
      }
    }
  }
  else {
    if (r >= 128) {
      if (g >= 128) {
        rgb = RGB(255.0, 255.0, 0.0); // Yellow
      }
      else {
        rgb = RGB(255.0, 0.0, 0.0); // Red
      }
    }
    else {
      if (r >= g) {
        rgb = RGB(255.0, 0.0, 0.0); // Red
      }
      else {
        rgb = RGB(0.0, 255.0, 0.0); // Green
      }
    }
  }

  return rgb;
}

RGB getNearestVertexKRGB(double r, double g, double b) {
  RGB rgb(0, 0, 0); // Black

  if (b > 128) {
    if (b >= r) {
      if (b >= g) {
        rgb = RGB(0.0, 0.0, 255.0); // Blue
      }
    }
  }

  if (g > 128) {
    if (g >= b) {
      if (g >= r) {
        rgb = RGB(0.0, 255.0, 0.0); // Green
      }
    }
  }

  if (r > 128) {
    if (r >= b) {
      if (r >= g) {
        rgb = RGB(255.0, 0.0, 0.0); // Red
      }
    }
  }

  return rgb;
}

RGB getNearestVertexRGBM(double r, double g, double b) {
  RGB rgb(0, 255, 0); // Green

  if (r > g) {
    if (r >= b) {
      if (b < 128) {
        rgb = RGB(255.0, 0.0, 0.0); // Red
      }
      else {
        rgb = RGB(255.0, 0.0, 255.0); // Magenta
      }
    }
  }

  if (b > g) {
    if (b >= r) {
      if (r < 128) {
        rgb = RGB(0.0, 0.0, 255.0); // Blue
      }
      else {
        rgb = RGB(255.0, 0.0, 255.0); // Magenta
      }
    }
  }

  return rgb;
}

RGB getNearestVertexCMGB(double r, double g, double b) {
  RGB rgb;

  if (b > 128) {
    if (r > 128) {
      if (g >= r) {
        rgb = RGB(0.0, 255.0, 255.0); // Cyan
      }
      else {
        rgb = RGB(255.0, 0.0, 255.0); // Magenta
      }
    }
    else {
      if (g > 128) {
        rgb = RGB(0.0, 255.0, 255.0); // Cyan
      }
      else {
        rgb = RGB(0.0, 0.0, 255.0); // Blue
      }
    }
  }
  else {
    if (r > 128) {
      if (r - g + b >= 128) {
        rgb = RGB(255.0, 0.0, 255.0); // Magenta
      }
      else {
        rgb = RGB(0.0, 255.0, 0.0); // Green
      }
    }
    else {
      if (g >= b) {
        rgb = RGB(0.0, 255.0, 0.0); // Green
      }
      else {
        rgb = RGB(0.0, 0.0, 255.0); // Blue
      }
    }
  }

  return rgb;
}
