# Assignment 4

## [Question 1 - Texture Analysis](<codes/Question 1/>)

### Full Question

- Feature Extraction and Texture Classification
- Method
  - Feature Extraction
    - Read raw files
    - Get image means
    - Subtract image means
    - Apply Laws filters(5x5) to obtain 2D vector for each image
      - Pad the image
      - Run Each laws filter
      - Calculate image energy after each filter
    - Run PCA to obtain 3D features
  - Texture Classification
    - Classificaiton using mahalanobis distance using centroids and calculate error rate
    - Run Kmeans on 25D vector and then on 3D and classify images based on distance and calculate respective error rates
    - Run SVM on 25D vector and then on 3D and classify images based on distance and calculate respective error rates
- [Code](<codes/Question 1/Prog1.m>)

## [Question 2 -Texture Segmentation](<codes/Question 2/>)

### Full Question

- Texture Segmentation
- Method
  - Read raw file
  - Obtain image mean
  - Subtract image mean
  - Run Laws Filters (5x5) on windows to obtain energies for each pixel
  - Normalise energy features
  - Run Kmeans to obtain segmented iamge
  - Run PCA to reduce dimensions
  - Run Keamsnon reduced dimensions to obtain segmented image
- [Code](<codes/Question 1/Prog2.m>)

## [Question 3 - SIFT and Image Matching](<codes/Question 3/>)

### Full Question

- SIFT and Feature Matching in images
- Method
  - Image Matching 
    - Read raw images
    - Convert color images to gray images
    - Run SIFT to obtain keypoints and descriptors
    - Plot each image features
    - Run matching on each pair of images and plot the matches
  - Bag of Words 
    - Obtain images 
    - Concatenate and run kmeans to obtain classification
    - Obtain histograms based on kmeans labels for bag of words 
    - Use histogram to calculate similarity index
- [Code](<codes/Question 3/Prog3.m>)
