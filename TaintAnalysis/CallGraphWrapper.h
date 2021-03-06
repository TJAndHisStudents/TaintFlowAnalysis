#ifndef CALLGRAPHWRAPPER
#define CALLGRAPHWRAPPER


#ifndef DEBUG_TYPE
#define DEBUG_TYPE "CallGraph"
#endif

#define USE_ALIAS_SETS true

#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Constants.h"
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/DominanceFrontier.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SCCIterator.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/IR/CallSite.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/CallGraphSCCPass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/Support/raw_ostream.h"
#include "AliasSets.h"
#include <deque>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "llvm/IR/DebugInfo.h"
#include "InputDep.h"



using namespace std;



typedef struct {

	std::string name = "";
	std::vector<Function*> functionList;

} IndCallerforCallee;

typedef std::vector<IndCallerforCallee> IndCallerList;

typedef struct {

	std::string functionName = "";
	std::vector<Function*> priorFunctionCallList;

} customCallsite;

typedef struct {
	
	std::string functionName = "";
	Function* function;
	Instruction* callInst;
} instructionCallSite;

typedef std::vector<customCallsite> ListofCallsite;

typedef std::vector<instructionCallSite> Path;
typedef std::vector<Path> PathSet;




 namespace llvm
{
 class CallSite;
// template<class PtrType, unsigned SmallSize>
 //class SmallPtrSet;

// Class CallGraphWrapperPass
class CallGraphWrapper : public ModulePass  {
    private:
            bool runOnModule(Module &M);
            //  bool runOnSCC(CallGraphSCC &SCC) override;
    public:
    CallGraph *CG;
        static char ID;
        std::set<Function*> addressTaken;
        int nodeCount;
        std::vector<Function*> calledFuncs;
        std::set<CallSite> unResolved;
        std::set<CallGraphNode*> IndTargetNodes;
        std::map<CallSite, std::vector<Function*> > IndMap;
        std::map<Function*, vector<CallSite> > FunctiontoIndCalls;
        std::set<CallSite> CompleteSites;
        std::list<CallSite> AllSites;
        void getAnalysisUsage(AnalysisUsage &AU) const;
        bool hasAddressTaken(llvm::Function *);
        std::vector<Function*> getPossibleCallees(CallSite cs);
        CallGraphWrapper();
        void toDot(CallGraphNode* RootNode, CallGraph CG, string fileName);
        void toDot(CallGraphNode* RootNode, CallGraph CG,raw_ostream *stream);
        std::string getNodeLabel(CallGraphNode *Node);
        CallGraphNode * getDefinedCallGraphNode(Function * F);
        std::map<CallSite, std::vector<Function*> >  getIndirectMap();

	void findAllPaths(PathSet* pathSet,
					const std::string& start,
					const std::string& end);
	void OtherFunction(PathSet& pathSet,
					Path& visited,
					const std::string& end,
					IndCallerList* indirectCallerList);

    	bool ContainsFunction( std::vector<Function*> functionList, Function* func);
	bool isOnPath( Path& functionStructs, Function* func);
	void createReverseMapping(IndCallerList* indirectCallerList);
	void printContextSensitiveCalls(std::vector<Function*> functionList);
	void printContextSensitivePaths(PathSet pathSets);

    //

    PathSet callPathsAll;

      PathSet callPathSets;
     PathSet getCallPaths();

};



class BottomUpPass : public CallGraphSCCPass  {
    private:
          //  bool runOnModule(Module &M);
              bool runOnSCC(CallGraphSCC &SCC) override;
    public:
       // CallGraph* CG;
        static char ID;
        void getAnalysisUsage(AnalysisUsage &AU) const;
        BottomUpPass();
};

}

#endif // CALLGRAPHWRAPPER
