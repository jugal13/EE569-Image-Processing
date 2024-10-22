# Details
Name: Jugal Wadhwa
USC ID: 2979673763
Email: jwadhwa@usc.edu
Submission Date: Feb 19th 2024
Assignment: Homework 2

# System Details:
OS: macOS Sonama 14.2.1
Compiling: g++ (Version: 15.0.0)

# C++ 
# Folders:
Codes
- Q1
  -- Problem1a
     -- Prog1a.cpp
  -- Problem1b
     -- Prog1b.cpp
     -- Prog1b_probability_map.cpp 
- Q2
  -- Problem2a
     -- Prog2a.cpp
  -- Problem2b
     -- Prog2b.cpp
- Q3
  -- Problem3a
     -- Prog3a.cpp
  -- Problem3b
     -- Prog3b.cpp

# Compilation
- Please include opencv for question 1b - canny edge detector
- Canny file name - Prog1b.cpp

g++ program_name.cpp -o program_executable 

# Execution

# Question 1

# Problem 1 a:

./program_executable command_1 command_2 command_3

- command_1 : Input Image (Tiger.raw)
- command_2 : Output Image (egdeMap.raw)
- command_3 : Threshold [0-1]

# Problem 1 b:

Prog1b (Use to run canny detection)

./program_executable command_1 command_2 command_3 command_4 command_5 command_6

- command_1 : Input Image (Tiger.raw)
- command_2 : Output Image (edgeMap.raw) 
- command_3 : Filter Size (Gaussian)
- command_4 : Sigma (Gaussian)
- command_5 : Lower Threshold (Canny)
- command_6 : Higher Threshold (Canny)

Prog1b_probability_map (Use to generate probability map for 1d)

./program_executable command_1 command_2 command_3 command_4 command_5 command_6

- command_1 : Input Image (Tiger.raw)
- command_2 : Output Image (probabilityMap.raw) 

# Question 2

# Problem 2 a:

./program_executable command_1 command_2 command_3 (command_4)

- command_1 : Input Image (Lighthouse.raw)
- command_2 : Output Image (ditheredImage.raw)
- command_3 : Method (fixed | random | matrix)
- command_4 : Matrix Size - (2,4,8,16,32) (Supply if method is "matrix")

# Problem2b 

./program_executable command_1 command_2 command_3

- command_1 : Input Image (Lighthouse.raw)
- command_2 : Output Image (ditheredImage.raw)
- command_3 : Method (fs | jjn | stucki)

# Question 3

# Problem 3 a:

./program_executable command_1 command_2

- command_1 : Input Image (Bird.raw)
- command_2 : Output Image (diffused.raw)

# Problem 3 b:

./program_executable command_1 command_2

- command_1 : Input Image (Bird.raw)
- command_2 : Output Image (diffused.raw)

# Matlab
# Folders 

Codes
- Q1
  -- Problem1c
     -- Prog1c.m
  -- Problem1d
     -- Prog1d.m

# Problem1c

Image paths are hardocded as it was run on matlab online please update the path manually if need to run
Tiger image path
Pig image path

# Problem1d

Image paths are hardocded as it was run on matlab online please update the path manually if need to run

Tiger_GT.mat - Ground truth for tiger
Tiger_SE.mat - Probability map for tiger
tiger_sobel.raw - normalised gradient from sobel edge detector for tiger
tiger_canny.raw - generated output of canny from Prog1b
tiger_canny_prob.raw - generated output of canny probability map of tiger from Prog1b_probability_map

Pig_GT.mat - Ground truth for pig
Pig_SE.mat - Probability map for pig
pig_sobel.raw - normalised gradient from sobel edge detector for pig
pig_canny.raw - generated output of canny from Prog1b
pig_canny_prob.raw - generated output of canny probability map of pig from Prog1b_probability_map
