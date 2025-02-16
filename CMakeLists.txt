# Require cmake 3.25
cmake_minimum_required(VERSION 3.22)
project(TST LANGUAGES CXX)

# Require C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # create complie_commands.json so nvim can read the cmake include paths


set(SOURCE_FILES
   src/app/main.cpp
   src/app/display.cpp
   src/utils/random.cpp
   src/utils/data.cpp
   src/3d/obj3d.cpp
   src/utils/matrix.cpp
   src/3d/camera.cpp
   src/3d/polygon.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
# set(SFML_STATIC_LIBRARIES TRUE)

# This is the master change
#Message(${CMAKE_CXX_COMPILER_ID})

# Include other header files
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Find SFML
find_package(SFML REQUIRED graphics window audio network system)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-audio sfml-network sfml-system)
