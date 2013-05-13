#!/bin/sh

CABUILD_SUBDIR=build_clang_analyzer

export CC=/usr/bin/ccc-analyzer
export CXX=/usr/bin/c++-analyzer

mkdir ${CABUILD_SUBDIR}
(cd ${CABUILD_SUBDIR} && cmake ../..)
