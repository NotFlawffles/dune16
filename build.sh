mkdir -p build
cd build
cmake .. -G Ninja
ninja -j 16
cd ..
