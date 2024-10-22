g++ Prog3a.cpp -std=c++11 && \
echo "Size $1 x $1, Image: $2" && \
./a.out $1 $1 ../../../images/$2.raw output.raw
