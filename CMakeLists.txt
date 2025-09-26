#-------------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.28.3)
#-------------------------------------------------------------------------------------------------------------
project(ASchrodingers_Code VERSION 0.2)  # Project name for IDE`s
#-------------------------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/app)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib_examples)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib_so)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib_o)
#-------------------------------------------------------------------------------------------------------------
add_subdirectory(lib_examples)  # dynamic library
add_subdirectory(lib_so)  # dynamic library
add_subdirectory(lib_o)  # static library
add_subdirectory(app)
#-------------------------------------------------------------------------------------------------------------
if(UNIX AND NOT APPLE) # For linux we want to put the executable and .so files in build/bin
    set_target_properties(AApplication PROPERTIES INSTALL_RPATH "$ORIGIN")  # $ORIGIN now points to build/bin
endif()
#-------------------------------------------------------------------------------------------------------------