#!/bin/sh

CLANGBUILD_SUBDIR=build_clang

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

mkdir ${CLANGBUILD_SUBDIR}
(cd ${CLANGBUILD_SUBDIR} && cmake ../..)
