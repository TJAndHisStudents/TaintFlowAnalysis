#ifndef FP_H_
#define FP_H_

#ifndef DEBUG_TYPE
#define DEBUG_TYPE "fp"
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
#include "InputDep.h"




using namespace std;

namespace llvm {

class FPAnalysis : public ModulePass {
	private:
		std::set<Value*> SSO;
		std::set<BasicBlock*>  defaultHooks;
		std::map<Value*, int> lineNo;
		std::set<Value*> inputDepValues;
		std::set<SourceType*> sourceTypes;
		bool runOnModule(Module &M);
	public:
		static char ID; // Pass identification, replacement for typeid.
		FPAnalysis();
		void getAnalysisUsage(AnalysisUsage &AU) const;
	//	std::set<CallInst*> getTargetFunctions();
		std::set<BasicBlock*> getDefaultHooks();
		std::set<Value*> getSSOs();
		int getLineNo(Value*);
		void GetUses(Value* v);
};

}
//something
#endif

