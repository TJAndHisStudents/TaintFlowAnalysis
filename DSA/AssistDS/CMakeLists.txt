add_llvm_library(AssistDS
    ArgCast.cpp
    ArgSimplify.cpp
    DSNodeEquivs.cpp
    DataStructureCallGraph.cpp
    Devirt.cpp
    DynCount.cpp
    FuncSimplify.cpp
    FuncSpec.cpp
    GEPExprArgs.cpp
    IndCloner.cpp
    Int2PtrCmp.cpp
    LoadArgs.cpp
    MergeGEP.cpp
    SVADevirt.cpp
    SimplifyExtractValue.cpp
    SimplifyGEP.cpp
    SimplifyInsertValue.cpp
    SimplifyLoad.cpp
    StructReturnToPointer.cpp
    TypeChecks.cpp
    TypeChecksOpt.cpp
  )
#target_link_libraries(TaintFlow anders-aa)
#find_package(Anders REQUIRED anders-aa)
#target_link_libraries(TaintFlow Anders)

include_directories(include)
