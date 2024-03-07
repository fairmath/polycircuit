project(FHE_components_sigmoid)

# Add the executable
add_executable(sigmoid main.cpp)

# Link with the library
target_link_libraries(sigmoid PRIVATE fhecomponents)

# Specify the output directory for the executable
set_target_properties(sigmoid PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/sigmoid")
