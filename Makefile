##===- lib/TaintAnalysis/Makefile ------------------------------===##
#
# Created : 
# By : Nirupama Talele
# Pennsylvania State University
#
##===----------------------------------------------------------------------===##

LEVEL = ../..
#PARALLEL_DIRS = Utils Instrumentation Scalar InstCombine IPO Vectorize Hello ObjCARC
LIBRARYNAME = TaintB
LOADABLE_MODULE = 1

include $(LEVEL)/Makefile.config


include $(LEVEL)/Makefile.common
