add_executable(logParser main.cpp)
target_link_libraries(logParser CONAN_PKG::spdlog)
