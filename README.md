CVFinal\_HVD
==================

IMPORTANT NOTICE
----------------

Student playground.\\
**DO NOT USE THIS SOFTWARE IN PRODUCTION ENVIRONMENT.**

Introduction
------------

HVD stands for Handicapped Vehicle Detection.\\
This is collaboration space for Computer Vision course final project.\\
Activity period will be from May 13th 2013 to June 20th 2013.

Although this is a computer vision homework, I happened to spend too much time on GUI, so the detection core is very, very weak.

Developers
----------

Developers of this project (github account) in alphabetical order:
* infinity-c
* janniin
* leoisgoodtoo

Build Requirements
------------------

As of June 21st 2013.

This program has been tested on the following systems.
* Linux
* Windows 7

The following packages are required:
* wxWidgets, 2.9.4 tested
* OpenCV, 2.4.3 tested
* CMake, 2.8.10 tested

You can choose either GNU gcc or Clang as compiler.\\
On Windows, the only supported compiler is MinGW. Microsoft compiler is not supported.

Build Instruction for Linux
---------------------------

Install required packages listed above, and start building.\\
You can execute CMake by yourself, or use prepared scripts under build directory to generate Makefile.

Build Instruction for Windows
-----------------------------

1. Download and build wxWidgets with options UNICODE=1, SHARED=1, and MONOLITHIC=0.
2. Download and build OpenCV. The default settings should be sufficient.
3. Open CMakeLists.txt with your favorite text editor. Edit wxWidgets root and OpenCV dir according to your installation.
4. Execute CMake and start compilation.

Notes:\\
1. It seems like prebuilt OpenCV package does not work well with CMake. You need to build/install OpenCV by yourself and point OpenCV dir to that copy.
2. When I last tested wxWidgets, static build will cause initialization crash. If you encounter such problem, please try to build with wxWidgets dynamically linked.
