cmake_minimum_required(VERSION 2.8)
project(icg17)

# load the common ICG configuration
include(common/icg_settings.cmake)

# homework
add_subdirectory(hw2p1_spiral)
add_subdirectory(hw2p2_checkerboard)
add_subdirectory(hw2p3_planets)
add_subdirectory(hw2p4_arkanoid)

