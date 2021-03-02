project(aniah C CXX)
cmake_minimum_required(VERSION 3.0)

if (CMAKE_BUILD_TYPE STREQUAL "")
	set(CMAKE_BUILD_TYPE "Debug")
endif(CMAKE_BUILD_TYPE STREQUAL "")

include(cmake/conan.cmake)
conan_cmake_run(REQUIRES spdlog/1.7.0 gtest/1.10.0 BASIC_SETUP CMAKE_TARGETS BUILD missing)

add_subdirectory(src)
add_subdirectory(tests)
