cmake_minimum_required(VERSION 3.16)

project(VesicleDiffusion LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/source/*.cpp")
file(GLOB HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")

add_executable(VesicleDiffusion ${SOURCES} ${HEADERS})

# Use system-installed SFML and Eigen
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

include_directories("${CMAKE_SOURCE_DIR}/include")

target_link_libraries(VesicleDiffusion
    sfml-graphics
    sfml-window
    sfml-system
    Eigen3::Eigen)
