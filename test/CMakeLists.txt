#----------------------------------------------------------------------------
# Do not edit lines below

# GCOV settings
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} --coverage") 
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage") 

# List up all source files including main.cpp
set(MAIN_FILE_PATTERN ".*/main.cpp$")
file(GLOB LIB_SRC 
            LIST_DIRECTORIES true 
            "${CMAKE_CURRENT_SOURCE_DIR}/../drivers/*.cpp")
list(FILTER LIB_SRC EXCLUDE REGEX ${MAIN_FILE_PATTERN})    # Filter out the main file

# List all test file in this directory
file(GLOB TEST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Test executable and library name
set(TEST_EXECUTABLE_NAME ${CMAKE_PROJECT_NAME})

# Add the executable for the testcase which is using googletest
add_executable(${TEST_EXECUTABLE_NAME} ${TEST_SRC} ${LIB_SRC})

    # Add the library under test.
    target_link_libraries(${TEST_EXECUTABLE_NAME}  
                            "gtest_main"
                            )
    # Add the include directory for test executable.
    target_include_directories(${TEST_EXECUTABLE_NAME}
                                PUBLIC
                                "${CMAKE_CURRENT_SOURCE_DIR}/../core"
                                "${CMAKE_CURRENT_SOURCE_DIR}/../drivers/codec"
                                )
    if(MSVC)
        target_compile_options(${TEST_EXECUTABLE_NAME} PRIVATE /W4 )
    else()
        target_compile_options(${TEST_EXECUTABLE_NAME} PRIVATE -Wall -Wextra -pedantic )
    endif()

# Find and integrate the Google Test package

# Load the FetchContent module
# New in CMake 3.11
include("FetchContent")

# Declaration of googletest as content
FetchContent_Declare(
  	"googletest"
  	GIT_REPOSITORY "https://github.com/google/googletest.git"
  	GIT_TAG "release-1.11.0"
)

# Fetch the googletest
FetchContent_MakeAvailable("googletest")

# Include the GoogleTest
include("GoogleTest")

# Search test cases and link 
# New in CMake 3.10
gtest_discover_tests(${TEST_EXECUTABLE_NAME})