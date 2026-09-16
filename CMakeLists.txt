cmake_minimum_required(VERSION 3.16)
project(test_proga_VM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/source/*.cpp")

add_library(sweep_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(sweep_lib
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

add_executable(run_main run/main.cpp)

target_link_libraries(run_main PRIVATE sweep_lib)