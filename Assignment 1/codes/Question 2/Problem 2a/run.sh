# Image 768 x 512 x 1
g++ Prog2a.cpp && \
echo "Mean" && \
./a.out ../../../images/Flower_gray.raw ../../../images/Flower_gray_noisy.raw Mean.raw mean && \
echo "Gaussian Filter 3x3 Sigma 1" && \
./a.out ../../../images/Flower_gray.raw ../../../images/Flower_gray_noisy.raw Gaussian3.raw gaussian 3 1 && \
echo "Gaussian Filter 3x3 Sigma 1" && \
./a.out ../../../images/Flower_gray.raw ../../../images/Flower_gray_noisy.raw Gaussian5.raw gaussian 5 1
