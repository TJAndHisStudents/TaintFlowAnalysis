#ifndef __TAINTFLOW_H__
#define __TAINTFLOW_H__

#include "llvm/Pass.h"
//#include "llvm-c/core.h"
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
#include "llvm/IR/DebugInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/CFG.h"
//#include "llvm/Analysis/Dominators.h"
//#include "DepGraph.h"
//#include "InputValues.h"
#include "InputDep.h"
#include "DataDepGraph.h"
#include "ChopControlModule.h"
//#include "AddStore.h"
//#include "blockAssign.h"
//#include "hookPlacement.h"
#include<set>
#include<utility>
#include<string.h>
//#include<../ptranalysis/anders-aa/anders-aa.h>
using namespace llvm;


class TaintFlow : public ModulePass {
	private:
		Graph* depGraph;
        std::set<Value*> inputDepValues;
        std::map<Value*, std::string> ValLabelmap;
		std::set<CallInst*> targetFunctions;
		std::set<BasicBlock*> targetBlocks;
		std::set<SourceType*> sourceTypes;
		std::set<SourceType*> sourceTypesDerived;
        std::set<std::string> mediatorFunctions;
        std::set<QueryInput*> queryinputs;
		bool runOnModule(Module &M);
		bool isValueInpDep(Value* V);
		std::set<GraphNode*> tainted;
        std::map<Value*, set<GraphNode*> > taintGraphMap;
        Value * targetVal;
	public:
		static char ID;
		void getAnalysisUsage(AnalysisUsage &AU) const;
		std::set<GraphNode*> getTaintedValues();
		bool isValueTainted(Value* v);
		void getSinkSourceDependence();
		void getSinkSourceBlockDependence();
		void getHookLocation();
		void testProcessing();
        void PrintTainted(std::set<GraphNode*> tainted);
        void HandleQueries(Module &M);
        void propagateLabel(GraphNode* snode);
        void updateTaintLabels();
        set<Value*> getTaintedVals(set<GraphNode*>);
		TaintFlow();

};

#endif
