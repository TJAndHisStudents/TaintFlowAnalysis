add_llvm_loadable_module(dsa
    AddressTakenAnalysis.cpp
    AllocatorIdentification.cpp
    Basic.cpp
    BottomUpClosure.cpp
    CallTargets.cpp
    CompleteBottomUp.cpp
    DSCallGraph.cpp
    DSGraph.cpp
    DSTest.cpp
    DataStructure.cpp
    DataStructureStats.cpp
    EntryPointAnalysis.cpp
    EquivClassGraphs.cpp
    GraphChecker.cpp
    Local.cpp
    Printer.cpp
    SanityCheck.cpp
    StdLibPass.cpp
    TopDownClosure.cpp
    TypeSafety.cpp
  )
#target_link_libraries(TaintFlow anders-aa)
#find_package(Anders REQUIRED anders-aa)
#target_link_libraries(TaintFlow Anders)

include_directories(include)
