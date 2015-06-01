#ifndef HOOKPLACE_H_
#define HOOKPLACE_H_

#ifndef DEBUG_TYPE
#define DEBUG_TYPE "hookplace"
#endif



#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CallSite.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/Support/raw_ostream.h"
#include <deque>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include "llvm/IR/DebugInfo.h"
#include "InputValues.h"
#include "InputDep.h"
//#include "AddStore.h"
#include "blockAssign.h"
#include "TaintFlow.h"
#include "ControlDependenceGraph.h"




using namespace std;

namespace llvm {


class SSObjects {

	SSObjects() {}
};


class SSOperations
{
	public:
	Value * sObject;
	Type* objType;
	Value * sField;
	Type* fieldType;
	//std::vector<int> indexSet;
	std::vector<Value*> indexSet;
	bool isWrite;
	
	SSOperations() {}
};


class SSOBlocks
{

SSOBlocks() {}
};


class defaultHook {
	public:
	std::set<SSOperations*> blockssOps;
	BasicBlock* hookBlock;
	
	defaultHook() {}
};




class hookPlacement : public ModulePass {
	private:
		std::set<Value*> SSO;
		std::set<BasicBlock*>  defaultHooks;
		std::map<Value*, int> lineNo;
		std::set<Value*> Lookups;
		
		std::set<defaultHook*> defHooks;
		
		
		
		bool runOnModule(Module &M);
		
		ControlDependenceGraph conDepGraph;
		ControlDependenceGraphBase  conDepGraphBase;
		std::set<ControlDependenceNode*> controlDepHookNodes;
		
		Graph* depGraph;
		std::set<Value*> inputDepValues;
		std::set<CallInst*> targetFunctions;
		std::set<BasicBlock*> targetBlocks;
		std::set<SourceType*> sourceTypes;
		std::set<SourceType*> sourceTypesDerived;
		std::set<GraphNode*> tainted;
	public:
		static char ID; // Pass identification, replacement for typeid.
		hookPlacement();
		void getAnalysisUsage(AnalysisUsage &AU) const;
	//	std::set<CallInst*> getTargetFunctions();
		std::set<defaultHook*> getDefaultHooks();
		void GenerateDefaultPlacement();
		void GenControlDepHookNodes();
		std::set<Value*> getLookups();
		std::set<Value*> getSSOs();
		int getLineNo(Value*);
};

}
//something
#endif

