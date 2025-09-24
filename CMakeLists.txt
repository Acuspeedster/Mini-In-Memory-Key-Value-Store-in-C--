cmake_minimum_required(VERSION 3.10)
project(KeyValueStore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directory
include_directories(include)

# Add source files
set(SOURCES
    src/key_value_store.cpp
    src/main.cpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Add threading support
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)