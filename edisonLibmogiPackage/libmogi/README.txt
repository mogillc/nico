Hexapod Libraries
Matt Bunting
12centdwarf <at> gmail <dot> com

Introduction:
	These libraries were designed to operate a 3DOF leg hexapod design using the Robotis Dynamixel series of motors.

Prerequisites:
	Must have the following libraries:
		libftdi1
		cmake

	To install cmake:
		sudo apt-get install cmake

	To install libftdi:
		sudo apt-get install libftdi-dev #Wrong version on ubuntu!
		
		cd ../libftdi1-1.2
		mkdir build
		cd build
		cmake ..
		make
		sudo make install

	To install libusb:
		sudo apt-get install libusb-dev 

	To install libjsoncpp
		sudo apt-get install libjsoncpp-dev
	
	To install GLUT
		sudo apt-get install freeglut3-dev

	To install GLEW
		sudo apt-get install libglew-dev

	To install SDL2 and SDL2-image
		sudo apt-get install libsdl2-dev
		sudo apt-get install libsdl2-image-dev

	To install GLUT Xmu
		sudo apt-get install libxmu-dev

	To install assimp
		sudo apt-get install libassimp-dev 

	To install curses (needed for examples)
		sudo apt-get install libncurses5-dev


	Additional libraries:
		libflycapture (download from point grey)

Installation:
	
1.)	Create a build directory:
		mkdir build
		cd build

2.)	Run Cmake:
		cmake ..

3.)	Build:
		make

4.)	Install:
		sudo make install
