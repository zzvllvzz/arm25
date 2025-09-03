# arm25/CMakeLists.txt (TOP-LEVEL FILE)
cmake_minimum_required(VERSION 3.12)

# This is the most important line for your setup.
# It sets the path for ALL sub-projects in this repository.
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH}) # Pulls from CLion's environment variable
set(PICO_BOARD pico_w)                 # Set the board for ALL exercises

# Include build functions from Pico SDK
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# This is the master project for all exercises. Its name is less important.
project(ARM25_Exercises)

# Add each exercise as a sub-project.
add_subdirectory(ex1)
# add_subdirectory(ex2) # You will uncomment these later
# add_subdirectory(ex3)
# add_subdirectory(ex4)
# add_subdirectory(project)