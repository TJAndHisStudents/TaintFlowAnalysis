Taint Analysis
=========
This project aims at leveraging taint flow information to identify possibly malicious behavior around authentication logic within a program. This document describes how to install and utilize our taint analysis framework and design and perform your own analysis.

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

Alternatively you can use our provided installation script. This script requires you have “git” and “cmake” installed on your system. 

1. Run the command “sudo apt-get install git cmake”
2. Copy install.sh to a location where you want LLVM and our tool installed
3. sh install.sh


Overview of the Tool
--------
We assume the code has been compiled into LLVM intermediate representation. During the course of this project we have decompiled binaries using the Hex-Rays Decompiler and cleaning up the c-like output to be compilable by LLVM and converted into LLVM intermediate representation. Instructions for using our clean up scripts on Hex-Rays decompiled code can be found in the Scripts/HexDecompilerScripts directory.

The tool parses the LLVM IR representation and generates a program dependence graph for the program, where the nodes are instructions and control points, while the edges represent the data or control dependences between any pair of nodes. The generation of the program dependence is partially minimized by computing a call graph utilizing program function source and sink inputs provided by the analyst. Even with this optimization the generation of the program dependence graph is embarrassingly slow, and while optimizations can be achieved through parallelism, most of our efforts have been on the research front, so this task will remain on the stack until we get around to it. 

The granularity of the graph nodes is also demand based and can be field sensitive to certain identified fields. The taint sources i.e. the user input, network and adversary input are identified semi automatically and are provided to the analysis. The sources are then mapped to the  program dependence graph and taint propagation is performed for these labels. Various inferences can be drawn based on the taint label combinations at the sinks. For performing authentication bypass analysis we need to identify the possible mediators. Querying for conditionals/statements where certain labels like password, policy and username influence the outcome can identify this. Updating the label at these points and propagating to the sink, helps determine whether a particular input has been mediated or not before reaching the sink.

If mediators are specified in the mediator input file, our mediator tampering analysis analyzes each specified mediator for data tampering. This is achieved by computing program chops (the intersection of a forward taint and a reverse taint between two program dependence nodes). Program chops describe the relationship between two entities. We generate 3 types of program chops, pre-, intra-, and post-mediator chops in order to understand the relationship between:

* Program inputs and mediator inputs
* Mediator inputs and mediator outputs (decisions)
* Mediator outputs and control logic that governs access to a sensitive sink

Once these program chops are captured for each mediator, they are analyzed for possible risky operations. This includes write operations where data is explicitly modified or overwritten, string manipulation operations, memory modification operations, and bitwise manipulation operations. Given a new set of risky operations the analysis performs a reverse tainting to identify all possible sources of modification. Depending on certain characteristics of the risky operations they are scored to generate a priority of which operations should be analyzed first. Although the constraints used to score operations is subject to change and many are still being tested for effectiveness the current running list is as follows:

* Is the data being modified directly tainted by sensitive input
* Is the data being modified used in sensitive library calls
* Is the data being modified by hard-coded constant data (integer or possible string)
* Is the operation governed by control logic that dictates access to it, are there other modification operations that lie on other branches, or before the control logic
* Does the data being modified contain any sources that are explicitly different from all other local risky operations

The output is generated as a directory for each type of program chop, for each mediator. Within contains a separate directory for each chop generated for that given mediator and chop type. Within that contains all information for that chop, including forward and reverse taint output graphs used to generate each chop, the chop output in graph format, and a directory for each write operation. Within each write operation directory contains a reverse taint graph and file outputting information regarding the constraints that were satisfied.

The output printed on screen shows the progress of the analysis as well as a priority list of which risky operations scored the highest based on our constraints.


Running the tool
---------
Inputs required: To run the tool the following inputs need to be prepared: 
1. C-like decompiled code from Hex-Rays. 
2. Manually update the c-like code to make it compilable in clang after running it through the provided scripts. 
3. Generate the bitcode file from the de-compiled code.

The tool operates on this llvm IR format. The supplementary inputs required for running a test are:
1.	source_sink.txt : This file contains the source and sink functions between which the flow needs to be analyzed. This file should be placed where the opt executable is located. 
2.	taintSource : This file specifies the taint sources in the format: <function varname label> and can be specified to the tool with option –taintSource=file_name. The standard input functions (scanf,read etc) are considered along with the specified ones. 
3.	fields: This file contains the list of fields which need to be made field sensitive for more precise data flow tracking.
4.	Mediator: The file specifies the functions identified as mediator. This can be input using the command option -mediator=file_name
5.	Query: This file specifies what kind of label intersection results need to be analyzed: format <intersect  label1 label2>, can be provided by option –query=file_name

Following is an example command to execute the tool.

opt -load <location_of_llvm-3.7_directory>/build/lib/dsa.so -load <location_of_llvm-3.7_directory>/build/lib/llvmPDG.so -TaintPDG -time-passes -full=false -mediator=<input_med_file> -taintSource=<input_taint_source_file> -S input.bc > input.ll

