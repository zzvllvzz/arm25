# arm25/ex1/CMakeLists.txt (EXERCISE-SPECIFIC FILE)
# This file is for building the "ex1" executable only.
# It RELIES on the top-level file for PICO_SDK_PATH, PICO_BOARD, and pico_sdk_import.cmake

# Set name of THIS SUB-PROJECT. Its target name will be "ex1".
# This also sets the variable ${PROJECT_NAME} to "ex1".
project(ex1 C CXX ASM) # <-- CHANGE THIS LINE: project name should be "ex1", not "arm25"
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK for this sub-project. This is required.
pico_sdk_init() # <-- This is OK. It initializes the SDK for this sub-directory.

add_compile_options(-Wall
        -Wno-format
        -Wno-unused-function
        -Wno-maybe-uninitialized
)

# Tell CMake where to find the executable source file
# Use the ${PROJECT_NAME} variable, which now equals "ex1"
add_executable(${PROJECT_NAME}
        main.cpp
)

# Create map/bin/hex/uf2 files for the target
pico_add_extra_outputs(${PROJECT_NAME})

# Link libraries for the target
target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        hardware_pwm
        hardware_gpio
)

# Disable usb output, enable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)