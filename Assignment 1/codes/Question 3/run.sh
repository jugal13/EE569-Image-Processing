# Image 768 x 512 x 3
g++ Prog3.cpp && \
echo "Median Filter Size $1 x $1, Bilateral Filtering $2 times" && \
./a.out ../../images/Flower.raw Output.raw $1 $2
