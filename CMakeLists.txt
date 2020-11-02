cmake_minimum_required(VERSION 3.2)

project(ARTagDetection VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(lib)

find_package(JPEG REQUIRED)

add_executable(
	program
	src/main.cpp)

target_link_libraries(program PRIVATE ARTagDetectionLib ${JPEG_LIBRARIES})
target_include_directories(program PRIVATE ${JPEG_INCLUDE_DIR})

