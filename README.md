Taint Analysis
======
This code generate taint flow information for the binary lifted bitcode files from two possible sources. For the BAP lifted input the taint sources are specified in the file, while for the hex decompiled code the standard input functions are considered as the source for tainting.

Prerequisites
------
1. LLVM 3.3 with corresponding clang (installed from Source)

Inputs Required:
------
This code can generate Taint information for two cases as described below. 

Case A:
1. Bitcode file of the binary generated from BAP codegen. 
2. Input files with information on caller, callee and def use information extracted from BAP.

Case B:
1. C-like decompiled code from hex.
2. Manually update the c-like code to make it compilable in clang after running it through the provided scripts.
3. Generate the bitcode file from the de-compiled code. 

Setup
-----
This code is compatible with LLVM 3.3
1. Please place the "TaintAnalysis" directory in the lib/ directory of LLVM source.
2. Execute make in TaintAnalysis. (if LLVM is built in debug mode execute the RunMake.sh )
3. A sample command to run the code is: (this requires that the file paths be updated)
```sh
   opt -load <path/to/TaintB.dylib> -TaintB <path/to/inputFile.bc> -src_file=sourcefile.txt -lookup=lookupfile.txt -stats
```
	3.1 Options:
		src_file : specifies the taint sources.
		lookup   : Lookup functions for the resource lookup.
		funcInfo : Function caller callee information.. for context and function boundary determination.
		def-use  : Enlists the def-use chains in the program. 

