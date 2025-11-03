cmake -B build/mingw -G "MinGW Makefiles"
cd build/mingw
make -j
cmake --install .
cd install/bin
chmod +x EasyTranslate-BaseExample
EasyTranslate-BaseExample
