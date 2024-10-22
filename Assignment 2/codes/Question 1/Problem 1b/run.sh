g++ Prog1b.cpp -std=c++11 `pkg-config  --cflags --libs opencv4` && \
echo "Image $1,\nGaussian Filter Size $2 x $2,\nSigma $3,\nLower Threshold $4,\nHigher Threshold $5" && \
./a.out ../../../images/$1.raw Output.raw $2 $3 $4 $5
