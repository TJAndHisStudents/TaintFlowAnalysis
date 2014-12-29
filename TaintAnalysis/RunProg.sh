#!/bin/bash

#This scirpt will make the files and run the tool on the test program.


echo "Running the Tool on test program"
opt -load ../../Debug+Asserts/lib/TaintB.dylib -TaintB ../../examples/Binrecurse/FunctionAndSysCall/functionandsyscall.bc -src_file=sourcefile.txt -lookup=lookupfile.txt -debug -stats -S

#Options:
# src_file : specifies the taint sources.
# lookup   : Lookup functions for the resource lookup.
# funcInfo : Function caller callee information.. for context and function boundary determination.
# def-use  : Enlists the def-use chains in the program. 
