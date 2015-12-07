add_llvm_loadable_module(TaintFlow
    AliasSets.cpp
    AliasSetsIza.cpp
    blockAssign.cpp
    BuildGraph.cpp
    ControlDependenceGraph.cpp
    FlowGraph.cpp
    fpt.cpp
    InputDep.cpp
    InputValues.cpp
    PADriver.cpp
    PointerAnalysis.cpp
#    ProcessControlDep.cpp
    hookPlacement.cpp
    TaintFlow.cpp
    CallGraphWrapper.cpp
  )
#target_link_libraries(TaintFlow anders-aa)
#find_package(Anders REQUIRED anders-aa)
#target_link_libraries(TaintFlow Anders)

include_directories(include)
