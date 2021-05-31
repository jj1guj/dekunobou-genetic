cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
file(GLOB src *.cpp *.hpp)

#最適化オプションの設定
set(CMAKE_CXX_FLAGS "-pthread -O3 -funroll-loops -mtune=native")

project(dekunobou-new)

add_executable(dekunobou ${src})
set_property(TARGET dekunobou PROPERTY CXX_STANDARD 17)
