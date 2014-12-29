#ifndef __INPUTDEP_H__
#define __INPUTDEP_H__

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/User.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/DebugInfo.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/ilist.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/IR/Metadata.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"
//#include "DepGraph.h"
#include "llvm/ADT/StringRef.h"
#include "rapidxml/rapidxml.hpp"
#include <fstream>
#include <cstdlib>
#include <sstream>
#include "FunctionInfo.h"
//#include <iostream>
#include<set>

using namespace llvm;




class InputPass : public ModulePass {
	private:
		std::set<Value*> inputDepValues;
		std::set<Function*> targetFunctions;
		std::map<Value*, int> lineNo;
		std::set<std::string>  targetNames;
		std::vector<FunctionInfo*> FunctionInfos;
		std::vector<DefUseInfo*> DefUseInfos;
		bool runOnModule(Module &M);
	public:
		static char ID; // Pass identification, replacement for typeid.
		InputPass();
		void getAnalysisUsage(AnalysisUsage &AU) const;
		bool isInputDependent(Value* V);
		std::set<Value*> getInputDepValues();
		std::set<std::string> getTargetNames();
		std::set<Function*> getTargetFunctions();
		std::vector<FunctionInfo*> getFunctionInfo();
		std::vector<DefUseInfo*> getDefUseInfo();
		int getLineNo(Value*);
		void printer();
		void ReadSources();
		void ReadFunctionInfo();
		void ReadTargets();
		void ReadDefUse();
		
		//FunctionInfoSet FS;
};

#endif
