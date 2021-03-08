find_package(BISON 3.0.0)
find_package(FLEX 2.6)

add_subdirectory(generator)

bison_target(cflParser cfl.y ${CMAKE_BINARY_DIR}/cflParser.cpp COMPILE_FLAGS "-vt --debug")
flex_target(cflParser cfl.l ${CMAKE_BINARY_DIR}/cflLexer.cpp COMPILE_FLAGS --header-file=${CMAKE_BINARY_DIR}/cflLexer.h)

add_library(clf ${CMAKE_CURRENT_SOURCE_DIR}/buffer_splitter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/config.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/file_watcher.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp
        ${BISON_cflParser_OUTPUTS}
        ${FLEX_cflParser_OUTPUTS})
target_include_directories(clf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(clf PUBLIC CONAN_PKG::boost CONAN_PKG::fswatch CONAN_PKG::spdlog CONAN_PKG::nlohmann_json)

add_executable(clfMonitor ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/core.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ui.cpp)
target_link_libraries(clfMonitor PRIVATE clf imtui-ncurses CONAN_PKG::lyra)