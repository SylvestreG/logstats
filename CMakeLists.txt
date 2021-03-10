project(logstats CXX)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if (CMAKE_BUILD_TYPE STREQUAL "")
	set(CMAKE_BUILD_TYPE "Debug")
endif(CMAKE_BUILD_TYPE STREQUAL "")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")

include(cmake/conan.cmake)
conan_cmake_run(REQUIRES boost/1.75.0
                             fswatch/1.14.0@bincrafters/stable
                             gtest/1.10.0
			     libgettext/0.20.1
                             lyra/1.5.1
                             nlohmann_json/3.9.1
                             spdlog/1.7.0
			     OPTIONS         spdlog:header_only=True
                             boost:header_only=True
                             BASIC_SETUP    CMAKE_TARGETS
                             BUILD          missing)

add_subdirectory(external/imtui)
add_subdirectory(src)
add_subdirectory(tests)
