Taint Analysis
=========
This code generates taint flow information for the binary lifted bitcode files. The binary is decompiled using hex and run through scripts to translate it into a compilable C code. This is then used by clang to generate the LLVM bitcode file for the analysis input. 

Installation
-------

Tested on Ubuntu 16.04.

1. apt-get install git cmake
2. mkdir LLVM
3. cd LLVM/
4. wget http://releases.llvm.org/3.7.0/llvm-3.7.0.src.tar.xz
5. tar -xvf llvm-3.7.0.src.tar.xz
6. git clone https://github.com/TJAndHisStudents/TaintFlowAnalysis.git
7. cp -r TaintFlowAnalysis/DSA/DSA/ llvm-3.7.0.src/lib/
8. cp -r TaintFlowAnalysis/DSA/AssistDS/ llvm-3.7.0.src/lib/
9. cp -r TaintFlowAnalysis/DSAHeaders/dsa/ llvm-3.7.0.src/include/
10. cp -r TaintFlowAnalysis/DSAHeaders/assistDS/ llvm-3.7.0.src/include/
11. cp -r TaintFlowAnalysis/DataFlowAnalysis/ llvm-3.7.0.src/lib/
12. echo 'add_subdirectory(DSA)' >> llvm-3.7.0.src/lib/CMakeLists.txt 
13. echo 'add_subdirectory(DataFlowAnalysis)' >> llvm-3.7.0.src/lib/CMakeLists.txt 
14. cd llvm-3.7.0.src/
15. mkdir build
16. cd build
17. cmake -G "Unix Makefiles" ../
18. make ENABLE_OPTIMIZED=0

Alternatively you can use our provided installation script. This script assumes you have git and cmake installed on your system.

1. Copy install.sh to a location where you want LLVM and our tool installed
2. sh install.sh

Overview of the Tool
--------
The tool parses the LLVM IR representation and generates a data flow graph for the program, where the nodes are memory locations and the edges represent the data dependence among these memory locations. The data flow graph is generated minimally by using only the subgraph info for the required source and sink functions. The granularity of the graph nodes is also demand based and can be field sensitive to certain identified fields. 
The taint sources i.e. the user input, network and adversary input are identified semi automatically and are provided to the analysis. The sources are then mapped to the data flow graph and then taint propagation is done for these labels. Various inferences can be drawn based on the taint label combinations at the sinks. For performing authentication bypass analysis we need to identify the possible mediators. Querying for conditionals/statements where certain labels like password, policy and username influence the outcome can identify this. Updating the label at these points and propagating to the sink, will help determine whether a particular input has been mediated or not before reaching the sink.

Running the tool
---------
Inputs required: To run the tool the following inputs need to be prepared: 
1. C-like decompiled code from Hex-Rays. 
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

