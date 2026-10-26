file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/source/*.cpp")

add_library(${ProjectName} STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${ProjectName}
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

add_executable(${ProjectName}Run main.cpp)
target_link_libraries(${ProjectName}Run PRIVATE ${ProjectName})

add_executable(${ProjectName}Spline spline_main.cpp)
target_link_libraries(${ProjectName}Spline PRIVATE ${ProjectName})
# абсолютный путь к скрипту графиков - чтобы он находился при любой рабочей папке
target_compile_definitions(${ProjectName}Spline PRIVATE PLOT_SCRIPT="${CMAKE_SOURCE_DIR}/tools/plot_spline.py")