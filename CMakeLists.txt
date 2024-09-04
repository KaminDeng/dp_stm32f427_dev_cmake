cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME dp_stm32f427_dev)

# Include toolchain file
include("lib/hal/cmake/gcc-arm-none-eabi.cmake")

# Modify Something from "lib/hal/cmake/gcc-arm-none-eabi.cmake"
#string(REPLACE <match_string> <replace_string> <output_variable> <input_string>)
string(REPLACE "-T \"${CMAKE_SOURCE_DIR}/STM32F427IIHx_FLASH.ld\"" "-T \"${CMAKE_SOURCE_DIR}/lib/hal/STM32F427IIHx_FLASH.ld\"" CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS}")
string(REPLACE "-T \"${CMAKE_SOURCE_DIR}/STM32F427IIHx_FLASH.ld\"" "-T \"${CMAKE_SOURCE_DIR}/lib/hal/STM32F427IIHx_FLASH.ld\"" CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS}")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,-u,_printf_float")
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,-u,_printf_float")


# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(lib/hal/cmake/stm32cubemx)
add_subdirectory(lib/osal_port)
add_subdirectory(lib/osal)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    src/main.cpp
    src/osal_overload_mgr.cpp
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    src
    include
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    osal_lib
#    osal_port
)
