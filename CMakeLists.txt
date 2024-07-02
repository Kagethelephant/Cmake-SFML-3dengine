# Require cmake 3.25
cmake_minimum_required(VERSION 3.25)
project(TST LANGUAGES CXX)

# Require C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES
    src/main.cpp
    src/display.cpp
    src/math.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# This is the master change
#Message(${CMAKE_CXX_COMPILER_ID})

# Include other header files
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Find SFML
find_package(SFML 2 REQUIRED graphics window audio network system)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-audio sfml-network sfml-system)