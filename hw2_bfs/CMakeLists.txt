cmake_minimum_required(VERSION 3.10)
project(parallel_bfs)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the Parlay library
find_package(Parlay REQUIRED)

# Add source file
set(SOURCE main.cpp bfs.cpp)

# Include the Parlay library headers
include_directories(${Parlay_INCLUDE_DIRS})

# Add the executable target
add_executable(main ${SOURCE})

# Link against the Parlay library
target_link_libraries(main ${Parlay_LIBRARIES})
