# Image 768 x 512 x 3
g++ Prog2d.cpp -std=c++11 `pkg-config  --cflags --libs opencv4` -o noise && \
echo "Filter Size: $1 x $1, Sigma: $2" && \
./noise ../../../images/Flower.raw ../../../images/Flower_noisy.raw Output.raw $1 $2 
