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
typedef std::vector<std::pair<double, double>> MatchVector;

ImageMatrixRGB readFile(std::string fileName, int width, int height);
void writeFile(std::string fileName, ImageMatrixRGB imageData, int width, int height);
ImageMatrixRGB createPanoramaCanvas(ImageMatrixRGB leftImage, ImageMatrixRGB middleImage, ImageMatrixRGB rightImage, int width, int height);
ImageMatrixRGB homographicTransform(ImageMatrixRGB panoramaImageData, int width, int height);

int main(int argc, char* argv[]) {
  int imageWidth = 605;
  int imageHeight = 454;

  std::string fileName1 = "toy_left.raw";
  std::string fileName2 = "toy_middle.raw";
  std::string fileName3 = "toy_right.raw";
  std::string writeFileName = "output.raw";

  if (argc < 5) {
    std::cout << "Syntax Error - Incorrect Parameter Usage: " << std::endl;
    std::cout << "program_name input_image1.raw input_image2.raw input_image3.raw output.raw" << std::endl;
    return 0;
  }

  fileName1 = argv[1];
  fileName2 = argv[2];
  fileName3 = argv[3];
  writeFileName = argv[4];

  ImageMatrixRGB leftImageData = readFile(fileName1, imageWidth, imageHeight);
  ImageMatrixRGB middleImageData = readFile(fileName2, imageWidth, imageHeight);
  ImageMatrixRGB rightImageData = readFile(fileName3, imageWidth, imageHeight);

  std::cout << "Create Panorama" << std::endl;
  ImageMatrixRGB panoramaImageData = createPanoramaCanvas(leftImageData, middleImageData, rightImageData, imageWidth, imageHeight);

  std::cout << "Panorama Stitching" << std::endl;
  ImageMatrixRGB homographicImageData = homographicTransform(panoramaImageData, imageWidth, imageHeight);

  writeFile("panoramaCanvas.raw", panoramaImageData, imageWidth * 4, imageHeight * 3);
  writeFile(writeFileName, homographicImageData, imageWidth * 4, imageHeight * 3);

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

ImageMatrixRGB createPanoramaCanvas(ImageMatrixRGB leftImage, ImageMatrixRGB middleImage, ImageMatrixRGB rightImage, int width, int height) {
  ImageMatrixRGB panoramaImageData(height * 3, ImageVectorRGB(width * 4));

  int middleOffset = (width + width / 2);

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int x = i + height; // 454
      int yl = j;
      int ym = j + middleOffset; //907
      int yr = j + (3 * width); // 1815


      panoramaImageData[x][yl] = leftImage[i][j];
      panoramaImageData[x][ym] = middleImage[i][j];
      panoramaImageData[x][yr] = rightImage[i][j];
    }
  }

  return panoramaImageData;
}

ImageMatrixRGB homographicTransform(ImageMatrixRGB panoramaImageData, int width, int height) {
  ImageMatrixRGB finalImageData(height * 3, ImageVectorRGB(width * 4));

  int middleOffset = (width + width / 2);

  // std::vector<std::vector<double>> H_left = {
    // {1.1667,-0.1543,-64.4870},
    // {0.1497,1.1173,503.6963},
    // {0.0001,-0.0000,1},
  // }; // Works

  /* Best match 1 (so far)
  std::vector<std::vector<double>> H_left = {
    {2.6410,0.4691,-865.9794},
    {0.9046,3.1012,-48.8959},
    {0.0007,0.0009,1}
  };

  std::vector<std::vector<double>> H_right = {
    {1.2417,0.1473,-301.8879},
    {0.1551,1.1348,-713.4715},
    {0.0002,0.0000,1}
  };
  */

  // std::vector<std::vector<double>> H_left = {
  //   {1.1667,-0.1543,-64.4870},
  //   {0.1497,1.1173,503.6963},
  //   {0.0001,-0.0000,1},
  // };

  std::vector<std::vector<double>> H_left = {
    {1.3090739831903, 0.0454629682658674, -190.763783377129},
    {0.170948481422893, 1.41653884789698, 481.475161607742},
    {8.8436606226852e-05, 0.000210089874349874, 1}
  };

  std::vector<std::vector<double>> H_right = {
    { 0.822023992972014, -0.00943336835980101, 26.5897704982926},
    { 0.0506800356689619, 0.556389093191757, -21.5351362513676},
    { 7.04697512817463e-05, -0.000133751108324279, 1 }
  };


  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int xm = i + height;
      int ym = j + (width + width / 2);
      finalImageData[xm][ym] = panoramaImageData[xm][ym];

      int lx = i + height;
      int ly = j;

      double lw = lx * H_left[2][0] + ly * H_left[2][1] + H_left[2][2];
      int lx_dash = round((lx * H_left[0][0] + ly * H_left[0][1] + H_left[0][2]) / lw);
      int ly_dash = round((lx * H_left[1][0] + ly * H_left[1][1] + H_left[1][2]) / lw);

      if (lx_dash >= height && lx_dash < height * 2 && ly_dash >= middleOffset && ly_dash < middleOffset + width) {
        int8 r = round((panoramaImageData[lx][ly].r + panoramaImageData[lx_dash][ly_dash].r) / 2);
        int8 g = round((panoramaImageData[lx][ly].g + panoramaImageData[lx_dash][ly_dash].g) / 2);
        int8 b = round((panoramaImageData[lx][ly].b + panoramaImageData[lx_dash][ly_dash].b) / 2);

        finalImageData[lx_dash][ly_dash] = RGB(r, g, b);
      }
      else {
        finalImageData[lx_dash][ly_dash] = panoramaImageData[lx][ly];
      }

      int rx = i + height;
      int ry = j + (3 * width);

      double rw = rx * H_right[2][0] + ry * H_right[2][1] + H_right[2][2];
      int rx_dash = round((rx * H_right[0][0] + ry * H_right[0][1] + H_right[0][2]) / rw);
      int ry_dash = round((rx * H_right[1][0] + ry * H_right[1][1] + H_right[1][2]) / rw);

      if (rx_dash >= height && rx_dash < height * 2 && ry_dash >= middleOffset && ry_dash < middleOffset + width) {
        int8 r = round((panoramaImageData[rx][ry].r + panoramaImageData[rx_dash][ry_dash].r) / 2);
        int8 g = round((panoramaImageData[rx][ry].g + panoramaImageData[rx_dash][ry_dash].g) / 2);
        int8 b = round((panoramaImageData[rx][ry].b + panoramaImageData[rx_dash][ry_dash].b) / 2);

        finalImageData[rx_dash][ry_dash] = RGB(r, g, b);
      }
      else {
        finalImageData[rx_dash][ry_dash] = panoramaImageData[rx][ry];
      }
    }
  }

  return finalImageData;
}
