cmake_minimum_required(VERSION 3.14)
project(Polycircuit
    VERSION 1.0.0
    DESCRIPTION "Template for C++ library built with CMake"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#----------------------------------------------------------------------------------------------------------------------
# general settings and options
#----------------------------------------------------------------------------------------------------------------------

include(cmake/utils.cmake)
include(GNUInstallDirs)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

# POLYCIRCUIT_SHARED_LIBS option (undefined by default) can be used to force shared/static build
option(POLYCIRCUIT_BUILD_TESTS "Build polycircuit tests" OFF)
option(POLYCIRCUIT_BUILD_EXAMPLES "Build polycircuit examples" ON)
option(POLYCIRCUIT_BUILD_DOCS "Build polycircuit documentation" OFF)
option(POLYCIRCUIT_INSTALL "Generate target for installing polycircuit" ${is_top_level})
set_if_undefined(POLYCIRCUIT_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/polycircuit" CACHE STRING
    "Install path for polycircuit package-related CMake files")

if(DEFINED POLYCIRCUIT_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ${POLYCIRCUIT_SHARED_LIBS})
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set_if_undefined(CMAKE_CXX_VISIBILITY_PRESET hidden)
set_if_undefined(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

add_library(polycircuit
        include/polycircuit/cifar10/cifar10.hpp
        include/polycircuit/lut/lut.hpp
        include/polycircuit/logreg/LogReg.hpp
        include/polycircuit/parity/parity.hpp
        include/polycircuit/max/max.hpp
        include/polycircuit/matrix_mul/matrix_mul.hpp
        include/polycircuit/shl/shl.hpp
        examples/shl/main.cpp
        src/ckks_component.cpp
) # initialized below
add_library(polycircuit::polycircuit ALIAS polycircuit)

#----------------------------------------------------------------------------------------------------------------------
# polycircuit dependencies
#----------------------------------------------------------------------------------------------------------------------

find_package(OpenFHE CONFIG REQUIRED)
if (OpenFHE_FOUND)
    message(STATUS "FOUND PACKAGE OpenFHE")
    message(STATUS "OpenFHE Version: ${BASE_OPENFHE_VERSION}")
    message(STATUS "OpenFHE installed as shared libraries: ${OpenFHE_SHARED}")
    message(STATUS "OpenFHE include files location: ${OpenFHE_INCLUDE}")
    message(STATUS "OpenFHE lib files location: ${OpenFHE_LIBDIR}")
    message(STATUS "OpenFHE Native Backend size: ${OpenFHE_NATIVE_SIZE}")
else()
    message(FATAL_ERROR "PACKAGE OpenFHE NOT FOUND")
endif ()
include_directories(/usr/local/include)
set(CMAKE_CXX_FLAGS ${OpenFHE_CXX_FLAGS} )
include_directories( ${OPENMP_INCLUDES} )
include_directories( ${OpenFHE_INCLUDE} )
include_directories( ${OpenFHE_INCLUDE}/third-party/include )
include_directories( ${OpenFHE_INCLUDE}/core )
include_directories( ${OpenFHE_INCLUDE}/pke )
include_directories( ${OpenFHE_INCLUDE}/binfhe )
### add directories for other OpenFHE modules as needed for your project
link_directories( ${OpenFHE_LIBDIR} )
link_directories( ${OPENMP_LIBRARIES} )
if(BUILD_STATIC)
    set( CMAKE_EXE_LINKER_FLAGS "${OpenFHE_EXE_LINKER_FLAGS} -static")
    link_libraries( ${OpenFHE_STATIC_LIBRARIES} )
else()
    set( CMAKE_EXE_LINKER_FLAGS ${OpenFHE_EXE_LINKER_FLAGS} )
    link_libraries( ${OpenFHE_SHARED_LIBRARIES} )
endif()

#----------------------------------------------------------------------------------------------------------------------
# polycircuit sources
#----------------------------------------------------------------------------------------------------------------------

include(GenerateExportHeader)
set(export_file_name "export_shared.h")

if(NOT BUILD_SHARED_LIBS)
    set(export_file_name "export_static.h")
endif()

generate_export_header(polycircuit EXPORT_FILE_NAME include/polycircuit/${export_file_name})

set(sources
    include/polycircuit/export.h
    include/polycircuit/polycircuit.h
    src/polycircuit.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

#----------------------------------------------------------------------------------------------------------------------
# polycircuit target
#----------------------------------------------------------------------------------------------------------------------

include(CMakePackageConfigHelpers)

target_sources(polycircuit PRIVATE ${sources})
target_compile_definitions(polycircuit PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:POLYCIRCUIT_STATIC_DEFINE>")

target_include_directories(polycircuit
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

set_target_properties(polycircuit PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION})

if(POLYCIRCUIT_INSTALL AND NOT CMAKE_SKIP_INSTALL_RULES)
    configure_package_config_file(cmake/polycircuit-config.cmake.in polycircuit-config.cmake
        INSTALL_DESTINATION "${POLYCIRCUIT_INSTALL_CMAKEDIR}")

    write_basic_package_version_file(polycircuit-config-version.cmake
        COMPATIBILITY SameMajorVersion)

    install(TARGETS polycircuit EXPORT polycircuit_export
        RUNTIME COMPONENT polycircuit
        LIBRARY COMPONENT polycircuit NAMELINK_COMPONENT polycircuit-dev
        ARCHIVE COMPONENT polycircuit-dev
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
    install(DIRECTORY include/
        TYPE INCLUDE
        COMPONENT polycircuit-dev)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/include/polycircuit/${export_file_name}"
        COMPONENT polycircuit-dev
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/polycircuit")

    set(targets_file "polycircuit-shared-targets.cmake")

    if(NOT BUILD_SHARED_LIBS)
        set(targets_file "polycircuit-static-targets.cmake")
    endif()

    install(EXPORT polycircuit_export
        COMPONENT polycircuit-dev
        FILE "${targets_file}"
        DESTINATION "${POLYCIRCUIT_INSTALL_CMAKEDIR}"
        NAMESPACE polycircuit::)

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/polycircuit-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/polycircuit-config-version.cmake"
        COMPONENT polycircuit-dev
        DESTINATION "${POLYCIRCUIT_INSTALL_CMAKEDIR}")

    if(MSVC)
        set(pdb_file "")
        set(pdb_file_destination "")

        if(BUILD_SHARED_LIBS)
            set(pdb_file "$<TARGET_PDB_FILE:polycircuit>")
            set(pdb_file_destination "${CMAKE_INSTALL_BINDIR}")
        else()
            # TARGET_PDB_FILE does not work for pdb file generated for static library build, determining it manually
            set(pdb_file "$<TARGET_FILE_DIR:polycircuit>/$<TARGET_FILE_PREFIX:polycircuit>$<TARGET_FILE_BASE_NAME:polycircuit>.pdb")
            set(pdb_file_destination "${CMAKE_INSTALL_LIBDIR}")
        endif()

        install(FILES "${pdb_file}"
            COMPONENT polycircuit-dev
            CONFIGURATIONS Debug RelWithDebInfo
            DESTINATION "${pdb_file_destination}"
            OPTIONAL)
    endif()
endif()

#----------------------------------------------------------------------------------------------------------------------
# other targets
#----------------------------------------------------------------------------------------------------------------------

if(POLYCIRCUIT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(POLYCIRCUIT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(POLYCIRCUIT_BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    doxygen_add_docs(docs include)
endif()
