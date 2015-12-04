#include "BuildGraph.h"
//#include "dsa/DataStructure.h"
//#include "dsa/DSGraph.h"

#include "llvm/ADT/PostOrderIterator.h"
#define debug false
#define code false


#define ProcessCallSiteFrequency 5
using namespace llvm;

///Calss CallSiteMap
///
CallSiteMap::CallSiteMap()
{

}


///Class RelevantFields
///
RelevantFields::RelevantFields()
{

}


cl::opt<std::string> CallPathFile("callPath", cl::desc("<Source sink call path functions>"), cl::init("-"));
cl::opt<std::string> Fields("fields", cl::desc("<Relevant fields file>"), cl::init("-"));
static cl::opt<bool, false> fullAnalysis("fullAnalysis", cl::desc("Long analysis"), cl::NotHidden);

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
    AU.addRequired<CallGraphWrapper> ();
    AU.addRequiredTransitive<AliasAnalysis>();
    AU.addRequired<MemoryDependenceAnalysis>();
    AU.addRequired<InputDep> ();
    // if(USE_DSA1)
    AU.addRequired<BUDataStructures> ();
    // AU.addRequired<CallGraphWrapper>();
    //    AU.addRequiredTransitive<CallGraphWrapperPass> ();
    AU.setPreservesAll();
}

bool moduleDepGraph::runOnModule(Module &M) {

    AliasSets* AS = NULL;
    CallGraph &CG  = getAnalysis<CallGraphWrapperPass>().getCallGraph();
    InputDep &IV = getAnalysis<InputDep> ();
    inputDepValues = IV.getInputDepValues();
    CallGraphWrapper &cgWrapper = getAnalysis<CallGraphWrapper> ();

    callPaths = cgWrapper.getCallPaths();

    //Read the relevant tainted fields supplied to the code to make it field sensitive.
    ReadRelevantFields();

    //if(USE_DSA1) //need to do this irrespective of the flag, null initialize not allowed.
    BUDataStructures &BU_ds = getAnalysis<BUDataStructures>();

    if (USE_ALIAS_SETS)
        AS = &(getAnalysis<AliasSets> ());

    //Making dependency graph
    depGraph = new Graph(AS);
    TopDownGraph = new Graph(AS);
    FullGraph = new Graph(AS);

    //depGraph->toDot("base","../../AliasSetGraph.dot");
    //Get the relevant functions on the call path.. approach before the call strings approach.
    //Check if the option provided..
    getcallPathFunctions();

    //Initialize DSA pointsto graph for each function.
    InitializePointtoGraphs(M, BU_ds);

    //Process Global variables... :
    ProcessGlobals(M,TopDownGraph);

    //Use the call strings generated for the provided source and sink functions if it generates paths
    if(useCallStrings && !(callPaths.size()==0))
    {

        //Compute the taint summary graphs for the functions in the callpath, and preempt the the rest.
        //Bottom up pass for building the data dep graph for individual functions..
        for (scc_iterator<CallGraph*> I = scc_begin(&CG); !I.isAtEnd(); ++I) {
            const std::vector<CallGraphNode *> &SCC = *I;
            for (unsigned i = 0, e = SCC.size(); i != e; ++i) {
                Function *F = SCC[i]->getFunction();
                if (F && !F->isDeclaration()) {
                    //Check if the functions is on the call strings.
               //     if(isFunctiononCallString(F))
                    {

                        Graph* Fdep = new Graph(AS);
                   //     if(code) errs()<<"\n\n Processing function fdep graph.. for "<<F->getName();
                  //      Process_Functions(F,Fdep,SensitivityDepth);
                        if(code) errs()<<"\n Processing function topdown graph.. for "<<F->getName()<<"\n";
                        Process_Functions(F,TopDownGraph,SensitivityDepth);
                          FuncDepGraphs[F] = Fdep;
                        std::string tmp = F->getName();
                        // replace(tmp.begin(), tmp.end(), '\\', '_');
                        std::string Filename = "../../demo1_" + tmp + ".dot";
                        //string fileName = "../../"+F->getName()+"_graph.dot";
                        //    errs()<<"\n Processing Function ******************* "<<F->getName();
             //           Fdep->toDot(F->getName(),Filename);
                    }

                }

            }
        }  //end of bottom up phase.

        int path_count = 0;
        for (std::vector<Path>::iterator I = callPaths.begin(); I != callPaths.end(); I++)
        {
            errs()<<"\n Proessing Path.... : "<<path_count++;
            Path currPath = (*I);
            //std::reverse(currPath.begin(), currPath.end());
            callStringFunctions.clear();
            // currentCallString = "";
            currentCallString.clear();
            for(std::vector<instructionCallSite>::const_iterator instCallSite = currPath.begin(); instCallSite != currPath.end(); instCallSite++)
            {

                instructionCallSite callSiteInfo = (*instCallSite);
                Function * F = callSiteInfo.function;

                if(F != NULL && !F->isDeclaration())
                {
                    Graph *F_Graph = FuncDepGraphs[F];
                    ForwardProcess(F,F_Graph);
                    ProcessedCallsites.push_back(&callSiteInfo);

                }
            }
        }
    }
    else  //Process the whole graph..
    {

            Function* main = M.getFunction("main");
            if (main) {
               //  Process_Functions(main,TopDownGraph);
                //Do a top down processing here of the call graph starting from main.
                TopDownProcessing(main,AS);
                depGraph = FullGraph;

            } //end of if main
    }


    //Assiging to depgraph for taint --- temporarily
    depGraph = TopDownGraph;

    for (Module::iterator Fit = M.begin(), Fend = M.end(); Fit != Fend; ++Fit) {

        // If the function is empty, do not do anything
        // Empty functions include externally linked ones (i.e. abort, printf, scanf, ...)
        if (Fit->begin() == Fit->end())
            continue;
        matchParametersAndReturnValues(*Fit);

    }

    std::string Filename = "../../noConstTopDownGraph.dot";
    errs()<<"\n Writing dot for **************** ..TopDownGraph";
  //  TopDownGraph->toDot("TopDown",Filename);

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
    Process_Functions(F,TopDownGraph,SensitivityDepth);


}


void moduleDepGraph::ProcessGlobals(Module& M, Graph* TopDownGraph)
{
    for (Module::global_iterator GVI = M.global_begin(), E = M.global_end();
         GVI != E; ) {
        GlobalVariable *GV = GVI++;
        // Global variables without names cannot be referenced outside this module.
        if (!GV->hasName() && !GV->isDeclaration() && !GV->hasLocalLinkage())
            GV->setLinkage(GlobalValue::InternalLinkage);
        //Process the GV to add node in topdowngraph.
        Value * GV_Val = GV;
        //GV->dump();
        //get val make node.. what all nodes to make and what all to eliminate.. nodes with no use delete..
        // can run globalopt before...
        TopDownGraph->addGlobalVal(GV);

    }
}


void moduleDepGraph::TopDownProcessing(Function * F, AliasSets * AS)
{
    //Processing the Function, generate the data flow graph.
    Process_Functions(F,FullGraph,SensitivityDepth);

    //Iterating over the function blocks to look for next call sites and process the targets.
    for (Function::iterator BBit = F->begin(), BBend = F->end(); BBit
         != BBend; ++BBit) {
        for (BasicBlock::iterator Iit = BBit->begin(), Iend = BBit->end(); Iit
             != Iend; ++Iit) {

            //TODO: also handle if && !isa<InvokeInst>(U) check how is it different.
            Instruction *Inst = &*Iit;

            if(CallInst * CI = dyn_cast<CallInst>(Inst))
            {

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


///Function to initialize the points to graph for each function from the the DSA points to analysis.
///Stores the result in funcPointsToMap[F]
void moduleDepGraph::InitializePointtoGraphs(Module &M,BUDataStructures &BU_ds)
{
    for (Module::iterator F = M.begin(), endF = M.end(); F != endF; ++F) {

        //  errs()<<"\n initialize points to for :"<<F->getName();
        //Initiate the points to grapg for the child.
        if (F && !F->isDeclaration())
        {
            DSGraph *PointstoGraph  = BU_ds.getDSGraph(*F);
            funcPointsToMap[F] = PointstoGraph;


            //Writes the Points to graph in dot files..

//            std::string ErrorInfo;
//            // sys::fs::OpenFlags Flags;
//            std::string fileName = "DSGraphInfo.txt";
//            // nodeCount =0;
//            raw_fd_ostream File(fileName.c_str(), ErrorInfo,sys::fs::F_None);

//            if (!ErrorInfo.empty()) {
//                errs() << "Error opening file " << fileName
//                       << " for writing! Error Info: " << ErrorInfo << " \n";
//                return;
//            }

//            std::string funcNames = PointstoGraph->getFunctionNames();

//            PointstoGraph->writeGraphToFile(File,"../../TestFiles/"+funcNames);
        }

    }
}


bool moduleDepGraph::isPathProcessed(Function* F, vector<Function*> callPath, CallSite callSite)
{

    callStringFunctions.push_back(F);
    // currentCallString = currentCallString +","+F->getName();
    currentCallString.append(",");
    currentCallString.append(F->getName());
    set< vector<Function*> > callStringFunc = ProcessedCallStringFunctions[F];
    set<string> callStrings = ProcessedCallStrings[F];
    errs()<<"\n\n Current Path:"<<currentCallString;
    for(set<string>::iterator callst = callStrings.begin(); callst != callStrings.end(); ++callst)
    {

        string callstring = (*callst);
        errs()<<"\n call string for function :  "<<callstring;
        // if(strcmp(currentCallString.c_str(),callstring.c_str()) == 0)
        if(currentCallString.compare(callstring) == 0)
        {
            errs()<<"\n Path already processsed" << currentCallString;
            return true;
        }
    }
    callStrings.insert(currentCallString);
    ProcessedCallStrings[F] = callStrings;
    callStringFunc.insert(callStringFunctions);
    ProcessedCallStringFunctions[F] = callStringFunc;
    return false;
}


void moduleDepGraph::Process_Functions(Function* F, Graph* F_Graph, int SensDepth)
{
    //Check if call path already processed if yes, dont process and return, else process with the appropriate context.

   // if(debug)
        errs()<<"\n Processing funtion recursive.. "<< F->getName() <<"  ";
    MDA = &getAnalysis<MemoryDependenceAnalysis>((*F));

    //Reset F_graph's store node map.
    F_Graph->StoreNodeMap.clear();


    //Add nodes for function parameters..
    for (Function::arg_iterator Arg = F->arg_begin(), aEnd = F->arg_end(); Arg != aEnd; Arg++) {
       // Arg->dump();
        GraphNode * pNode = F_Graph->addParamNode(Arg ,F);
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
        // errs()<<"Processing the block now :"<<bbit->getName()<< "  ";

        while(!WorkList.empty())
        {
            BasicBlock *BBproc = WorkList.front();
            WorkList.pop();
            //Trial.. also try and remove the element from set as well.. will get only recursive elements..
            // setBlock.erase(BBproc); dont process loop blocks again..
            //TODO: reprocess the loop blocks only if context changes
            //Process the popped block
            affected_BB.clear();
            affected_BB = Process_Block(BBproc, F_Graph);

            //Iterate over the affected blocks and add them in worklist.
            for(set<BasicBlock*>::iterator afB = affected_BB.begin(); afB != affected_BB.end();++afB)
            {
                WorkList.push(*afB);
                //TODO: need some base condition to stop adding the affected blocks recursively., update: handle at proc block
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


    //Write the Graph in the dot file
//    std::string tmp = F->getName();
    // replace(tmp.begin(), tmp.end(), '\\', '_');
 //   std::string Filename = "../../demo1_" + tmp + ".dot";
    //string fileName = "../../"+F->getName()+"_graph.dot";
    //errs()<<"\n Processing Function ******************* "<<F->getName();
  //  F_Graph->toDot(F->getName(),Filename);

    //if the sensitivity depth is not zero, then process the functions called in this function.
    /*
    if(SensDepth!=0)
    {
    for (Function::iterator BB = F->begin(), endBB = F->end(); BB != endBB; ++BB) {

        for (BasicBlock::iterator I = BB ->begin(), endI = BB->end(); I != endI; ++I){

            if (CallInst* callInst = dyn_cast<CallInst>(&*I)) {

               // callInst->dump();
                if (callInst->getCalledFunction() != NULL )
                {
                   // errs()<<"\n processing child funct : "<<callInst->getCalledFunction()->getName();
                    Function * calledFunc = callInst->getCalledFunction();
                    if(!calledFunc->isDeclaration())
                    {
                        Process_Functions(calledFunc,F_Graph,SensDepth-1);
                    }
                }
            }
        }
    }
    }
    */


    //Print storeNode map for the function..!!
    if(code)
    {
        for(map<Value*,GraphNode*>::iterator stMap= F_Graph->StoreNodeMap.begin(); stMap != F_Graph->StoreNodeMap.end();++stMap)
        {
            errs()<<"\n========";
            Value *SI = (*stMap).first;
            SI->dump();
            GraphNode * gNode = (*stMap).second;
            if(gNode!=NULL)
            {
                if(isa<MemNode>(gNode))
                {
                    MemNode* mNode = dyn_cast<MemNode>(gNode);
                    errs()<<" ------ Mem node label: --"<<mNode->getLabel();
                }
            }
        }
        errs()<<"\n========";
    }


    ///Bakward processing pass to process all loads..

    if(fullAnalysis)
    {
        // BasicBlock* TermBlock = &F->back();
        map<Value*, set< Value*> > LoadQueue;
        //  LoadStoreMap(LoadQueue,TermBlock,F_Graph);
 //       errs()<<"\n CAlling BFS for function "<< F->getName();
        BFSLoadStoreMap(LoadQueue,F,F_Graph);
    }

}


///Function: BFSLoadStoreMap
/// Reverse trackign function used for attaching the loads to appropriate stores..
void moduleDepGraph::BFSLoadStoreMap(map<Value*, set< Value*> > LoadQueue, Function* F, Graph * F_Graph)
{
  //  BasicBlock* ProcBlock = &F->back();
    //Process block for loads and stores..
//    if(code)
//    {
//        //Print the load queue for the block ...
//        errs()<<"\n\n------ Reverse------- Processing Block :"<<ProcBlock->getName();
//        for(map<Value*, set< Value*> >::iterator Qit = LoadQueue.begin();Qit!= LoadQueue.end();++Qit)
//        {
//            Value* LoadPtr = (*Qit).first;
//            errs()<<"\nLoad ptr :";
//            LoadPtr->dump();
//            errs()<<"\n Load insts :";
//            set<Value *> loadI = (*Qit).second;
//            for(set< Value*>::iterator LI_IT = loadI.begin(); LI_IT != loadI.end(); ++LI_IT)
//            {
//                (*LI_IT)->dump();
//            }

//        }
//    }

    set<BasicBlock*> processedsuccBlocks;
    bool removeLoads = false;

 //   errs()<<"\n\n\n Processing Function : "<<F->getName();

    //Reverse process all blocks in function F until the begin. ..
    // merge peration..
    for (Function::iterator BB = --F->end(), endBB = F->begin(); BB != endBB; BB--)
    {

        BasicBlock * ProcBlock = (BB);

        //Check if all the succ blocks have been processed if so set the flag to remove mapped loads..

        //processedsuccBlocks
       // for()
        const TerminatorInst *TInst = ProcBlock->getTerminator();
        for (unsigned I = 0, NSucc = TInst->getNumSuccessors(); I < NSucc; ++I) {
          BasicBlock *Succ = TInst->getSuccessor(I);
           if(processedsuccBlocks.count(Succ)>0)
                removeLoads = true;
           else
               removeLoads = false;
        }
        processedsuccBlocks.insert(ProcBlock);

//        if(removeLoads)
//            errs()<<"\n---------------------- Processing block: "<<ProcBlock->getValueName();
//        else
     //       errs()<<"\n Processing block: "<<ProcBlock->getValueName();

        for (BasicBlock::reverse_iterator Iit = ProcBlock->rbegin(), Iend = ProcBlock->rend(); Iit != Iend; ++Iit)
        {

            Function* parentFunc = ProcBlock->getParent();
            //check if load and update queue..
            if(isa<LoadInst>(&*Iit))
            {
                LoadInst *LI = dyn_cast<LoadInst>(&*Iit);
                //Update the loadinst queue for load ptr..
                Value * LoadPtr = LI->getOperand(0);
                set< Value*> LoadInsts = LoadQueue[LoadPtr];
             //   errs()<<"\n  adding loads  ";
                LoadInsts.insert(LI);
                LoadQueue[LoadPtr] = LoadInsts;

                //TODO: Check if we need to add for the root ptr as well.. if not then done..

            }

            //check if store and maps to any load...
            if(isa<StoreInst>(&*Iit))
            {
                StoreInst *SI = dyn_cast<StoreInst>(&*Iit);
                Value * StorePtr = SI->getPointerOperand();
                Value * StoreRootPtr = SI->getPointerOperand();
                bool gepInst_S = false;
                if(isa<GetElementPtrInst>(StorePtr))
                {
                    //    errs()<<"\nStore is is a GEP..!!";
                    //get root
                    gepInst_S = true;
                    GetElementPtrInst* GI = dyn_cast<GetElementPtrInst>(StorePtr);
                    StoreRootPtr =  GI->getPointerOperand();
                }
                //Iterate through the Queue..
                set<Value*> loadKeystobeRemoved;

            //    errs()<<"\n LoadQueue Size : "<<LoadQueue.size();
                for(map<Value*, set< Value*> >::iterator loadQIt = LoadQueue.begin();loadQIt != LoadQueue.end(); ++loadQIt)
                {
                    bool gepInst_L = false;
                    Value* LoadPtr = (*loadQIt).first;
                    Value* LoadRootPtr = (*loadQIt).first;
                    if(isa<GetElementPtrInst>(LoadPtr))
                    {
                        // errs()<<"\n\n\n IT is a GEP..!!";
                        //get root
                        gepInst_L = true;
                        GetElementPtrInst* GI = dyn_cast<GetElementPtrInst>(LoadPtr);
                        LoadRootPtr =  GI->getPointerOperand();
                    }

                    if(checkPointer(LoadPtr,StorePtr,parentFunc))
                    {
                        GraphNode * stNode = F_Graph->StoreNodeMap[SI];

                        if(code)
                        {errs()<<"\n Store found for load : ";
                            LoadPtr->dump();
                            errs()<<" -------- Store inst  : ";
                            SI->dump();
                            errs()<<" --------Node   : ";
                            if(stNode != NULL)
                            { if(isa<MemNode>(stNode))
                                {
                                    MemNode* mNode = dyn_cast<MemNode>(stNode);
                                    errs()<<" ------ Mem node label: --"<<mNode->getLabel();
                                }
                                //stNode->getLabel();
                            }
                            else
                                errs()<<" NULL \n";

                        }


                        if(stNode != NULL)
                        {
                            //Iterate through all load inst for that pointer..
                            set< Value*> LoadInsts = (*loadQIt).second;
                            for(set< Value*>::iterator LI_IT = LoadInsts.begin(); LI_IT != LoadInsts.end(); ++LI_IT)
                            {
                                GraphNode * ldNode = F_Graph->findNode(*LI_IT);
                                if(ldNode != NULL )
                                {
                                    if(isa<MemNode>(stNode))
                                    {
                                        MemNode* mNode = dyn_cast<MemNode>(stNode);
                                        //stNode->connect(ldNode);
                                        mNode->connect(ldNode,etData);
                                    }
                                    // if(code) errs()<<"\n +++++ Connecting the pointer nodes...  "<<stNode->getLabel()<<" -- "<<ldNode->getLabel()<<" for ptrs ";
                                    //if(code) StorePtr->dump();
                                    //if(code) LoadPtr->dump();
                                }
                            }
                        }

                        if(removeLoads)
                        {
                       //     errs()<<"\nREmove loads true and match for load found..";
                            loadKeystobeRemoved.insert((*loadQIt).first);
                        }

                    }
                }
                for(set<Value*>::iterator ldIt = loadKeystobeRemoved.begin();ldIt != loadKeystobeRemoved.end();++ldIt)
                {
           //         errs()<<"\n Erasing load ...... :"<<LoadQueue.size();
               //     (*ldIt)->dump();
                    LoadQueue.erase(*ldIt);
            //        errs()<<" after removal.."<<LoadQueue.size();
                }

            }
        }

    }
    //Recursive call on all preds for LoadQueue.
    //    for (pred_iterator PI = pred_begin(ProcBlock), E = pred_end(ProcBlock); PI != E; ++PI) {
    //        BasicBlock *Pred = *PI;
    //        int processedSize = BT_ProcessedBlocks.size();
    //        BT_ProcessedBlocks.insert(Pred);
    //        //Don't process preds if current block processed..
    //        if(processedSize!=BT_ProcessedBlocks.size())
    //        {
    //            //errs()<<"\n Prcessed Block Size --- "<<ProcessedBlocks.size();
    //            LoadStoreMap(LoadQueue,Pred,F_Graph);
    //        }
    //    }


}



///Function: LoadStoreMap
/// Reverse trackign function used for attaching the loads to appropriate stores..
void moduleDepGraph::LoadStoreMap(map<Value*, set< Value*> > LoadQueue,BasicBlock* ProcBlock, Graph * F_Graph)
{
    //Process block for loads and stores..
    if(code)
    {
        //Print the load queue for the block ...
        errs()<<"\n\n------ Reverse------- Processing Block :"<<ProcBlock->getName();
        for(map<Value*, set< Value*> >::iterator Qit = LoadQueue.begin();Qit!= LoadQueue.end();++Qit)
        {
            Value* LoadPtr = (*Qit).first;
            errs()<<"\nLoad ptr :";
            LoadPtr->dump();
            errs()<<"\n Load insts :";
            set<Value *> loadI = (*Qit).second;
            for(set< Value*>::iterator LI_IT = loadI.begin(); LI_IT != loadI.end(); ++LI_IT)
            {
                (*LI_IT)->dump();
            }

        }
    }

    for (BasicBlock::reverse_iterator Iit = ProcBlock->rbegin(), Iend = ProcBlock->rend(); Iit != Iend; ++Iit)
    {

        Function* parentFunc = ProcBlock->getParent();
        //check if load and update queue..
        if(isa<LoadInst>(&*Iit))
        {
            LoadInst *LI = dyn_cast<LoadInst>(&*Iit);
            //Update the loadinst queue for load ptr..
            Value * LoadPtr = LI->getOperand(0);
            set< Value*> LoadInsts = LoadQueue[LoadPtr];
            LoadInsts.insert(LI);
            LoadQueue[LoadPtr] = LoadInsts;

            //TODO: Check if we need to add for the root ptr as well.. if not then done..

        }

        //check if store and maps to any load...
        if(isa<StoreInst>(&*Iit))
        {
            StoreInst *SI = dyn_cast<StoreInst>(&*Iit);
            Value * StorePtr = SI->getPointerOperand();
            Value * StoreRootPtr = SI->getPointerOperand();
            bool gepInst_S = false;
            if(isa<GetElementPtrInst>(StorePtr))
            {
                //    errs()<<"\nStore is is a GEP..!!";
                //get root
                gepInst_S = true;
                GetElementPtrInst* GI = dyn_cast<GetElementPtrInst>(StorePtr);
                StoreRootPtr =  GI->getPointerOperand();
            }
            //Iterate through the Queue..

            for(map<Value*, set< Value*> >::iterator loadQIt = LoadQueue.begin();loadQIt != LoadQueue.end(); ++loadQIt)
            {
                bool gepInst_L = false;
                Value* LoadPtr = (*loadQIt).first;
                Value* LoadRootPtr = (*loadQIt).first;
                if(isa<GetElementPtrInst>(LoadPtr))
                {
                    // errs()<<"\n\n\n IT is a GEP..!!";
                    //get root
                    gepInst_L = true;
                    GetElementPtrInst* GI = dyn_cast<GetElementPtrInst>(LoadPtr);
                    LoadRootPtr =  GI->getPointerOperand();
                }

                if(checkPointer(LoadPtr,StorePtr,parentFunc))
                {
                    GraphNode * stNode = F_Graph->StoreNodeMap[SI];

                    if(code)
                    {errs()<<"\n Store found for load : ";
                    LoadPtr->dump();
                    errs()<<" -------- Store inst  : ";
                    SI->dump();
                     errs()<<" --------Node   : ";
                     if(stNode != NULL)
                     { if(isa<MemNode>(stNode))
                         {
                             MemNode* mNode = dyn_cast<MemNode>(stNode);
                             errs()<<" ------ Mem node label: --"<<mNode->getLabel();
                         }
                      //stNode->getLabel();
                     }
                     else
                          errs()<<" NULL \n";

                    }


                    if(stNode != NULL)
                    {
                        //Iterate through all load inst for that pointer..
                        set< Value*> LoadInsts = (*loadQIt).second;
                        for(set< Value*>::iterator LI_IT = LoadInsts.begin(); LI_IT != LoadInsts.end(); ++LI_IT)
                        {
                            GraphNode * ldNode = F_Graph->findNode(*LI_IT);
                            if(ldNode != NULL )
                            {
                                if(isa<MemNode>(stNode))
                                                         {
                                                             MemNode* mNode = dyn_cast<MemNode>(stNode);
                                //stNode->connect(ldNode);
                                                             mNode->connect(ldNode,etData);
                                }
                               // if(code) errs()<<"\n +++++ Connecting the pointer nodes...  "<<stNode->getLabel()<<" -- "<<ldNode->getLabel()<<" for ptrs ";
                                //if(code) StorePtr->dump();
                                //if(code) LoadPtr->dump();
                            }
                        }
                    }

                }
            }

        }
    }

    //Recursive call on all preds for LoadQueue.
    for (pred_iterator PI = pred_begin(ProcBlock), E = pred_end(ProcBlock); PI != E; ++PI) {
        BasicBlock *Pred = *PI;
        int processedSize = BT_ProcessedBlocks.size();
        BT_ProcessedBlocks.insert(Pred);
        //Don't process preds if current block processed..
        if(processedSize!=BT_ProcessedBlocks.size())
        {
            //errs()<<"\n Prcessed Block Size --- "<<ProcessedBlocks.size();
            LoadStoreMap(LoadQueue,Pred,F_Graph);
        }
    }


}




set<BasicBlock*>  moduleDepGraph::Process_Block(BasicBlock* BB, Graph* F_Graph)
{

    set<BasicBlock*> affected_BB;

    Function * parentFunction = BB->getParent();

   // if(code) errs()<<"\n Processing Block :"<<BB->getName()<<"\n";
    int counter =0;
    //Iterate over each instruction in the block and process it.
    for (BasicBlock::iterator Iit = BB->begin(), Iend = BB->end(); Iit
         != Iend; ++Iit) {

      //  if(code) Iit->dump();
        if(isa<GetElementPtrInst>(&*Iit))
        {
            GetElementPtrInst * GI = dyn_cast<GetElementPtrInst>(&*Iit);
            CheckifRelevantField(GI,F_Graph);
        }

        if(isa<CallInst>(&*Iit))  //  handle the call inst.
        {
            CallInst *CI = dyn_cast<CallInst>(&*Iit);
            CallSite cs(&*Iit);

            Function * calledFunc = CI->getCalledFunction();
            if(calledFunc && !calledFunc->isDeclaration())
            {
                //Map params or pre-empt funciton..
            } //if called func

            if(calledFunc && calledFunc->isDeclaration())
            {
                //    errs()<<"\nPROPAGATE: Call Site with called func and IS delaration.. !!";
                //  CI->dump();
                HandleLibraryFunctions(CI,parentFunction);
            }

        }

        //Process each statement in Graph add node..
        F_Graph->addInst(Iit,counter,parentFunction);
        counter++;
    }

    return affected_BB;
}



void moduleDepGraph::CheckifRelevantField(GetElementPtrInst * GI, Graph* F_Graph)
{

    //Check if it is the field of interest.
    Function * parentFunction = GI->getParent()->getParent();
    Value* baseptr = GI->getOperand(0);
    Type* ty = baseptr->getType();

    if(code) errs()<<"\n\n   GEP :"<<relevantFields.size();
    if(code) GI->dump();
    if(code) baseptr->dump();
    if(code) ty->dump();

    if(code)
    {errs()<<"\n is pointer :"<<ty->isPointerTy()<<"\n";
    if(ty->isPointerTy())
    {
        ty->getPointerElementType()->dump();
        errs()<<"\n is structure now.. :"<<ty->getPointerElementType()->isStructTy()<<"\n";

    }
    //get what struct it points to..
    if(Instruction *In = dyn_cast<Instruction>(baseptr))
    {
        int num = In->getNumOperands();
        for(int i =0; i<num; i++)
        {
            Value* operand = In->getOperand(i);
            operand->getType()->dump();
            errs()<<"\n is struct : "<<operand->getType()->isStructTy();
            if(operand->hasName())
            {
                errs()<<"\n has name:  "<<operand->getName();
            }
        }
    }
}

    //check if the bae pointer is an struct type.. or pointer to struct.... else just get all operands and and check the same...
    for(set<RelevantFields*>::iterator fieldIt = relevantFields.begin(); fieldIt != relevantFields.end(); ++fieldIt)
    {
        if(code) errs()<<"\n Function name read from file.. : "<<(*fieldIt)->functionName<<" parent funct name.. :"<<parentFunction->getName();

        if((*fieldIt)->functionName==parentFunction->getName())
        {
            if(code) errs()<<"\nIn function ... looking for.... : "<<(*fieldIt)->functionName<<" "<<(*fieldIt)->variable;

            if(baseptr->hasName())
            {
                if(code)  errs()<<"\n The base ptr.. has name...!!";
                if(baseptr->getName()== (*fieldIt)->variable)
                {
                    if(code) errs()<<"\nTesting for var : "<<(*fieldIt)->variable;

                    int fieldIndex;
                    for (int i = GI->getNumOperands() - 2; i > 0; i--) {
                        Type *ty = GI->getType();
                        //  assert(ty->isArrayTy() || ty->isStructTy());
                        if (ty->isStructTy()) { // structure field

                            fieldIndex = cast<ConstantInt>(GI->getOperand(i + 1))->getZExtValue();

                        }
                    }
                    //match index.
                    if(fieldIndex == (*fieldIt)->index)
                    {
                        FieldVals.insert(GI);
                        //update fields in graph.
                        F_Graph->FieldsVal = FieldVals;
                        if(code) errs()<<"\n\n   Found field";
                        if(code) GI->dump();
                    }
                }
            }
            else
                if(code)  errs()<<"\n The base ptr.. has NONAME...!!";

        }
    }
}


void moduleDepGraph::CollectLiveStores_Params(BasicBlock* predBlock, Instruction * Cinst,Value* pointer, Graph* F_Graph)
{
    BasicBlock * parentBlock = Cinst->getParent();
    Function * parentFunc = parentBlock->getParent();
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
                if(checkPointer(LoadPtr,StorePtr,parentFunc) || checkPointer(RootPtr,StorePtr,parentFunc))
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

                    if(checkPointer(LoadPtr,StoreRootPtr,parentFunc) || checkPointer(RootPtr,StoreRootPtr,parentFunc))
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
    Function * parentFunc =  parentBlock->getParent();
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
                if(checkPointer(LoadPtr,StorePtr,parentFunc) || (checkPointer(RootPtr,StorePtr,parentFunc)))
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

                    // if((LoadPtr==StoreRootPtr) || (RootPtr==StoreRootPtr))
                    if(checkPointer(LoadPtr,StoreRootPtr,parentFunc) || checkPointer(RootPtr,StoreRootPtr,parentFunc))
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




//In this funciton: it is checked if any of the targetptr is an alias of ptr when using aliasSEts.
//For points to info... check if ptr and targetptr points to the same memory location using the DSA points to info.
bool moduleDepGraph::checkPointer(Value * Ptr, Value* targetPtr, Function * parentFunc)
{
    Value * pointer =  Ptr;
    bool foundMatch = false;
    DSGraph * pointstoGraph = funcPointsToMap[parentFunc];
    //currently using TopDownGraph ... later change it to the passed F_GRaph for consistency.
    GraphNode* Var = TopDownGraph->findNode(Ptr);
    // errs()<<"\n checking alias for :";
    //Ptr->dump();
    if(Var && USE_ALIAS)
    {
        if(isa<MemNode>(Var))
        {
            MemNode * memNew = dyn_cast<MemNode>(Var);
            // Value * val = memNew->defLocation.second;
            std::set<Value*> aliases = memNew->getAliases();
            for(set<Value*>::iterator alVal = aliases.begin(); alVal != aliases.end();++alVal)
            {
                //(*alVal)->dump();
                if((*alVal)==targetPtr)
                {
                    pointer = (*alVal);
                    foundMatch = true;
                    //   errs()<<"\n\n***************************Alias Match found!!";
                    //  Ptr->dump();
                    // (*alVal)->dump();
                }
            }
        }
    }
    //----------DSA points to checks-------
    if(Var && USE_DSA1)
    {
        //  DSNodeHandle
        DSNodeHandle ptrNode =  pointstoGraph->getNodeForValue(Ptr);
        DSNodeHandle targetPtrNode =  pointstoGraph->getNodeForValue(targetPtr);
        if(ptrNode == targetPtrNode)
        {
            // errs()<<"\n\n ---DSA pointer match found for :";
            //Ptr->dump();
            //targetPtr->dump();
            foundMatch = true;
        }

    }

    //-------

    if(Ptr==targetPtr)
        foundMatch = true;

    return foundMatch;

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


void moduleDepGraph::ForwardProcess(Function* F, Graph * F_Graph)
{
    for (Function::iterator BB = F->begin(), endBB = F->end(); BB != endBB; ++BB) {

        for (BasicBlock::iterator I = BB ->begin(), endI = BB->end(); I != endI; ++I){

            if (CallInst* callInst = dyn_cast<CallInst>(&*I)) {

               // callInst->dump();
                if (callInst->getCalledFunction() != NULL )
                {
                   // errs()<<"\n processing child funct : "<<callInst->getCalledFunction()->getName();
                    Function * calledFunc = callInst->getCalledFunction();
                    if(!calledFunc->isDeclaration())
                    {
                        MatchContextParams(F,calledFunc,callInst,F_Graph);
                    }
                }
            }
        }
    }
    //MatchContextParams
}


void moduleDepGraph::MatchContextParams(Function* Caller, Function *Callee, CallInst *CI,Graph* F_Graph)
{
    // Only do the matching if F has any use
    if (Callee->isVarArg() || !Callee->hasNUsesOrMore(1)) {
        return;
    }
    if(debug) errs()<<"\n Function arg matching for;; "<< Callee->getName();

    // Data structure which contains the matches between formal and real parameters
    // First: formal parameter
    // Second: real parameter
    SmallVector<std::pair<GraphNode*, GraphNode*>, 4> Parameters(Callee->arg_size());

    // Fetch the function arguments (formal parameters) into the data structure
    Function::arg_iterator argptr;
    Function::arg_iterator e;
    unsigned i;

    //Create the PHI nodes for the formal parameters
    for (i = 0, argptr = Callee->arg_begin(), e = Callee->arg_end(); argptr != e; ++i, ++argptr) {

        OpNode* argPHI = new OpNode(Instruction::PHI);
        GraphNode* argNode = NULL;
        argNode = F_Graph->addInst(argptr,-1,Callee);

        if (argNode != NULL)
        {
            F_Graph->addEdge(argPHI, argNode);
            if (debug) errs()<<"\nAdding Edge between "<< argPHI->getLabel()<<" -- "<<argNode->getLabel();
        }

        Parameters[i].first = argPHI;
    }

    // Check if the function returns a supported value type. If not, no return value matching is done
    bool noReturn = Callee->getReturnType()->isVoidTy();

    // Creates the data structure which receives the return values of the function, if there is any
    SmallPtrSet<llvm::Value*, 8> ReturnValues;

    if (!noReturn) {
        // Iterate over the basic blocks to fetch all possible return values
        for (Function::iterator bb = Callee->begin(), bbend = Callee->end(); bb != bbend; ++bb) {
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
    for (Value::user_iterator UI = Callee->user_begin(), E = Callee->user_end(); UI != E; ++UI) {
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
            Parameters[i].second = F_Graph->addInst(*AI,-1,Callee);
        }

        // Match formal and real parameters
        for (i = 0; i < Parameters.size(); ++i) {

            F_Graph->addEdge(Parameters[i].second, Parameters[i].first);
            if(debug) errs()<<"\nAdding Edge between "<< Parameters[i].second->getLabel()<<" -- "<<Parameters[i].first->getLabel();
        }

        // Match return values
        if (!noReturn) {

            OpNode* retPHI = new OpNode(Instruction::PHI);
            GraphNode* callerNode = F_Graph->addInst(caller,-1,Callee);
            F_Graph->addEdge(retPHI, callerNode);

            for (SmallPtrSetIterator<llvm::Value*> ri = ReturnValues.begin(),
                 re = ReturnValues.end(); ri != re; ++ri) {
                GraphNode* retNode = F_Graph->addInst(*ri,-1,Callee);
                F_Graph->addEdge(retNode, retPHI);
            }

        }

        // Real parameters are cleaned before moving to the next use (for safety's sake)
        for (i = 0; i < Parameters.size(); ++i)
            Parameters[i].second = NULL;
    }

    F_Graph->deleteCallNodes(Callee);


}


//For the call strings method.. check if the function is in the callstrings.
bool moduleDepGraph::isFunctiononCallString(Function * F)
{
    //Code that checks for function in any of the call strings.
    for (std::vector<Path>::iterator I = callPaths.begin();
         I != callPaths.end();
         I++)
    {
        Path currPath = (*I);
        //   std::reverse(currPath.begin(), currPath.end());
        for(std::vector<instructionCallSite>::const_iterator instCallSite = currPath.begin(); instCallSite != currPath.end(); instCallSite++)
        {

            instructionCallSite callSiteInfo = (*instCallSite);
            Function * Func = callSiteInfo.function;
            if(Func == F)
                return true;
        }
    }
    return false;
}




///Function: isFunctiononCallPath
/// Description: To check if the function to process is on the provided call path.
bool moduleDepGraph::isFunctiononCallPath(Function * F)
{

    //Code that checks for functions in the call path list read from the file.
    string funcName = F->getName();
    if(std::find(callPathFunctions.begin(), callPathFunctions.end(), funcName)!=callPathFunctions.end())
        return true;
    else
        return false;

}



///Function: getcallPathFunctions
/// Description: Read the call paths file and store the functions on the call path to process.
///
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




void moduleDepGraph::ReadRelevantFields(){

    std::ifstream srcFile (Fields.c_str(), std::ifstream::in);
    std::string line;
    if(!srcFile)
    {
        errs() << " Could not open the Field information file..? \n";
    }
    else
    {
        while(srcFile >> line)
        {
         //   if(code) errs() << " Reading fields--- ";
            line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
            RelevantFields * rs = new RelevantFields();
            rs->functionName = line;
            if(srcFile >> line)
            { line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
                rs->variable = line;
                if(srcFile >> line)
                { line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
                    rs->index = std::atoi(line.c_str());
                    rs->fieldName = line;
                    relevantFields.insert(rs);
                }
                // taintSources.insert(ts);
            }
            //.insert(line);
        }
    }
}





////----------------------------------------------------------------------------------------------
/// Funtion pointer Indirect targets computation!!
///Start:


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

////----------------------------------------------------------------------------------------------
/// Funtion pointer Indirect targets computation!!
///End/
