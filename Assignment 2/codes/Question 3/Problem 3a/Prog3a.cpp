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

  RGB normaliseValue(double normalisingFactor) {
    double r = this->r / normalisingFactor;
    double g = this->g / normalisingFactor;
    double b = this->b / normalisingFactor;

    return RGB(r, g, b);
  }

  RGB scaleNormaliseValue(double normalisingFactor) {
    double r = this->r * normalisingFactor;
    double g = this->g * normalisingFactor;
    double b = this->b * normalisingFactor;

    return RGB(r, g, b);
  }

  std::vector<double> rgbString() {
    std::vector<double> rgb(3, 0);

    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;

    return rgb;
  }
};

class CMY {
public:
  double c;
  double m;
  double y;

  CMY() {
    c = m = y = 0;
  }

  CMY(double c_value, double m_value, double y_value) : c(c_value), m(m_value), y(y_value) {}

  CMY(const CMY& cmy) {
    c = cmy.c;
    m = cmy.m;
    y = cmy.y;
  }

  CMY(double cmy[3]) {
    c = cmy[0];
    m = cmy[1];
    y = cmy[2];
  }

  CMY binarise() {
    double c = this->c > 0.5 ? 1.0 : 0.0;
    double m = this->m > 0.5 ? 1.0 : 0.0;
    double y = this->y > 0.5 ? 1.0 : 0.0;

    return CMY(c, m, y);
  }

  CMY operator-(CMY const& cmy) {
    double c = this->c - cmy.c;
    double m = this->m - cmy.m;
    double y = this->y - cmy.y;

    return CMY(c, m, y);
  }
};

typedef std::vector<RGB> ImageVectorRGB;
typedef std::vector<std::vector<RGB> > ImageMatrixRGB;
typedef std::vector<CMY> ImageVectorCMY;
typedef std::vector<std::vector<CMY> > ImageMatrixCMY;
typedef std::vector<double> ImageVectorDouble;
typedef std::vector<std::vector<double> > ImageMatrixDouble;

ImageMatrixRGB readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrixRGB& imageData, int width, int height);
ImageMatrixRGB normaliseImageData(ImageMatrixRGB& imageData, int width, int height);
ImageMatrixCMY convertRGBtoCMY(ImageMatrixRGB& imageData, int width, int height);
ImageMatrixCMY getMirrorReflectedImage(ImageMatrixCMY imageData, int width, int height, int padding);
ImageMatrixDouble generateDiffusionMatrix();
ImageMatrixCMY generateDitheredImage(ImageMatrixCMY precisionImageData, ImageMatrixDouble& errorMatrix, int width, int height, int filterSize, int padding);
int channelRounding(double value);
ImageMatrixRGB convertCMYtoRGB(ImageMatrixCMY& imageData, int width, int height);
ImageMatrixRGB scaleNormaliseImageData(ImageMatrixRGB& imageData, int width, int height);

int main(int argc, char* argv[]) {
  int imageWidth = 500;
  int imageHeight = 375;

  std::string fileName = "Bird.raw";
  std::string writeFileName = "output3a.raw";

  int filterSize = 3;
  int padding = filterSize / 2;

  if (argc < 3) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name input_image.raw output_image.raw" << std::endl;
    return 0;
  }

  fileName = argv[1];
  writeFileName = argv[2];

  ImageMatrixRGB imageData = readFile(fileName, imageWidth, imageHeight);

  ImageMatrixRGB normalisedImageData = normaliseImageData(imageData, imageWidth, imageHeight);

  ImageMatrixCMY cmyImageData = convertRGBtoCMY(normalisedImageData, imageWidth, imageHeight);

  ImageMatrixCMY cmyPaddedImageData = getMirrorReflectedImage(cmyImageData, imageWidth, imageHeight, padding);

  ImageMatrixDouble diffusionMatrix = generateDiffusionMatrix();

  ImageMatrixCMY ditheredImageData = generateDitheredImage(cmyPaddedImageData, diffusionMatrix, imageWidth, imageHeight, filterSize, padding);

  ImageMatrixRGB rgbImageData = convertCMYtoRGB(ditheredImageData, imageWidth, imageHeight);

  ImageMatrixRGB finalImageData = scaleNormaliseImageData(rgbImageData, imageWidth, imageHeight);

  writeFile(writeFileName, finalImageData, imageWidth, imageHeight);
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

ImageMatrixRGB normaliseImageData(ImageMatrixRGB& imageData, int width, int height) {
  ImageMatrixRGB normalisedImageData(height, ImageVectorRGB(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      normalisedImageData[i][j] = imageData[i][j].normaliseValue(255);
    }
  }

  return normalisedImageData;
}

ImageMatrixCMY convertRGBtoCMY(ImageMatrixRGB& imageData, int width, int height) {
  ImageMatrixCMY cmyImageData(height, ImageVectorCMY(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      RGB rgb = imageData[i][j];
      double c = 1 - rgb.r;
      double m = 1 - rgb.g;
      double y = 1 - rgb.b;

      cmyImageData[i][j] = CMY(c, m, y);
    }
  }

  return cmyImageData;
}

ImageMatrixCMY getMirrorReflectedImage(ImageMatrixCMY imageData, int width, int height, int padding) {
  int mirrorImageWidth = width + padding * 2;
  int mirrorImageHeight = height + padding * 2;

  ImageMatrixCMY mirroredImageData(mirrorImageHeight, ImageVectorCMY(mirrorImageWidth));

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

ImageMatrixCMY generateDitheredImage(ImageMatrixCMY precisionImageData, ImageMatrixDouble& errorMatrix, int width, int height, int filterSize, int padding) {
  ImageMatrixCMY ditheredImage(height, ImageVectorCMY(width));

  for (int i = padding; i <= height + (padding - 1); i += 2) {
    int row = i;

    for (int j = padding; j <= width + (padding - 1); j++) {
      CMY preciseData = precisionImageData[row][j];
      CMY binarisedData = preciseData.binarise();
      CMY error = preciseData - binarisedData;
      ditheredImage[row - padding][j - padding] = binarisedData;

      for (int k = row - padding, filterX = 0; k <= row + padding; k++, filterX++) {
        for (int l = j - padding, filterY = 0; l <= j + padding; l++, filterY++) {
          double c = precisionImageData[k][l].c + errorMatrix[filterX][filterY] * error.c;
          double m = precisionImageData[k][l].m + errorMatrix[filterX][filterY] * error.m;
          double y = precisionImageData[k][l].y + errorMatrix[filterX][filterY] * error.y;
          precisionImageData[k][l] = CMY(c, m, y);
        }
      }
    }


    row = i + 1;
    if (row > height) {
      break;
    }
    for (int j = width + (padding - 1); j >= padding; j--) {
      CMY preciseData = precisionImageData[row][j];
      CMY binarisedData = preciseData.binarise();
      CMY error = preciseData - binarisedData;
      ditheredImage[row - padding][j - padding] = binarisedData;

      for (int k = row - padding, filterX = 0; k <= row + padding; k++, filterX++) {
        for (int l = j - padding, filterY = filterSize - 1; l <= j + padding; l++, filterY--) {
          double c = precisionImageData[k][l].c + errorMatrix[filterX][filterY] * error.c;
          double m = precisionImageData[k][l].m + errorMatrix[filterX][filterY] * error.m;
          double y = precisionImageData[k][l].y + errorMatrix[filterX][filterY] * error.y;
          precisionImageData[k][l] = CMY(c, m, y);
        }
      }
    }
  }

  return ditheredImage;
}

ImageMatrixRGB convertCMYtoRGB(ImageMatrixCMY& imageData, int width, int height) {
  ImageMatrixRGB rgbImageData(height, ImageVectorRGB(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      CMY cmy = imageData[i][j];
      double r = 1 - cmy.c;
      double g = 1 - cmy.m;
      double b = 1 - cmy.y;

      rgbImageData[i][j] = RGB(r, g, b);
    }
  }

  return rgbImageData;
}

ImageMatrixRGB scaleNormaliseImageData(ImageMatrixRGB& imageData, int width, int height) {
  ImageMatrixRGB normalisedImageData(height, ImageVectorRGB(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      normalisedImageData[i][j] = imageData[i][j].scaleNormaliseValue(255);
    }
  }

  return normalisedImageData;
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
