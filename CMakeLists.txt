cmake_minimum_required(VERSION 3.5)

project(GameOfLife LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(GameOfLife main.cpp
    README.md
    build.sh)

include(GNUInstallDirs)
install(TARGETS GameOfLife
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

target_link_libraries(GameOfLife ${SDL2_LIBRARIES})
