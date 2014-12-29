#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/User.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/ilist.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/IR/Metadata.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"
#include "llvm/DebugInfo.h"
#include "llvm/ADT/StringRef.h"
#include<set>
#include<utility>



using namespace llvm;




class FunctionInfo {
	public:
		//static char ID; // Pass identification, replacement for typeid.
		FunctionInfo();
		//std::set<std::string> functionName;
		std::string FunctionName;
		int begin;
		int end;
		std::string begin_block;
		std::string end_block;	  
};


class DefUseInfo {
	public:
		DefUseInfo();
		bool isdef;
		std::string definedVar;
		std::string targetVar;
		std::string defBlock;
		std::string useBlock;
		int lineNumber;
		std::set<DefUseInfo*> UseInfo;
};


/*
class FunctionInfoSet {

public:
 std::set<FunctionInfo*> FunctionInfos;
  void print();

}; */
