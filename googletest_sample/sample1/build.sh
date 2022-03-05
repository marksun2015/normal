#$ git clone https://github.com/google/googletest
#$ cd googletest
#$ mkdir build
#$ cd build
#$ cmake -DCMAKE_CXX_FLAGS=-std=c++11 -Dgtest_build_samples=ON ..
#$ make -j4
#$ make install

g++ sample1.cpp -o sample1 -Igtest/include -Lgtest/lib -lgtest -lpthread
