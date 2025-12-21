# **Cmake Game Workflow Trial Run**

* [1. Cmake Notes](#1-cmake-notes)
    * [1.1. Build and Run in Windows](#11-build-and-run-in-windows)
    * [1.1. Build and Run in Windows](#12-build-and-run-in-linux)
* [2. SFML](#2-sfml)

## 1. Cmake Notes

**Why CMake?**

Really all CMake does is make Makefiles for you. Makefiles are just instructions used by Make to automate compiling, linking, and building binaries from source code. This might seem superfluous since you can just write Makefiles, but with CMakeLists.txt files are often only a few lines of code while Makefiles are hundreds to thousands of lines of code. You can test this by building this project and looking at the Makefile in the build directory.

Using CMake allows people like me to share silly little projects like this with people like you, and give you the ability to alter the source code and build the binaries on your own without having to write hundreds of lines of Make instructions.


### 1.1. Build and Run in Windows
This goes without saying but you will need to install CMake whether you are on Linux or Windows.

If you are using VScode I would highly recommend using the CMake plugin. If you are using the CMake plugin make sure you have CMake set as your configuration provider or there will be some linker diagnostics errors (Ctrl+Shift+P - Change configuration provider). 

Since this game is cross-compatible with Linux you will need to use GCC to compile it. You need to install GCC (needs to be matching GCC version from SFML download page) on windows as the default C++ compiler is MSVC. You need to add to add the location of the GCC binaries to your system path, and configure CMake to use GCC rather than MSVC. (Ctrl+Shift+P - Select a Kit)

If you are using VScode and the CMake plugin you should now be able to just use `F5` to build and run the game (don't quote me on this, I have not used VScode in a while)

**How to build in the terminal without the VScode CMake plugin**
1. `cmake -G "MinGW Makefiles" ..` builds the Makefile and CMake files in the current directory (using GCC) if the CMakeLists.txt is one level above.
2. `mingw32-make` runs the Makefile and spits out the binary into the current directory.
3. If this builds successfully you can run the exe in the current directory using `./filename.exe` where the filename is your project name


### 1.2 Build and Run in Linux
In Linux you will just have to install GCC, CMake, Make, and SFML to build and run this program. That is pretty much all for setup!

**How to build in the terminal**
1. `cmake ..` builds the Makefile and CMake files in the current directory (using GCC) if the CMakeLists.txt is one level above.
2. `make .` runs the Makefile and spits out the binary into the current directory.
3. If this builds successfully you can run the binary in the current directory using `./filename` where the filename is your project name


## 2. SFML
SFML is included as a shared/dynamic library in this project. This means in Linux, SMFL should be installed (adding the libraries to system bin), or in windows that you need to download the SFML library and add the bin directory to your system path.


There are only 2 lines that are required to set up SFML in the CMakeLists.txt:
- `find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)` this will find the SFML CMake package
- `target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)` links the found libraries (in order)
- `set(SFML_STATIC_LIBRARIES TRUE)` this is not required if linking dynamically. If you turn this on and drop the necessary SFML files (SFML include and the static libraries) than you will link statically and the build will be relitively portable.
