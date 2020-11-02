add_library(
	ARTagDetectionLib
	svpng/svpng.h
	svpng/svpng.cpp
	)

target_include_directories(ARTagDetectionLib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
