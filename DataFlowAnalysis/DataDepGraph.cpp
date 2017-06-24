#include "DataDepGraph.h"
//#include "dsa/DataStructure.h"
//#include "dsa/DSGraph.h"

#include "llvm/ADT/PostOrderIterator.h"
#define debug false
#define code false
#define fdb false

bool useCallStrings =true;
bool AddControlEdges = false;

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
    //    if (USE_ALIAS_SETS)
    //        AU.addRequired<AliasSets> ();

    AU.setPreservesAll();
}

bool functionDepGraph::runOnFunction(Function &F) {

    // AliasSets* AS = NULL;

    //    if (USE_ALIAS_SETS)
    //        AS = &(getAnalysis<AliasSets> ());

    //Making dependency graph
    depGraph = new llvm::Graph();
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
    //    if (USE_ALIAS_SETS)
    //        AU.addRequired<AliasSets> ();

    AU.addRequired<CallGraphWrapperPass> ();
    AU.addRequired<CallGraphWrapper> ();
    //AU.addRequiredTransitive<AliasAnalysis>();
    AU.addRequired<MemoryDependenceAnalysis>();
    AU.addRequired<PostDominatorTree>();
    //AU.addRequired<MemDepPrinter>();
    AU.addRequired<InputDep> ();
    //if(USE_DSA1)
    AU.addRequired<BUDataStructures> ();
    AU.setPreservesAll();
}

bool moduleDepGraph::runOnModule(Module &M) {

    //Get results from all the previous analysis:
    CallGraph &CG  = getAnalysis<CallGraphWrapperPass>().getCallGraph();
    InputDep &IV = getAnalysis<InputDep> ();
    inputDepValues = IV.getInputDepValues();
    CallGraphWrapper &cgWrapper = getAnalysis<CallGraphWrapper> ();

    callPaths = cgWrapper.getCallPaths();
    IndMap = cgWrapper.getIndirectMap();

    //Initalize all the config params.
    configData = IV.getConfigInfo();
    useCallStrings = configData.useCallString;

    //Read the relevant tainted fields supplied to the code to make it field sensitive.
    //TODO :NT: once the struct is found in parsing--update the struct and field Types
    ReadRelevantFields();

    //if(USE_DSA1) //need to do this irrespective of the flag, null initialize not allowed.
    BUDataStructures &BU_ds = getAnalysis<BUDataStructures>();

    //Making dependency graph
    depGraph = new Graph();
    TopDownGraph = new Graph();
    FullGraph = new Graph();

    //depGraph->toDot("base","../../AliasSetGraph.dot");

    //Get the relevant functions on the call path.. approach before the call strings approach.
    //Check if the option provided..
    //getcallPathFunctions();

    //Initialize DSA pointsto graph for each function.
    InitializePointtoGraphs(M, BU_ds);

    //Process Global variables... it handles the globals when they are used, no need to add seperately unless source.
    // ProcessGlobals(M,TopDownGraph);


    //Generate the flow graph for all the funcitons:
     for (Module::iterator Fit = M.begin(), Fend = M.end(); Fit != Fend; ++Fit) {
         Function *F = Fit;
         if (F && !F->isDeclaration()) {
                // Graph* Fdep = new Graph();
                 //     if(code) errs()<<"\n\n Processing function fdep graph.. for "<<F->getName();
                 //      Process_Functions(F,Fdep,SensitivityDepth);
                 if(code) errs()<<"\n Processing function topdown graph.. for "<<F->getName()<<"\n";
                 Process_Functions(F,TopDownGraph,SensitivityDepth);
                 if(fdb)
                 {
                 int NumNodes = TopDownGraph->getNodes().size();
                 errs()<<"Graph nodes updated : " <<NumNodes <<"\n\n";
                 }

                // FuncDepGraphs[F] = Fdep;
                 std::string tmp = F->getName();
                 // replace(tmp.begin(), tmp.end(), '\\', '_');
                 std::string Filename = "../../demo1_" + tmp + ".dot";
                 //string fileName = "../../"+F->getName()+"_graph.dot";
                 //    errs()<<"\n Processing Function ******************* "<<F->getName();
                 //           Fdep->toDot(F->getName(),Filename);
         }

     }

     //Process each call site in sequence..
   //  Graph * moduleGraph = new Graph();

     errs()<<"\n Process the call inst in each function..";
     for (Module::iterator Fit = M.begin(), Fend = M.end(); Fit != Fend; ++Fit) {
         //Check if function in callstrings, parse to connect callsites to targets that are in callstrings.
         if(configData.useCallString && !(callPaths.size()==0) && isFunctiononCallString(Fit))
         {
            // errs()<<"\ncalstring funct.. "<<Fit->getName();
             ProcessCSinFunc(Fit,true,TopDownGraph);
         }
         else
         {
              //errs()<<"\n Call process.. "<<Fit->getName();
             ProcessCSinFunc(Fit,false,TopDownGraph);
         }

     }

    //Assiging to depgraph for taint --- temporarily
    depGraph = TopDownGraph;


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

bool moduleDepGraph::isCallProcessed(CallInst* CI, Function* calledFunc)
{
    bool isProcessed = false;
    for(vector<instructionCallSite*>::iterator csIt = ProcessedCallsites.begin(); csIt != ProcessedCallsites.end();++csIt)
    {
        if((*csIt)->callInst == CI && (*csIt)->function == calledFunc)
        {
            isProcessed = true;
            continue;
        }
    }

   return isProcessed;
}

void moduleDepGraph::ConnectCallerCallee(CallInst* CI, Function* Callee, Graph * modGraph)
{
    Function * caller = CI->getParent()->getParent();
    if (Callee->isVarArg() || !Callee->hasNUsesOrMore(1)) {
        return;
    }
    FuncParamInfo* paramInfo = modGraph->functionParamMap[Callee];
    if(paramInfo)
    {
        CallSite cs(CI);
        GraphNode* callSiteNode = modGraph->findInstNode(CI);
        if(!callSiteNode)
            callSiteNode = modGraph->addNode(CI,ntInst);

        //Add control edge to function entry node:
        if(GraphNode * entryNode = paramInfo->entryNode){
            if(AddControlEdges)
                callSiteNode->connect(entryNode,etCall);
        }

        //iterate over the args and find the nodes in caller func...
        //connect act in to formal in... formal out to act in from callsite..
        //return nodes to the call stmt.

        // Data structure which contains the matches between formal and real parameters
        // First: formal parameter
        // Second: real parameter
        SmallVector<std::pair<GraphNode*, GraphNode*>, 4> FormalIns(Callee->arg_size());
        SmallVector<std::pair<GraphNode*, GraphNode*>, 4> FormalOuts(Callee->arg_size());

        // Fetch the function arguments (formal parameters) into the data structure
        Function::arg_iterator argptr;
        Function::arg_iterator e;
        unsigned i;

        //Create the PHI nodes for the formal parameters
        for (i = 0, argptr = Callee->arg_begin(), e = Callee->arg_end(); argptr != e; ++i, ++argptr) {

           // OpNode* argPHI = new OpNode(Instruction::PHI);
            Value *argValue = argptr;
            //GraphNode* formArg = NULL;
            GraphNode* formArg = paramInfo->FormalInMap[argValue];
            //
            if(!formArg)
                formArg = modGraph->findInstNode(argValue); //,-1,Callee);
            if(!formArg)
                if(Instruction *inst = dyn_cast<Instruction>(argValue))
                    formArg = modGraph->addNode(inst,ntFormalIn);
            //
            FormalIns[i].second = formArg;
           // errs()<<"\n Formal args  : "<<formArg->getLabel();
            //Collect corresponding formal out params..
            GraphNode* formArgout = paramInfo->FormalOutMap[argValue];
            //
            if(!formArgout)
                formArgout = modGraph->findInstNode(argValue); //,-1,Callee);
            if(!formArgout)
                if(Instruction *inst = dyn_cast<Instruction>(argValue))
                    formArgout = modGraph->addNode(inst,ntFormalIn);
            FormalOuts[i].first = formArgout;
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

        //Get all correspoding graph nodes for rturns and connect it to the callsite node..
        for(SmallPtrSet<Value*,8>::iterator retVals = ReturnValues.begin(); retVals != ReturnValues.end();++retVals)
        {
            Value * retval = *retVals;
            if(GraphNode* retNode = paramInfo->RetValMap[retval]){
                //errs()<<"\n retnode  : "<<retNode->getLabel()<<" to -- call site "<<callSiteNode->getLabel();
                retNode->connect(callSiteNode,etData);
            }
            else{
            if(Instruction *inst = dyn_cast<Instruction>(retval)){
                if(GraphNode * retNode = modGraph->findInstNode(inst)){
                    //errs()<<"\n inst node retnode  : "<<retNode->getLabel()<<" to -- call site "<<callSiteNode->getLabel();
                    retNode->connect(callSiteNode,etData);
                }
            }
            }
        }

        //Get actual param nodes.
        CallSite::arg_iterator AI;
        CallSite::arg_iterator EI;

        for (i = 0, AI = cs.arg_begin(), EI = cs.arg_end(); AI != EI; ++i, ++AI) {
            if(Instruction * argin = dyn_cast<Instruction>(*AI)){
                if(GraphNode* argNode = modGraph->findInstNode(argin)){
                    FormalIns[i].first = argNode;
                    FormalOuts[i].second = argNode;
                }
            }
           // Parameters[i].second = F_Graph->addInst(*AI,-1,Callee);
        }

        for (i = 0; i < FormalIns.size(); ++i) {
            if(FormalIns[i].first && FormalIns[i].second){
                FormalIns[i].first->connect(FormalIns[i].second,etParam);
            }
            // modGraph->addEdge(Parameters[i].second, Parameters[i].first);
            //   if(debug) errs()<<"\nAdding Edge between "<< Parameters[i].second->getLabel()<<" -- "<<Parameters[i].first->getLabel();
        }

        //Connecting the formal outs to actual vars from the call site..to capture the sideeffects.
        for (i = 0; i < FormalOuts.size(); ++i) {
            if(FormalOuts[i].first && FormalOuts[i].second){
                FormalOuts[i].first->connect(FormalOuts[i].second,etParam);
            }
            // modGraph->addEdge(Parameters[i].second, Parameters[i].first);
            //   if(debug) errs()<<"\nAdding Edge between "<< Parameters[i].second->getLabel()<<" -- "<<Parameters[i].first->getLabel();
        }


//        //  for (Value::use_iterator UI = F.use_begin(), E = F.use_end(); UI != E; ++UI) {
//        for (Value::user_iterator UI = Callee->user_begin(), E = Callee->user_end(); UI != E; ++UI) {
//            User *U = *UI;
//            //                Use *BU = &UI.getUse();
//            // Value::const_user_iterator & CUI =UI::reference;
//            //  const Use CU = UI.getUse();
//            // Use *U = &*UI;
//            // Ignore blockaddress uses

//            //NT: TODO: Solve the error in the following:
//            if (isa<BlockAddress> (U))
//                continue;

//            // Used by a non-instruction, or not the callee of a function, do not
//            // match.
//            if (!isa<CallInst> (U) && !isa<InvokeInst> (U))
//                continue;

//            Instruction *caller = cast<Instruction> (U);

//            CallSite CS(caller);
//            if (!CS.isCallee(&UI.getUse()))
//                continue;

//            // Iterate over the real parameters and put them in the data structure
//            CallSite::arg_iterator AI;
//            CallSite::arg_iterator EI;

//            for (i = 0, AI = CS.arg_begin(), EI = CS.arg_end(); AI != EI; ++i, ++AI) {
//                Parameters[i].second = F_Graph->addInst(*AI,-1,Callee);
//            }

//            // Match formal and real parameters
//            for (i = 0; i < Parameters.size(); ++i) {

//                F_Graph->addEdge(Parameters[i].second, Parameters[i].first);
//                if(debug) errs()<<"\nAdding Edge between "<< Parameters[i].second->getLabel()<<" -- "<<Parameters[i].first->getLabel();
//            }

//            // Match return values
//            if (!noReturn) {

//                OpNode* retPHI = new OpNode(Instruction::PHI);
//                GraphNode* callerNode = F_Graph->addInst(caller,-1,Callee);
//                F_Graph->addEdge(retPHI, callerNode);

//                for (SmallPtrSetIterator<llvm::Value*> ri = ReturnValues.begin(),
//                     re = ReturnValues.end(); ri != re; ++ri) {
//                    GraphNode* retNode = F_Graph->addInst(*ri,-1,Callee);
//                    F_Graph->addEdge(retNode, retPHI);
//                }

//            }

//            // Real parameters are cleaned before moving to the next use (for safety's sake)
//            for (i = 0; i < Parameters.size(); ++i)
//                Parameters[i].second = NULL;
//        }

//        F_Graph->deleteCallNodes(Callee);



    }

}

void moduleDepGraph::ProcessCSinFunc(Function * F, bool callString, Graph * modGraph)
{
    for (Function::iterator BBit = F->begin(), BBend = F->end(); BBit
         != BBend; ++BBit) {
        for (BasicBlock::iterator Iit = BBit->begin(), Iend = BBit->end(); Iit
             != Iend; ++Iit) {

            if(isa<CallInst>(Iit))  //  handle the call inst.
            {
                CallInst *CI = dyn_cast<CallInst>(Iit);
                CallSite cs(Iit);


                Function * calledFunc = CI->getCalledFunction();
                //errs()<<"\n Check called func and callstring...";
                if(calledFunc && (!callString || (callString && isFunctiononCallString(F))))
                {
                    //errs()<<"\n check if processed...CalledFunc : "<<calledFunc->getName();
                    //Process only if not processed....
                    if(isCallProcessed(CI,calledFunc))
                        continue;
                    //Check if library/external function

                    //errs()<<"\n Called func cs, not processed....";
                    if(calledFunc->isDeclaration())
                    {
                        if(calledFunc->getName().equals("llvm.dbg.declare"))
                            continue;
                        //ToDO: Process the library functions..
                         HandleLibraryFunctions(CI,F);
                    }
                    else
                    {
                        //Handle the call connect here...
                        ConnectCallerCallee(CI,calledFunc,modGraph);
                    }

                 //errs()<<"\n push back processed cs..)";
                instructionCallSite* processedcall = new instructionCallSite();
                processedcall->callInst = CI;
                processedcall->function = calledFunc;
                ProcessedCallsites.push_back(processedcall);
                //errs()<<"\n done push back processed cs..";
                } //end of if function to be procesed..

                //ToDo: Check for indirect calls.. calledFunc will be empty..
                if(!calledFunc)
                {
                    Type *ty = cs.getCalledValue()->getType();
                    if(ty->isPointerTy())
                    {
                        if(ty->getPointerElementType()->isFunctionTy())
                        {
                           vector<Function*> targetFunctions = IndMap[cs];
                           for(vector<Function*>::iterator tfunc = targetFunctions.begin();tfunc != targetFunctions.end();++tfunc)
                           {
                               Function * targetF = (*tfunc);

                               //Check if the called function is defined and is in call string
                               if(targetF && (!callString || (callString && isFunctiononCallString(targetF))))
                               {
                                   //check if callsite with the given target processed.
                                   if(isCallProcessed(CI,targetF))
                                       continue;

                                   //Handle the call connect here...
                                   ConnectCallerCallee(CI,targetF,modGraph);

                               }
                               else if(!targetF)
                               {
                                   //ToDo: handle indirect call to external function....
                               }

                               instructionCallSite* processedcall = new instructionCallSite();
                               processedcall->callInst = CI;
                               processedcall->function = targetF;
                               ProcessedCallsites.push_back(processedcall);
                           }
                        }
                    }
                }


            } //end if of isa<callisnt>

        } //end for bb iterator
    }
}



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

//Add the global values that are defined in the graph if the uses if not empty
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
        if(!GV->use_empty())
        {
            //   errs()<<"\n Adding global value: ";
            //  GV->dump();
            TopDownGraph->addGlobalVal(GV);
        }

    }
}


void moduleDepGraph::TopDownProcessing(Function * F)
{
    //Processing the Function, generate the data flow graph.
    Process_Functions(F,TopDownGraph,SensitivityDepth);

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

                      //  Graph * CallF_Graph = FuncDepGraphs[calledFunc];
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
    //Default case : Adding edge from the callsite params to return variable. done by default
    //handle special library functions here..


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
/// Also initialize global points to graph.
void moduleDepGraph::InitializePointtoGraphs(Module &M,BUDataStructures &BU_ds)
{
    //initlialize global points to graph.
    globalPointsToGraph = BU_ds.getGlobalsGraph();

    //initialize functions points to graphs.
    for (Module::iterator F = M.begin(), endF = M.end(); F != endF; ++F) {
        if (F && !F->isDeclaration())
        {
            DSGraph *PointstoGraph  = BU_ds.getDSGraph(*F);
            funcPointsToMap[F] = PointstoGraph;

            //Writes the Points to graph in dot files..

            //            std::string ErrorInfo;
            //            std::string fileName = "DSGraphInfo.txt";
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
    errs()<<"\n Processing function: "<< F->getName() <<"  ";
    MDA = &getAnalysis<MemoryDependenceAnalysis>((*F));

    //Make individual graphs for each functions..
   // Graph* F_Graph = new Graph();
   // FuncGraphMap[F] = F_Graph;
    F_Graph->graph_type = gtFunctionData;

    //Initialize the function paraminfo even if no parameters
    FuncParamInfo* funcParam;
    funcParam = F_Graph->functionParamMap[F];
    if(!funcParam)
        funcParam = new FuncParamInfo();

    //Reset F_graph's store node map.

    F_Graph->StoreNodeMap.clear();


    //Add nodes for function parameters.. Actuals will be added at call statements.
    //   errs()<<"\n Generating Formal in and out nodes...";
    for (Function::arg_iterator Arg = F->arg_begin(), aEnd = F->arg_end(); Arg != aEnd; Arg++) {
        // Arg->dump();
        GraphNode * pinNode = F_Graph->addParamNode(Arg ,F,ntFormalIn);
        // errs()<<"\n Param added form in-- "<<pinNode->getLabel()<<pinNode->getId();
        GraphNode * poutNode = F_Graph->addParamNode(Arg ,F,ntFormalOut);
        // errs()<<"\n Param added form out-- "<<poutNode->getLabel()<<poutNode->getId();
    }
    //    errs()<<"\n After Generating actual in and out nodes...";

    //Add the ret val node..
    //F provides return type..not the return value directly.. for each block check if return, ad node and push back in retmap.

    //Get first block to process:

    set<BasicBlock*> affected_BB;
    queue<BasicBlock*> WorkList;
    std::set<BasicBlock*> setBlock;

    //Add all the instruction nodes for the function..
    addNodes(F,F_Graph);



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

    //AddControl DEp edges for the Function...
    if(AddControlEdges)
        addControlDependencies(F,F_Graph);




    ///Bakward processing pass to process all loads..

//    if(fullAnalysis)
//    {
//        // BasicBlock* TermBlock = &F->back();
//        map<Value*, set< Value*> > LoadQueue;
//        //  LoadStoreMap(LoadQueue,TermBlock,F_Graph);
//        //       errs()<<"\n CAlling BFS for function "<< F->getName();
//        BFSLoadStoreMap(LoadQueue,F,F_Graph);
//    }

//    //Print just the function GRaph....--
//    std::string funcName = F->getName();
//    std::string Filename = "graph_" + funcName + "_data.dot";
//    F_Graph->toDot("M_getModuleIdentifier",Filename);

}


void moduleDepGraph::addControlDependencies(Function* F,Graph * F_Graph)
{

    //Add an entry node for the Function.
    GraphNode *entryNode = F_Graph->addEntryNode(F);
    //Add entry node in funcparam info for further reference.
    FuncParamInfo* funcParam;
    funcParam = F_Graph->functionParamMap[F];
    if(!funcParam)
        funcParam = new FuncParamInfo();
    funcParam->entryNode = entryNode;
    F_Graph->functionParamMap[F] = funcParam;


    PostDominatorTree &PDT = getAnalysis<PostDominatorTree>(*F);

    //   // BasicBlock * BB = ;
    DomTreeNode *node = PDT[&F->getEntryBlock()];
    //F_Graph->nodes.insert(entryNode);
    while (node && node->getBlock())
    {
        /*
          * Walking the path backward and adding dependencies.
          */
        addControlDep(entryNode, node->getBlock(), F_Graph,etControl);
        node = node->getIDom();
    }


    std::vector<std::pair<BasicBlock *, BasicBlock *> > EdgeSet;

    for (Function::iterator I = F->begin(), E = F->end(); I != E; ++I)
    {
        ///Zhiyuan comment: find adjacent BasicBlock pairs in CFG, but the predecessor does not dominate successor.
        for (succ_iterator SI = succ_begin(I), SE = succ_end(I); SI != SE; ++SI)
        {
            assert(I && *SI);
            if (!PDT.dominates(*SI, I)) {

                //          errs() << I->getName() << "\n";
                //          errs() << "I:   " << I << "\n";
                //          errs() << "*SI: " << (*SI)->getName() << "\n";
                //          errs() << "Added to (A, B) edge set.\n";
                EdgeSet.push_back(std::make_pair(I, *SI));
            }
        }
    }

    typedef std::vector<std::pair<BasicBlock *, BasicBlock *> >::iterator EdgeItr;

    ///Zhiyuan comment: find nearest common ancestor in Post Dominator Tree for the BasicBlock pair.

    for (EdgeItr I = EdgeSet.begin(), E = EdgeSet.end(); I != E; ++I)
    {
        std::pair<BasicBlock *, BasicBlock *> Edge = *I;
        BasicBlock *L = PDT.findNearestCommonDominator(Edge.first, Edge.second);
        //  int type = getDependenceType(Edge.first, Edge.second);
        //      BasicBlockWrapper *A = BasicBlockWrapper::bbMap[Edge.first];

        // capture loop dependence
        if (L == Edge.first) {
            errs() << "\t find A == L: " << L->getName() << "\n";

            addControlDep(Edge.first, L, F_Graph,etControl);
            //     errs() << "DepType: " << type << "\n";
        }

        DomTreeNode *domNode = PDT[Edge.second];
        while (domNode->getBlock() != L)
        {
            addControlDep(Edge.first, domNode->getBlock(),F_Graph, etControl);
            domNode = domNode->getIDom();
        }
    }

    return;
}


void moduleDepGraph::addControlDep(GraphNode* from,BasicBlock *to, Graph * F_Graph, edgeType eType)
{
    for (llvm::BasicBlock::iterator ii = to->begin(), ie = to->end(); ii != ie; ++ii) {
        if (llvm::Instruction* Ins = llvm::dyn_cast<llvm::Instruction>(ii) ) {
            if (llvm::DebugFlag) {
                llvm::errs() << "[i_cdg debug] dependence from type (" << from->getLabel() << ") to instruction (" << *Ins << ")\n";
            }
            if(GraphNode *iw = F_Graph->findInstNode(Ins))
                from->connect(iw,eType);
        }
    }

}

void moduleDepGraph::addControlDep(BasicBlock* from,BasicBlock *to, Graph * F_Graph, edgeType eType)
{
    Instruction *Ins = from->getTerminator();
    assert(Ins);
    if(GraphNode *iw = F_Graph->findInstNode(Ins))
    {
        //self loop
        if (from == to) {

            if (llvm::DebugFlag) {
                llvm::errs() << "[i_cdg debug] loop dependence from (" << *from << ") to (" << *to << ")\n";
                llvm::errs() << "Terminator: " << *Ins << "\n";
            }
            /// Zhiyuan: Bug Fix 04/23/2015: in a "while condition" basic block, all the instructions
            /// before the br inst including the branch inst control depend on the br inst.
            for (llvm::BasicBlock::iterator ii = from->begin(), ie = from->end(); ii != ie; ++ii) {
                if(GraphNode *iwFrom = F_Graph->findInstNode(ii))
                {
                    iwFrom->connect(iw,eType);
                    //CDG->addDependency(iwFrom, iw, type);
                }
            }
        } else {
            if (llvm::DebugFlag) {
                llvm::errs() << "[i_cdg debug] dependence from (" << *from << ") to (" << *to << ")\n";
                llvm::errs() << "Terminator: " << *Ins << "\n";
            }
            for (llvm::BasicBlock::iterator ii = to->begin(), ie = to->end(); ii != ie; ++ii) {
                if(GraphNode *iwTo = F_Graph->findInstNode(ii))
                {
                    iw->connect(iwTo,eType);
                }
            }
        }
    }

}


void moduleDepGraph::addNodes(Function* F, Graph * F_Graph)
{
    for (llvm::Function::iterator bi = F->begin(), be = F->end(); bi != be; ++bi)
    {
        llvm::BasicBlock &BB = *bi;

        for (llvm::BasicBlock::iterator ii = BB.begin(), ie = BB.end(); ii != ie; ++ii)
        {

            if(isa<CallInst>(&*ii))  //  handle the call inst.
            {
                CallInst *CI = dyn_cast<CallInst>(&*ii);
                CallSite cs(&*ii);

                Function * calledFunc = CI->getCalledFunction();
                if(calledFunc && !calledFunc->isDeclaration())
                {
                    //Map params or pre-empt funciton..
                } //if called func

                if(calledFunc && calledFunc->isDeclaration())
                {
                    //    errs()<<"\nPROPAGATE: Call Site with called func and IS delaration.. !!";
                    //  CI->dump();
                    // errs()<<"\n Name :"<<calledFunc->getName();
                    if(calledFunc->getName().equals("llvm.dbg.declare"))
                        continue;
                    // HandleLibraryFunctions(CI,parentFunction);
                }

            }
            GraphNode *node = F_Graph->addNode(ii,ntInst);
//            node->initialize(ii,ntInst);
//            F_Graph->instMap[ii] = node;

            //            if (instMap.find(ii) == instMap.end()) //if not in instMap yet, insert
            //            {
            //                InstructionWrapper *iw = new InstructionWrapper(ii, bi, &F);
            //                nodes.insert(iw);
            //                instMap[ii] = iw;
            //            }
        }
    }
}


std::vector<Instruction*> moduleDepGraph::getDependencyInFunction(Function* F, Instruction* pLoadInst){

    //  AliasAnalysis &AA = getAnalysis<AliasAnalysis>();

    std::vector<Instruction*> _flowdep_set;

    //find and save all Store/Load instructions
    std::vector<Instruction*> StoreVec;
    for(inst_iterator instIt = inst_begin(F), E = inst_end(F); instIt != E; ++instIt){

        Instruction *pInstruction = dyn_cast<Instruction>(&*instIt);

        if(isa<StoreInst>(pInstruction))
            StoreVec.push_back(pInstruction);
    }

    //for each Load Instruction, find related Store Instructions(alias considered)

    //  errs() << "LoadInst: " << *pLoadInst << '\n';
    LoadInst* LI = dyn_cast<LoadInst>(pLoadInst);


    Value * LoadPtr = LI->getOperand(0);


    for(int j = 0; j < StoreVec.size(); j++)
    {
        StoreInst* SI = dyn_cast<StoreInst>(StoreVec[j]);
        //Check if LI and SI pointers alias each other..
        Value * StorePtr = SI->getPointerOperand();

        if(checkPointer(LoadPtr,StorePtr,F))
        {
            _flowdep_set.push_back(StoreVec[j]);
        }
    }

    return _flowdep_set;
}



void moduleDepGraph::addDependencies(Value *v, Graph *F_Graph, Function *F)
{
    FuncParamInfo* funcParam = F_Graph->functionParamMap[F];

    if(Instruction *pInstruction = dyn_cast<Instruction>(v))
    {
        if(isa<LoadInst>(pInstruction)){
            //Get dependency using the DSA BU phase of alias analysis.
            std::vector<Instruction*> flowdep_set = getDependencyInFunction(F, pInstruction);

            for(int i = 0; i < flowdep_set.size(); i++){
                GraphNode *sNode = F_Graph->findInstNode(flowdep_set[i]);
                if(sNode != NULL)
                {
                    GraphNode* lNode = F_Graph->findInstNode(pInstruction);
                    if(lNode != NULL)
                    {
                        sNode->connect(lNode,etData);
                    }
                }
                else if(funcParam != NULL)
                    if(GraphNode *pNode = funcParam->FormalInMap[flowdep_set[i]])
                    {
                        GraphNode* lNode = F_Graph->findInstNode(pInstruction);
                        if(lNode != NULL)
                        {
                            pNode->connect(lNode,etData);
                        }
                    }
            }

            flowdep_set.clear();
        }
        //Adds SSA based def use flows.
        if(!isa<StoreInst>(pInstruction) )
        {
            for (Instruction::const_op_iterator cuit = pInstruction->op_begin(); cuit != pInstruction->op_end(); ++cuit)
            {
                if(Instruction* ptInstruction = dyn_cast<Instruction>(*cuit)){
                    //     errs() << "Connecting " << *pInstruction << " --> " << *(*cuit) << '\n';
                    GraphNode *sNode = F_Graph->findInstNode(ptInstruction);
                    if(sNode != NULL)
                    {
                        GraphNode* lNode = F_Graph->findInstNode(pInstruction);
                        if(lNode != NULL)
                        {
                            sNode->connect(lNode,etData);
                        }
                    }
                }
                else if(Value* ptInstruction = dyn_cast<Value>(*cuit)){
                    if(funcParam != NULL)
                    {
                        GraphNode *sNode = funcParam->FormalInMap[ptInstruction];
                        if(sNode != NULL)
                        {
                            GraphNode* lNode = F_Graph->findInstNode(pInstruction);
                            if(lNode != NULL)
                            {
                                sNode->connect(lNode,etData);
                            }
                        }
                    }
                }
            }
        } // end of ifnot store inst.. for def-use chain adding.

        //Need to process Store for in and out params and flow connections.
        if(isa<StoreInst>(pInstruction) )
        {
            StoreInst * SI = dyn_cast<StoreInst>(pInstruction);
            //SI->dump();
            Value* v = SI->getValueOperand();
            Value* sPtr = SI->getPointerOperand();
            GraphNode* lNode = F_Graph->findInstNode(pInstruction);
            if(lNode != NULL)
            {
                GraphNode *sNode = F_Graph->findInstNode(v);
                if(sNode != NULL)
                {
                    sNode->connect(lNode,etData);
                }
                //Connect the value parameter if used from parameter...
                else if(funcParam!=NULL)
                    if(GraphNode *pNode = funcParam->FormalInMap[v])
                    {
                        pNode->connect(lNode,etData);
                    }
                //Connects the ptr to formal out since value is stored in out param alias
                if(funcParam!=NULL)
                {
                    //Check for all the formal out params if the stored in val is an alias of those..
                    for(map<Value*,GraphNode*>::iterator fmap = funcParam->FormalOutMap.begin();fmap != funcParam->FormalOutMap.end();++fmap)
                    {
                        //if the param is of ptr type..
                        Value * param = fmap->first;
                        if(checkPointer(param,sPtr,F))
                            if(GraphNode *pNode = fmap->second)
                            {
                                lNode->connect(pNode,etData);
                            }
                    }
                }
            }

        }

    }
}



///Function: BFSLoadStoreMap
/// Reverse trackign function used for attaching the loads to appropriate stores..
void moduleDepGraph::BFSLoadStoreMap(map<Value*, set< Value*> > LoadQueue, Function* F, Graph * F_Graph)
{
    BasicBlock* ProcLastBlock = &F->back();
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
    for (pred_iterator PI = pred_begin(ProcLastBlock), E = pred_end(ProcLastBlock); PI != E; ++PI) {
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
            //   CheckifRelevantField(GI,F_Graph);
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
        //ToDO: Fix;;; adding duplicate return node.. (remove the other copy or add that in here.. if present.



        //Process each statement in Graph add node..
        //F_Graph->addInst(Iit,counter,parentFunction);
        addDependencies(Iit,F_Graph,parentFunction);
        counter++;

        //If the added statement is a return statement, add the appropriate ret val in funcparaminfo
        if(isa<ReturnInst>(Iit))
        {
            ReturnInst *RI = dyn_cast<ReturnInst>(Iit);
            Value * retVal = RI->getReturnValue();
            GraphNode * retNode = F_Graph->findInstNode(retVal);
            FuncParamInfo* funcParam = F_Graph->functionParamMap[parentFunction];
            if(!funcParam)
                funcParam = new FuncParamInfo();

            if(retNode)
                funcParam->RetValMap[retVal] = retNode;
            else if(retNode =F_Graph->findInstNode(RI))
                funcParam->RetValMap[retVal] = retNode;

             F_Graph->functionParamMap[parentFunction] = funcParam;

        }

    }

    return affected_BB;
}



// get type after n'th index in getelementptr
static Type * typeOfGetElementPtr(GetElementPtrInst *e, unsigned n) {
    assert(n < e->getNumOperands() && "over the last index in getelementptr?");
    Value *val = e->getOperand(0);
    Type *ty = val->getType();
    for (unsigned i = 1; i <= n; i++) {
        if (ty->isPointerTy())
            ty = ty->getPointerElementType();
        else if (ty->isArrayTy())
            ty = ty->getArrayElementType();
        else if (ty->isStructTy())
            ty = ty->getStructElementType(
                        cast<ConstantInt>(e->getOperand(i))->getZExtValue());
        else
            assert(false && "unknown type in GEP?");
    }
    return ty;
}


void moduleDepGraph::CheckifRelevantField(GetElementPtrInst * GI, Graph* F_Graph)
{

    //Check if it is the field of interest.
    Function * parentFunction = GI->getParent()->getParent();
    Value* baseptr = GI->getOperand(0);
    Type* ty = baseptr->getType();

    if(code) errs()<<"\n\n   GEP :\n"; //<<relevantFields.size();
    if(code) GI->dump();
    // if(code) baseptr->dump();
    //if(code) ty->dump();

    // if(code)
    // {errs()<<"\n is pointer :"<<ty->isPointerTy()<<"\n";
    if(ty->isPointerTy())
    {
        errs()<<"\n";
        ty->dump();
        errs()<<" ..elem ty-> ";
        Type* structty = ty->getPointerElementType(); //->dump();
        ty->getPointerElementType()->dump();
        errs()<<"\n The baseptr is pointer to struct if true.. : "<<ty->getPointerElementType()->isStructTy()<<"\n";
        if(baseptr->hasName())
            errs()<<"\n baseptr has name:  "<<baseptr->getName();
        if(structty->isStructTy())
        {
            errs()<<"\n baseptr struct name:  "<<structty->getStructName();
            // now get the field index refered..--:

            for (int i = GI->getNumOperands() - 2; i > 0; i--) {
                Type *ty = typeOfGetElementPtr(GI, i);
                assert(ty->isArrayTy() || ty->isStructTy());
                if (ty->isStructTy()) { // structure field
                    // t.ta_type = TAINT_FLD;
                    // t.ta_un.ta_fld.f_struct = ty;
                    int index = cast<ConstantInt>
                            (GI->getOperand(i + 1))->getZExtValue();

                    errs()<<"\n\n At operand no: "<<i<<" with type : ";
                    ty->dump();
                    errs()<<" index is : "<<index;
                }
            }
        }
    }

    //get what struct it points to..
    //    if(Instruction *In = dyn_cast<Instruction>(baseptr))
    //    {
    //        errs()<<"\n\n The pointer element is isntructions";
    //        In->dump();
    //        int num = In->getNumOperands();
    //        for(int i =0; i<num; i++)
    //        {
    //            Value* operand = In->getOperand(i);
    //            operand->dump();
    //            errs()<<"\n Operand type: ";
    //            operand->getType()->dump();
    //            errs()<<"\n is struct : "<<operand->getType()->isStructTy();
    //            if(operand->getType()->isPointerTy())
    //                errs()<<"\n is pointer to struct : "<<operand->getType()->getPointerElementType()->isStructTy();
    //            if(operand->hasName())
    //            {
    //                errs()<<"\n has name:  "<<operand->getName();
    //            }
    //        }
    //    }
    //    else if(ty->isStructTy())
    //    {
    //        errs()<<"\n The basepointer is of type struct.. ";
    //    }
    //    else
    //    {
    //        errs()<<"\n Not instruction and base ptr type not struct.... ";
    //        baseptr->dump();

    //    }
    //}



    //check if the bae pointer is an struct type.. or pointer to struct.... else just get all operands and and check the same...
    //    for(set<RelevantFields*>::iterator fieldIt = relevantFields.begin(); fieldIt != relevantFields.end(); ++fieldIt)
    //    {
    //        if(code) errs()<<"\n Function name read from file.. : "<<(*fieldIt)->functionName<<" parent funct name.. :"<<parentFunction->getName();

    //        if((*fieldIt)->functionName==parentFunction->getName())
    //        {
    //            if(code) errs()<<"\nIn function ... looking for.... : "<<(*fieldIt)->functionName<<" "<<(*fieldIt)->variable;

    //            if(baseptr->hasName())
    //            {
    //                if(code)  errs()<<"\n The base ptr.. has name...!!";
    //                if(baseptr->getName()== (*fieldIt)->variable)
    //                {
    //                    if(code) errs()<<"\nTesting for var : "<<(*fieldIt)->variable;

    //                    int fieldIndex;
    //                    for (int i = GI->getNumOperands() - 2; i > 0; i--) {
    //                        Type *ty = GI->getType();
    //                        //  assert(ty->isArrayTy() || ty->isStructTy());
    //                        if (ty->isStructTy()) { // structure field

    //                            fieldIndex = cast<ConstantInt>(GI->getOperand(i + 1))->getZExtValue();
    //                            errs()<<"\n---field index..---- : "<<fieldIndex;

    //                        }
    //                    }
    //                    //match index.
    //                    if(fieldIndex == (*fieldIt)->index)
    //                    {
    //                        FieldVals.insert(GI);
    //                        //update fields in graph.
    //                        F_Graph->FieldsVal = FieldVals;
    //                        if(code) errs()<<"\n\n   Found field";
    //                        if(code) GI->dump();

    //                    }
    //                }
    //            }
    //            else
    //                if(code)  errs()<<"\n The base ptr.. has NONAME...!!";

    //        }
    //    }

    //end of fucnt
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
            std::set<Value*> aliases; // = memNew->getAliases();
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



//void moduleDepGraph::buildTypeTree(Argument *arg, TypeWrapper *tyW, TreeType treeTy){

//  errs() << "\n" << "buildTypeTree in TIME: " << "\n";

//  if(arg == nullptr){
//    errs() << "In buildTypeTree, incomming arg is a nullptr\n";
//    exit(1);
//  }
//  if(tyW->getType()->isPointerTy()){
//    errs() << "IN buildTypeTree, ty = "<< *tyW->getType() << "is a pointer type. arg_addr = " << arg << " *arg = " << *arg << "\n";
//    errs() << "NumContainedTypes = " << tyW->getType()->getContainedType(0)->getNumContainedTypes() << "\n";

//    //find inserting loction
//    tree<InstructionWrapper*>::iterator insert_loc;

//    ArgumentWrapper *pArgW = getArgWrapper(FunctionWrapper::funcMap[arg->getParent()], arg);

//    if(pArgW == nullptr){
//      errs() << "getArgWrapper returns a NULL pointer!" << "\n";
//      exit(1);
//    }
//    else{
//      if(pArgW->getArg() == arg){
//    for(tree<InstructionWrapper*>::iterator treeIt = pArgW->getTree(treeTy).begin(),
//          treeE = pArgW->getTree(treeTy).end(); treeIt != treeE; ++treeIt){
//      if((*treeIt)->getFieldId() == tyW->getId()){
//        insert_loc = treeIt;
//        break;
//      }
//    }//end for tree iteration

//    //if ty is a pointer, its containedType [ty->getContainedType(0)] means the type ty points to
//    for(unsigned int i = 0; i < tyW->getType()->getContainedType(0)->getNumContainedTypes(); i++){
//      errs() << "containedType " << i << " " << *tyW->getType()->getContainedType(0)->getContainedType(i) << "\n";

//      TypeWrapper *tempTyW = new TypeWrapper(tyW->getType()->getContainedType(0)->getContainedType(i),id);
//      InstructionWrapper *typeFieldW = new InstructionWrapper(arg->getParent(),arg,tempTyW->getType(),id++,PARAMETER_FIELD);
//      InstructionWrapper::nodes.insert(typeFieldW);

//      pArgW->getTree(treeTy).append_child(insert_loc,typeFieldW);

//      buildTypeTree(arg, tempTyW, treeTy);
//    }//end for ty getContainedType

//      }// end if pArgW->getArg() == arg
//    }// end else (pArgW != nullptr)

//  }//end if isPointerTy

//  else{
//    if(arg != nullptr)
//      errs() << *tyW->getType() << " is a Non-pointer type. arg = " << *arg << "\n";
//    else
//      errs() << "arg is nullptr!\n";
//  }
//}




void moduleDepGraph::matchParametersAndReturnValues(Function &F, Graph * Full_Graph) {

    // Only do the matching if F has any use
    if (F.isVarArg() || !F.hasNUsesOrMore(1)) {
        return;
    }
    //if(debug)
    errs()<<"\n Function arg matching for;; "<< F.getName();

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
        argNode = Full_Graph->addInst(argptr,-1,&F);

        if (argNode != NULL)
        {
            Full_Graph->addEdge(argPHI, argNode);
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

        //Add the root to the tree here..
        for (i = 0, AI = CS.arg_begin(), EI = CS.arg_end(); AI != EI; ++i, ++AI) {
            Parameters[i].second = Full_Graph->addInst(*AI,-1,&F);
        }

        // Match formal and real parameters
        for (i = 0; i < Parameters.size(); ++i) {

            Full_Graph->addEdge(Parameters[i].second, Parameters[i].first);
            if(debug) errs()<<"\nAdding Edge between "<< Parameters[i].second->getLabel()<<" -- "<<Parameters[i].first->getLabel();
        }

        // Match return values
        if (!noReturn) {

            OpNode* retPHI = new OpNode(Instruction::PHI);
            GraphNode* callerNode = Full_Graph->addInst(caller,-1,&F);
            Full_Graph->addEdge(retPHI, callerNode);

            for (SmallPtrSetIterator<llvm::Value*> ri = ReturnValues.begin(),
                 re = ReturnValues.end(); ri != re; ++ri) {
                GraphNode* retNode = Full_Graph->addInst(*ri,-1,&F);
                Full_Graph->addEdge(retNode, retPHI);
            }

        }

        // Real parameters are cleaned before moving to the next use (for safety's sake)
        for (i = 0; i < Parameters.size(); ++i)
            Parameters[i].second = NULL;
    }

    Full_Graph->deleteCallNodes(&F);
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


//void llvm::moduleDepGraph::deleteCallNodes(Function* F) {
//    depGraph->deleteCallNodes(F);
//}



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
        //errs() << " Could not open the Field information file \n";
    }
    else
    {
        while(srcFile >> line)
        {
            if(code) errs() << " Reading fields--- ";
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








