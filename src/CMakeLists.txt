find_package(BISON 3.0.0)
find_package(FLEX 2.6)

bison_target(cflParser cfl.y ${CMAKE_BINARY_DIR}/cflParser.cpp COMPILE_FLAGS "-vt --debug")
flex_target(cflParser cfl.l ${CMAKE_BINARY_DIR}/cflLexer.cpp COMPILE_FLAGS --header-file=${CMAKE_BINARY_DIR}/cflLexer.h)

add_library(logLib  ${CMAKE_CURRENT_SOURCE_DIR}/config.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/core.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp ${BISON_cflParser_OUTPUTS} ${FLEX_cflParser_OUTPUTS})
target_include_directories(logLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(logLib PUBLIC CONAN_PKG::boost CONAN_PKG::libevent CONAN_PKG::spdlog CONAN_PKG::nlohmann_json CONAN_PKG::lyra)

add_executable(clfMonitor main.cpp)
target_link_libraries(clfMonitor PRIVATE logLib)
