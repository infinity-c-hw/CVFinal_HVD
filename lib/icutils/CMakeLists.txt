cmake_minimum_required(VERSION 2.6)
project(icutils)

# compile library
unset(src)
set(src ${src} sem.cpp)
set(src ${src} thread_runner.cpp)

add_library(icutils ${src})
