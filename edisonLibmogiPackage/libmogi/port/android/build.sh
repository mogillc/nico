#!/bin/bash

#
# Build libMogi for Android
# Author: Adrian Lizarraga


print_usage_and_exit() {
	echo "Invalid number of arguments."
	echo "Usage: $0 -n [android_ndk_path] -a [assimp_path] -c [copy_location]"
	echo " - android_ndk_path   The path to the android NDK (REQUIRED)"
	echo " - assimp_path		The path the the assimp folder containing src, include files (OPTIONAL)"
	echo " - copy_location      The path to copy the libraries to (OPTIONAL)"
	exit 1
} 

get_abs_filename() {
  # $1 : relative filename
  echo "$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
}

get_abs_directory() {
  # $1 : relative filename
  echo "$(cd "$(dirname "$1")" && pwd)/"
}


if [[ $# -gt 6 || $# -lt 1 ]] ; then
	print_usage_and_exit
fi

# Parse command line arguments
# See: http://stackoverflow.com/questions/192249/how-do-i-parse-command-line-arguments-in-bash
# Use > 1 to consume two arguments per pass in the loop (e.g. each
# argument has a corresponding value to go with it).
# Use > 0 to consume one or more arguments per pass in the loop (e.g.
# some arguments don't have a corresponding value to go with it).
# note: if this is set to > 0 the /etc/hosts part is not recognized ( may be a bug )
while [[ $# > 1 ]]
do
key="$1"

case $key in
    -n|--ndk)
    ANDROID_NDK_PATH=$(get_abs_filename $2)
    shift # past argument
    ;;
    -a|--assimp)
    ASSIMP_PATH=$(get_abs_filename $2)
    shift # past argument
    ;;
    -c|--copy)
    COPY_PATH=$(get_abs_filename $2)
    shift # past argument
    ;;
    --default)
    DEFAULT=YES
    ;;
    *)
            # unknown option
    ;;
esac
shift # past argument or value
done

echo "                          "
echo "Android ndk: ${ANDROID_NDK_PATH}"
echo "Assimp: ${ASSIMP_PATH}"
echo "Copy to: ${COPY_PATH}"


BUILD_ROOT_PATH=$(get_abs_directory "$0")
BUILD_DIRECTORY=$(get_abs_filename "lib")	# output directory name
ASSIMP_BUILD_DIRECTORY=$(get_abs_filename "lib-assimp") # output directory for compiled assimp libraries
ARCHITECTURES=("arm64-v8a" "armeabi-v7a") #IMPORTANT: List of supported android architectures
ANDROID_TOOLCHAIN_FILE=$(get_abs_filename "android-cmake/toolchain/android.toolchain.cmake")
echo "Android toolchain: ${ANDROID_TOOLCHAIN_FILE}"


#########################################
# Build assimp library for android if the user has specified path to assimp source files
#########################################

if [ -n "$ASSIMP_PATH" ]; then
	echo "                          "
	echo "--------------------------"
	echo "Buiding assimp library ..."
	echo "--------------------------"

	if [ ! -d ${ASSIMP_BUILD_DIRECTORY}  ]; then
		echo " - Creating ${ASSIMP_BUILD_DIRECTORY}"
		mkdir ${ASSIMP_BUILD_DIRECTORY}
	fi

	cd ${ASSIMP_BUILD_DIRECTORY}

	for arch in ${ARCHITECTURES[*]}
	do
		# Create a directory for this architecture
		if [ ! -d ${arch}  ]; then
			echo " - Creating ${arch}"
			mkdir ${arch}
		fi

		#cd into arch directory
		cd ${arch}
	
		echo " "
		# cmake
		if [ "${arch}" == "arm64-v8a" ]; then
			echo " - Running cmake for architecture: ${arch}"

			cmake -DASSIMP_ANDROID_JNIIOSYSTEM=ON -DBUILD_SHARED_LIBS=OFF -DANDROID_NDK=${ANDROID_NDK_PATH} -DANDROID_ABI="arm64-v8a" -DANDROID_STL=gnustl_static -DANDROID_TOOLCHAIN_NAME=aarch64-linux-android-4.9 -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${ANDROID_TOOLCHAIN_FILE} ${ASSIMP_PATH}

			cmake --build .

		elif [ "${arch}" == "armeabi-v7a" ]; then
			echo " - Running cmake for architecture: ${arch}"
			echo " - Using android-native-api 21 because Dynamixel/Com.cpp needs <linux/serial.h>"

			cmake -DASSIMP_ANDROID_JNIIOSYSTEM=ON -DBUILD_SHARED_LIBS=OFF -DANDROID_NDK=${ANDROID_NDK_PATH} -DANDROID_ABI="armeabi-v7a" -DANDROID_STL=gnustl_static -DANDROID_NATIVE_API_LEVEL="android-21" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${ANDROID_TOOLCHAIN_FILE} ${ASSIMP_PATH}

			cmake --build .
		else
			echo " - Running cmake for unexpected architecture: ${arch}"
			cmake -DASSIMP_ANDROID_JNIIOSYSTEM=ON -DBUILD_SHARED_LIBS=OFF -DANDROID_NDK=${ANDROID_NDK_PATH} -DANDROID_ABI="${arch}" -DANDROID_STL=gnustl_static -DANDROID_NATIVE_API_LEVEL="android-21" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${ANDROID_TOOLCHAIN_FILE} ${ASSIMP_PATH}

			cmake --build .
		fi
	
		cd .. # Go back up to ${ASSIMP_BUILD_DIRECTORY}
	done

	cd ${BUILD_ROOT_PATH}
else
	echo "--------------------------"
	echo "I hope you already built assimp for android (lib-assimp/) ..."
	echo "--------------------------"
fi

#########################################
# Build libmogi
#########################################

echo "                          "
echo "--------------------------"
echo "Buiding libmogi library ..."
echo "--------------------------"

if [ ! -d ${BUILD_DIRECTORY}  ]; then
	echo "Creating ${BUILD_DIRECTORY}"
	mkdir ${BUILD_DIRECTORY}
fi

cd ${BUILD_DIRECTORY}

for arch in ${ARCHITECTURES[*]}
do
	# Create a directory for this architecture
	if [ ! -d ${arch}  ]; then
		echo " - Creating ${arch}"
		mkdir ${arch}
	fi

	#cd into arch directory
	cd ${arch}
	
	echo " "
	# cmake
	if [ "${arch}" == "arm64-v8a" ]; then
		echo " - Running cmake for architecture: ${arch}"

		cmake -DBUILD_SIMULATION=ON -DBUILD_WITH_SDL2=OFF -DCMAKE_TOOLCHAIN_FILE=${ANDROID_TOOLCHAIN_FILE} -DANDROID_NDK=${ANDROID_NDK_PATH} -DANDROID_STL=gnustl_static -DANDROID_TOOLCHAIN_NAME=aarch64-linux-android-4.9 -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI="arm64-v8a" ../../../..

		cmake --build .

	elif [ "${arch}" == "armeabi-v7a" ]; then
		echo " - ARMEABI-V7A IS NOT SUPPORTED YET. GL draw buffers issue soon to be resolved"
		echo " - Running cmake for architecture: ${arch}"
		echo " - Using android-native-api 21 because Dynamixel/Com.cpp needs <linux/serial.h>"

		#cmake -DBUILD_SIMULATION=ON -DBUILD_WITH_SDL2=OFF -DCMAKE_TOOLCHAIN_FILE=../../android-cmake/toolchain/android.toolchain.cmake -DANDROID_NDK=${ANDROID_NDK_PATH} -DANDROID_STL=gnustl_static -DANDROID_NATIVE_API_LEVEL="android-21" -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI="armeabi-v7a" ../../../..

		#cmake --build .
	else
		echo " - Running cmake for unexpected architecture: ${arch}"
		#cmake -DBUILD_SIMULATION=ON -DBUILD_WITH_SDL2=OFF -DCMAKE_TOOLCHAIN_FILE=../../android-cmake/toolchain/android.toolchain.cmake -DANDROID_NDK=${ANDROID_NDK_PATH} -DANDROID_STL=gnustl_static -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI="${arch}" ../../../..

		#cmake --build .
	fi
	
	cd .. # Go back up to ${BUILD_DIRECTORY}
done

cd ${BUILD_ROOT_PATH} 

#########################################
# Copy lib files to specified directory
#########################################

if [ -n "$COPY_PATH" ]; then

	echo "                          "
	echo "--------------------------"
	echo "Copying libraries ..."
	echo "--------------------------"

	for arch in ${ARCHITECTURES[*]}
	do
		echo " " 
		
		# Create a directory for this architecture
		if [ ! -d "${COPY_PATH}/${arch}"  ]; then
			echo " - Creating ${COPY_PATH}/${arch}"
			mkdir "${COPY_PATH}/${arch}"
		fi

		libs_mogi=$(find "${BUILD_DIRECTORY}/${arch}" -name "*a" -type f)
		libs_assimp=$(find "${ASSIMP_BUILD_DIRECTORY}/${arch}" -name "*a" -type f)
		
		#combine libs
		all_libs=( ${libs_mogi[@]} ${libs_assimp[@]} )
		#echo " - All libs for ${arch}: ${all_libs}"

		# copy each lib
		for lib in ${all_libs[*]}
		do
			echo " - Copying ${lib} to ${COPY_PATH}/${arch}"
			cp ${lib} "${COPY_PATH}/${arch}"
		done
		
	done
fi

