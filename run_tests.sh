mkdir -p build && cd build
cmake -G Ninja ..
ninja
ninja test
cd ..
