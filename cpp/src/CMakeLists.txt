set(LIBRARY_SOURCES
    signum/ckks.cpp
    matrix_mult/ckks.cpp
    sigmoid/ckks.cpp
)

# Optionally, specify headers for IDEs
set(LIBRARY_HEADERS
    signum/ckks.hpp
    matrix_mult/ckks.hpp
    sigmoid/ckks.cpp
)

# Define the library
add_library(fhecomponents STATIC ${LIBRARY_SOURCES})

# Specify include directories
target_include_directories(fhecomponents PUBLIC ../include)
