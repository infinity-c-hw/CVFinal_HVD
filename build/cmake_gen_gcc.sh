#!/bin/sh

GCCBUILD_SUBDIR=build_gcc

export CC=gcc
export CXX=g++

mkdir ${GCCBUILD_SUBDIR}
(cd ${GCCBUILD_SUBDIR} && cmake ../..)
