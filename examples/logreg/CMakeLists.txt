cmake_minimum_required(VERSION 3.14)
project(polycircuit-logreg LANGUAGES CXX)

include("../../cmake/utils.cmake")
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

if (is_top_level)
    find_package(polycircuit REQUIRED)
endif ()

set(sources main.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

add_executable(polycircuit-logreg)
target_sources(polycircuit-logreg PRIVATE ${sources})
target_link_libraries(polycircuit-logreg PRIVATE polycircuit::polycircuit)

if (NOT is_top_level)
    win_copy_deps_to_target_dir(polycircuit-logreg polycircuit::polycircuit)
endif ()
