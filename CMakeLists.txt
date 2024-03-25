cmake_minimum_required(VERSION 3.29.0-rc4)

project(main)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_CXX_EXTENSIONS off)

# add sources file here
file(GLOB SOURCES
    main.cpp
    data.cpp
    system.cpp
    volume.cpp
)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
add_executable(main ${SOURCES})