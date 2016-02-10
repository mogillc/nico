To build the static libraries for iOS, simply run the script "build.sh"

./build.sh

Optionally, a destination directory may be set, typically a project directory:
./build.sh ~/Desktop/testHexapod/testHexapod 

Currently cmake is not fully compatible on all computers due to some hardcoded directories for arbitrary libAssimp locations, as notable in libmogi/simulation/CmakeLists.txt  It is also unfortunately expected to have libassimp-fat.a and libzlibstatic.a from assimp iOS compile in the libmogi/simulation/ directory.