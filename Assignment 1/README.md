# Assignment 1

## [Question 1 - Image Demosaicing & Histogram Manipulation](<Assignment 1/codes/Question 1/>)

### Part A

- Bilinear Demosaicing
- Method
  - Read raw file
  - Apply mirror padding to prevent reduction in size due to convolution
  - Run convolution treating the input image as single channel and having bayer pattern to reconstruct the image
  - Write the reconstructed image to raw file
- [Code](<Assignment 1/codes/Question 1/Problem 1a/Prog1a.cpp>)

### Part B

- Histogram Manipulation
  - Method 1 - Transfer function method
  - Method 2 - Bucket filling method
- Single Program to run both methods
- Implemented for grayscale images (i.e. single channel images)
- Method
  - Read raw file
  - Method 1 - Transfer function method
    - Calculate the normalised histogram
    - Calculate the CDF distribution histogram
    - Apply transfer function for histogram equalisation I(x, y) = 255 \* CDF(I(x, y))
    - Write new image to raw file
  - Method 2 - Bucket filling method
    - Calculate the bucket histogram
    - Calculate the equalised bucket histogram (i.e positions to remap)
    - Remap the equalised pixel values
    - Write new image to raw file
- [Code](<Assignment 1/codes/Question 1/Problem 1b/Prog1b.cpp>)

### Part C

- Contrast Limited Adaptive Histogram Equalization (CLAHE) comparison
  - Method 1 - Transfer function method
  - Method 2 - Bucket filling method
  - Method 3 - CLAHE
- Implemented for color images
- Method
  - Read raw file
  - Transform image from RGB space to YUV space
  - Apply histogram equalisation algorithm on Y channel to obtain Y'
  - Replace YUV with Y'UV
  - Remap Y'UV to RGB space
  - Write new image to raw file
- [Code - Transfer & Bucket](<Assignment 1/codes/Question 1/Problem 1a/Prog1c.cpp>)
- [Code - CLAHE](<Assignment 1/codes/Question 1/Problem 1a/Prog1c_clahe.cpp>)

## [Question 2 - Image Denoising](<Assignment 1/codes/Question 2/>)

### Part A

- Simple Denoising
  - Low Pass Filter (Mean of n x n grid)
  - Gaussian Filter (n x n filter)
- Method
  - Read raw files (noise free image and noisy image)
  - Apply mirror padding to prevent reduction in size due to convolution
  - Generate filter and run convolution using the filter
  - Calculate mean square error & psnr
  - Write denoised image to file
- [Code](<Assignment 1/codes/Question 2/Problem 2a/Prog2a.cpp>)

### Part B

- Bilateral Filter Denoising
- Method
  - Read raw files (noise free and noisy images)
  - Apply mirror padding to prevent reduction in size due to convolution
  - Generate filter and run convolution using the filter
  - Calculate mean square error & psnr
  - Write denoised image to file
- [Code](<Assignment 1/codes/Question 2/Problem 2b/Prog2b.cpp>)

### Part C

- Non-local means Denoising
- Method
  - Read raw files (noise free and noisy images)
  - Apply mirror padding to prevent reduction in size due to convolution
  - Generate filter and run convolution using the filter (using opencv)
  - Calculate mean square error & psnr
  - Write denoised image to file
- [Code](<Assignment 1/codes/Question 2/Problem 2c/Prog2c.cpp>)

### Part D

- Color Denoising
- Method
  - Read raw files (noise free and noisy images)
  - Calculate each channel R,G,B historgram
  - Identify noise (Salt & Pepper Noise, Gaussian Noise)
  - Apply mirror padding to prevent reduction in size due to convolution
  - Run median filtering for salt and pepper noise
  - Apply mirror padding to prevent reduction in size due to convolution
  - Run gaussian filtering for bell curve noise
  - Calculate mean square error and psnr
  - Write denoised image to file
- [Code](<Assignment 1/codes/Question 2/Problem 2d/Prog2d.cpp>)

## [Question 3 - Special Effect Image Filter - Watercolor Painting Effect](<Assignment 1/codes/Question 3/>)

### Full Question

- Water Color Painting Effect
- Method
  - Read raw files (noise free and noisy images)
  - Two paths on original image
  - Path 1 (I<sub>m</sub>)
    - Apply mirror padding to prevent reduction in size due to convolution
    - Run median filtering
    - Run K times
      - Apply mirror padding to prevent reduction in size due to convolution
      - Run bilateral filtering
  - Path 2 (I<sub>g</sub>)
    - Apply mirror padding to prevent reduction in size due to convolution
    - Generate Gaussian filter and run convolution using the filter
  - Linear Combination (I = 1.4 \* I<sub>m</sub> + 0.4 \* I<sub>g</sub>)
  - Write combined image to file
- [Code](<Assignment 1/codes/Question 3/Problem 3.cpp>)
