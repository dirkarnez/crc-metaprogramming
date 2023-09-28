cmake_minimum_required(VERSION 3.18)

project("crc-metaprogramming")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth=9999")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

add_executable(${PROJECT_NAME}
    "src/main.cpp"
    "src/CRC.h"
)

# target_link_libraries(${PROJECT_NAME})
