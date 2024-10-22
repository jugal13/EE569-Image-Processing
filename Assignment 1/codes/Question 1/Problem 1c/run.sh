# Input Image 750 x 422 x 3
# Output Image 750 x 422 x 3
g++ prob1c.cpp && ./a.out ../../../images/City.raw Transfer.raw Bucket.raw

g++ prob1c_clahe.cpp -std=c++11 `pkg-config  --cflags --libs opencv4` -o clahe && ./clahe ../../../images/City.raw Clahe.raw
