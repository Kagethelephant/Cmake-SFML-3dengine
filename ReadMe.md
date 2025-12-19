# **Cmake Game Workflow Trial Run**


 ### Cmake Notes

**Build and Run in Windows**
Make sure you have Cmake Tools as your configuration provider in vscode or there will be some include problems (Ctrl+Shift+P to change provider). 

to get GCC to work you have to download it from https://winlibs.com/ or the SFML website. I would recomend the latter if using SFML just so you know that they are compatable. you can then add the bin to the system path and search for kits with (ctrl+shift+P)

**How to build in the terminal withouth the vscode plugin**
1. `cmake -G "MinGW Makefiles" ..` builds the cmake files in the current directory if the CMakeLists.txt is one level above (`cmake ..` for linux)
2. `mingw32-make` to make the exe (`make` for linux)
3. If this builds succesfully you can run the exe in the current directory using `./filename.exe`

**Cmake file stuff**
`target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)` This will add an iinclude directory seperate from the CMAKE_SOURCE_DIR




### SFML

Make sure the SFML version matches the compiler. If you are using GCC i would recommend downloading the compiler from the SFML website.
Really all that you have to do for SFML is download the correct version, plop it wherever and add the path to the system path. On linux you should just be able to install it and it will automatically be added to the path. The hardest part about gettin SFML to work is in the CMakeLists.txt file.

If you look close at the pixels you can see that the pixels are a little blury. There is some problems with pixel perfect rendering in SFML by default and you need to get a pre-compiled version of SFML, change the necessary settings and build SFML using Cmake. I dont remember what you need to change but it should be somewhere on the interwebs.

There are only 2 lines that are required to set up SFML in the cmake file
- `find_package(SFML REQUIRED graphics window audio network system)` this will find the SFML libraries if the libraries are somewhere in the system paths
- `target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-audio sfml-network sfml-system)` links the found libraries (in order)
- `set(SFML_STATIC_LIBRARIES TRUE)` this is not required if linking dynamically. If you turn this on and drop the necessary SFML files (SFML include and the static libraries) than you will link statically and the build will be relitively portable.
