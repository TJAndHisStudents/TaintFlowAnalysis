Taint Analysis
======
This code generates taint flow information for the binary lifted bitcode files. The binary is decompiled using hex and run through scripts to translate it into a compilable c code. This is then used by clang to generate the LLVM bitcode file for the analysis input. The standard input functions(scanf,read etc) are considered as the sources for tainting.

Prerequisites
------
1. LLVM 3.5 with corresponding clang (installed from Source http://llvm.org/releases/download.html 3.5 sources)
2. Follow the instructions for installation of LLVM, might need g++ installed which is not specified explicitly in the instructions. 

Inputs Required:
------
This code can generate Taint information as described below. 
1. C-like decompiled code from hex.
2. Manually update the c-like code to make it compilable in clang after running it through the provided scripts.
3. Generate the bitcode file from the de-compiled code. 

Setup
-----
This code is compatible with LLVM 3.5

1. Please place the "TaintAnalysis" directory in the lib/ directory of LLVM source.
2. Place the header file DominatorInternals.h from LLVM 3.3 version in the llvm/include/llvm/Analysis (this backwrd dependence will be rectified once complete upgrade to 3.5 is done)
3. Execute make in TaintAnalysis. (if LLVM is built in debug mode execute the RunMake.sh, or use the ENABLE_OPTIMIZED=0 flag while running make)
4. A sample command to run the code is: (this requires that the file paths be updated)
```sh
   opt -load <path/to/TaintFlow.dylib> -TaintS <path/to/inputFile.bc> -callPath=CallPath.txt -taintSource=sourcefile.txt -lookup=lookupfile.txt -stats
```
	3.1 Options:
		taintSource : specifies the taint sources.
		lookup   : Lookup functions for the resource lookup.
		callPath : The relevant functions in the call paths between the sources and sinks.
		funcInfo : Function caller callee information.. for context and function boundary determination.

