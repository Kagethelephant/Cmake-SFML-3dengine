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

# Combine all the source files under SOURCE_FILES variable
set(SOURCE_FILES
   src/app/main.cpp
   src/app/display.cpp
   src/utils/utils.cpp
   src/utils/data.cpp
   src/3d/obj3d.cpp
   src/utils/matrix.cpp
   src/3d/polygon.cpp)

# Creates executable from source files
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Include other header files
target_include_directories(${PROJECT_NAME} PRIVATE 
   ${CMAKE_SOURCE_DIR}/src/3d
   ${CMAKE_SOURCE_DIR}/src/app
   ${CMAKE_SOURCE_DIR}/src/utils) 

# Find SFML
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

# Link libraries to the excecutable
target_link_libraries(${PROJECT_NAME} PRIVATE 
   SFML::Graphics SFML::Window SFML::System)


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

