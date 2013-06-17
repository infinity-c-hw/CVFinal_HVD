cmake_minimum_required(VERSION 2.6)
project(osdep)

# compile library
unset(src)
set(src ${src} gettime.cpp)
set(src ${src} msleep.cpp)

add_library(osdep ${src})
