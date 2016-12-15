
#include "CallGraphWrapper.h"

#define debug false
#define USEDSA false

using namespace llvm;


STATISTIC(NumFuncitons, "Number of functions");
STATISTIC(NumCalls, "Number of call sites");


static cl::opt<bool, false>
Level("fp-level", cl::desc("What heuristic to apply: 1. number of params, 2. number and type of params, ."), cl::NotHidden);

STATISTIC(DirCall, "Number of direct calls");
STATISTIC(IndCall, "Number of indirect calls");
STATISTIC(CompleteInd, "Number of complete indirect calls");
STATISTIC(CompleteEmpty, "Number of complete empty calls");


///Function to determeine if the address of the given function/value has been taken or not.
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

///------------------------------------------------------------------------
///
BottomUpPass::BottomUpPass():
    CallGraphSCCPass(ID) {
}


bool BottomUpPass::runOnSCC(CallGraphSCC &SCC)
{
    // NumCalls =0;
    //  CallGraph CG = getAnalysis<CallGraphWrapperPass>().getCallGraph();
    //DataLayoutPass *DLP = getAnalysisIfAvailable<DataLayoutPass>();
    //const DataLayout *DL = DLP ? &DLP->getDataLayout() : nullptr;
    //const TargetLibraryInfo *TLI = getAnalysisIfAvailable<TargetLibraryInfo>();

    SmallPtrSet<Function*, 8> SCCFunctions;
    DEBUG(dbgs() << "\nInliner visiting SCC:");
    for (CallGraphSCC::iterator I = SCC.begin(), E = SCC.end(); I != E; ++I) {
        Function *F = (*I)->getFunction();
        if (F) {SCCFunctions.insert(F); NumFuncitons++; }
        DEBUG(dbgs() << " " << (F ? F->getName() : "INDIRECTNODE"));
    }
    //  NumCalls = SCCFunctions.size();


    DEBUG(dbgs() << "\nSizeof the SCC: "<<SCC.size());
    //Collect all the call sites.
    SmallVector<std::pair<CallSite, int>, 16> CallSites;


    for (CallGraphSCC::iterator I = SCC.begin(), E = SCC.end(); I != E; ++I) {
        Function *F = (*I)->getFunction();
        if (!F)
        {
            //Check waht kind of call it is:
            //  errs()<<"\nCallGRaph node dump -- ";
            //      (*I)->dump();
            continue;
        }

        for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
            for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I) {
                CallSite CS(cast<Value>(I));
                // If this isn't a call, or it is a call to an intrinsic, it can
                // never be inlined.
                if (!CS || isa<IntrinsicInst>(I))
                    continue;

                // If this is a direct call to an external function, we can never inline
                // it.  If it is an indirect call, inlining may resolve it to be a
                // direct call, so we keep it.
                if (CS.getCalledFunction() && CS.getCalledFunction()->isDeclaration())
                    continue;

                CallSites.push_back(std::make_pair(CS, -1));
            }
    }

    DEBUG(dbgs() << ": " << CallSites.size() << " call sites.\n");
    NumCalls +=CallSites.size();
}

///------------------------------------------------------------------------


//CallGraphWrapperPass

CallGraphWrapper::CallGraphWrapper():
    ModulePass(ID) {
}

bool CallGraphWrapper::hasAddressTaken(llvm::Function *F){
    return addressTaken.find(F) != addressTaken.end();
}



///The call graph warapper modules first gets the existing call graph.
/// It then computes the address taken functions
///
bool CallGraphWrapper::runOnModule(Module &M) {
    // CallGraphWrapperPass &WP = getAnalysis<CallGraphWrapperPass>();

    dsa::CallTargetFinder<TDDataStructures>* CTF = &getAnalysis<dsa::CallTargetFinder<TDDataStructures> >();

    CG = &getAnalysis<CallGraphWrapperPass>().getCallGraph();
    if(USEDSA)
    {
        //runtime ERROR occurs here..Symbol not found: __ZN3dsa16CallTargetFinderIN4llvm16TDDataStructuresEE10getTargetsEv
        //REsolved by directly accessing indMap.
        //  IndMap = copyIndMap(CTF->IndMap);

        IndMap = copyIndMap(CTF->getTargets());
    }
    else
    {
        //Array just to record calls to functions with diff number of arguments:
        int callto[10]= {0,0,0,0,0,0,0,0,0};


        //Collect all the address take funcitons in a collection.
        for (Module::iterator FI = M.begin(), FE = M.end(); FI != FE; ++FI){
            if(isAddressTaken(FI)) {
                addressTaken.insert(FI);
            }
        }

        DEBUG(errs()<<"Address Taken Functions: "<< addressTaken.size());


        // Print Address Taken..uncomment to see results.

        //       for(std::set<Function*>::iterator FI = addressTaken.begin();FI!=addressTaken.end();++FI)
        //    {
        //        errs()<<"\nAddress Taken For: "<<(*FI)->getName();
        //        errs()<<" Type:  ";
        //        (*FI)->getType()->dump();
        //       // Function * f;

        //    }


        //Consider the root node as external node if you dont find the main functions to start with.
        CallGraphNode* RootNode = CG->getCallsExternalNode();

        bool externalFunction;
        //Traversal of Call Graph.
        for (scc_iterator<CallGraph*> I = scc_begin(CG); !I.isAtEnd(); ++I) {
            const std::vector<CallGraphNode *> &SCC = *I;


            for (unsigned i = 0, e = SCC.size(); i != e; ++i) {
                Function *F = SCC[i]->getFunction();
                if (!F) {
                    externalFunction = true;
                    break;
                }
                else
                {
                    //   errs()<<"\nFunction in call graph  :"<<F->getName();
                    if(F->getName()=="main")
                        RootNode =SCC[i];
                }
            }
        }

        errs()<<"\n Root Node ";

        //Start from the root node here and iterate over the call graph..
        // std::set<CallGraphNode> workList = RootNode->CalledFunctionsVector;

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

                                //                  DEBUG(errs()<<"\n\n Called Value : ";
                                //                          cs.getCalledValue()->dump();
                                //                          errs()<<"Called Value stripped pointer cast : ";
                                //                          calledValue->dump();
                                //                          cs.getCalledValue()->getType()->dump();

                                //                  );

                                Type *ty = cs.getCalledValue()->getType();
                                if(ty->isPointerTy())
                                {
                                    if(ty->getPointerElementType()->isFunctionTy())
                                    {
                                        // cs.getCalledValue()->dump();
                                        //  FunctionType *F_type = dyn_cast<FunctionType*>(ty);
                                        //    calledFuncs = getPossibleCallees(cs);
                                        //int calSiteParams = cs.arg_size();

                                        calledFuncs = getPossibleCallees(cs);


                                        int calledFuncCount = calledFuncs.size();
                                        //  errs()<<"\n Number of possible calls at the call site: "<<calledFuncCount;

                                        if(calledFuncCount==0)
                                        {
                                            unResolved.insert(cs);
                                        }
                                        else
                                        {
                                            //  errs()<<"\n\nREsolved CAll !! ?";
                                            //Resolved calls
                                            //  pair<CallSite,std::vector<Function*> > cllInfo;
                                            // cllInfo.first = cs;
                                            //cllInfo.second = calledFuncs;
                                            // IndMap.insert(cllInfo);
                                            //IndMap[cs]calledFuncs;
                                        }
                                        Type *Fty = ty->getPointerElementType();
                                        int numArgs = Fty->getFunctionNumParams();
                                        callto[numArgs]++;
                                    }
                                }


                                if (isa<ConstantPointerNull>(calledValue)) {
                                    ++DirCall;
                                    CompleteSites.insert(cs);
                                } else {
                                    IndCall++;




                                }
                            } else {
                                ++DirCall;
                                //  IndMap[cs].push_back(CF);
                                CompleteSites.insert(cs);
                            }
                        } //end if call or invoke
                    } //end block iterator
                } //end func iterator
            } //end if not declaration
        } //end for module iterator


        //Print Unresolved Calls:
        /*
      errs()<<"\n Unresolved CAlls :";
      for(std::set<CallSite>::iterator callS = unResolved.begin();callS != unResolved.end();++callS)
      {
          CallSite cs = (*callS);

        //cs.getCalledValue()->getType()->dump();
        errs()<<"\n";
        Instruction *ins = cs.getInstruction();
        Function *func = ins->getParent()->getParent();
        errs()<<"  In Function: "<<func->getName();
        (*callS)->dump();
      }

*/

        errs()<<"\nDirect Calls  :"<<DirCall;
        errs()<<"\nIndirect Calls : "<<IndCall;


//        for(int i=0;i<10;i++)
//        {
//            errs()<<"\nFor # arguments : "<<i<<" calls made "<< callto[i];
//        }
//        errs()<<"\n\n\n";


    }

//         //Print REsolved Calls:

//        for(std::map<CallSite, std::vector<Function*> >::iterator mapIt = IndMap.begin(); mapIt != IndMap.end(); ++mapIt)
//        {

//            CallSite cs = (*mapIt).first;
//             std::vector<Function*> calledF = (*mapIt).second;
//           // errs()<<"\n---Targets for CallSite :  ";
//           //  CallSite cs = (*callS);
//             if(Function* calledFunc = cs.getCalledFunction())
//                 continue;

//             errs()<<"\n";
//            Instruction *ins = cs.getInstruction();
//           Function *func = ins->getParent()->getParent();
//             errs()<<"\n["<<func->getName()<<":0]  ";
//            //cs.getCalledValue()->getType()->dump();

//             //Add Info in FunctiontoIndCalls
//             vector<CallSite> callSitesinFunc = FunctiontoIndCalls[func];
//             callSitesinFunc.push_back(cs);
//             FunctiontoIndCalls[func] = callSitesinFunc;


//            cs.getInstruction()->dump();
//            errs()<<"========";
//            for(std::vector<Function*>::iterator fs = calledF.begin();fs != calledF.end();++fs)
//            {
//                errs()<<"\n"<<(*fs)->getName(); //<<"  Type  ";
//               // (*fs)->getType()->dump();
//            }

//        }



    // Run the find all paths function to get all call graph paths from a source to a sink.
    // Parse file for source and sink
    ifstream fin;
    fin.open("source_sink.txt"); //open file that contains start and end functions
    if (!fin.good())
    {
        errs()<<"\nFile not found when loading source sink list\n";
        return false;
    }


    const int MAX_CHARS_PER_LINE = 512;
    const int MAX_TOKENS_PER_LINE = 20;
    const char* token[MAX_TOKENS_PER_LINE] = {}; // initialize to 0


    while (!fin.eof())
    {
        char buf[MAX_CHARS_PER_LINE];
        fin.getline(buf, MAX_CHARS_PER_LINE);

        int n = 0; // a for-loop index

        // array to store memory addresses of the tokens in buf


        // parse the line
        token[0] = strtok(buf, " "); // first token
        if (token[0]) // zero if line is blank
        {
            for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
            {
                token[n] = strtok(0, " "); // subsequent tokens
                if (!token[n]) break; // no more tokens
            }
        }

    }

    const std::string start(token[1]);
    const std::string end(token[2]);

    PathSet* pathSet;
    //Pass source and sink to find all paths
    findAllPaths(pathSet, start, end);

    return false;
}




std::map<CallSite, std::vector<Function*> > CallGraphWrapper::copyIndMap(std::map<CallSite, std::vector<const Function*> > srcMap)
{
    std::map<CallSite, std::vector<Function*> > destInd;
    for(std::map<CallSite, std::vector<const Function*> >::iterator mapIt = srcMap.begin(); mapIt != srcMap.end(); ++mapIt)
    {
        //errs()<<"\n";
        CallSite cs = (*mapIt).first;
        std::vector<const Function*> calledF = (*mapIt).second;
        // std::vector<Function*> calledF = const_cast<std::vector<Function*> >(*mapIt).second;
        for(std::vector<const Function*>::iterator fs = calledF.begin();fs != calledF.end();++fs)
        {
            Function * tfs = const_cast<Function*>(*fs);
            destInd[cs].push_back(tfs);
        }
        //destInd[cs] = calledF;
        // std::vector<Function*> calledF = (*mapIt).second;
    }

    return destInd;
}

void CallGraphWrapper::findAllPaths(PathSet* pathSet,
                                    const std::string& start,
                                    const std::string& end)
{


    CallGraphNode* sinkNode;
    Function *sinkFunction; // REMOVE
    instructionCallSite sinkCSStruct;
    int sinkFound = 0;

    bool externalFunction;


    //Initial Traversal of the call graph to find the sink.
    for (scc_iterator<CallGraph*> I = scc_begin(CG); !I.isAtEnd(); ++I) {
        const std::vector<CallGraphNode *> &SCC = *I;


        for (unsigned i = 0, e = SCC.size(); i != e; ++i) {
            Function *F = SCC[i]->getFunction();
            if (!F) {
                externalFunction = true;
                break;
            }
            else
            {
                //errs()<<"\nFunction in call graph  :"<<F->getName();
                if(F->getName()==end)
                {
                    sinkFound = 1;
                    sinkNode =SCC[i];
                    sinkFunction = F; // REMOVE

                    sinkCSStruct.function = F;
                    sinkCSStruct.functionName = F->getName();
                    sinkCSStruct.callInst = NULL;


                }
            }
        }
    }


    // Check to make sure the sink function was found within the program.
    if (sinkFound == 0)
    {
        errs()<<"***ERROR: The sink specified was not found within the program.\n";
        return;
    }


    //Test printing out node name
    errs()<<"Sink Node: "<<getNodeLabel(sinkNode)<<"\n";





    // Setup reverse mapping of indirect calls

    errs()<<"Running Reverse Mapping\n";

    IndCallerList indirectCallerList;

    //struct IndCallforFunction tempStruct;
    //tempStruct.name = sinkNode->getFunction()->getName();


    //indirectCallerList->push_back(tempStruct);


    createReverseMapping(&indirectCallerList);


    for(IndCallerList::iterator currCallee = indirectCallerList.begin();
        currCallee != indirectCallerList.end();
        ++currCallee)
    {

        //errs()<<"Name of Callee: "<<(*currCallee).name<<"\n";

        for(std::vector<Function*>::iterator func = (*currCallee).functionList.begin();
            func != (*currCallee).functionList.end();
            func ++)
        {
            //errs()<<"	Name of caller: "<<(*func)->getName()<<"\n";
        }
    }








    // Create data structure that will hold current path visited. Add start node to current path.
    Path visited;
    //visited.push_back( sinkFunction );
    visited.push_back(sinkCSStruct);

    // Create data structure to hold a list of paths. We will only add a path to the pathset when the source is reached.
    PathSet pathSets;



    // Enter a recursive function to gather all of the paths from source to sink.
    OtherFunction(pathSets, visited, start, &indirectCallerList);

    errs()<<"Make it through entire path finding algorithm.\n";
    // For each function on each path, gather a list of calls for context sensitivity.





    std::vector<Function*> functionList;
    functionList.push_back( sinkFunction );

    for (std::vector<Path>::iterator I = pathSets.begin();
         I != pathSets.end();
         I++)
    {
        for(std::vector<instructionCallSite>::const_iterator callSite = (*I).begin(); callSite != (*I).end(); callSite++)
        {
            Function* func = (*callSite).function;
            if (!ContainsFunction(functionList, (func)))
            {
                functionList.push_back(func);
            }

        }
    }



    errs()<<"Successfully gathered a full list of functions.\n";

    /* 	The next stage is to compile the gathered information into a human meaningful
        format. We will output two sets of output. The first is a list of each first level
        function and all context sensitive calls within.

        The second format is a context sensitive path output. Where each level of depth
        is partitioned through the use of (). For example assume a call path:

        function 1 -> function 2 -> function 3.

        This would be displayed as (function 1 (what function 1 calls) function 2 (...)...)

        Based on the k-step parameter the depth can very.

        Such as:

        (function 1 (subcall 1 (subsubcall 1) subcall 2 (...) ...) function 2 (...) ...)
    */

    // Output a list of all context sensitive calls to a file.
    printContextSensitiveCalls(functionList);

    // Output a list of all context sensitive paths to a file.
    printContextSensitivePaths(pathSets);
    callPathSets = pathSets;



    // Test printing out the identified call paths.
    int count = 0;
    errs()<<"Number of paths: "<<pathSets.size()<<"\n";
    for (std::vector<Path>::iterator I = pathSets.begin();
         I != pathSets.end();
         I++)
    {
        errs()<<"Path "<<++count<<": Size: "<<(*I).size()<<"\n";
        for(std::vector<instructionCallSite>::const_iterator callSite = (*I).begin(); callSite != (*I).end(); callSite++)
        {
            errs()<<" : Function on Path: "<<(*callSite).functionName<<"\n";
        }
        errs()<<"\n\n";
    }
}



void CallGraphWrapper::OtherFunction(PathSet& pathSet,
                                     Path& visited,
                                     const std::string& end,
                                     IndCallerList* indirectCallerList)
{
    instructionCallSite calleeSite;
    calleeSite = visited.back();
    Function* back = calleeSite.function;


    for (Value::user_iterator i = back->user_begin(), e = back->user_end(); i != e; ++i)
    {

        errs()<<"Searching uses for function: "<<back->getName()<<"\n";

        if (Instruction* use = dyn_cast<Instruction>(*i)) {



            BasicBlock *bb = use->getParent();
            Function *func = bb->getParent();
            errs()<<" Use for: "<<back->getName()<<": Instruction Name: "<<func->getName()<<" : : "<<use<<"\n";


            if (!isOnPath(visited, func))
            {
                /*
                // Testing Purposes Only
                for(std::vector<Function*>::const_iterator f = visited.begin();
                                       f != visited.end();
                                       f++)
                {


                    errs()<<" : Function on Path: "<<(*f)->getName()<<"\n";
                }
                errs()<<" : Function on Path: "<<func->getName()<<"\n\n";

                */
                if (func->getName() == end) {

                    instructionCallSite tempStruct;

                    tempStruct.functionName = func->getName();
                    tempStruct.function = func;
                    tempStruct.callInst = use;


                    //visited.push_back(func); REMOVE
                    visited.push_back( tempStruct );

                    // Get hop count for this path
                    const int size = (int) visited.size();
                    const int hops = size - 1;

                    // Add current path to path set
                    Path path(visited.begin(), visited.begin() + size);

                    pathSet.push_back(path);

                    // Remove only the last node for continued search (backtrack)
                    visited.erase( visited.begin() + hops);


                }
                else
                {
                    instructionCallSite tempStruct;

                    tempStruct.functionName = func->getName();
                    tempStruct.function = func;
                    tempStruct.callInst = use;


                    //visited.push_back(func); REMOVE
                    visited.push_back( tempStruct );

                    // Get hop count for this path
                    const int size = (int) visited.size();
                    const int hops = size - 1;

                    OtherFunction(pathSet, visited, end, indirectCallerList);

                    visited.erase( visited.begin() + hops);
                }
            }
        }
    }


    // Handle Indirect Callers Here.

    for(IndCallerList::iterator currCallee = indirectCallerList->begin();
        currCallee != indirectCallerList->end();
        ++currCallee)
    {

        if( back->getName() == (*currCallee).name )
        {
            errs()<<"Match found: "<<back->getName()<<"     "<<(*currCallee).name<<"\n";
            //errs()<<"Name of current caller: "<<Func->getName()<<"Name of current callee: "<<(*currCallee).name<<"\n";
            //	(*currCallee).functionList.push_back(Func);
            //}

            for(std::vector<Function*>::iterator func = (*currCallee).functionList.begin();
                func != (*currCallee).functionList.end();
                func ++)
            {
                Function* function = (*func);
                if (!isOnPath(visited, function))
                {

                    if (function->getName() == end) {

                        instructionCallSite tempStruct;

                        tempStruct.functionName = function->getName();
                        tempStruct.function = function;
                        tempStruct.callInst = NULL;

                        //visited.push_back(func); REMOVE
                        visited.push_back( tempStruct );

                        // Get hop count for this path
                        const int size = (int) visited.size();
                        const int hops = size - 1;

                        // Add current path to path set
                        Path path(visited.begin(), visited.begin() + size);

                        pathSet.push_back(path);

                        // Remove only the last node for continued search (backtrack)
                        visited.erase( visited.begin() + hops);


                    }
                    else
                    {

                        instructionCallSite tempStruct;

                        tempStruct.functionName = function->getName();
                        tempStruct.function = function;
                        tempStruct.callInst = NULL;

                        //visited.push_back(func); REMOVE
                        visited.push_back( tempStruct );

                        // Get hop count for this path
                        const int size = (int) visited.size();
                        const int hops = size - 1;

                        OtherFunction(pathSet, visited, end, indirectCallerList);

                        visited.erase( visited.begin() + hops);
                    }

                    //errs()<<"	Name of caller: "<<(*func)->getName()<<"\n";
                }
            }

        }
    }






}

/**

    A simple check as to whether the passed in function resides in the passed in path.

**/

bool CallGraphWrapper::ContainsFunction( std::vector<Function*> functionList, Function* func)
{
    for (std::vector<Function*>::iterator I = functionList.begin();
         I != functionList.end();
         I++)
    {
        Function* funcNode = (*I);
        if(funcNode->getName() == func->getName()) return true;
    }


    return false;
}


bool CallGraphWrapper::isOnPath( Path& functionStructs, Function* func)
{
    for (std::vector<instructionCallSite>::iterator I = functionStructs.begin();
         I != functionStructs.end();
         I++)
    {
        Function* funcNode = (*I).function;
        if(funcNode->getName() == func->getName()) return true;
    }


    return false;
}



/**

    This function will create a mapping from all callee's to possible indirect caller's.

    This is a two step process. First go through all functions in the program and initialize a struct
    representing a callee for that function.

    Then, we traverse the call graph searching for all indirect calls for each function. For each indirect
    call add the caller to the respective callee's list of possible indirect callers within its
    respective callee structure.

    Return the list of callee structs at the end.

**/
void CallGraphWrapper::createReverseMapping(IndCallerList* indirectCallerList)
{

    //IndCallerList newList;
    // Initialize a struct to hold indirect callers for each function and add to vector.

    for (scc_iterator<CallGraph*> I = scc_begin(CG); !I.isAtEnd(); ++I)

    {
        const std::vector<CallGraphNode *> &SCC = *I;
        for (unsigned i = 0, e = SCC.size(); i != e; ++i)
        {


            if (Function *F = SCC[i]->getFunction()) {
                IndCallerforCallee tempStruct;
                tempStruct.name = F->getName();
                //errs()<<"Function name of current callee: "<<tempStruct.name<<"\n";

                indirectCallerList->push_back(tempStruct);                                                 /////////////////////FIX ME//////////
                //errs()<<newList.size()<<"\n";
            }
        }
    }

    //errs()<<"Created Structs \n";

    // Traverse through all callgraph nodes searching for indirect calls. For each indirect call
    // add the caller to the callee's list of known indirect callers
    for (scc_iterator<CallGraph*> I = scc_begin(CG); !I.isAtEnd(); ++I)

    {
        const std::vector<CallGraphNode *> &SCC = *I;

        for (unsigned i = 0, e = SCC.size(); i != e; ++i)

        {

            CallGraphNode *node = SCC[i];

            if (Function *Func = (node)->getFunction()) {

                //errs()<<"\nIndirect call function: "<<(node)->getFunction()->getName()<<"\n";
                vector<CallSite> callSitesinFunc = FunctiontoIndCalls[Func];
                for(vector<CallSite>::iterator csIt = callSitesinFunc.begin();
                    csIt != callSitesinFunc.end();
                    ++csIt)
                {

                    // Based on the indirect mapping, get the target functions
                    std::vector<Function*> calledF = IndMap[*csIt];
                    for(std::vector<Function*>::iterator fs = calledF.begin();
                        fs != calledF.end();
                        ++fs)
                    {

                        // Any target in this loop is an indirect function.
                        // For each function add the caller to the callee's list of known indirect callers.

                        // First find the struct for the callee.

                        for(IndCallerList::iterator currCallee = indirectCallerList->begin();
                            currCallee != indirectCallerList->end();
                            ++currCallee)
                        {

                            if( (*fs)->getName() == (*currCallee).name )
                            {
                                if(!ContainsFunction((*currCallee).functionList, Func))
                                {
                                    //	errs()<<"Name of current caller: "<<Func->getName()<<"Name of current callee: "<<(*currCallee).name<<"\n";
                                    (*currCallee).functionList.push_back(Func);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //return indirectCallerList;

}


PathSet CallGraphWrapper::getCallPaths()
{
    //return callPathSets;
    return callPathsAll;
}

void CallGraphWrapper::printContextSensitiveCalls(std::vector<Function*> functionList)
{


    ofstream output_fo;
    output_fo.open ("context-sensitive-calls.out");

    // Output a list of all context sensitive calls to a file.
    for (std::vector<Function*>::const_iterator func = functionList.begin();
         func != functionList.end();
         func++)
    {

        output_fo <<"Function: ";
        const std::string s = (*func)->getName();
        output_fo <<s;
        output_fo <<"\n";
        for (Function::iterator BB = (*func)->begin(), endBB = (*func)->end(); BB != endBB; ++BB) {

            for (BasicBlock::iterator I = BB ->begin(), endI = BB->end(); I != endI; ++I){

                if (CallInst* callInst = dyn_cast<CallInst>(&*I)) {


                    if (callInst->getCalledFunction() != NULL)
                    {
                        const std::string ss = callInst->getCalledFunction()->getName();
                        output_fo << " Calling: ";
                        output_fo << ss;
                        output_fo << "\n";
                    }
                }
            }
        }
        output_fo <<"\n";
    }

    output_fo.close();

}


void CallGraphWrapper::printContextSensitivePaths(PathSet pathSets)
{

    ofstream output_fo;
    output_fo.open ("context-sensitive-paths.out");


    //callPathsAll
    //pathSets.



    for (std::vector<Path>::iterator I = pathSets.begin();
         I != pathSets.end();
         I++)
    {
        Path currPath = (*I);
        // = (*I);
        Path currPathnew;
        std::reverse(currPath.begin(), currPath.end());

        output_fo<<"( ";

        for(std::vector<instructionCallSite>::const_iterator callSite = currPath.begin(); callSite != currPath.end(); callSite++)
        {

            Function* func = (*callSite).function;
            currPathnew.push_back(*callSite);

            const std::string funcName = (*callSite).functionName;
            output_fo<<funcName;
            output_fo<<" ( ";

            for (Function::iterator BB = func->begin(), endBB = func->end(); BB != endBB; ++BB) {

                for (BasicBlock::iterator I = BB ->begin(), endI = BB->end(); I != endI; ++I){

                    if (CallInst* callInst = dyn_cast<CallInst>(&*I)) {

                        if (callInst->getCalledFunction() != NULL)
                        {
                            if (callInst != (*callSite).callInst)
                            {
                                //NT: Added the function in the call path for processing.
                                instructionCallSite newCall;
                                newCall.callInst = callInst;
                                newCall.function = callInst->getCalledFunction();
                                newCall.functionName = callInst->getCalledFunction()->getName();
                                currPathnew.push_back(newCall);

                                const std::string ss = callInst->getCalledFunction()->getName();
                                output_fo << ss;
                                output_fo << " ";
                            }

                            else {
                                break;
                            }
                        }
                    }
                }
            }

            output_fo<<") ";
        }

        callPathsAll.push_back(currPathnew);
        output_fo <<")\n";

    }

    output_fo.close();

}						


void CallGraphWrapper::toDot(CallGraphNode* RootNode, CallGraph CG, string fileName)
{
    std::string ErrorInfo;
    sys::fs::OpenFlags Flags;
    std::error_code errCode;

    nodeCount =0;
    raw_fd_ostream File(fileName.c_str(), errCode,Flags);

    if (!ErrorInfo.empty()) {
        errs() << "Error opening file " << fileName
               << " for writing! Error Info: " << ErrorInfo << " \n";
        return;
    }

    this->toDot(RootNode,CG, &File);
}


std::string CallGraphWrapper::getNodeLabel(CallGraphNode *Node) {
    if (Function *Func = Node->getFunction())
        return Func->getName();

    return "external node";
}


void CallGraphWrapper::toDot(CallGraphNode* RootNode, CallGraph CG, raw_ostream *stream)
{
    (*stream) << "digraph \"CG for \'" << "test file" << "\'  \"{\n";
    (*stream) << "label=\"CG for \'" << "test file" << "\' \";\n";

    std::map<CallGraphNode*, int> DefinedNodes;
    std::map<int,CallGraphNode*> NodeMap;

    // set<pair<int,int> > processed;
    map<int,int> processed;
    set<pair<CallGraphNode*,int> > Worklist;
    Worklist.insert(pair<CallGraphNode*,int>(RootNode,-1));
    vector<pair<WeakVH,CallGraphNode*> > CallRecords;
    int currNode;


    while(!Worklist.empty())
    {
        //Define the callGrpah node in the file..
        // get the called functions.. if we found possible targets then get those instead of external..

        set<pair<CallGraphNode*,int> >::iterator wlItem = Worklist.begin();
        // wlItem.
        // pair<CallGraphNode*,int>
        CallGraphNode * node = (*wlItem).first;

        //If not already added in the file add the node in dot file.
        if (DefinedNodes.count(node) == 0) {
            (*stream) << "Node_"<<nodeCount << "[shape=round"  << ",style=solid" << ",label=\"" << getNodeLabel((node))<< "\"]\n";
            DefinedNodes[node] = nodeCount;
            NodeMap[nodeCount] = node;
            currNode = nodeCount;

            nodeCount++;
            //    errs()<<"\n\nAdded Node..: "<<getNodeLabel((node));

            if (Function *Func = (node)->getFunction())
            {
                // errs()<<"\n Function : "<<node->getFunction()->getName();
                for(vector<pair<WeakVH,CallGraphNode*> >::iterator crIt = (node)->begin();crIt != (node)->end();++crIt)
                {
                    // if(CallSite cs = dyn_cast<CallInst>(*crIt).first)
                    {
                        //found call siet..
                    }
                    //  errs()<<"\n In the iterator";
                    if(CallGraphNode* cgNode = (*crIt).second)
                        Worklist.insert(pair<CallGraphNode*,int>(cgNode,currNode));
                }

                //If the function contains any indirect callsites the add the targets to worklist..
                vector<CallSite> callSitesinFunc = FunctiontoIndCalls[Func];
                for(vector<CallSite>::iterator csIt = callSitesinFunc.begin(); csIt != callSitesinFunc.end(); ++csIt)
                {
                    //get the target functions from indMAp
                    std::vector<Function*> calledF = IndMap[*csIt];
                    for(std::vector<Function*>::iterator fs = calledF.begin();fs != calledF.end();++fs)
                    {
                        //For each called function get/create the corresponding callgraph node and add in worklist
                        errs()<<"\n"<<(*fs)->getName();
                        CallGraphNode* cgNode = getDefinedCallGraphNode(*fs);
                        if(cgNode)
                        {
                            Worklist.insert(pair<CallGraphNode*,int>(cgNode,currNode));
                        }

                    }
                }


            }
        }


        //Add edges to parents:
        int parent = (*wlItem).second;
        //int currNode  = nodeCount-1;
        currNode = DefinedNodes[node];

        if(NodeMap.count(parent)!=0 && (parent!=currNode) && (processed[parent]!=currNode))
        {
            // errs()<<"\n adding edge to parent";
            processed[parent] = currNode;
            // CallGraphNode* parentNode =
            (*stream) << "\"" << "Node_"<<parent << "\"";

            (*stream) << "->";

            //Destination
            (*stream) << "\"" << "Node_"<<currNode  << "\"";
            (*stream) << "\n";

        }
        //      errs()<<"\nNode has size...: "<<(node)->size();




        Worklist.erase(wlItem);
    }

    (*stream) << "}\n\n";
}



CallGraphNode * CallGraphWrapper::getDefinedCallGraphNode(Function * F)
{
    //IndTargetNodes;
    //Find it in callgraph for the program..
    for (scc_iterator<CallGraph*> I = scc_begin(CG); !I.isAtEnd(); ++I) {
        const std::vector<CallGraphNode *> &SCC = *I;


        for (unsigned i = 0, e = SCC.size(); i != e; ++i) {
            Function *Func = SCC[i]->getFunction();
            if (Func) {
                if(Func==F)
                    return SCC[i];
            }
        }
    }
    //if not then check in created nodes..

    for(set<CallGraphNode*>::iterator cgIter = IndTargetNodes.begin();cgIter!=IndTargetNodes.end();++cgIter)
    {
        Function *Func = (*cgIter)->getFunction();
        if(Func)
        {
            if(Func==F)
                return *cgIter;
        }
    }
    //if not then create new node here.
    CallGraphNode* cgNode = new CallGraphNode(F);
    IndTargetNodes.insert(cgNode);
    return cgNode;

}


std::vector<Function*> CallGraphWrapper::getPossibleCallees(CallSite cs)
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

        //Function* f;
        Type * retType = (*fs)->getReturnType();

        // cs.isReturnNonNull()

        //  errs()<<"\nType Vals--------------------\n";
        //raw_ostream *func_stream;
        //funcType->print(func_stream);
        //string fstring;
        //fstring<<func_stream;


        //int n1 =
        if(PointerType *pt = dyn_cast<PointerType>(retType))
        {
            pt->getElementType();
        }
        int numParams = (*fs)->arg_size();
        int numCall = cs.arg_size();
        // if(funcType==callType)
        if(numParams==numCall)
        {
            if(retType->isVoidTy() && cs.doesNotReturn())
            {
                errs()<<"\n\n ...VOID return!!";
            }
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




std::map<CallSite, std::vector<Function*> >  CallGraphWrapper::getIndirectMap()
{
    return IndMap;
}




void CallGraphWrapper::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<CallGraphWrapperPass> ();
    AU.addRequired<TDDataStructures> ();
    AU.addRequired<dsa::CallTargetFinder<TDDataStructures> > ();
    // CallGraphSCCPass::getAnalysisUsage(AU);
    //  AU.addRequired<BottomUpPass> ();

}

char CallGraphWrapper::ID = 0;


static RegisterPass<CallGraphWrapper> X("callGraphFP2", "Function pointer augmented Call graph");






void BottomUpPass::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<CallGraphWrapperPass> ();
    //  CallGraphSCCPass::getAnalysisUsage(AU);

}

char BottomUpPass::ID = 0;
static RegisterPass<BottomUpPass> B("bottomUp", "Function pointer augmented Call graph");


