#-------------------------------------------------------------------------------------------------------------
add_library(ALib_Examples SHARED)  # Create a shared library

target_sources(ALib_Examples PRIVATE
        src/lib_examples.cpp
        src/Object.cpp
        src/Task.cpp
        src/Task_Manager.cpp
        src/Task_Awaiter.cpp
        src/Example_Task.cpp
    )
#-------------------------------------------------------------------------------------------------------------
target_precompile_headers(ALib_Examples PRIVATE include/pch.h)  # Precompiled headers
#-------------------------------------------------------------------------------------------------------------
target_compile_definitions(ALib_Examples PRIVATE BUILDING_LIB_EXAMPLES)
#-------------------------------------------------------------------------------------------------------------
target_include_directories(ALib_Examples PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)  # include directory library
#-------------------------------------------------------------------------------------------------------------