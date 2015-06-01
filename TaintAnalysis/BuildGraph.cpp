#include "BuildGraph.h"

#include "llvm/ADT/PostOrderIterator.h"
#define debug false

#define ProcessCallSiteFrequency 5
using namespace llvm;

///Calss CallSiteMap
///
CallSiteMap::CallSiteMap()
{

}

cl::opt<std::string> CallPathFile("callPath", cl::desc("<Source sink call path functions>"), cl::init("-"));


//Class functionDepGraph
void functionDepGraph::getAnalysisUsage(AnalysisUsage &AU) const {
    if (USE_ALIAS_SETS)
        AU.addRequired<AliasSets> ();

    AU.setPreservesAll();
}

bool functionDepGraph::runOnFunction(Function &F) {

    AliasSets* AS = NULL;

    if (USE_ALIAS_SETS)
        AS = &(getAnalysis<AliasSets> ());

    //Making dependency graph
    depGraph = new llvm::Graph(AS);
    //Insert instructions in the graph
    for (Function::iterator BBit = F.begin(), BBend = F.end(); BBit != BBend; ++BBit) {
        for (BasicBlock::iterator Iit = BBit->begin(), Iend = BBit->end(); Iit
             != Iend; ++Iit) {
            depGraph->addInst_new(Iit);
        }
    }

    //We don't modify anything, so we must return false
    return false;
}

char functionDepGraph::ID = 0;
static RegisterPass<functionDepGraph> X("functionDepGraph",
                                        "Function Dependence Graph");

//Class moduleDepGraph
void moduleDepGraph::getAnalysisUsage(AnalysisUsage &AU) const {
    if (USE_ALIAS_SETS)
        AU.addRequired<AliasSets> ();

    AU.addRequired<CallGraphWrapperPass> ();
    AU.addRequiredTransitive<AliasAnalysis>();
    AU.addRequired<MemoryDependenceAnalysis>();
    AU.addRequired<InputDep> ();
   // AU.addRequired<CallGraphWrapper>();
    //    AU.addRequiredTransitive<CallGraphWrapperPass> ();
    AU.setPreservesAll();
}

bool moduleDepGraph::runOnModule(Module &M) {

    AliasSets* AS = NULL;
    CallGraph &CG  = getAnalysis<CallGraphWrapperPass>().getCallGraph();
    AliasAnalysis &AA = getAnalysis<AliasAnalysis>();
    InputDep &IV = getAnalysis<InputDep> ();
    inputDepValues = IV.getInputDepValues();
    // CallGraphWrapper &CGW = getAnalysis<CallGraphWrapper>();
  //  IndMap = CGW.getIndirectMap();


    //Quick target functions identification.. since getting callgraphwrapper pass is not allowing further processing due to ~CallGraph() error.
    //. need to fix this later.
    ComputeIndMap(M);

    if (USE_ALIAS_SETS)
        AS = &(getAnalysis<AliasSets> ());

    //Making dependency graph
    depGraph = new Graph(AS);
    TopDownGraph = new Graph(AS);
    depGraph->toDot("base","../../AliasSetGraph.dot");
    getcallPathFunctions();


    //Bottom up pass for building the data dep graph for individual functions..
    for (scc_iterator<CallGraph*> I = scc_begin(&CG); !I.isAtEnd(); ++I) {
        const std::vector<CallGraphNode *> &SCC = *I;
        for (unsigned i = 0, e = SCC.size(); i != e; ++i) {
            Function *F = SCC[i]->getFunction();
            if (F && !F->isDeclaration()) {
                //Generate data dep for function in call path.
               if(isFunctiononCallPath(F))
                {Graph* Fdep = new Graph(AS);
                    Process_Functions(F,Fdep);
                    Process_Functions(F,TopDownGraph);
                    //  FuncDepGraphs[F] = Fdep;
                    std::string tmp = F->getName();
                    // replace(tmp.begin(), tmp.end(), '\\', '_');
                    std::string Filename = "../../demo1_" + tmp + ".dot";
                    //string fileName = "../../"+F->getName()+"_graph.dot";
                    errs()<<"\n Writing dot for ******************* "<<F->getName();
                    Fdep->toDot(F->getName(),Filename);
                }

                /*
                        //Print the generated RDS map for each definition in the given function..
                        errs()<<"\n+++++++++++++++++++++RDS Size:  "<<Fdep->RDSMap.size();

                        for(map<Value*,set<MemoryOperation*> >::iterator rdIt = Fdep->RDSMap.begin(); rdIt != Fdep->RDSMap.end();++rdIt)
                        {
                            errs()<<"\n+++===================";
                            Value* ptrVal = (*rdIt).first;
                            set<MemoryOperation*> locations = (*rdIt).second;
                            ptrVal->dump();
                            for(set<MemoryOperation*>::iterator locIt = locations.begin();locIt != locations.end();++locIt)
                            {
                                errs()<<"Location :"<<(*locIt)->parentBlock->getName()<<" "<<(*locIt)->parentFunction->getName()<<" "<<(*locIt)->lineNumber;
                                (*locIt)->assignedValue->dump();
                            }
                        }
                        */
            }

        }
    }  //end of bottom up phase.





    //  Graph* tempGraph;
    //Get the main funciton to start the parsing.
    //TopDownProcessing(main);



    //    Function* main = M.getFunction("main");
    //    if (main) {
    //       //  Process_Functions(main,TopDownGraph);
    //        //Do a top down processing here of the call graph starting from main.
    //        TopDownProcessing(main,AS);

    //    } //end of if main




    //The call Site map after processing all the functions..
    /*   errs()<<"\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
    for(map<CallSite*, map<Value*,GraphNode*> >::iterator csMap = TopDownGraph->callParamNodeMap.begin(); csMap != TopDownGraph->callParamNodeMap.end();++csMap)
    {
        //CallSite cs;
        //cs.getCalledValue()getInstruction()
        errs()<<"\n CAll Site:-----------------\n ";
        CallSite *cs = (*csMap).first;
        if(!cs)
            errs()<<"\n No call Site";

        map<Value*,GraphNode*> paramMap = (*csMap).second;
        for( map<Value*,GraphNode*>::iterator pMap = paramMap.begin();pMap != paramMap.end();++pMap)
        {
            errs()<<"\n param pointer values: ";
            (*pMap).first->dump();
            errs()<<"    node label:   "<<(*pMap).second->getLabel();
        }
    }
    errs()<<"\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
    */

    //Assiging to depgraph for taint --- temporarily
    depGraph = TopDownGraph;

    for (Module::iterator Fit = M.begin(), Fend = M.end(); Fit != Fend; ++Fit) {

        // If the function is empty, do not do anything
        // Empty functions include externally linked ones (i.e. abort, printf, scanf, ...)
        if (Fit->begin() == Fit->end())
            continue;

        //    matchParametersAndReturnValues_new(*Fit);
            matchParametersAndReturnValues(*Fit);

    }

    std::string Filename = "../../noConstTopDownGraph.dot";
    errs()<<"\n Writing dot for **************** ..TopDownGraph";
    TopDownGraph->toDot("TopDown",Filename);




    //Print Stats:
    //     errs()<<"\n Var nodes:"<<depGraph->getNumVarNodes();
    //     errs()<<"\n Mem nodes:"<<depGraph->getNumMemNodes();
    //     errs()<<"\n Op nodes:"<<depGraph->getNumOpNodes();
    //     errs()<<"\n control Edges:"<<depGraph->getNumControlEdges();
    //     errs()<<"\n Data Edges:"<<depGraph->getNumDataEdges();






    //We don't modify anything, so we must return false
    return false;
}

///Merge function in Flow graph.
//void moduleDepGraph::MergeGRaphs(Graph * fullGraph, Graph * funcGraph)
//{
// for(GraphNode::iterator gNode = funcGraph->begin(); )
//put all nodes and edges from funcGRaph to full graph... this function should be in FlowGRaph.
//}

void moduleDepGraph::Process_CallSite(Instruction* inst, Function *F)
{
    //at a call site which needs to be processed...
    //get the actual params for the function.. .. check the function  map params..


    for (Function::arg_iterator Arg = F->arg_begin(), aEnd = F->arg_end(); Arg != aEnd; Arg++) {
        //  Arg->dump();
        //
    }
    //Create the graph add edges in the graph for the given function..!!
    Process_Functions(F,TopDownGraph);


}


void moduleDepGraph::TopDownProcessing(Function * F, AliasSets * AS)
{

    //Check if the graph exists for Function..
    //FuncDepGraphs[F]
    //   Graph * FDep = new Graph(AS);

    //Processing the Function, generate the data flow graph.
    Process_Functions(F,TopDownGraph);


    // FuncDepGraphs[F]  = FDep;

    //Graph * TopDownGraph = new

    //Iterating over the function blocks to look for next call sites and process the targets.
    for (Function::iterator BBit = F->begin(), BBend = F->end(); BBit
         != BBend; ++BBit) {
        for (BasicBlock::iterator Iit = BBit->begin(), Iend = BBit->end(); Iit
             != Iend; ++Iit) {

            /// get the function graph..
            ///merge the function graph in the bigger topdown graph..
            ///look for the actual nodes and  then look for the param nodes.. and add connections.
            /// if the params are gep then do the root mapping as before..
            /// maintin a call string.. of as u reach the funcitons.. if a pattern starts repeating in the call string then we need to get truncate it.
            /// Getting all the domianant conditionals..?
            //TODO: also handle if && !isa<InvokeInst>(U) check how is it different.
            Instruction *Inst = &*Iit;

            if(CallInst * CI = dyn_cast<CallInst>(Inst))
            {
                //    errs()<<"\nPROPAGATE:";
                //   CI->dump();
                CallSite cs(CI);

                Function * calledFunc = CI->getCalledFunction();
                if(calledFunc && !calledFunc->isDeclaration())
                {
                    //      errs()<<"\nPROPAGATE: Call Site with called func and not delaration.. !!";
                    //    CI->dump();

                    //Update the callSite structs;
                    CallSiteMap* cs_data = new CallSiteMap();
                    cs_data->cs = &cs;
                    cs_data->CalledFunc = calledFunc;
                    int processFreq = isCallSiteProcessed(cs_data);
                    if(processFreq == 0 || processFreq <= ProcessCallSiteFrequency)
                    {
                        Process_CallSite(Inst,calledFunc);

                        Graph * CallF_Graph = FuncDepGraphs[calledFunc];
                        // map params and merge graph..
                    }
                } //if called func

                //   errs()<<"\n";
                if(calledFunc && calledFunc->isDeclaration())
                {
                    //    errs()<<"\nPROPAGATE: Call Site with called func and IS delaration.. !!";
                    //  CI->dump();
                    HandleLibraryFunctions(CI,F);
                }

                //TODO: else if indirect call then process for all target calls.
                if(!calledFunc)
                {
                    //     errs()<<"\nPROPAGATE: in !calledFunc i.e no Func Body";
                    //   CI->dump();
                    Type *ty = cs.getCalledValue()->getType();
                    if(ty->isPointerTy())
                    {
                        //       errs()<<"\nPROPAGATE: Call Site with no Func Body and is pointer type.. indirect call";
                        //     CI->dump();

                        if(ty->getPointerElementType()->isFunctionTy())
                        {

                            //Check if we have the targets identified for this..
                            vector<Function*> targetFunctions = IndMap[cs];
                            //        errs()<<"\nPROPAGATE: Function pointer type !!  : "<<targetFunctions.size();
                            //       CI->dump();
                            for(vector<Function*>::iterator funit = targetFunctions.begin(); funit != targetFunctions.end();++funit)
                            {

                                CallSiteMap* cs_data = new CallSiteMap();
                                cs_data->cs = &cs;
                                cs_data->CalledFunc = (*funit);
                                int processFreq = isCallSiteProcessed(cs_data);
                                if(processFreq == 0 || processFreq <= ProcessCallSiteFrequency)
                                {
                                    Process_CallSite(Inst,(*funit));
                                }
                            }
                        }
                    }


                    //                    //calledFunc->isDeclaration() ||
                    //                    if(calledFunc)
                    //                    {
                    //                        if(calledFunc->isDeclaration())
                    //                        {
                    //                            errs()<<"\n error case !!";
                    //                        }
                    //                    }

                    if((ty->isPointerTy()  && IndMap[cs].size()==0 ))
                    {
                        //Library call case:: -- where the if(!calledfunc && (isdeclaration  || (if pointer..then pointer targets==0)  ))
                        // Hnalde lib call-- default way and then add more specifications.

                        //   errs()<<"\n\n Handling LIBRARY CALL for: ";
                        // CI->dump();
                        HandleLibraryFunctions(CI,F);

                    }

                } //end of indirect function check.

            } //if call inst


        }
    }

}


void moduleDepGraph::HandleLibraryFunctions(CallInst * CI, Function * F)
{
    //   errs()<<"\nPROPAGATE: Identified LIBRARY FUNCT ";
    // CI->dump();
    //Default case : Adding edge from the callsite params to return variable.

    //   errs()<<"\n In funct handle lib call!";


    CallSite::arg_iterator AI;
    CallSite::arg_iterator EI;
    bool noReturn;
    OpNode* retPHI;

    CallSite CS(CI);

    noReturn = CS.doesNotReturn();
    Instruction *caller = cast<Instruction> (CI);
    //  SmallVector<std::pair<GraphNode*, GraphNode*>, 4> Parameters(F->arg_size());
    map<int, GraphNode*> parameters;


    //TODO: Correct the return node param from the matchparam functions..
    //    errs()<<"\n Check no return !";
    if (!noReturn) {

        //    errs()<<"\n Create Phi node !";
        retPHI = new OpNode(Instruction::PHI);
        //    errs()<<"\n Add inst !";
        GraphNode* callerNode = TopDownGraph->addInst(caller,0,F);
        if(callerNode)
            TopDownGraph->addEdge(retPHI, callerNode);

    }


    //Get the callSite param to graphnode map generated for the callsite
    CallSiteMap * callParams = TopDownGraph->CallSiteParams[CI];

    //   errs()<<"\n Before checking call params!";
    //     if(!callParams)
    //         errs()<<"\n\n CallSite map not found !!";
    int i;

    //   errs()<<"\n before iterating over args!";
    for (i = 0, AI = CS.arg_begin(), EI = CS.arg_end(); AI != EI; ++i, ++AI) {
        //Parameters[i].second = TopDownGraph->addInst(*AI,i);
        Use * useparam = &*AI;
        Value * param =  useparam->get();
        GraphNode *  paramNode;

        //      errs()<<"\n assigning the argnode map..!";
        if(callParams)
        {
            if(callParams->CI == caller)
            {
                paramNode = callParams->ArgNodeMap[param];
            }
            else
                paramNode = TopDownGraph->addInst(*AI,i,F);
        }

        //    errs()<<"\n param location: "<<paramNode->getLabel();
        //   errs()<<"\n Checked if param node null";
        if(paramNode)
            parameters[i] = paramNode;
        else
            errs()<<"\n CallSite param node not found..: ";

        //    errs()<<"\n Check if param of i null!  : "<<i;
        if(parameters[i] == NULL)
        {
            //          errs()<<"\n Param node found empty at call Site  for param: ";
            // Generate a new node for the parameter.
            parameters[i] = TopDownGraph->addInst(*AI,i,F);
        }

        //Add connection from each param to the retrun (Phi) node of the funciton!
        if(!noReturn)
            TopDownGraph->addEdge(parameters[i],retPHI);

    }


    //Handle specific cases:


}


int moduleDepGraph::isCallSiteProcessed(CallSiteMap* csm)
{
    int freq = 0;
    for(set<CallSiteMap*>::iterator csIT = CallSitesProcessed.begin(), csITEnd = CallSitesProcessed.end(); csIT != csITEnd; ++csIT)
    {
        if((csm->cs == (*csIT)->cs) && (csm->CalledFunc == (*csIT)->CalledFunc))
        {
            return (*csIT)->frequency;
        }
    }

    return freq;
}


//Additional function to wrap the queue so that it will only add unique elements.
void moduleDepGraph::AddUniqueQueueItem(set<BasicBlock*> &setBlock, queue<BasicBlock*> &queueBlock, BasicBlock * bb)
{
    //  queue<BasicBlock*>::iterator it = find(queueBlock.begin(),queueBlock.end(),bb);


    if (setBlock.find(bb) == setBlock.end())
    {
        queueBlock.push(bb);
        setBlock.insert(queueBlock.back());  // or "s.emplace(q.back());"
    }
}



//Precosses functions as per the control flow of and process the individual basic blocks..
void moduleDepGraph::Process_Functions(Function* F, Graph* F_Graph)
{
    //
    if(debug) errs()<<"\n Processing funtion recursive.. "<< F->getName() <<"  ";
    MDA = &getAnalysis<MemoryDependenceAnalysis>((*F));


    //Add nodes for function parameters..
    for (Function::arg_iterator Arg = F->arg_begin(), aEnd = F->arg_end(); Arg != aEnd; Arg++) {
        // Arg->dump();
        GraphNode * pNode = F_Graph->addInst(Arg,-10,F);

        /*    if(pNode)
        {
        errs()<<"\n\n Prama node created : "<<pNode->getLabel();
        if(isa<MemNode>(pNode))
            errs()<<"  MEm node: ";
        Arg->dump();
        }
        else
        {    errs()<<"\n No Prama node created... ! ";
             Arg->dump();
        }
*/
        //The live map should also consider the call statements as the store for pointer only for the formal params..
        //While doing a live check for a paramter to a function...
        //if it reaches the entry and there is no store.. we should map it to the node from the call site....
        //and add it in live map...
        //also at the point of


    }

    //Get first block to process:

    set<BasicBlock*> affected_BB;
    queue<BasicBlock*> WorkList;
    std::set<BasicBlock*> setBlock;

    //Get the entry block in the function.
    Function::iterator bbit = F->begin();
    if(bbit)
    {
        WorkList.push(bbit);

        while(!WorkList.empty())
        {
            BasicBlock *BBproc = WorkList.front();
            WorkList.pop();
            //Trial.. also try and remove the element from set as well.. will get only recursive elements..
            // setBlock.erase(BBproc); dont process loop blocks again..
            //TODO: reprocess the loop blocks only if context changes somehow.. (need to apply some proper policy to this..)


            //Process the popped block
            affected_BB.clear();
            affected_BB = Process_Block(BBproc, F_Graph);

            //Iterate over the affected blocks and add them in worklist.
            for(set<BasicBlock*>::iterator afB = affected_BB.begin(); afB != affected_BB.end();++afB)
            {
                WorkList.push(*afB);
                //TODO: need some base condition to stop adding the affected blocks recursively....
            }

            //Iterate over the successors of the block and add them in the worklist.
            TerminatorInst *TermInt =   BBproc->getTerminator();
            int SuccNum = TermInt->getNumSuccessors();
            for(int i =0;i<SuccNum;i++)
            {
                BasicBlock *BBnext = TermInt->getSuccessor(i);
                // WorkList.push(BBnext);
                AddUniqueQueueItem(setBlock,WorkList,BBnext);
                //   errs()<<"\n\n Adding Block..  "<< BBnext->getName()<<"  worklist Size   "<<WorkList.size();
                //  errs()<<" = "<<setBlock.size();
            }
        }
    }

    //Mark Graph generated for the function
    FunctionGraphGen[F] = 1;

}


set<BasicBlock*>  moduleDepGraph::Process_Block(BasicBlock* BB, Graph* F_Graph)
{

    set<BasicBlock*> affected_BB;

    Function * parentFunction = BB->getParent();
    //  if(debug) errs()<<"\n\n Processing Block recursive.. "<< BB->getName() <<" in function "<<BB->getParent()->getName();

    int counter =0;
    //Iterate over each instruction in the block and process it.
    for (BasicBlock::iterator Iit = BB->begin(), Iend = BB->end(); Iit
         != Iend; ++Iit) {


        /*  Code to check memory dependence (see if it helps).. work on it later..

         errs()<<"\n\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
            Iit->dump();

            if(isa<LoadInst>(&*Iit))
            {
                errs()<<"\n@@@@@@@@@@@@@@@@@ is load..!! ";
                MemDepResult memDep = MDA->getDependency(&*Iit);
               // if(memDep.isDef())
                //{
                    Instruction * dep = memDep.getInst();
                    errs()<<"\n\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
                    (*Iit).dump();
                    errs()<<"  depends on ";
                    dep->dump();
                //}
            }
            */


        //Code to backtrack the stores for each load...keep a record of it in the depgraph.
        //F_Graph->LiveMap;
        if(isa<LoadInst>(&*Iit))
        {
            //   errs()<<"\n\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
            //    Iit->dump();

            BasicBlock * parentBlock = Iit->getParent();
            // Value* loadPtr = Iit->getOperand(0);

            //set<BasicBlock*> ProcessedBlocks;
            ProcessedBlocks.clear();
            ProcessedBlocks.insert(parentBlock);
            Value * LoadPtr = Iit->getOperand(0);
            //m2.insert(m1.begin(), m1.end());
            F_Graph->BackupLiveMap[LoadPtr].clear();
            F_Graph->BackupLiveMap[LoadPtr].insert(F_Graph->LiveMap[LoadPtr].begin(),F_Graph->LiveMap[LoadPtr].end()); // = F_Graph->LiveMap[LoadPtr];
            F_Graph->LiveMap.clear();
            CollectLiveStores(parentBlock, Iit,F_Graph);
            // errs()<<"\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
        }
        if(isa<CallInst>(&*Iit))
        {
            //collect Live stores for the parameters.
            // map<CallSite*,map<Value*,set<MemoryOperation*> > >  callSiteLiveParams;

            CallSite CS(&*Iit);
            //map<Value*,set<MemoryOperation*> > paramLiveMap;
            //set<MemoryOperation*>  stores;
            //TODO: record the live information at the call site..
            CallSite::arg_iterator AI;
            CallSite::arg_iterator EI;

            //   errs()<<"\n\n CallSite arguments fo ..... : ";
            //CS.getCalledValue()->dump();
            for (AI = CS.arg_begin(), EI = CS.arg_end(); AI != EI; ++AI) {
                //Parameters[i].second = TopDownGraph->addInst_new(*AI);
                //  Value * param = &*AI;
                Use * useparam = &*AI;
                Value * param =  useparam->get();
                GraphNode* Gnode = F_Graph->findNode(param);
                if(Gnode != NULL)
                {

                    //check if memory node/pointer.. if so then update the live stores
                    if(isa<MemNode>(Gnode))
                    {
                        //    errs()<<"\n MEm param Node found : "<<Gnode->getLabel();
                        //   param->dump();
                        CollectLiveStores_Params(Iit->getParent(),&*Iit,param,F_Graph);
                    }

                }

                //param->get()->dump();

            }



        }

        F_Graph->addInst(Iit,counter,parentFunction);
        counter++;
    }

    return affected_BB;
}



void moduleDepGraph::CollectLiveStores_Params(BasicBlock* predBlock, Instruction * Cinst,Value* pointer, Graph* F_Graph)
{
    BasicBlock * parentBlock = Cinst->getParent();
    CallSiteMap * callParams = new CallSiteMap();
    bool begin;
    int LineNumber = 0;
    if(parentBlock==predBlock)
        begin = false;
    else
        begin = true;

    //  set<BasicBlock*> worklist;
    // for each basic block walking over the predecessors in the graph
    //  errs()<<"\n  Processing  Block --- "<<predBlock->getName();
    LineNumber = predBlock->size();

    // ProcessedBlocks.insert(predBlock);
    for (BasicBlock::reverse_iterator Iit = predBlock->rbegin(), Iend = predBlock->rend(); Iit != Iend; ++Iit) {
        //   Iit--;
        LineNumber--;
        if((&*Iit) == Cinst)
        {
            //      errs()<<"\nLoad at.. "<<LineNumber;
            begin =true;
        }
        if(begin==true)
        {
            if(isa<StoreInst>(&*Iit))
            {
                //     Iit->dump();
                Value * LoadPtr = pointer; // Linst->getOperand(0);
                Value * StorePtr = Iit->getOperand(1);
                Value * RootPtr = pointer; //Linst->getOperand(0);
                if(isa<GetElementPtrInst>(LoadPtr))
                {
                    // errs()<<"\n\n\n IT is a GEP..!!";
                    //get root

                    GetElementPtrInst* GI = dyn_cast<GetElementPtrInst>(LoadPtr);
                    RootPtr =  GI->getPointerOperand();
                }
                if((LoadPtr==StorePtr) || (RootPtr==StorePtr))
                {
                    //    errs()<<"\n----Matching store found at line.. "<<LineNumber;
                    MemoryOperation *storeLoc = new MemoryOperation;
                    storeLoc->parentBlock = predBlock;
                    storeLoc->parentFunction = predBlock->getParent();
                    storeLoc->lineNumber = LineNumber;
                    storeLoc->MemInst = (&*Iit);
                    storeLoc->assignedValue = StorePtr;
                    storeLoc->setVal();
                    //Also copy all the live stores for the root as well.
                    F_Graph->LiveMap[LoadPtr].insert(F_Graph->LiveMap[RootPtr].begin(),F_Graph->LiveMap[RootPtr].end());

                    set<MemoryOperation*> memOps = F_Graph->LiveMap[LoadPtr];
                    memOps.insert(storeLoc);
                    F_Graph->LiveMap[LoadPtr] = memOps;
                    //      errs()<<"\n IDVALUE Obtained: ---- "<<storeLoc->IdValue;
                    return;
                }


                //Check for the root pointer as well if the store happens to a field of the structure.
                if(isa<GetElementPtrInst>(StorePtr))
                {
                    //    errs()<<"\nStore is is a GEP..!!";
                    //get root

                    GetElementPtrInst* GI = dyn_cast<GetElementPtrInst>(StorePtr);
                    Value * StoreRootPtr =  GI->getPointerOperand();
                    //  StoreRootPtr->dump();
                    //  StorePtr->dump();
                    //  RootPtr->dump();
                    //  LoadPtr->dump();

                    if((LoadPtr==StoreRootPtr) || (RootPtr==StoreRootPtr))
                    {
                        //           errs()<<"\n----Matching store found at line.. "<<LineNumber;
                        MemoryOperation *storeLoc = new MemoryOperation;
                        storeLoc->parentBlock = predBlock;
                        storeLoc->parentFunction = predBlock->getParent();
                        storeLoc->lineNumber = LineNumber;
                        storeLoc->MemInst = (&*Iit);
                        storeLoc->assignedValue = StorePtr;
                        storeLoc->setVal();
                        set<MemoryOperation*> memOps = F_Graph->LiveMap[LoadPtr];
                        memOps.insert(storeLoc);
                        F_Graph->LiveMap[LoadPtr] = memOps;
                        //          errs()<<"\n IDVALUE Obtained: ---- "<<storeLoc->IdValue;
                        return;
                    }
                }

            }

            //If it reaches a load from the saem pointer before reaching any store then reuse the live map for the exisiting load
            else if(isa<LoadInst>(&*Iit))
            {
                Value * NewLoadPtr = Iit->getOperand(0);
                Value * LoadPtr = pointer; //Linst->getOperand(0);
                if(NewLoadPtr==LoadPtr)
                {
                    //         errs()<<"\n LOad map found at :  ";
                    //        Iit->dump();
                    F_Graph->LiveMap[LoadPtr].insert(F_Graph->BackupLiveMap[LoadPtr].begin(),F_Graph->BackupLiveMap[LoadPtr].end());
                    //  F_Graph->LiveMap[LoadPtr] = F_Graph->BackupLiveMap[LoadPtr];
                    return;
                }

                //that means no related store to the pointer has been found.. then add the live map from before..?
                //how u erased it..
            }
        }
    }


    // Recursively process all pred blocks..
    for (pred_iterator PI = pred_begin(predBlock), E = pred_end(predBlock); PI != E; ++PI) {
        BasicBlock *Pred = *PI;
        int processedSize = ProcessedBlocks.size();
        ProcessedBlocks.insert(Pred);
        if(processedSize!=ProcessedBlocks.size())
        {
            //         errs()<<"\n Prcessed Block Size --- "<<ProcessedBlocks.size();
            CollectLiveStores_Params(Pred,Cinst,pointer,F_Graph);
        }
    }

}




void moduleDepGraph::CollectLiveStores(BasicBlock* predBlock, Instruction * Linst,Graph* F_Graph)
{
    BasicBlock * parentBlock = Linst->getParent();
    bool begin;
    int LineNumber = 0;
    if(parentBlock==predBlock)
        begin = false;
    else
        begin = true;

    //  set<BasicBlock*> worklist;
    // for each basic block walking over the predecessors in the graph
    //  errs()<<"\n  Processing  Block --- "<<predBlock->getName();
    LineNumber = predBlock->size();

    // ProcessedBlocks.insert(predBlock);
    for (BasicBlock::reverse_iterator Iit = predBlock->rbegin(), Iend = predBlock->rend(); Iit != Iend; ++Iit) {
        //   Iit--;
        LineNumber--;
        if((&*Iit) == Linst)
        {
            //      errs()<<"\nLoad at.. "<<LineNumber;
            begin =true;
        }
        if(begin==true)
        {
            if(isa<StoreInst>(&*Iit))
            {
                //     Iit->dump();
                Value * LoadPtr = Linst->getOperand(0);
                Value * StorePtr = Iit->getOperand(1);
                Value * RootPtr = Linst->getOperand(0);
                if(isa<GetElementPtrInst>(LoadPtr))
                {
                    // errs()<<"\n\n\n IT is a GEP..!!";
                    //get root

                    GetElementPtrInst* GI = dyn_cast<GetElementPtrInst>(LoadPtr);
                    RootPtr =  GI->getPointerOperand();
                }
                if((LoadPtr==StorePtr) || (RootPtr==StorePtr))
                {
                    //    errs()<<"\n----Matching store found at line.. "<<LineNumber;
                    MemoryOperation *storeLoc = new MemoryOperation;
                    storeLoc->parentBlock = predBlock;
                    storeLoc->parentFunction = predBlock->getParent();
                    storeLoc->lineNumber = LineNumber;
                    storeLoc->MemInst = (&*Iit);
                    storeLoc->assignedValue = StorePtr;
                    storeLoc->setVal();
                    //Also copy all the live stores for the root as well.
                    F_Graph->LiveMap[LoadPtr].insert(F_Graph->LiveMap[RootPtr].begin(),F_Graph->LiveMap[RootPtr].end());

                    set<MemoryOperation*> memOps = F_Graph->LiveMap[LoadPtr];
                    memOps.insert(storeLoc);
                    F_Graph->LiveMap[LoadPtr] = memOps;
                    //      errs()<<"\n IDVALUE Obtained: ---- "<<storeLoc->IdValue;
                    return;
                }


                //Check for the root pointer as well if the store happens to a field of the structure.
                if(isa<GetElementPtrInst>(StorePtr))
                {
                    //    errs()<<"\nStore is is a GEP..!!";
                    //get root

                    GetElementPtrInst* GI = dyn_cast<GetElementPtrInst>(StorePtr);
                    Value * StoreRootPtr =  GI->getPointerOperand();
                    //  StoreRootPtr->dump();
                    //  StorePtr->dump();
                    //  RootPtr->dump();
                    //  LoadPtr->dump();

                    if((LoadPtr==StoreRootPtr) || (RootPtr==StoreRootPtr))
                    {
                        //           errs()<<"\n----Matching store found at line.. "<<LineNumber;
                        MemoryOperation *storeLoc = new MemoryOperation;
                        storeLoc->parentBlock = predBlock;
                        storeLoc->parentFunction = predBlock->getParent();
                        storeLoc->lineNumber = LineNumber;
                        storeLoc->MemInst = (&*Iit);
                        storeLoc->assignedValue = StorePtr;
                        storeLoc->setVal();
                        set<MemoryOperation*> memOps = F_Graph->LiveMap[LoadPtr];
                        memOps.insert(storeLoc);
                        F_Graph->LiveMap[LoadPtr] = memOps;
                        //          errs()<<"\n IDVALUE Obtained: ---- "<<storeLoc->IdValue;
                        return;
                    }
                }

            }

            //If it reaches a load from the saem pointer before reaching any store then reuse the live map for the exisiting load
            else if(isa<LoadInst>(&*Iit))
            {
                Value * NewLoadPtr = Iit->getOperand(0);
                Value * LoadPtr = Linst->getOperand(0);
                if(NewLoadPtr==LoadPtr && ((&*Iit) != Linst))
                {
                    //         errs()<<"\n LOad map found at :  ";
                    //        Iit->dump();
                    F_Graph->LiveMap[LoadPtr].insert(F_Graph->BackupLiveMap[LoadPtr].begin(),F_Graph->BackupLiveMap[LoadPtr].end());
                    //  F_Graph->LiveMap[LoadPtr] = F_Graph->BackupLiveMap[LoadPtr];
                    return;
                }

                //that means no related store to the pointer has been found.. then add the live map from before..?
                //how u erased it..
            }
        }
    }


    // Recursively process all pred blocks..
    for (pred_iterator PI = pred_begin(predBlock), E = pred_end(predBlock); PI != E; ++PI) {
        BasicBlock *Pred = *PI;
        int processedSize = ProcessedBlocks.size();
        ProcessedBlocks.insert(Pred);
        if(processedSize!=ProcessedBlocks.size())
        {
            //         errs()<<"\n Prcessed Block Size --- "<<ProcessedBlocks.size();
            CollectLiveStores(Pred,Linst,F_Graph);
        }
    }

}


void moduleDepGraph::matchParametersAndReturnValues_new(Function &F) {

    // Only do the matching if F has any use
    //Commenting cause we need to do the mapping even if there is no use of the return val of the function.
    // if (F.isVarArg() || !F.hasNUsesOrMore(1)) {
    //        return;
    // }

    if(debug) errs()<<"\n Function arg matching for;; "<< F.getName();

    // Data structure which contains the matches between formal and real parameters
    // First: formal parameter
    // Second: real parameter
    SmallVector<std::pair<GraphNode*, GraphNode*>, 4> Parameters(F.arg_size());

    // Fetch the function arguments (formal parameters) into the data structure
    Function::arg_iterator argptr;
    Function::arg_iterator e;
    unsigned i;



    FuncParamInfo * funcParams = TopDownGraph->functionParamMap[&F];
    //Create the PHI nodes for the formal parameters
    for (i = 0, argptr = F.arg_begin(), e = F.arg_end(); argptr != e; ++i, ++argptr) {

        OpNode* argPHI = new OpNode(Instruction::PHI);
        GraphNode* argNode = NULL;
        if(funcParams)
            if(TopDownGraph->isMemoryPointer(argptr))
            {
                argNode = funcParams->ParamNodeMap[argptr];
            }
            else
                argNode = TopDownGraph->addInst(argptr,i,&F);

        if (argNode != NULL)
        {

            TopDownGraph->addEdge(argPHI, argNode);
            if (debug) errs()<<"\nAdding Edge between the phi to the formal node in mem.. "<< argPHI->getLabel()<<" -- "<<argNode->getLabel();
        }

        Parameters[i].first = argPHI;
    }


    // Check if the function returns a supported value type. If not, no return value matching is done
    bool noReturn = F.getReturnType()->isVoidTy();

    // Creates the data structure which receives the return values of the function, if there is any
    SmallPtrSet<llvm::Value*, 8> ReturnValues;

    if (!noReturn) {
        // Iterate over the basic blocks to fetch all possible return values
        for (Function::iterator bb = F.begin(), bbend = F.end(); bb != bbend; ++bb) {
            // Get the terminator instruction of the basic block and check if it's
            // a return instruction: if it's not, continue to next basic block
            Instruction *terminator = bb->getTerminator();

            ReturnInst *RI = dyn_cast<ReturnInst> (terminator);

            if (!RI)
                continue;

            // Get the return value and insert in the data structure
            ReturnValues.insert(RI->getReturnValue());
        }
    }

    //  for (Value::use_iterator UI = F.use_begin(), E = F.use_end(); UI != E; ++UI) {
    for (Value::user_iterator UI = F.user_begin(), E = F.user_end(); UI != E; ++UI) {
        User *U = *UI;

        // Ignore blockaddress uses

        //NT: TODO: Solve the error in the following:
        if (isa<BlockAddress> (U))
            continue;

        // Used by a non-instruction, or not the callee of a function, do not
        // match.
        //Handle Invoke instructions..!! && !isa<InvokeInst> (U)
        if (!isa<CallInst> (U))
            continue;

        CallInst * CI = dyn_cast<CallInst>(U);

        Instruction *caller = cast<Instruction> (U);

        CallSite CS(caller);
        if (!CS.isCallee(&UI.getUse()))
            continue;

        // Iterate over the real parameters and put them in the data structure
        CallSite::arg_iterator AI;
        CallSite::arg_iterator EI;

        map<Value*,GraphNode*> paramMap = TopDownGraph->callParamNodeMap[&CS];


        CallSiteMap * callParams = TopDownGraph->CallSiteParams[CI];
        if(!callParams)
            errs()<<"\n\n CallSite map not found !!";

        //    errs()<<"\n Param Map size : "<<paramMap.size();
        for (i = 0, AI = CS.arg_begin(), EI = CS.arg_end(); AI != EI; ++i, ++AI) {
            //Parameters[i].second = TopDownGraph->addInst(*AI,i);
            Use * useparam = &*AI;
            Value * param =  useparam->get();
            GraphNode *  paramNode;

            if(callParams)
            {
                if(callParams->CI == caller)
                {
                    paramNode = callParams->ArgNodeMap[param];
                }
            }

            if(paramNode!=NULL)
                Parameters[i].second = paramNode;
            else
                errs()<<"\n CallSite param node not found..: ";

            /*     errs()<<"\n\n################################ Callmap used for at param location : "<<i<<"   map size :  "<<callParams->ArgNodeMap.size();
             // errs()<<"\n\n++++++++++++++++++++++ Callmap updated for:"<<callParamNodeMap[&CS].size()<<"  "<<callParamNodeMap.size();
              callParams->CI->dump();
              param->dump();
              errs()<<"  " <<callParams->ArgNodeMap[param]->getLabel();
              errs()<<"\n++++++++++++++++++++++";
              errs()<<"\n################################";

                */

            if(Parameters[i].second == NULL)
            {
                errs()<<"\n Param node found empty at call Site  for param: ";
                param->dump();
                Parameters[i].second = TopDownGraph->addInst(*AI,i,&F);
            }
        }

        // Match formal and real parameters
        for (i = 0; i < Parameters.size(); ++i) {


            TopDownGraph->addEdge(Parameters[i].second, Parameters[i].first);
            if(debug) errs()<<"\nAdding Edge between formal and real "<< Parameters[i].second->getLabel()<<" -- "<<Parameters[i].first->getLabel();
        }

        // Match return values
        if (!noReturn) {

            OpNode* retPHI = new OpNode(Instruction::PHI);
            GraphNode* callerNode = TopDownGraph->addInst(caller,0,&F);
            TopDownGraph->addEdge(retPHI, callerNode);

            for (SmallPtrSetIterator<llvm::Value*> ri = ReturnValues.begin(),
                 re = ReturnValues.end(); ri != re; ++ri) {
                GraphNode* retNode = TopDownGraph->addInst(*ri,0,&F);

                TopDownGraph->addEdge(retNode, retPHI);
            }

        }

        // Real parameters are cleaned before moving to the next use (for safety's sake)
        for (i = 0; i < Parameters.size(); ++i)
            Parameters[i].second = NULL;
    }

    if (!F.isVarArg() && F.hasNUsesOrMore(1))
        TopDownGraph->deleteCallNodes(&F);
}





void moduleDepGraph::matchParametersAndReturnValues(Function &F) {

    // Only do the matching if F has any use
    if (F.isVarArg() || !F.hasNUsesOrMore(1)) {
        return;
    }
    if(debug) errs()<<"\n Function arg matching for;; "<< F.getName();

    // Data structure which contains the matches between formal and real parameters
    // First: formal parameter
    // Second: real parameter
    SmallVector<std::pair<GraphNode*, GraphNode*>, 4> Parameters(F.arg_size());

    // Fetch the function arguments (formal parameters) into the data structure
    Function::arg_iterator argptr;
    Function::arg_iterator e;
    unsigned i;

    //Create the PHI nodes for the formal parameters
    for (i = 0, argptr = F.arg_begin(), e = F.arg_end(); argptr != e; ++i, ++argptr) {

        OpNode* argPHI = new OpNode(Instruction::PHI);
        GraphNode* argNode = NULL;
        argNode = depGraph->addInst(argptr,-1,&F);

        if (argNode != NULL)
        {
            depGraph->addEdge(argPHI, argNode);
            if (debug) errs()<<"\nAdding Edge between "<< argPHI->getLabel()<<" -- "<<argNode->getLabel();
        }

        Parameters[i].first = argPHI;
    }

    // Check if the function returns a supported value type. If not, no return value matching is done
    bool noReturn = F.getReturnType()->isVoidTy();

    // Creates the data structure which receives the return values of the function, if there is any
    SmallPtrSet<llvm::Value*, 8> ReturnValues;

    if (!noReturn) {
        // Iterate over the basic blocks to fetch all possible return values
        for (Function::iterator bb = F.begin(), bbend = F.end(); bb != bbend; ++bb) {
            // Get the terminator instruction of the basic block and check if it's
            // a return instruction: if it's not, continue to next basic block
            Instruction *terminator = bb->getTerminator();

            ReturnInst *RI = dyn_cast<ReturnInst> (terminator);

            if (!RI)
                continue;

            // Get the return value and insert in the data structure
            ReturnValues.insert(RI->getReturnValue());
        }
    }

    //  for (Value::use_iterator UI = F.use_begin(), E = F.use_end(); UI != E; ++UI) {
    for (Value::user_iterator UI = F.user_begin(), E = F.user_end(); UI != E; ++UI) {
        User *U = *UI;
        //                Use *BU = &UI.getUse();
        // Value::const_user_iterator & CUI =UI::reference;
        //  const Use CU = UI.getUse();
        // Use *U = &*UI;
        // Ignore blockaddress uses

        //NT: TODO: Solve the error in the following:
        if (isa<BlockAddress> (U))
            continue;

        // Used by a non-instruction, or not the callee of a function, do not
        // match.
        if (!isa<CallInst> (U) && !isa<InvokeInst> (U))
            continue;

        Instruction *caller = cast<Instruction> (U);

        CallSite CS(caller);
        if (!CS.isCallee(&UI.getUse()))
            continue;

        // Iterate over the real parameters and put them in the data structure
        CallSite::arg_iterator AI;
        CallSite::arg_iterator EI;

        for (i = 0, AI = CS.arg_begin(), EI = CS.arg_end(); AI != EI; ++i, ++AI) {
            Parameters[i].second = depGraph->addInst(*AI,-1,&F);
        }

        // Match formal and real parameters
        for (i = 0; i < Parameters.size(); ++i) {

            depGraph->addEdge(Parameters[i].second, Parameters[i].first);
            if(debug) errs()<<"\nAdding Edge between "<< Parameters[i].second->getLabel()<<" -- "<<Parameters[i].first->getLabel();
        }

        // Match return values
        if (!noReturn) {

            OpNode* retPHI = new OpNode(Instruction::PHI);
            GraphNode* callerNode = depGraph->addInst(caller,-1,&F);
            depGraph->addEdge(retPHI, callerNode);

            for (SmallPtrSetIterator<llvm::Value*> ri = ReturnValues.begin(),
                 re = ReturnValues.end(); ri != re; ++ri) {
                GraphNode* retNode = depGraph->addInst(*ri,-1,&F);
                depGraph->addEdge(retNode, retPHI);
            }

        }

        // Real parameters are cleaned before moving to the next use (for safety's sake)
        for (i = 0; i < Parameters.size(); ++i)
            Parameters[i].second = NULL;
    }

    depGraph->deleteCallNodes(&F);
}



bool moduleDepGraph::isFunctiononCallPath(Function * F)
{
    string funcName = F->getName();
    if(std::find(callPathFunctions.begin(), callPathFunctions.end(), funcName)!=callPathFunctions.end())
        return true;
    else
        return false;
}



void moduleDepGraph::getcallPathFunctions()
{
   //read file if specified.
    std::ifstream cpFile (CallPathFile.c_str(), std::ifstream::in);
       std::string line;
       if(!cpFile)
       {
           errs() << " Could not open the Call Path file \n";
       }
       else
       {
           while (getline(cpFile, line))
           {
               line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
               callPathFunctions.push_back(line);
           }
       }


    //Get directly from the callgraph path is possible.

}





void llvm::moduleDepGraph::deleteCallNodes(Function* F) {
    depGraph->deleteCallNodes(F);
}



char moduleDepGraph::ID = 0;
static RegisterPass<moduleDepGraph> Y("moduleDepGraph",
                                      "Module Dependence Graph");

char ViewModuleDepGraph::ID = 0;
static RegisterPass<ViewModuleDepGraph> Z("view-depgraph",
                                          "View Module Dependence Graph");





////----------------------------------------------------------------------------------------------
/// Funtion pointer Indirect targets computation!!
///
///



std::vector<Function*> moduleDepGraph::getPossibleCallees(CallSite cs)
{
    std::vector<Function*> calledF;
    Function* ft;
    // ft->getType()
    // errs()<<"\nCall Site : ";
    //cs.getCalledValue()->getType()->dump();

    for(std::set<Function*>::iterator fs = addressTaken.begin();fs != addressTaken.end();++fs)
    {
        Type *funcType = (*fs)->getType();
        Type *callType = cs.getCalledValue()->getType();

        //  errs()<<"\nType Vals--------------------\n";
        //raw_ostream *func_stream;
        //funcType->print(func_stream);
        //string fstring;
        //fstring<<func_stream;


        //int n1 =
        if(funcType==callType)
        {
            //  errs()<<"\n";
            // callType->dump();
            //errs()<<"\n Possible Target Found :"<< (*fs)->getName();
            //(*fs)->getType()->dump();
            calledF.push_back(*fs);
            IndMap[cs].push_back(*fs);
        }
        // FunctionType

    }

    return calledF;
}


static bool isAddressTaken(Value* V) {
    //Find the users for the value to see if it was assigned.
    for (Value::const_use_iterator I = V->use_begin(), E = V->use_end(); I != E; ++I) {
        User *U = I->getUser();
        if(isa<StoreInst>(U))
            return true;
        if (!isa<CallInst>(U) && !isa<InvokeInst>(U)) {
            if(U->use_empty())
                continue;
            if(isa<GlobalAlias>(U)) {
                if(isAddressTaken(U))
                    return true;
            } else {
                if (Constant *C = dyn_cast<Constant>(U)) {
                    if (ConstantExpr *CE = dyn_cast<ConstantExpr>(C)) {
                        if (CE->getOpcode() == Instruction::BitCast) {
                            return isAddressTaken(CE);
                        }
                    }
                }
                return true;
            }

            // FIXME: Can be more robust here for weak aliases that
            // are never used
        } else {
            llvm::CallSite CS(cast<Instruction>(U));
            if (!CS.isCallee(&*I))
                return true;
        }
    }
    return false;
}

void moduleDepGraph::ComputeIndMap(Module  &M)
{
    //Collect all the address take funcitons in a collection.
    for (Module::iterator FI = M.begin(), FE = M.end(); FI != FE; ++FI){
        if(isAddressTaken(FI)) {
            addressTaken.insert(FI);
        }
    }


    //Get all the callSites:
    for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I){
        if (!I->isDeclaration()){
            for (Function::iterator F = I->begin(), FE = I->end(); F != FE; ++F){
                for (BasicBlock::iterator B = F->begin(), BE = F->end(); B != BE; ++B){
                    if (isa<CallInst>(B) || isa<InvokeInst>(B)) {
                        CallSite cs(B);
                        AllSites.push_back(cs);
                        Function* CalledF = cs.getCalledFunction();

                        if (isa<UndefValue>(cs.getCalledValue())) continue;
                        if (isa<InlineAsm>(cs.getCalledValue())) continue;

                        //
                        // If the called function is casted from one function type to
                        // another, peer into the cast instruction and pull out the actual
                        // function being called.
                        //
                        if (!CalledF)
                            CalledF = dyn_cast<Function>(cs.getCalledValue()->stripPointerCasts());

                        if (!CalledF) {
                            Value * calledValue = cs.getCalledValue()->stripPointerCasts();

                            /*    DEBUG(errs()<<"\n\n Called Value : ";
                          cs.getCalledValue()->dump();
                          errs()<<"Called Value stripped pointer cast : ";
                          calledValue->dump();
                          cs.getCalledValue()->getType()->dump();

                  ); */

                            Type *ty = cs.getCalledValue()->getType();
                            if(ty->isPointerTy())
                            {
                                if(ty->getPointerElementType()->isFunctionTy())
                                {
                                    // cs.getCalledValue()->dump();
                                    //  FunctionType *F_type = dyn_cast<FunctionType*>(ty);
                                    calledFuncs = getPossibleCallees(cs);

                                    int calledFuncCount = calledFuncs.size();
                                    //  errs()<<"\n Number of possible calls at the call site: "<<calledFuncCount;

                                    if(calledFuncCount==0)
                                    {
                                        unResolved.insert(cs);
                                    }
                                    else
                                    {
                                        //     errs()<<"\n\nREsolved CAll !! ?";
                                        //Resolved calls
                                        //  pair<CallSite,std::vector<Function*> > cllInfo;
                                        // cllInfo.first = cs;
                                        //cllInfo.second = calledFuncs;
                                        // IndMap.insert(cllInfo);
                                        //IndMap[cs]calledFuncs;
                                    }
                                    Type *Fty = ty->getPointerElementType();
                                    int numArgs = Fty->getFunctionNumParams();
                                    //   callto[numArgs]++;
                                }
                            }


                            if (isa<ConstantPointerNull>(calledValue)) {
                                //   ++DirCall;
                                CompleteSites.insert(cs);
                            } else {
                                //    IndCall++;




                            }
                        } else {
                            //      ++DirCall;
                            //  IndMap[cs].push_back(CF);
                            CompleteSites.insert(cs);
                        }
                    } //end if call or invoke
                } //end block iterator
            } //end func iterator
        } //end if nor declaration
    } //end for module iterator




}
