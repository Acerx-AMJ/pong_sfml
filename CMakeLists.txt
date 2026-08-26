cmake_minimum_required(VERSION 3.10)
project(Project)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/lib)
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(release ${SOURCES})
set_target_properties(release PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)
target_link_libraries(release sfml-graphics sfml-window sfml-system sfml-audio)