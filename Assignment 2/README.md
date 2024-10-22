# Assignment 2

## [Question 1 - Edge Detection](<Assignment 2/codes/Question 1/>)

### Part A

- Sobel Edge Detector
- Method
  - Read raw file
  - Convert RGB image to grayscale
  - Apply mirror padding to prevent reduction in size due to convolution
  - Run G~x Sobel filter
  - Run G~y Sobel filter
  - Normalise the two images
  - Calculate the magnitude i.e G = sqrt(G~x^2 + G~y^2)
  - Select pixels with intensity greater than threshold
  - Generate Edge map
  - Write edge map to file
- [Code](<Assignment 2/codes/Question 1/Problem 1a/Prog1a.cpp>)

### Part B

- Canny Edge Detector
- Method
  - Read raw file
  - Convert RGB image to grayscale
  - Apply mirror padding to prevent reduction in size due to convolution
  - Run Canny Edge detector (using Opencv)
  - Write edge map to file
- [Code](<Assignment 2/codes/Question 1/Problem 1b/Prog1b.cpp>)

### Part C

- Structured Edge
- Method
  - Train structured edge model on BSDS500 (Model code provided by library)
  - Read raw image
  - Run edge detetion using trained model
- [Code](<Assignment 2/codes/Question 1/Problem 1c/Prog1c.m>)

### Part D

- Performace Evaluation
- Method
  - Obtain probabilty maps for each edge detector
  - Using the ground truths provided evualate and obtain precision, recall and F scores
  - Plot F scores
- [Code](<Assignment 2/codes/Question 1/Problem 1d/Prog1d.m>)

## [Question 2 - Digital Half-toning](<Assignment 2/codes/Question 2/>)

### Part A

- Dithering
  - Method 1 - Fixed Thresholding
  - Method 2 - Random Thresholding
  - Method 3 - Dithering Matrix
- Method
  - Read raw files (noise free image and noisy image)
  - Method 1 - Fixed Thresholding
    - Run fixed thresholding (I > 127)
  - Method 2 - Random Thresholding
    - Run random thresholding (I > rand(x, y))
  - Method 3 - Dithering Matrix
    - Generate dithering matrix based on input size
    - Generate threshold matrix
    - Run dithering using the threshold matrix
  - Write the dithered image to a file
- [Code](<Assignment 1/codes/Question 2/Problem 2a/Prog2a.cpp>)

### Part B

- Error Diffusion
  - Method 1 - Floyd-Stengberg
  - Method 2 - Jarvis, Judice, and Ninke (JJN)
  - Method 3 - Stucki
- Method
  - Read raw files (noise free and noisy images)
  - Generate error diffusion matrix based on input method
  - Generate filter and run convolution using the filter
  - Apply mirror padding to prevent reduction in size due to convolution
  - Run convolution using serpentine scanning to propogate error diffusion
  - Write dithered image to file
- [Code](<Assignment 2/codes/Question 2/Problem 2b/Prog2b.cpp>)

## [Question 3 - Color Half-toning with Error Diffusion](<Assignment 2/codes/Question 3/>)

### Part A

- Separable Error Diffusion
- Method
  - Read raw file
  - Normalise image data from 0-255 to 0-1 scale
  - Convert RGB to CMY space
  - Apply mirror padding to prevent reduction in size due to convolution
  - Create Error diffusion matrix
  - Run dithering using the error diffusion matrix on each channel
  - Convert CMY to RGB space
  - Scale the data from 0-1 to 0-255 scale
  - Write image to file
- [Code](<Assignment 2/codes/Question 3/Problem 3a/Prog3a.cpp>)

### Part B

- MBVQ-based Error diffusion
- Method
  - Read raw file
  - Apply mirror padding to prevent reduction in size due to convolution
  - Create Error diffusion matrix
  - Run dithering using the MBVQ and nearest vertex algorithm
  - Write image to file
- [Code](<Assignment 2/codes/Question 3/Problem 3b/Prog3b.cpp>)
