/*
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Jan 28th 2024
*/

#include <iostream>
#include <map>

typedef unsigned char int8;

typedef std::vector<int8> ImageVector;
typedef std::vector<std::vector<int8> > ImageMatrix;

typedef std::vector<int> PixelLocation;
typedef std::vector<PixelLocation> BucketLocations;
typedef std::map<int, BucketLocations> BucketFrequency;

ImageMatrix readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrix imageData, int width, int height);
std::vector<float> calculateNormalisedHistogram(ImageMatrix imageData, int width, int height);
std::vector<float> calculateCDFHistogram(std::vector<float> pixelHistogram);
int8 channelRounding(float value);
ImageMatrix transferFunctionHistogramEqualisation(ImageMatrix imageData, std::vector<float> cdfHistogram, int width, int height);
BucketFrequency calculateBucketHistogram(ImageMatrix imageData, int width, int height);
std::vector<BucketLocations> calculateEqualisedBucketHistogram(BucketFrequency equalisedBucketHistogram, int width, int height);
ImageMatrix remapEqualisedHistogram(std::vector<BucketLocations> equalisedBucketHistogram, int width, int height);

int main(int argc, char const* argv[]) {
  int imageWidth = 596;
  int imageHeight = 340;

  std::string fileName = "DimLight.raw";
  std::string transferWriteFileName = "output1b_transfer.raw";
  std::string bucketWriteFileName = "output1b_bucket.raw";

  if (argc < 4) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name input_image.raw output_image_transfer.raw output_image_bucket.raw" << std::endl;
    return 0;
  }

  fileName = argv[1];


  ImageMatrix imageData = readFile(fileName, imageWidth, imageHeight);

  // Transfer code
  std::cout << "Transfer method" << std::endl;
  std::cout << "Before" << std::endl;
  std::vector<float> pixelHistogram = calculateNormalisedHistogram(imageData, imageWidth, imageHeight);

  std::vector<float> cdfHistogram = calculateCDFHistogram(pixelHistogram);

  ImageMatrix remappedImageData = transferFunctionHistogramEqualisation(imageData, cdfHistogram, imageWidth, imageHeight);

  std::cout << "After" << std::endl;
  calculateNormalisedHistogram(remappedImageData, imageWidth, imageHeight);

  transferWriteFileName = argv[2];
  writeFile(transferWriteFileName, remappedImageData, imageWidth, imageHeight);

  // Bucket code 
  BucketFrequency bucketHistogram = calculateBucketHistogram(imageData, imageWidth, imageHeight);

  std::vector<BucketLocations> equalisedBucketHistogram = calculateEqualisedBucketHistogram(bucketHistogram, imageWidth, imageHeight);

  ImageMatrix remappedImageDataBucket = remapEqualisedHistogram(equalisedBucketHistogram, imageWidth, imageHeight);

  std::cout << "Bucket Method" << std::endl;
  std::cout << "After" << std::endl;
  calculateNormalisedHistogram(remappedImageDataBucket, imageWidth, imageHeight);

  bucketWriteFileName = argv[3];
  writeFile(bucketWriteFileName, remappedImageDataBucket, imageWidth, imageHeight);

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
      int8* data = &imageData[i][j];

      fwrite(data, sizeof(int8), 1, outfile);
    }
  }

  fclose(outfile);
}

std::vector<float> calculateNormalisedHistogram(ImageMatrix imageData, int width, int height) {
  std::vector<int> pixelHistogram(256, 0);
  int imageSize = width * height;

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      pixelHistogram[imageData[i][j]] += 1;
    }
  }

  std::cout << "i" << " " << "Intensity" << std::endl;
  for (int i = 0; i < pixelHistogram.size(); i++) {
    std::cout << i << " " << pixelHistogram[i] << std::endl;
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

ImageMatrix transferFunctionHistogramEqualisation(ImageMatrix imageData, std::vector<float> cdfHistogram, int width, int height) {
  ImageMatrix remappedImageData(height, ImageVector(width));

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      remappedImageData[i][j] = channelRounding(255 * cdfHistogram[imageData[i][j]]);
    }
  }

  return remappedImageData;
}

BucketFrequency calculateBucketHistogram(ImageMatrix imageData, int width, int height) {
  BucketFrequency pixelHistogram;

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int pixelIndex = imageData[i][j];

      PixelLocation loc;

      loc.push_back(i);
      loc.push_back(j);

      pixelHistogram[pixelIndex].push_back(loc);
    }
  }

  return pixelHistogram;
}

std::vector<BucketLocations> calculateEqualisedBucketHistogram(BucketFrequency bucketHistogram, int width, int height) {
  std::vector<BucketLocations> equalisedBucketHistogram(256);

  int bucketSize = ceil((width * height) / 256.0);

  BucketFrequency::iterator itr;
  int bucketIndex = 0;

  for (BucketFrequency::iterator itr = bucketHistogram.begin(); itr != bucketHistogram.end(); itr++) {
    int pixelIntensity = itr->first;
    BucketLocations locations = itr->second;

    for (int i = 0; i < locations.size(); i++) {
      equalisedBucketHistogram[bucketIndex].push_back(locations[i]);

      if (equalisedBucketHistogram[bucketIndex].size() >= bucketSize) {
        bucketIndex++;
      }
    }
  }

  return equalisedBucketHistogram;
}

ImageMatrix remapEqualisedHistogram(std::vector<BucketLocations> equalisedBucketHistogram, int width, int height) {
  ImageMatrix imageData(height, ImageVector(width));

  for (int i = 0; i < equalisedBucketHistogram.size(); i++) {
    for (int j = 0; j < equalisedBucketHistogram[i].size(); j++) {
      int x = equalisedBucketHistogram[i][j][0];
      int y = equalisedBucketHistogram[i][j][1];

      imageData[x][y] = i;
    }
  }

  return imageData;
}
