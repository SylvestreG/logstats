project(logstats CXX)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if (CMAKE_BUILD_TYPE STREQUAL "")
	set(CMAKE_BUILD_TYPE "Debug")
endif(CMAKE_BUILD_TYPE STREQUAL "")

include(cmake/conan.cmake)
if(USE_SYSTEM_BISON)
	conan_cmake_run(REQUIRES boost/1.75.0
                             fswatch/1.14.0@bincrafters/stable
                             gtest/1.10.0
                             lyra/1.5.1
                             nlohmann_json/3.9.1
                             spdlog/1.7.0
                             OPTION         spdlog:header_only=True
                             boost:header_only=True
                             BASIC_SETUP    CMAKE_TARGETS
                             BUILD          missing)
elseif(USE_DOCKER_BUILD)
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup(TARGETS)
else()
    conan_cmake_run(REQUIRES bison/3.7.1
                             boost/1.75.0
                             flex/2.6.4
                             fswatch/1.14.0@bincrafters/stable
                             m4/1.4.18
                             gtest/1.10.0
                             lyra/1.5.1
                             nlohmann_json/3.9.1
                             spdlog/1.7.0
                    OPTIONS     spdlog:header_only=True
                             boost:header_only=True
                    BASIC_SETUP	CMAKE_TARGETS
                    BUILD       missing)
endif()

add_subdirectory(external/imtui)
add_subdirectory(src)
add_subdirectory(tests)
