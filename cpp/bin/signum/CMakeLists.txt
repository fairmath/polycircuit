project(FHE_components_signum)

# Add the executable
add_executable(signum main.cpp)

# Link with the library
target_link_libraries(signum PRIVATE fhecomponents)

# Specify the output directory for the executable
set_target_properties(signum PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/signum")
