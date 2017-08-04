#!/bin/bash
# Run Demo


wget http://releases.llvm.org/3.7.0/llvm-3.7.0.src.tar.xz
tar -xvf llvm-3.7.0.src.tar.xz
git clone https://github.com/TJAndHisStudents/TaintFlowAnalysis.git
cp -r TaintFlowAnalysis/DSA/DSA/ llvm-3.7.0.src/lib/
cp -r TaintFlowAnalysis/DSA/AssistDS/ llvm-3.7.0.src/lib/
cp -r TaintFlowAnalysis/DSAHeaders/dsa/ llvm-3.7.0.src/include/
cp -r TaintFlowAnalysis/DSAHeaders/assistDS/ llvm-3.7.0.src/include/
cp -r TaintFlowAnalysis/DataFlowAnalysis/ llvm-3.7.0.src/lib/
echo 'add_subdirectory(DSA)' >> llvm-3.7.0.src/lib/CMakeLists.txt 
echo 'add_subdirectory(DataFlowAnalysis)' >> llvm-3.7.0.src/lib/CMakeLists.txt 
cd llvm-3.7.0.src/
mkdir build
cd build
cmake -G "Unix Makefiles" ../
make ENABLE_OPTIMIZED=0

