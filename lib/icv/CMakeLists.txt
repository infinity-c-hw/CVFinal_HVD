cmake_minimum_required(VERSION 2.6)
project(icv)

# compile library
unset(src)
set(src ${src} vidreader.cpp)
set(src ${src} cvshm.cpp)

add_library(icv ${src})
