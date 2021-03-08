add_executable(clfGenerator ${CMAKE_CURRENT_SOURCE_DIR}/log_generator.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/config.cpp)
target_link_libraries(clfGenerator CONAN_PKG::spdlog CONAN_PKG::lyra CONAN_PKG::boost CONAN_PKG::nlohmann_json)