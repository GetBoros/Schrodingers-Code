#-------------------------------------------------------------------------------------------------------------
project(ASchrodingers_Code VERSION 0.1) # Project name for IDE`s
#-------------------------------------------------------------------------------------------------------------
add_executable(AApplication src/main.cpp)
#-------------------------------------------------------------------------------------------------------------
target_link_libraries(AApplication PRIVATE
    ALibrary
    ALibrary_Dynamic
    ALib_Examples
    ) # Link the library to the executable
#-------------------------------------------------------------------------------------------------------------
target_include_directories(AApplication PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)  # include directory library
#-------------------------------------------------------------------------------------------------------------
