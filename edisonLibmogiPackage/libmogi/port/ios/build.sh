#!/bin/bash

#
# Build libMogi for iOS

if [[ $# -gt 1 ]] ; then
	echo "Invalid number of arguments."
	echo "Usage: $0 [copy_location]"
	echo " - copy_location     The path to copy the libraries to"
	exit 1
fi

SDK_OS="iphoneos9.3"
SDK_SIMULATOR="iphonesimulator9.3"

BUILD_DIRECTORY="build-ios"	# output directory name

#if [ -d ${BUILD_DIRECTORY}  ]; then
#	rm -rf ${BUILD_DIRECTORY}
#fi

if [ ! -d ${BUILD_DIRECTORY}  ]; then
	echo "Creating ${BUILD_DIRECTORY}"
	mkdir ${BUILD_DIRECTORY}
fi

cd ${BUILD_DIRECTORY}
cmake -DCMAKE_TOOLCHAIN_FILE=../iOS.cmake ../../.. -DIOS_PLATFORM=OS -DBUILD_FOR_IOS=ON -DBUILD_SIMULATION=ON -DBUILD_WITH_SDL2=OFF -G Xcode
xcodebuild -target mogi-static -configuration Debug -sdk ${SDK_OS}
xcodebuild -target mbsimulation-static -configuration Debug -sdk ${SDK_OS}
cd ..

cd ${BUILD_DIRECTORY}
cmake -DCMAKE_TOOLCHAIN_FILE=../iOS.cmake ../../.. -DIOS_PLATFORM=SIMULATOR -DBUILD_FOR_IOS=ON -DBUILD_SIMULATION=ON -DBUILD_WITH_SDL2=OFF -G Xcode
xcodebuild -target mogi-static -configuration Debug -sdk ${SDK_SIMULATOR}
xcodebuild -target mbsimulation-static -configuration Debug -sdk ${SDK_SIMULATOR}
cd ..

lipo -create ${BUILD_DIRECTORY}/simulation/Debug-iphoneos/libmbsimulation.a ${BUILD_DIRECTORY}/simulation/Debug-iphonesimulator/libmbsimulation.a -output libmogisim.a
lipo -create ${BUILD_DIRECTORY}/Debug-iphoneos/libmogi.a ${BUILD_DIRECTORY}/Debug-iphonesimulator/libmogi.a -output libmogi.a

if [[ $# -eq 1 ]] ; then
	cp libmogisim.a $1
	cp libmogi.a $1
fi