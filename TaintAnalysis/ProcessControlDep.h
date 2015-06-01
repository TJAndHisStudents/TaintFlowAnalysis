

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
#include "ControlDependenceGraph.h"
#include "hookPlacement.h"

#include<set>
#include<utility>
#include<string.h>

using namespace llvm;


class ProcessControlDep : public ModulePass {
	private:
		ControlDependenceGraphs ControlDep;
		bool runOnModule(Module &M);
	public:
		static char ID; // Pass identification, replacement for typeid.
		ProcessControlDep();
		void getAnalysisUsage(AnalysisUsage &AU) const;
};
