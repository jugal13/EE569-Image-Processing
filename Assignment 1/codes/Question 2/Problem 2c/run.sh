# Image 768 x 512 x 1
g++ Prog2c.cpp -std=c++11 `pkg-config  --cflags --libs opencv4` -o nlm && \
./nlm ../../../images/Flower_gray.raw ../../../images/Flower_gray_noisy.raw Output.raw
