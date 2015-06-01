#include "ProcessControlDep.h"

using namespace llvm;

ProcessControlDep::ProcessControlDep() :
	ModulePass(ID) {
}


bool ProcessControlDep::runOnModule(Module &M) {
	
	
	int one =0;
	for (Module::iterator F = M.begin(), eM = M.end(); F != eM; ++F) {
		for (Function::iterator BB = F->begin(), e = F->end(); BB != e; ++BB) {
			for (BasicBlock::iterator I = BB->begin(), ee = BB->end(); I != ee; ++I) {
			    if(one<=2)
			    {
			  //  Value* V_in;
			    //FIXME: Temporary assignment of the hook blocks  for testing...
			     if(BB->getName().equals("BB_204"))  // change this condition to get the region block.
			     {
			    //	defaultHooks.insert(BB);
			        one++;
			     }
			 	}
			    
			  }
			}
		}
	return false;
}



void ProcessControlDep::getAnalysisUsage(AnalysisUsage &AU) const {
	AU.setPreservesAll();
	AU.addRequired<ControlDependenceGraphs> ();
	AU.addRequired<hookPlacement> ();
}



char ProcessControlDep::ID = 0;
static RegisterPass<ProcessControlDep> Q("proessCont",
		"bbbs");
