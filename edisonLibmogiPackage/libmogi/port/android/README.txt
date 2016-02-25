To build the static libraries for android, simply run the script "build.sh", specifying the path to the android ndk as a command line argument.

./build.sh -n /path/to/android/ndk

Optionally, the directory to assimp's source file may be set to generate assimp libraries.

./build.sh -n /path/to/android/ndk -a /path/to/assimp

Optionally, a destination directory may be set to which all library files will be copied.

./build.sh -n /path/to/android/ndk -c /path/to/copy/destination

Currently cmake is not fully compatible on all computers due to some hardcoded directories for arbitrary libAssimp locations, as notable in libmogi/simulation/CmakeLists.txt  
