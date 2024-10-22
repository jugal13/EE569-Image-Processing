/* 
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Jan 28th 2024
*/

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

typedef unsigned char int8;

int8 channelRounding(float value);

class RGB {
public:
  int8 r;
  int8 g;
  int8 b;

  RGB() {
    r = g = b = 0;
  }

  RGB(int8 r_value, int8 g_value, int8 b_value) : r(r_value), g(g_value), b(b_value) {}

  RGB(int8 rgb[3]) {
    r = rgb[0];
    g = rgb[1];
    b = rgb[2];
  }

  RGB(const RGB& rgb) {
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

class YUV {
public:
  int8 y;
  int8 u;
  int8 v;

  YUV() {
    y = u = v = 0;
  }

  YUV(int8 y_value, int8 u_value, int8 v_value) : y(y_value), u(u_value), v(v_value) {};

  YUV(const YUV& yuv) {
    y = yuv.y;
    u = yuv.u;
    v = yuv.v;
  }
};

typedef std::vector<RGB> ImageVectorRGB;
typedef std::vector<std::vector<RGB> > ImageMatrixRGB;
typedef std::vector<YUV> ImageVectorYUV;
typedef std::vector<std::vector<YUV> > ImageMatrixYUV;
typedef std::vector<int> PixelLocation;
typedef std::vector<PixelLocation> BucketLocations;
typedef std::vector<BucketLocations> BucketFrequency;

ImageMatrixRGB readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrixRGB imageData, int width, int height);
YUV getYUV(RGB rgb);
RGB getRGB(YUV yuv);
ImageMatrixYUV mapRGBtoYUVSpace(ImageMatrixRGB imageData, int width, int height);
ImageMatrixRGB mapYUVtoRGBSpace(ImageMatrixYUV yuvImageData, int width, int height);
std::vector<std::vector<std::vector<int8> > > getYUVMatrix(ImageMatrixYUV imageData, int width, int height);
ImageMatrixYUV getYUVImage(std::vector<std::vector<std::vector<int8> > > yuvImageMatrix, int width, int height);

int main(int argc, char* argv[]) {
  int imageWidth = 750;
  int imageHeight = 422;

  std::string fileName = "City.raw";
  std::string writeFileName = "output_1c_clahe.raw";

  if (argc < 3) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name input_image.raw output_image.raw" << std::endl;
    return 0;
  }

  fileName = argv[1];
  writeFileName = argv[2];

  std::cout << "Read" << std::endl;
  ImageMatrixRGB imageData = readFile(fileName, imageWidth, imageHeight);

  std::cout << "RGB to YUV" << std::endl;
  ImageMatrixYUV yuvImageData = mapRGBtoYUVSpace(imageData, imageWidth, imageHeight);

  std::cout << "YUV matrix" << std::endl;
  std::vector<std::vector<std::vector<int8> > > yuvImageMatrix = getYUVMatrix(yuvImageData, imageWidth, imageHeight);

  cv::Mat yuvImage(imageHeight, imageWidth, CV_8UC3);

  for (int i = 0; i < imageHeight; i++) {
    for (int j = 0; j < imageWidth; j++) {
      for (int k = 0; k < 3; k++) {
        yuvImage.at<cv::Vec3b>(i, j)[k] = yuvImageMatrix[i][j][k];
      }
    }
  }

  std::vector<cv::Mat> channels(3);
  cv::split(yuvImage, channels);

  std::cout << "CLAHE" << std::endl;
  cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
  clahe->setClipLimit(3);
  clahe->setTilesGridSize(cv::Size(8, 8));
  clahe->apply(channels[0], channels[0]);

  cv::merge(channels, yuvImage);

  std::cout << "Back to YUV matrix" << std::endl;
  for (int i = 0; i < imageHeight; i++) {
    for (int j = 0; j < imageWidth; j++) {
      for (int k = 0; k < 3; k++) {
        yuvImageMatrix[i][j][k] = (int8)yuvImage.at<cv::Vec3b>(i, j)[k];
      }
    }
  }

  std::cout << "YUV Image" << std::endl;
  yuvImageData = getYUVImage(yuvImageMatrix, imageWidth, imageHeight);

  std::cout << "YUV to RGB" << std::endl;
  imageData = mapYUVtoRGBSpace(yuvImageData, imageWidth, imageHeight);

  std::cout << "Write" << std::endl;
  writeFile(writeFileName, imageData, imageWidth, imageHeight);
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

YUV getYUV(RGB rgb) {
  float y_channel = (0.257 * rgb.r) + (0.504 * rgb.g) + (0.098 * rgb.b) + 16;
  float u_channel = -(0.148 * rgb.r) - (0.291 * rgb.g) + (0.439 * rgb.b) + 128;
  float v_channel = (0.439 * rgb.r) - (0.368 * rgb.g) - (0.071 * rgb.b) + 128;

  int8 y = channelRounding(y_channel);
  int8 u = channelRounding(u_channel);
  int8 v = channelRounding(v_channel);

  return YUV(y, u, v);
}

RGB getRGB(YUV yuv) {
  float red = 1.164 * (yuv.y - 16) + 1.596 * (yuv.v - 128);
  float green = 1.164 * (yuv.y - 16) - 0.813 * (yuv.v - 128) - 0.391 * (yuv.u - 128);
  float blue = 1.164 * (yuv.y - 16) + 2.018 * (yuv.u - 128);

  int8 r = channelRounding(red);
  int8 g = channelRounding(green);
  int8 b = channelRounding(blue);

  return RGB(r, g, b);
}

ImageMatrixYUV mapRGBtoYUVSpace(ImageMatrixRGB imageData, int width, int height) {
  ImageMatrixYUV yuvImageData(height, ImageVectorYUV(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      yuvImageData[i][j] = getYUV(imageData[i][j]);
    }
  }

  return yuvImageData;
}

ImageMatrixRGB mapYUVtoRGBSpace(ImageMatrixYUV yuvImageData, int width, int height) {
  ImageMatrixRGB imageData(height, ImageVectorRGB(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      imageData[i][j] = getRGB(yuvImageData[i][j]);
    }
  }

  return imageData;
}

int8 channelRounding(float value) {
  int channelValue = (int)value;

  if (channelValue > 255) {
    return 255;
  }

  if (channelValue < 0) {
    return 0;
  }

  return (int8)channelValue;
}

std::vector<std::vector<std::vector<int8> > > getYUVMatrix(ImageMatrixYUV imageData, int width, int height) {
  std::vector<std::vector<std::vector<int8> > > yuvImageData(height, std::vector<std::vector<int8> >(width, std::vector<int8>(3, 0)));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      yuvImageData[i][j][0] = imageData[i][j].y;
      yuvImageData[i][j][1] = imageData[i][j].u;
      yuvImageData[i][j][2] = imageData[i][j].v;
    }
  }

  return yuvImageData;
}

ImageMatrixYUV getYUVImage(std::vector<std::vector<std::vector<int8> > > yuvImageMatrix, int width, int height) {
  ImageMatrixYUV yuvImageData(height, ImageVectorYUV(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      yuvImageData[i][j] = YUV(yuvImageMatrix[i][j][0], yuvImageMatrix[i][j][1], yuvImageMatrix[i][j][2]);
    }
  }

  return yuvImageData;
}
