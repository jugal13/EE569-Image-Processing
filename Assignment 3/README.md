# Assignment 2

## [Question 1 - Geometric Image Modification](<codes/Question 1/>)

### Full Question

- Warping and de-warping
- Method
  - Read raw file
  - Forward mapping for warping
  - Inverse mapping for dewarping
  - write warped and dewarped images to file
- [Code](<codes/Question 1/Prog1.cpp>)

## [Question 2 - Homographic Transformation and Image Stitching](<codes/Question 2/>)

### Full Question
- Homographic Transformation & Image Stitching 
- Method
  - SURF Detection (Using OpenCV)
    - Read raw files (left, middle and right images)
    - Obtain keypoints and descriptors for each image
    - Run matching for left and middle
    - Run matching for middle and right
    - Use thresholding to obtain good matches
    - Choose best 4 matches
    - Calculate homographic matrix for left transform and right transform 
  - Image Stitching
    - Read raw files (left, middle and right images)
    - Create panoramic canvas
    - Apply homographic transform on left and right images in canvas
    - Write stitched image to file
- [Code (SURF detection)](<codes/Question 2/Prog2_surf.cpp>)
- [Code (Stitching)](<codes/Question 1/Prog2.cpp>)


## [Question 3 - Morphological processing](<codes/Question 3/>)

### Part A

- Basic morphological process implementation
- Method
  - Read raw file
  - Binarise the image data
  - Normalise the image data
  - Zero padding for convolution
  - Run morphological thinning based on conditional and unconditional mark patterns
  - Denormalise the image data
  - Write image to file
- [Code](<codes/Question 3/Problem 3a/Prog3a.cpp>)

### Part B

- Shape detection and counting
- Method
  - Read raw file
  - Binarise the image
  - Normalise the image
  - Part 1
    - Invert the image
    - Run shrinking 
    - Count the holes as a pattern 3x3 grid with only the center as true value
  - Part 2
    - Invert the image
    - Obtain the centers using part 1
    - Fill the holes using the centers
    - Invert the image
    - Run shrinking
    - Count the holes as a pattern 3x3 grid with only the center as true value
  - Part 3 & 4
    - Invert the image
    - Obtain the centers using part 1
    - Fill the holes using the centers
    - Run breadth first search obtain islands and their size, if size equals area of bounding box its a rectangle otherwise a circle
    - Count the rectangles and circles
- [Code](<codes/Question 3/Problem 3b/Prog3b.m>)

## Part C

- Object Segmentation and Analysis
- Method
  - Read raw file
  - Convert to grayscale
  - Binarise the image
  - Invert the image
  - Find the connected components using 4 neighbors
  - Count the number of beans
  - Using the connected components labels count the size of the bean and sort by size
- [Code](<codes/Question 3/Problem 3c/Prog3c.m>)
