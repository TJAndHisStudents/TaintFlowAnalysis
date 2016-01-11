Taint Analysis
=========
This code generates taint flow information for the binary lifted bitcode files. The binary is decompiled using hex and run through scripts to translate it into a compilable C code. This is then used by clang to generate the LLVM bitcode file for the analysis input. 

Prerequisites
-------
1.	LLVM 3.5 with corresponding clang (installed from Source http://llvm.org/releases/download.html 3.5 sources)
2.	Uses DSA pointer analysis and is required as a prerequisite pass before analysis.
3.	Follow the instructions at (http://clang.llvm.org/get_started.html) for installation of LLVM, might need g++ installed which is not specified explicitly in the instructions.
Setup
This code is compatible with LLVM 3.5
1.	For DSA place the DSA and AssistDSA code in lib and the corresponding headers in the include directory of LLVM source. 
2.	Perform make for DSA creating the appropriate required library.
3.	Please place the "TaintAnalysis" directory in the lib/ directory of LLVM source.
4.	Place the header file DominatorInternals.h from LLVM 3.3 version in the llvm/include/llvm/Analysis (this backward dependence will be rectified once complete upgrade to 3.5 is done)
5.	Execute make in TaintAnalysis. (if LLVM is built in debug mode execute the RunMake.sh, or use the ENABLE_OPTIMIZED=0 flag while running make)
6.	This will generate the TaintAnalysis library to be loaded and run using the opt pass manager. 


Overview of the Tool
--------
The tool parses the LLVM IR representation and generates a data flow graph for the program, where the nodes are memory locations and the edges represent the data dependence among these memory locations. The data flow graph is generated minimally by using only the subgraph info for the required source and sink functions. The granularity of the graph nodes is also demand based and can be field sensitive to certain identified fields. 
The taint sources i.e. the user input, network and adversary input are identified semi automatically and are provided to the analysis. The sources are then mapped to the data flow graph and then taint propagation is done for these labels. Various inferences can be drawn based on the taint label combinations at the sinks. For performing authentication bypass analysis we need to identify the possible mediators. Querying for conditionals/statements where certain labels like password, policy and username influence the outcome can identify this. Updating the label at these points and propagating to the sink, will help determine whether a particular input has been mediated or not before reaching the sink.

Running the tool
---------
Inputs required: To run the tool the following inputs need to be prepared: 
1. C-like decompiled code from hex. 
2. Manually update the c-like code to make it compilable in clang after running it through the provided scripts. 
3. Generate the bitcode file from the de-compiled code.

The tool operates on this llvm IR format. The supplementary inputs required for running a test are:
1.	source_sink.txt : This file contains the source and sink functions between which the flow needs to be analyzed. This file should be placed where the opt executable is located. 
2.	taintSource : This file specifies the taint sources in the format: <function varname label> and can be specified to the tool with option –taintSource=<file>. The standard input functions (scanf,read etc) are considered along with the specified ones. 
3.	fields: This file contains the list of fields which need to be made field sensitive for more precise data flow tracking.
4.	Mediator: The file specifies the functions identified as mediator. 
5.	Query: This file specifies what kind of label intersection results need to be analyzed: format <intersect  label1 label2>, can be provided by option –query=<file>

Following is the command to execute the tool, (update the file paths wherever necessary and the library extensions for linux would be .so)

-load ../lib/dsa.dylib -load ../lib/TaintFlow.dylib  -TaintS -time-passes -full=false -includeAllInstsInDepGraph=true -taintSource=TaintInput.txt  -fields=fields.txt -mediator=mediator.txt -query=queries.txt -fullAnalysis=true -printall=true inputProg.bc -S > inputProg.ll

