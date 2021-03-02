add_executable(units ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp)

target_link_libraries(units PRIVATE CONAN_PKG::gtest logLib)
