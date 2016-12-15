add_llvm_loadable_module(llvmPDG
    CallGraphWrapper.cpp
    InputDep.cpp
    FlowGraph.cpp
    DataDepGraph.cpp
	ChopControlModule.cpp
    TaintFlow.cpp
  #  ConnectFunctions.cpp
  #  ControlDependencies.cpp
  #  DataDependencies.cpp
  #  FlowDependenceAnalysis.cpp
  #  ProgramDependencies.cpp
    
  )


include_directories(include)

#set_target_properties(llvmPDG PROPERTIES COMPILE_FLAGS " -fcxx-exceptions")
set_target_properties(llvmPDG PROPERTIES CMAKE_CXX_FLAGS "-Wall -fno-rtti")
