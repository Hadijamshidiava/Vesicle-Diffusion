cmake_minimum_required(VERSION 3.16)

project(VesicleDiffusion LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(VesicleDiffusion main.cpp
    Cell.h
    Vesicle.h
    Visualizer.h

)
include_directories(/home/hadi/development/SFML-3.0.0-linux-gcc-64-bit/SFML-3.0.0/include)

find_library(SFML_GRAPHICS_LIBRARY sfml-graphics PATHS /home/hadi/development/SFML-3.0.0-linux-gcc-64-bit/SFML-3.0.0/lib)
find_library(SFML_WINDOW_LIBRARY sfml-window PATHS /home/hadi/development/SFML-3.0.0-linux-gcc-64-bit/SFML-3.0.0/lib)
find_library(SFML_SYSTEM_LIBRARY sfml-system PATHS /home/hadi/development/SFML-3.0.0-linux-gcc-64-bit/SFML-3.0.0/lib)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

include(GNUInstallDirs)
install(TARGETS VesicleDiffusion
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_link_libraries(VesicleDiffusion
    ${SFML_GRAPHICS_LIBRARY}
    ${SFML_WINDOW_LIBRARY}
    ${SFML_SYSTEM_LIBRARY}
    Eigen3::Eigen)
