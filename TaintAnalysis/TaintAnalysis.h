#ifndef __VUL_ARRAYS_H__
#define __VUL_ARRAYS_H__

//#include "TaintAnalysis.h"
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
#include "InputPass.h"
#include "InputValues.h"
#include "BuildGraph.h"
#include<set>
#include<utility>


using namespace llvm;


class TaintAnalysis : public ModulePass {
	private:
		bool isValueInpDep(Value* V);
		std::set<GraphNode*> tainted;
	public:
		Graph* depGraph;
		bool runOnModule(Module &M);
		std::set<Value*> inputDepValues;
		std::set<Value*> inputValues;
	//	std::set<CallInst*> targetFunctions;
		std::set<BasicBlock*> targetBlocks;
		TaintAnalysis() : ModulePass(ID) {
	    }
		static char ID;
	//	TaintAnalysis();
		void getAnalysisUsage(AnalysisUsage &AU) const;
		std::set<GraphNode*> getTaintedValues();
	//	void getSinkSourceDependence();
		void getSinkSourceBlockDependence();
	//	void getHookLocation();
		
};

#endif
