#-------------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.28.3)
#-------------------------------------------------------------------------------------------------------------
project(ASchrodingers_Code VERSION 0.1)  # Project name for IDE`s
#-------------------------------------------------------------------------------------------------------------
add_library(ALibrary_Dynamic STATIC src/lib_dynamic.cpp)  # Create a static library
#-------------------------------------------------------------------------------------------------------------
target_include_directories(ALibrary_Dynamic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)  # include directory library
#-------------------------------------------------------------------------------------------------------------
target_precompile_headers(ALibrary_Dynamic PRIVATE src/pch.h)  # Precompiled headers
#-------------------------------------------------------------------------------------------------------------
target_compile_definitions(ALibrary_Dynamic PRIVATE BUILDING_LIB_DYNAMIC)
#-------------------------------------------------------------------------------------------------------------
