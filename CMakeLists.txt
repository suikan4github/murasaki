# CMake 3.15 or newer is requried because of CMP0091.
cmake_minimum_required(VERSION "3.15")

# The CMake compatibility flag for MSVC runtime library flag. New in 3.15.  Must set "NEW"
cmake_policy(SET CMP0091 "NEW")   
# Enforce the  /MT or /MTd as link option for MS Visual Studio C++
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Parameters inside project
project("murasaki_test")
#----------------------------------------------------------------------------
# Parameters inside project

# Compiler option
# GoogleTest requirest C++11 or later
set(CMAKE_CXX_STANDARD "11")
set(CMAKE_CXX_STANDARD_REQUIRED "ON")
set(CMAKE_CXX_EXTENSIONS "OFF")


#----------------------------------------------------------------------------
# Do not edit the lines below 

# Enable CTest
# This command automatically run the enable_test() command.
# And the CMake specification requires to place the enable_test() at the
# root of the project, to run the ctest correctly. 
# Thus, place this command at the root of project.
include("CTest")

# Using thread inside application
find_package("Threads" REQUIRED)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include("CPack")

# Subdirectories
add_subdirectory("test")

