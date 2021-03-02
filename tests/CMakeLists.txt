add_executable(units ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(units CONAN_PKG::gtest)
