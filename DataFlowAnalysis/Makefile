##===- lib/llvmPDG/Makefile -----------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

CXX=`$(LLVM_CONFIG) --bindir`/clang++
CXXFLAGS=`$(LLVM_CONFIG) --cppflags` -fPIC -fno-rtti -fexceptions -g -O3 -Wno-deprecated -std=c++11
LDFLAGS=`$(LLVM_CONFIG) --ldflags`

LEVEL = ../..
LIBRARYNAME = llvmPDG
LOADABLE_MODULE = 1
USEDLIBS = 

include $(LEVEL)/Makefile.common

