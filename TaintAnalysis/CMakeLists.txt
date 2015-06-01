add_llvm_loadable_module(TaintFlow
    AliasSets.cpp
    AliasSetsIza.cpp
    blockAssign.cpp
    BuildGraph.cpp
    ControlDependenceGraph.cpp
    FlowGraph.cpp
    FPAnalysis.cpp
    fpt.cpp
    hookPlacement.cpp
    InputDep.cpp
    InputValues.cpp
    PADriver.cpp
    PointerAnalysis.cpp
    ProcessControlDep.cpp
    TaintFlow.cpp
    CallGraphWrapper.cpp
  )
#target_link_libraries(TaintFlow anders-aa)
#find_package(Anders REQUIRED anders-aa)
#target_link_libraries(TaintFlow Anders)

include_directories(include)
