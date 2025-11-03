cmake -B build/vc2022 -G "Visual Studio 17 2022"
cd build/vc2022
msbuild EasyTranslate-BaseExample.sln /p:Configuration=Release /p:Platform=x64
cmake --install .
cd install/bin
EasyTranslate-BaseExample
