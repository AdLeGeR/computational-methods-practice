file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/source/*.cpp")

add_library(${ProjectName} STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${ProjectName}
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

add_executable(${ProjectName}Run main.cpp)
target_link_libraries(${ProjectName}Run PRIVATE ${ProjectName})