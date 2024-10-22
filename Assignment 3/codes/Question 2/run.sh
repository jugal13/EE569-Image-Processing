g++ Prog2_surf.cpp -std=c++11 `pkg-config  --cflags --libs opencv4` -o surf && \
./surf ../../images/toys_left.raw ../../images/toys_middle.raw ../../images/toys_right.raw output.raw

g++ Prog2.cpp -std=c++11 && \
./a.out ../../images/toys_left.raw ../../images/toys_middle.raw ../../images/toys_right.raw output.raw
