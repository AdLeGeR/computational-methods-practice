cmake_minimum_required(VERSION 3.20)

project(CppTasks LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ищем все .cpp в корне проекта и во всех подпапках.
file(GLOB_RECURSE CPP_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

foreach(CPP_FILE ${CPP_FILES})

    # Путь относительно корня проекта:
    # exercises/math/task01.cpp
    file(RELATIVE_PATH REL_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CPP_FILE}"
    )

    # exercises/math/task01.cpp
    # ->
    # exercises_math_task01
    string(REPLACE "/" "_" TARGET_NAME "${REL_PATH}")
    string(REPLACE " " "_" TARGET_NAME "${TARGET_NAME}")
    string(REPLACE "\\" "_" TARGET_NAME "${TARGET_NAME}")
    string(REGEX REPLACE "\\.cpp$" "" TARGET_NAME "${TARGET_NAME}")

    add_executable(${TARGET_NAME} ${CPP_FILE})

    # Чтобы исходник отображался нормально в IDE
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
        PREFIX "Source Files"
        FILES "${CPP_FILE}"
    )

endforeach()