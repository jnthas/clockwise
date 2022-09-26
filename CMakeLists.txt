cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS "firmware/clockfaces" "firmware/lib")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(clockwise)
