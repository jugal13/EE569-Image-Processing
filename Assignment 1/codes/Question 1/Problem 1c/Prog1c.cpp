/*
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Jan 28th 2024
*/

#include <iostream>
#include <string>
#include <vector>
#include <map>

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
std::vector<float> calculateNormalisedHistogram(ImageMatrixYUV imageData, int width, int height);
std::vector<float> calculateCDFHistogram(std::vector<float> pixelHistogram);
ImageMatrixYUV transferFunctionHistogramEqualisation(ImageMatrixYUV yuvImageData, std::vector<float> cdfHistogram, int width, int height);
BucketFrequency calculateBucketHistogram(ImageMatrixYUV imageData, int width, int height);
std::vector<BucketLocations> calculateEqualisedBucketHistogram(BucketFrequency equalisedBucketHistogram, int width, int height);
ImageMatrixYUV remapEqualisedHistogram(std::vector<BucketLocations> equalisedBucketHistogram, ImageMatrixYUV imageData, int width, int height);


int main(int argc, char* argv[]) {
  int imageWidth = 750;
  int imageHeight = 422;

  std::string fileName = "City.raw";
  std::string transferWriteFileName = "output1c_transfer.raw";
  std::string bucketWriteFileName = "output1c_bucket.raw";

  if (argc < 4) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name input_image.raw output_image_transfer.raw output_image_bucket" << std::endl;
    return 0;
  }

  fileName = argv[1];

  ImageMatrixRGB imageData = readFile(fileName, imageWidth, imageHeight);

  ImageMatrixYUV yuvImageData = mapRGBtoYUVSpace(imageData, imageWidth, imageHeight);

  std::vector<float> yChannelHistogram = calculateNormalisedHistogram(yuvImageData, imageWidth, imageHeight);

  std::vector<float> yChannelCDFHistogram = calculateCDFHistogram(yChannelHistogram);

  ImageMatrixYUV yuvTransferImageData = transferFunctionHistogramEqualisation(yuvImageData, yChannelCDFHistogram, imageWidth, imageHeight);

  ImageMatrixRGB transferImageData = mapYUVtoRGBSpace(yuvTransferImageData, imageWidth, imageHeight);

  transferWriteFileName = argv[2];
  writeFile(transferWriteFileName, transferImageData, imageWidth, imageHeight);

  BucketFrequency yBucketHistogram = calculateBucketHistogram(yuvImageData, imageWidth, imageHeight);

  std::vector<BucketLocations> yEqualisedBucketHistogram = calculateEqualisedBucketHistogram(yBucketHistogram, imageWidth, imageHeight);

  ImageMatrixYUV yuvBucketImageData = remapEqualisedHistogram(yEqualisedBucketHistogram, yuvImageData, imageWidth, imageHeight);

  ImageMatrixRGB rgbBucketImageData = mapYUVtoRGBSpace(yuvBucketImageData, imageWidth, imageHeight);

  bucketWriteFileName = argv[3];
  writeFile(bucketWriteFileName, rgbBucketImageData, imageWidth, imageHeight);

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

std::vector<float> calculateNormalisedHistogram(ImageMatrixYUV imageData, int width, int height) {
  std::vector<int> pixelHistogram(256, 0);
  int imageSize = width * height;

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      pixelHistogram[imageData[i][j].y] += 1;
    }
  }

  std::vector<float> normalisedHistogram(256, 0);

  for (int i = 0; i < pixelHistogram.size(); i++) {
    normalisedHistogram[i] = (pixelHistogram[i] / (float)imageSize);
  }

  return normalisedHistogram;
}

std::vector<float> calculateCDFHistogram(std::vector<float> pixelHistogram) {
  std::vector<float> cdfHistogram(256, 0);

  cdfHistogram[0] = pixelHistogram[0];

  for (int i = 1; i < pixelHistogram.size(); i++) {
    cdfHistogram[i] = cdfHistogram[i - 1] + pixelHistogram[i];
  }

  return cdfHistogram;
}

ImageMatrixYUV transferFunctionHistogramEqualisation(ImageMatrixYUV yuvImageData, std::vector<float> cdfHistogram, int width, int height) {
  ImageMatrixYUV remappedImageData(height, ImageVectorYUV(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int8 updatedY = channelRounding(255 * cdfHistogram[yuvImageData[i][j].y]);
      int8 u = yuvImageData[i][j].u;
      int8 v = yuvImageData[i][j].v;

      remappedImageData[i][j] = YUV(updatedY, u, v);
    }
  }

  return remappedImageData;
}

BucketFrequency calculateBucketHistogram(ImageMatrixYUV imageData, int width, int height) {
  BucketFrequency bucketHistogram(256);

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int pixelIndex = imageData[i][j].y;

      PixelLocation loc;

      loc.push_back(i);
      loc.push_back(j);

      bucketHistogram[pixelIndex].push_back(loc);
    }
  }

  return bucketHistogram;
}

std::vector<BucketLocations> calculateEqualisedBucketHistogram(BucketFrequency bucketHistogram, int width, int height) {
  std::vector<BucketLocations> finalHistogram(256);

  int bucketSize = ceil((width * height) / 256.0);

  int bucketIndex = 0;

  for (int i = 0; i < bucketHistogram.size(); i++) {
    BucketLocations locations = bucketHistogram[i];

    for (int i = 0; i < locations.size(); i++) {
      std::vector<int> loc;

      loc.push_back(locations[i][0]);
      loc.push_back(locations[i][1]);

      finalHistogram[bucketIndex].push_back(loc);

      if (finalHistogram[bucketIndex].size() >= bucketSize) {
        bucketIndex++;
      }
    }
  }

  return finalHistogram;
}

ImageMatrixYUV remapEqualisedHistogram(std::vector<BucketLocations> equalisedBucketHistogram, ImageMatrixYUV imageData, int width, int height) {
  ImageMatrixYUV yuvImageData(height, ImageVectorYUV(width));

  for (int i = 0; i < equalisedBucketHistogram.size(); i++) {
    for (int j = 0; j < equalisedBucketHistogram[i].size(); j++) {
      int x_coord = equalisedBucketHistogram[i][j][0];
      int y_coord = equalisedBucketHistogram[i][j][1];

      int8 y = i;
      int8 u = imageData[x_coord][y_coord].u;
      int8 v = imageData[x_coord][y_coord].v;

      yuvImageData[x_coord][y_coord] = YUV(y, u, v);
    }
  }

  return yuvImageData;
}
