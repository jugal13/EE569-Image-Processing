# Image 768 x 512 x 1
g++ Prog2b.cpp && \
echo "Filter Size: $1 x $1, SigmaC: $2, SigmaS: $3" && \
./a.out ../../../images/Flower_gray.raw ../../../images/Flower_gray_noisy.raw Output.raw $1 $2 $3 
