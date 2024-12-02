cmake_minimum_required(VERSION 3.14)
project(polycircuit-max LANGUAGES CXX)

include("../../cmake/utils.cmake")
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

if (is_top_level)
    find_package(polycircuit REQUIRED)
endif ()

set(sources main.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

add_executable(polycircuit-max)
target_sources(polycircuit-max PRIVATE ${sources})
target_link_libraries(polycircuit-max PRIVATE polycircuit::polycircuit)

if (NOT is_top_level)
    win_copy_deps_to_target_dir(polycircuit-max polycircuit::polycircuit)
endif ()
