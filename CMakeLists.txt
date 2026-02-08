# Require cmake 4.2.0
cmake_minimum_required(VERSION 4.2.0)
project(TST LANGUAGES CXX)

# Require C++14
# Force compiler to adhere to C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# create complie_commands.json so nvim can read the cmake include paths
# This is usefull for troubleshooting compile errors and link errors
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_language(C) # This enables comiling for the glad library written in C



# Add GLFW CMake source location
add_subdirectory(lib/GLFW)

# Combine all the source files under SOURCE_FILES variable
set(SOURCE_FILES
   lib/glad/src/glad.c
   src/app/main.cpp
   src/app/object.cpp
   src/utils/utils.cpp
   src/utils/data.cpp
   src/utils/matrix.cpp
   src/cpuRender/cpuRender.cpp
   src/gpuRender/window.cpp
   src/gpuRender/gpuRender.cpp
   src/gpuRender/text.cpp
   src/shaders/shader.cpp
)

# Creates executable from source files
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Include other header files
target_include_directories(${PROJECT_NAME} PRIVATE 
   ${CMAKE_SOURCE_DIR}/src
   ${CMAKE_SOURCE_DIR}/lib/glad/include
   ${CMAKE_SOURCE_DIR}/lib/freeType/include
)

# Find SFML
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)
find_package(Freetype REQUIRED)

# Link libraries to the excecutable
target_link_libraries(${PROJECT_NAME} PRIVATE 
   SFML::Graphics
   SFML::Window
   SFML::System
   glfw
   Freetype::Freetype
)


## This creates the instructions for an install files structure
# Install the binaries
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
# Install the resources folder for objects, images, fonts, etc.
install(DIRECTORY resources/ DESTINATION resources)
# Example: Install a library named MyLib
# install(TARGETS MyLib LIBRARY DESTINATION lib)
# Example: Install data files
# install(FILES data.txt DESTINATION share/MyAppData)

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "T")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_RESOURCE_FILE_LICENCE "${CMAKE_SOURCE_DIR}/licence.txt")
# TGZ: gzipped, STGZ: shell, DEB: Debian, RPM: Red Hat, NSIS: Windows NSIS
set(CPACK_GENERATOR "TGZ;STGZ;NSIS") 
include(CPack)

