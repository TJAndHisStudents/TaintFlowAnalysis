//===- TaintAnalysis.cpp - Taint tracking solution for source code ---------------===//
//
//                     
// 
//
//===----------------------------------------------------------------------===//
//
// Description: The file implements a taint tracking solution for source code.
// Input files: 
// Output : 
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "TaintAnalysis"
#include "TaintAnalysis.h"

#define debug_taint = false

using namespace llvm;
static cl::opt<llvm::cl::boolOrDefault>
		source( "source",
				cl::desc("true = Source, false = Binary; default is source"),
				cl::value_desc("Input type description"));



//STATISTIC(TaintedNodes_count, "The number of nodes marked as tainted in the dependence graph");
//STATISTIC(Nodes_count, "The number of nodes in the dependence graph");
STATISTIC(Functions_count, "The number of Functions in the Code");
STATISTIC(NumTaintedNodes, "The number of nodes marked as tainted in the dep graph");
STATISTIC(NumNodes, "The number of nodes in the dep graph");

//TaintAnalysis::TaintAnalysis() : ModulePass(ID) { }


 bool TaintAnalysis::runOnModule(Module &M) {

	InputPass &IV = getAnalysis<InputPass> ();   //TODO: Change to the binary input pass.
	inputValues = IV.getInputDepValues();
	InputValues &IDV = getAnalysis<InputValues> ();
	inputDepValues = IDV.getInputDepValues();
//	targetFunctions = IV.getTargetFunctions();
//	targetBlocks = IDV.getTargetBlock();
	
		 	
	BuildGraph& DepGraph = getAnalysis<BuildGraph>();
//	functionDepGraph& funcDepGraph = getAnalysis<functionDepGraph>();
 
     errs()<< "\n\n Input Variable  -- : "<< inputDepValues.size();

	//Getting dependency graph
	Graph *g = DepGraph.depGraph;
	g->toDot(M.getModuleIdentifier(), "depGraphtest1.dot");
//	g->toDotLines(M.getModuleIdentifier(), "depGraphtestlines1.dot");
	
//	Graph *g2 = funcDepGraph.depGraph;
//	g2->toDot(M.getModuleIdentifier(), "fucntionDepGraph.dot");
//	g2->toDotLines(M.getModuleIdentifier(), "fucntionDepGraphlines.dot");

      ++Functions_count;
      errs() << "\nTaint Analysis: ";
      std::string tmp = M.getModuleIdentifier();
      errs() << tmp << "\n\n";
      
      
      int inputDepVal_count = 0;
	std::string appendVal ="a";
	std::set<Value*> inputDepValuespart;
    errs()<<"----------------------Taint Flow Analysis-----------------\n";
    for(std::set<Value*>::iterator tv = inputDepValues.begin(); tv != inputDepValues.end(); ++tv)
    {

       errs()<<**tv <<"\n";
       std::string appendVal = (**tv).getName();
       inputDepValuespart.clear();
		 errs()<<"----------------------Input deps above-----------------\n";
		//tainted = depGraph->getDepValues(inputDepValues);

		inputDepValuespart.insert(*tv);
		tainted =  g->getDepValues(inputDepValuespart);



		NumTaintedNodes = tainted.size();
		NumNodes = g->getNodes().size();
		errs()<<"\nNumber of tainted nodes: " <<NumTaintedNodes <<"\n";
		errs()<<"Number of total nodes: " <<NumNodes <<"\n\n\n";
		
	
    	for (Module::iterator F = M.begin(), endF = M.end(); F != endF; ++F) {
		for (Function::iterator BB = F->begin(), endBB = F->end(); BB != endBB; ++BB) {
			for (BasicBlock::iterator I = BB->begin(), endI = BB->end(); I
					!= endI; ++I) {
				 if(depGraph->isValidInst(I))
				 {
					GraphNode* g = depGraph->findNode(I);
					if (tainted.count(g)) {
						LLVMContext& C = I->getContext();
						MDNode* N = MDNode::get(C, MDString::get(C, "TaintAnalysis"));
					   // char numstr[21]; // enough to hold all numbers up to 64-bits
						//sprintf(numstr, "%d", inputDepVal_count);
						std::string taintVal = "TAINT_"+appendVal ;
						//std::string taintVal = std::strcat("tainted",numstr);
						I->setMetadata(taintVal, N);
						//errs() << "Tainted Value    " << *I << "\n";
					}
				}
			}
		}
	}
	inputDepVal_count++;
      }
      
     //Access the function variable only after checking.. not all codes will have main..
      //Only for Source:
      Function* main = M.getFunction("main");
      if(main != NULL)
       errs().write_escaped(main->getName())<< '\n';
      //errs().write_escaped(M.getModuleIdentifier()) << '\n';
      return false;
    }
    
    
 /*   
///Function:
// This function will get the sink values from the input file,
//and check the source dependent path for each of the sinks.
void TaintAnalysis::getSinkSourceDependence(){

	std::map<llvm::GraphNode*, std::vector<GraphNode*> > result;
	std::pair<GraphNode*, int>  nearestDep;

	for(std::set<CallInst *>::iterator target = targetFunctions.begin(); target != targetFunctions.end();++target)
	{
		//get the value for the sink..
		Value* sink = (*target)->getCalledValue();
		//Value* sink = (*target)->getOp();
		result = depGraph->getEveryDependency(sink,inputDepValues,true);
		nearestDep = depGraph->getNearestDependency(sink,inputDepValues,true);
       // errs()<<" Nearest Dep Result: "<< (*nearestDep.first).getName();   //getName gives error:

		errs()<<"\n**********Sink**************        "<<  sink->getName()   <<"\n";
       // errs()<<
		typedef std::map<llvm::GraphNode*, std::vector<GraphNode*> >::iterator resultIterator;
        for(resultIterator res = result.begin();res!=result.end();++res)
        {
        	errs()<<"Sink node: "<< res->first->getName() <<" Path : \n";
        	typedef std::vector<GraphNode*>::iterator pathIterator;
        	for(pathIterator path = res->second.begin(); path != res->second.end(); ++path)
        	{
        		errs()<<"    Path nodes: "<< (*path)->getName() <<"\n";
        	}
        }
	}
}
*/

/*

void TaintAnalysis::getSinkSourceBlockDependence(){

	errs()<<"\n \nFunction getting called..!!";
	for(std::set<BasicBlock*>::iterator TB = targetBlocks.begin();TB != targetBlocks.end(); ++TB)
	{
		errs()<<"\nTarget Block " <<(*TB)->getName();
		for(pred_iterator PI = pred_begin(*TB); PI != pred_end(*TB); ++PI){
			 errs()<< "\n Preds in sequence:  "<< (*PI)->getName();
			 for (BasicBlock::iterator TI = (*PI)->begin(), endTI = (*PI)->end(); TI
			 					!= endTI; ++TI) {
				errs()<<"\n Instruction" <<*TI;
			 }

		}
	}

}
*/

/*
void TaintAnalysis::getHookLocation(){
	errs()<<"\n***************************** Hooks Identified ***********************************\n";

	std::map<CallInst*,std::pair<BasicBlock*,BasicBlock*> >  HookMap;
	for(std::set<CallInst *>::iterator target = targetFunctions.begin(); target != targetFunctions.end();++target)
		{
		   // errs()<<"\nTainted Target  : " << *target;
		    //Check if tainted:
		    //TODO: Need the kind string to get metadata.. but it is taint+value.. dont' know the value here..
		    //Check if you can get all of the metadat and find this is tainted.. and what taint.
		   // MDNode* TaintData = target.getMetadata();

		    std::pair<CallInst*,std::pair<BasicBlock*,BasicBlock*> > element;
		    element.first =  *target;
		    BasicBlock* parent = (*target)->getParent();
		    element.second = std::make_pair(parent,parent->getSinglePredecessor());
			//errs()<<"\nParent  : " << *parentInst;
		    HookMap.insert(element);
		}

	typedef std::map<CallInst*,std::pair<BasicBlock*,BasicBlock*> >::iterator  hookMapIterator;
	for(hookMapIterator HM = HookMap.begin(); HM!= HookMap.end(); ++HM)
	{
		Function *Callee = (*HM).first->getCalledFunction();
		errs()<<"\nTainted Target  : " << Callee->getName();
		BasicBlock* previousBlock= (*HM).second.second;
		TerminatorInst* TI = previousBlock->getTerminator();
		if(TI->getNumSuccessors() == 2)
		{
			BasicBlock* TrueBranch =  TI->getSuccessor(0);
			//BasicBlock* FalseBranch =  TI->getSuccessor(1);
			if(TrueBranch == (*HM).second.first)
					errs()<<"\n \t Hook on True branch for ";
			else
				errs()<<"\n \t Hook on False branch for ";
		}
		//Iterate on the last Block and get the comp statement..
		for (BasicBlock::iterator I = previousBlock->begin(), endI = previousBlock->end(); I
							!= endI; ++I) {
			if (CmpInst *CI = dyn_cast<CmpInst>(I))
				CI->dump();
		}
	}

	errs()<<"\n*---Optimized--- When a branching needs hook on both branches for same Taint and same target.\n";

	//Process Optimization here

	errs()<<"\n*************************************************************************\n";
	HookMap.clear();
}
*/

    // We don't modify the program, so we preserve all analyses
void TaintAnalysis::getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
      AU.addRequired<InputPass> ();
      AU.addRequired<InputValues> ();
  //    AU.addRequired<functionDepGraph> ();
      AU.addRequired<BuildGraph> ();

    }



char TaintAnalysis::ID = 0;
static RegisterPass<TaintAnalysis> X("TaintB", "Version 1.2: Taint Analysis Pass");


