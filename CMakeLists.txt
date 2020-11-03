cmake_minimum_required(VERSION 3.2)

project(ARTagDetection VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(lib)

add_executable(
	program
	src/main.cpp)

target_link_libraries(program PRIVATE ARTagDetectionLib)

