cmake_minimum_required(VERSION 2.8)
project(icg17)

# load the common ICG configuration
include(common/icg_settings.cmake)

add_subdirectory(hw4p1_fastsmooth)
add_subdirectory(hw4p2_mirrorfloor)
add_subdirectory(hw4p3_motionblur)

