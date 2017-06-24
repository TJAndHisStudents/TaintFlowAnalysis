
// LLVM includes
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"

// Tool includes
#include "FlowGraph.h"

// C++ Includes
#include <string.h>

using namespace llvm;

class ChopControlModule : public ModulePass {
 private:
  bool runOnModule(Module &M);

 

 public:
  static char ID;
  void getAnalysisUsage(AnalysisUsage &AU) const;
  ChopControlModule();
  void processChops(Module &M, Graph *depGraph, std::set<Value*> source_vals, std::list<string> mediators);

  //Get values for intramediator analysis
  std::vector<Value*> getArgVars(Function &F);
  Value* getReturnVar(Function &F);
	std::vector<Instruction*> getMedCallRetVar(Module &M, Function &F);

  //Get values for before mediator analysis
  int getParamVars(Function *caller, Function *callee);
  int getSourceVars();

  int runQueries(std::list<Instruction*> inst_list); 
  std::list<Instruction*> test_function(Function &F, Graph *depGraph);
	std::list<Instruction*> identify_instructions(Function &F, Graph *depGraph);
	int analyzeChops(std::list<string> inst_list);
  //Get values for after mediator analysis
  int getCallSiteReturnVar();
  int getSinkVar();

  std::list<GraphNode*> computeForwardTaintSlice(GraphNode *node, int height);
  std::list<GraphNode*> computeReverseTaintSlice(GraphNode *node, int height);
  
};
