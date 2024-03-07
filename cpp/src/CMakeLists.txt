set(LIBRARY_SOURCES
    signum/ckks.cpp
)

# Optionally, specify headers for IDEs
set(LIBRARY_HEADERS
    signum/ckks.hpp
)

# Define the library
add_library(fhecomponents STATIC ${LIBRARY_SOURCES})

# Specify include directories
target_include_directories(fhecomponents PUBLIC ../include)
