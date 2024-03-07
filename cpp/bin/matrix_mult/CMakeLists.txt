project(FHE_components_matrix_mult)

# Add the executable
add_executable(matrix-mult main.cpp)

# Link with the library
target_link_libraries(matrix-mult PRIVATE fhecomponents)

# Specify the output directory for the executable
set_target_properties(matrix-mult PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/matrix-mult")

# Copy the input file next to the executable
configure_file(config.json "${CMAKE_BINARY_DIR}/bin/matrix-mult/config.json" COPYONLY)
