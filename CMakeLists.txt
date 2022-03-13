cmake_minimum_required(VERSION 3.13)
project(SpaceWire VERSION 0.0.1 LANGUAGES C CXX)

include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime.cmake")
include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime-Code.cmake")

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SpaceWire")
