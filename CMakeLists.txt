cmake_minimum_required(VERSION 3.28)

project(ASchrodingers_Code VERSION 0.1) # Имя проекта, используется для IDE

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(ASchrodingers_Code src/main.cpp)
