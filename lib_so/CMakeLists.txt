#-------------------------------------------------------------------------------------------------------------
add_library(ALibrary_Dynamic SHARED)

target_sources(ALibrary_Dynamic PRIVATE
    src/lib_dynamic.cpp
    )   # Create a shared library
#-------------------------------------------------------------------------------------------------------------
target_precompile_headers(ALibrary_Dynamic PRIVATE include/pch.h)  # Precompiled headers
#-------------------------------------------------------------------------------------------------------------
target_compile_definitions(ALibrary_Dynamic PRIVATE BUILDING_LIB_DYNAMIC)
#-------------------------------------------------------------------------------------------------------------
target_include_directories(ALibrary_Dynamic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)  # include directory library
#-------------------------------------------------------------------------------------------------------------
