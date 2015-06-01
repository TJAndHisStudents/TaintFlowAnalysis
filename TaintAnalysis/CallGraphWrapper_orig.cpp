
#include "CallGraphWrapper.h"

#define debug false
using namespace llvm;

STATISTIC(NumFuncitons, "Number of functions");
STATISTIC(NumCalls, "Number of call sites");


static cl::opt<bool, false>
Level("fp-level", cl::desc("What heuristic to apply: 1. number of params, 2. number and type of params, ."), cl::NotHidden);

STATISTIC(DirCall, "Number of direct calls");
STATISTIC(IndCall, "Number of indirect calls");
STATISTIC(CompleteInd, "Number of complete indirect calls");
STATISTIC(CompleteEmpty, "Number of complete empty calls");


BottomUpPass::BottomUpPass():
    CallGraphSCCPass(ID) {

}


//CallGraphWrapperPass


CallGraphWrapper::CallGraphWrapper():
    ModulePass(ID) {

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
          errs()<<"\nCallGRaph node dump -- ";
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



///Function to determeine if the address of the given function/value has been taken or not.
/// This still needs to be verified properly.

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


bool CallGraphWrapper::hasAddressTaken(llvm::Function *F){
  return addressTaken.find(F) != addressTaken.end();
}



///The call graph warapper modules first gets the existing call graph.
/// It then computes the address taken functions
///
bool CallGraphWrapper::runOnModule(Module &M) {
  // CallGraphWrapperPass &WP = getAnalysis<CallGraphWrapperPass>();

    //CallGraph
            CG = &getAnalysis<CallGraphWrapperPass>().getCallGraph();


    //Collect all the address take funcitons in a collection.
    for (Module::iterator FI = M.begin(), FE = M.end(); FI != FE; ++FI){
        if(isAddressTaken(FI)) {
          addressTaken.insert(FI);
        }
    }

    DEBUG(errs()<<"Address Taken Functions: "<< addressTaken.size());

    for(std::set<Function*>::iterator FI = addressTaken.begin();FI!=addressTaken.end();++FI)
    {
        errs()<<"\nAddress Taken For: "<<(*FI)->getName();
        errs()<<" Type:  ";
        (*FI)->getType()->dump();
       // Function * f;

    }


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
           // errs()<<"\nFunction in call graph  :"<<F->getName();
            if(F->getName()=="main")
                RootNode =SCC[i];
        }
      }
    }

    errs()<<"\n Root Node ";

    //Start from the root node here and iterate over the call graph..
   // std::set<CallGraphNode> workList = RootNode->CalledFunctionsVector;

    //Array just to record calls to functions with diff number of arguments:
    int callto[10]= {0,0,0,0,0,0,0,0,0};


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

                  DEBUG(errs()<<"\n\n Called Value : ";
                          cs.getCalledValue()->dump();
                          errs()<<"Called Value stripped pointer cast : ";
                          calledValue->dump();
                          cs.getCalledValue()->getType()->dump();

                  );

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



      //Print REsolved Calls:

for(std::map<CallSite, std::vector<Function*> >::iterator mapIt = IndMap.begin(); mapIt != IndMap.end(); ++mapIt)
{
    errs()<<"\n";
    CallSite cs = (*mapIt).first;
     std::vector<Function*> calledF = (*mapIt).second;
   // errs()<<"\n---Targets for CallSite :  ";
   //  CallSite cs = (*callS);
   Instruction *ins = cs.getInstruction();
   Function *func = ins->getParent()->getParent();
     errs()<<"\n["<<func->getName()<<":0]  ";
    //cs.getCalledValue()->getType()->dump();

     //Add Info in FunctiontoIndCalls
     vector<CallSite> callSitesinFunc = FunctiontoIndCalls[func];
     callSitesinFunc.push_back(cs);
     FunctiontoIndCalls[func] = callSitesinFunc;


    cs.getInstruction()->dump();
    errs()<<"========";
    for(std::vector<Function*>::iterator fs = calledF.begin();fs != calledF.end();++fs)
    {
        errs()<<"\n"<<(*fs)->getName(); //<<"  Type  ";
       // (*fs)->getType()->dump();
    }

}


    errs()<<"\n\n Direct Calls  :"<<DirCall;
    errs()<<"\n\n Indirect Calls : "<<IndCall;


    for(int i=0;i<10;i++)
    {
        errs()<<"\nFor # arguments : "<<i<<" calls made "<< callto[i];
    }
 errs()<<"\n\n\n";

 toDot(RootNode,*CG,"../../CallGTest.dot");

 //Free the Call Graph

    return false;
}



void CallGraphWrapper::toDot(CallGraphNode* RootNode, CallGraph CG, string fileName)
{
           std::string ErrorInfo;
           sys::fs::OpenFlags Flags;

            nodeCount =0;
           raw_fd_ostream File(fileName.c_str(), ErrorInfo,Flags);

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
   // CallGraphSCCPass::getAnalysisUsage(AU);
  //  AU.addRequired<BottomUpPass> ();

}

char CallGraphWrapper::ID = 0;


static RegisterPass<CallGraphWrapper> S("callGraphFP", "Function pointer augmented Call graph");






void BottomUpPass::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<CallGraphWrapperPass> ();
  //  CallGraphSCCPass::getAnalysisUsage(AU);

}

char BottomUpPass::ID = 0;
static RegisterPass<BottomUpPass> B("bottomUp", "Function pointer augmented Call graph");


