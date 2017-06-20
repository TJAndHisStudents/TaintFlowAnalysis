#include "TaintFlow.h"
//#define DEBUG_TYPE "TaintFlow"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/GraphWriter.h"

STATISTIC(NumTaintedNodes, "The number of nodes marked as tainted in the dep graph");
STATISTIC(NumNodes, "The number of nodes in the dep graph");
STATISTIC(NumberofBlocks,"The total number of blocks in the code");


#define debug true

//Command line Prompt.
static cl::opt<llvm::cl::boolOrDefault>
Input(
        "full",
        cl::desc(
            "Specify if every value from libraries should be treated as tainted. Default is true."),
        cl::value_desc("Input description"));

static cl::opt<bool, false> printall("printall", cl::desc("Prints all query data"), cl::NotHidden);
static cl::opt<bool, false> printcond("printcond", cl::desc("Prints only conditionals"), cl::NotHidden);



bool PrintTaintedExtCalls = true;


TaintFlow::TaintFlow() :
    ModulePass(ID) {

}


bool TaintFlow::runOnModule(Module &M) {

    errs()<<"---getting from InputDep--------------\n";
    InputDep &IV = getAnalysis<InputDep> ();
    inputDepValues = IV.getInputDepValues();
    targetFunctions = IV.getTargetFunctions();
    targetBlocks = IV.getTargetBlock();
    sourceTypes = IV.getSourceTypes();
    ValLabelmap = IV.getValueLabelMap();
    mediatorFunctions = IV.getMediators();
    queryinputs = IV.getQueryVals();

    moduleDepGraph &DDG = getAnalysis<moduleDepGraph> ();


    depGraph = DDG.depGraph;   //bssa.newGraph;
    //DEBUG( // display dependence graph
    string Error;
    std::string tmp = M.getModuleIdentifier();
    replace(tmp.begin(), tmp.end(), '\\', '_');
    std::string Filename = "/tmp/" + tmp + ".dot";

    //Print dependency graph (in dot format)
    depGraph->toDot(M.getModuleIdentifier(), "depGraphtest.dot");
    //	depGraph->toDotLines(M.getModuleIdentifier(), "depGraphtestlines.dot");
    //	sys::Path Filename_path(Filename);
    //	llvm::DisplayGraph(Filename_path, true, GraphProgram::DOT);
    //	);

    int inputDepVal_count = 0;
    std::string appendVal ="a";
    std::set<Value*> inputDepValuespart;
    set<Instruction*> LoopupInsts;
    set<Value*> SecSensitiveObjs;
    errs()<<"----------------------Taint Flow Analysis-----------------\n";
    for(std::set<Value*>::iterator tv = inputDepValues.begin(); tv != inputDepValues.end(); ++tv)
    {

        int branches = 0;
        errs()<<**tv <<"\n";
        //std::string appendVal = (**tv).getName();

        inputDepValuespart.clear();
        errs()<<"----------------------Input deps above-----------------\n";
        //tainted = depGraph->getDepValues(inputDepValues);

        Value* currentTaintVal = *tv;
        std::string appendVal = ValLabelmap[currentTaintVal];
        inputDepValuespart.insert(currentTaintVal);
        errs()<<"\n\nInputDep Values for "<<*currentTaintVal;
        tainted =  depGraph->getDepValues(inputDepValuespart);

        //Graph * subGraph = depGraph->generateSubGraph(currentTaintVal,)


        //To print strings used with this taint...
        std::error_code ErrorInfo;
        bool writeStrings = true;
        string stringfileName = tmp+"_"+ appendVal + ".txt";

        raw_fd_ostream FileS(stringfileName.c_str(), ErrorInfo,sys::fs::F_None);
        if (ErrorInfo.value() != 0) {
            errs() << "Error opening file " << stringfileName
                   << " for writing! Error code: " << ErrorInfo.value() << " \n";
            writeStrings = false;
        }

        //To print the external functions which take the tainted values..
        string fileNameTaintOut = tmp + "_taintOut.txt";

        raw_fd_ostream FiletOut(fileNameTaintOut.c_str(), ErrorInfo,sys::fs::F_None);
        if (ErrorInfo.value() != 0) {
            errs() << "Error opening file " << fileNameTaintOut
                   << " for writing! Error code: " << ErrorInfo.value() << " \n";
            PrintTaintedExtCalls = false;
        }


        taintGraphMap[currentTaintVal] = tainted;

        NumTaintedNodes = tainted.size();
        NumNodes = depGraph->getNodes().size();
        errs()<<"\nNumber of tainted nodes: " <<NumTaintedNodes <<"\n";
        errs()<<"Number of total nodes updates: " <<NumNodes <<"\n\n";


        //  PrintTainted(tainted);
        errs()<<"\n---------------------\n";
        //TODO: Memory error in the insert... verify and solve...



        std::set<GraphNode *>::iterator G;
        std::set<GraphNode *>::iterator endG;
        //        for (G = tainted.begin(), endG = tainted.end(); G != endG; ++G)
        //        {
        //            //  errs()<<" The tainted graph node : "<<(*G)->getName()<<"\n";
        //        }

        // errs()<<" \n \n";




        //DEBUG( // If debug mode is enabled, add metadata to easily identify tainted values in the llvm IR
        for (Module::iterator F = M.begin(), endF = M.end(); F != endF; ++F) {
            for (Function::iterator BB = F->begin(), endBB = F->end(); BB != endBB; ++BB) {
                NumberofBlocks++;
                for (BasicBlock::iterator I = BB->begin(), endI = BB->end(); I
                     != endI; ++I) {

                    GraphNode* g = depGraph->findNode(I);
                    if (tainted.count(g)) {
                        g->taintSet.insert(appendVal);
                        if (BranchInst *BI = dyn_cast<BranchInst>(I))
                        {
                            //   errs()<<"Branch Inst tainted : ";
                            //  BI->dump();
                            branches++;
                        }
                        if(CallInst *CI = dyn_cast<CallInst>(I))
                        {

                            Function* func = CI->getCalledFunction();
                            if(func && !func->isDeclaration())
                            {


                                for(set<string>::iterator med = mediatorFunctions.begin(); med != mediatorFunctions.end();med++)
                                {
                                    string medfunc = (*med);
                                    string funcName = func->getName();

                                    if(strcmp(medfunc.c_str(),funcName.c_str())==0)
                                    {
                                        g->taintSet.erase(appendVal);
                                        errs()<<"\n+++++++++++ Med func  : function name :  "<<funcName;
                                        appendVal = appendVal+"_Med";
                                        g->taintSet.insert(appendVal);
                                    }
                                }
                            }

                        }
                        if (PrintTaintedExtCalls)
                        {
                            if(CallInst *ci = dyn_cast<CallInst>(I))
                            {
                                CallSite cs(ci);
                                Function * calledFunc = ci->getCalledFunction();
                                if(calledFunc && calledFunc->isDeclaration())
                                {
                                    //    errs()<<"\nPROPAGATE: Call Site with called func and IS delaration.. !!";
                                    //  CI->dump();
                                    // HandleLibraryFunctions(CI,parentFunction);
                                    //externalCalls.push_back(ci);
                                    //Get args and check taint...
                                    errs()<<"\n Tainted Ext call :"<<*ci;
                                    CallSite::arg_iterator argptr;
                                    CallSite::arg_iterator e;
                                    unsigned i;

                                    //Create the PHI nodes for the formal parameters
                                    for (i = 0, argptr = cs.arg_begin(), e = cs.arg_end(); argptr != e; ++i, ++argptr)
                                    {
                                        Value *argValue = argptr->get();

                                        if(Instruction *inst = dyn_cast<Instruction>(argValue))
                                        {

                                            GraphNode* g = depGraph->findInstNode(inst);

                                            if (tainted.count(g))
                                            {
                                                inst->dump();
                                                 FiletOut <<tmp<<","<< F->getName()<<","<<calledFunc->getName()<<","<<i<<","<<"ext"<<","<<appendVal<<"\n";
                                            }
                                        }
                                    }
                                    // for()
                                }

                            }
                        } //end if print tainted ext calls.



                        //Print all the lookup instructions with tainted values..:

                        if(GetElementPtrInst *GI = dyn_cast<GetElementPtrInst>(I))
                        {
                            moduleDepGraph* mdG = new  moduleDepGraph();
                            // mdG->CheckifRelevantField(GI,depGraph);
                            LoopupInsts.insert(I);
                            Value* sensitiveObject = GI->getPointerOperand();
                            SecSensitiveObjs.insert(sensitiveObject);
                        }

                        //check if any operand is global string..
                        if(writeStrings)
                        {
                            for (unsigned int i = 0; i < cast<User> (I)->getNumOperands(); i++)
                            {
                                Value *v1 = cast<User> (I)->getOperand(i);
                                if(isa<GlobalVariable> (v1))
                                {
                                    if(isa<Constant> (v1))
                                    {
                                        //string sName = v1->getName()
                                        v1->print(FileS);
                                        (FileS) << "\n";
                                    }
                                }
                            }
                        }

                        LLVMContext& C = I->getContext();
                        MDNode* N = MDNode::get(C, MDString::get(C, "TaintFlow"));
                        // char numstr[21]; // enough to hold all numbers up to 64-bits
                        //sprintf(numstr, "%d", inputDepVal_count);
                        //appendVal = ()
                        std::string taintVal = "TAINT_"+appendVal;
                        //    if(debug)	errs()<<"\nFunction : "<< F->getName()<<"  Tainted Val " << *I <<"  val: " << appendVal;

                        //For the given tainted Val try priniting all the uses of it.
                        // errs()<<"\nUsed in :: "<<I->getNumUses();

                        //  std::string taintVal = std::strcat("tainted",numstr);
                        I->setMetadata(taintVal, N);


                        //Print the line number info for the tainted line..:-
                        //                        if (MDNode *N = I->getMetadata("dbg")) {  // Here I is an LLVM instruction
                        //                          DILocation Loc(N);                      // DILocation is in DebugInfo.h
                        //                          unsigned Line = Loc.getLineNumber();
                        //                          StringRef File = Loc.getFilename();
                        //                          StringRef Dir = Loc.getDirectory();
                        //                          errs()<<"\n ^^^^^^^^^^^^^^^^^for inst   :";
                        //                          I->dump();
                        //                          errs()<<"\n Line number : "<<Line<<" File name : "<<File<<" Dir name : "<<Dir;
                        //                        }
                        //                        else
                        //                        {
                        //                            errs()<<"\n ~~~~~~~~~~~~~no info for inst   :";
                        //                            I->dump();
                        //                        }

                    }


                    /*	if (GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(&*I)) {
                    // Dump the GEP instruction
                //	gep->dump();
                    Value* firstOperand = gep->getOperand(0);
                    Type* type = firstOperand->getType();

                    // Figure out whether the first operand points to an array
                    if (PointerType *pointerType = dyn_cast<PointerType>(type)) {
                        Type* elementType = pointerType->getElementType();
                        //errs() << "The element type is: " << *elementType << "\n";

                        if (elementType->isArrayTy()) {

                            errs() << "\n  .. points to an array!\n";
                            errs()<< "First op  ..: "<< firstOperand <<"  full def :";
                            gep->dump();
                        }
                        else if(elementType->isStructTy ()) {
                            errs() << "\n  *** points to a Struct !\n";
                            errs()<< "First op  ..: "<< firstOperand <<"  full def :";
                            gep->dump();
                        }
                    }
               }  */
                }
            }
        }

        //Print how many source types added
        //errs()<<"\nSource Types added  :"<<sourceTypes.size();

        inputDepVal_count++;
        //appendVal = appendVal+ 1;
        errs()<<"\n Branch Inst tainted : "<<branches;
    } //closing the loop for inputdepVal

    errs()<<"\n\n --- Total Lookup Insts.------: "<<LoopupInsts.size()<<"\n";
    //    for(set<Instruction*>::iterator lookupVal = LoopupInsts.begin(); lookupVal != LoopupInsts.end();++lookupVal)
    //    {
    //        (*lookupVal)->dump();
    //        if(Instruction * ssoInst = dyn_cast<Instruction>(*lookupVal))
    //        {
    //            errs()<<" in func: "<<ssoInst->getParent()->getParent()->getName();
    //            DebugLoc Loc = ssoInst->getDebugLoc();
    //             // Here I is an LLVM instruction
    //              unsigned Line = Loc.getLine();
    //          //    StringRef File = Loc.getFilename();
    //           //   StringRef Dir = Loc.getDirectory();
    //              errs()<<"\nLine: "<<Line; //<<" File : "<<File<<"   Dir  :"<<Dir<<"\n";
    //              Loc.dump(M.getContext());

    //        }
    //    }

    errs()<<"\n\n **********************************--- \n\n";

    errs()<<"\n\n --- total sensitve objects .------: "<<SecSensitiveObjs.size()<<"\n";
    for(set<Value*>::iterator secVal = SecSensitiveObjs.begin(); secVal !=    SecSensitiveObjs.end();++secVal)
    {
        (*secVal)->dump();
        if(Instruction * ssoInst = dyn_cast<Instruction>(*secVal))
        {
            errs()<<" in func: "<<ssoInst->getParent()->getParent()->getName();
        }
    }

    updateTaintLabels();

    tainted =  depGraph->getDataDepValues(inputDepValues);

    //Add taint labels in metadata:
    for (Module::iterator F = M.begin(), endF = M.end(); F != endF; ++F) {
        for (Function::iterator BB = F->begin(), endBB = F->end(); BB != endBB; ++BB) {
            for (BasicBlock::iterator I = BB->begin(), endI = BB->end(); I
                 != endI; ++I) {
                GraphNode* g = depGraph->findInstNode(I);
                if (tainted.count(g)) {


                    //                    errs()<<"\n ----Label info for each value..:";
                    //                    I->dump();
                    //                    errs()<<" ===size:  "<<g->taintSet.size();
                    //                    for(set<string>::iterator label = g->taintSet.begin();label!=g->taintSet.end();++label)
                    //                    {
                    //                        errs()<<" "<<*label;
                    //                    }


                    LLVMContext& C = I->getContext();
                    MDNode* N = MDNode::get(C, MDString::get(C, "TaintFlow"));
                    for(set<string>::iterator label = g->taintSet.begin();label!=g->taintSet.end();++label)
                    {
                        std::string taintVal = *label;
                        I->setMetadata(taintVal, N);
                    }
                }
            }
        }
    }

    //
    //  M.dump();

    //  getSinkSourceBlockDependence();
    // getSinkSourceDependence();
    //   getHookLocation();
    //testProcessing();

    //	);



    HandleQueries(M);



    //errs()<<"\n\n\n\n*********  Source Types ******************";
    for(std::set<SourceType*>::iterator st = sourceTypes.begin(); st!=sourceTypes.end();++st)
    {
        (*st)->Print();
    }


    //errs()<<"\n\n\n\n*********  Derived Types ******************";
    for(std::set<SourceType*>::iterator st = sourceTypesDerived.begin(); st!=sourceTypesDerived.end();++st)
    {
        (*st)->Print();
    }



    for(std::set<Value*>::iterator tv = inputDepValues.begin(); tv != inputDepValues.end(); ++tv)
    {

        //errs()<<(*tv)->getName() <<"\n";

    }



    std::string mod_name = M.getModuleIdentifier();
    
    //errs() << "MOD IDENT " << mod_name << mod_name.find(".") << "\n";
    int period_loc = mod_name.find(".");
    std::string article_name = mod_name.substr(0, period_loc);
    std::string mediator_file = article_name + "_mediator.txt";

    //errs() << article_name << "\n";
    //errs() << mediator_file << "\n";

    std::list<string> mediator_list;

    ifstream fin;
    fin.open(mediator_file);
    if (!fin.good())
    {
        errs() << "\nMediator File not found...?\n";
        return false;
    }

    const int MAX_CHARS_PER_LINE = 512;
    const int MAX_TOKENS_PER_LINE = 20;
    const char* token[MAX_TOKENS_PER_LINE] = {};

    while (!fin.eof())
    {
        char buf[MAX_CHARS_PER_LINE];
        fin.getline(buf, MAX_CHARS_PER_LINE);
        //errs() << buf << "\n";
        if ((buf != "\n") and (buf != " "))
        {
            mediator_list.push_back(buf);
        }
    }
    
    ChopControlModule *CCM = &getAnalysis<ChopControlModule>();
    CCM->processChops(M, depGraph, inputDepValues, mediator_list);

    return false;
}




void TaintFlow::updateTaintLabels()
{
    set<GraphNode*> taintSourceNodes;
    //update base labels..
    errs()<<"\nInput dep node size :"<<inputDepValues.size()<<"\n";
    for(std::set<Value*>::iterator tv = inputDepValues.begin(); tv != inputDepValues.end(); ++tv)
    {
        //  string appendVal = NULL;
        Value* currentTaintVal = *tv;
        currentTaintVal->dump();
        string appendVal = ValLabelmap[currentTaintVal];
        GraphNode* sourceNode = depGraph->findNode(currentTaintVal);
        if(sourceNode!=NULL)
        {
            sourceNode->taintSet.insert(appendVal);
            taintSourceNodes.insert(sourceNode);

        }
        else
            errs()<<"\nNo graph node found for current val:";
        //   if(appendVal!=NULL)
        //   sourceNode->taintSet.insert(appendVal);
    }

    //traverse and propagate the taint labels forward from the base label.
    errs()<<"\nSource node size :"<<taintSourceNodes.size();
    for(set<GraphNode*>::iterator gnode = taintSourceNodes.begin(); gnode != taintSourceNodes.end();++gnode)
    {
        propagateLabel(*gnode);
    }

}


void TaintFlow::propagateLabel(GraphNode* snode)
{
    std::set<GraphNode*> visited;
    std::list<GraphNode*> worklist;
    std::map<GraphNode*, edgeType> neigh;
    std::set<string> parentTaintset;

    worklist.push_back(snode);
    //errs()<<"\n source node in worklist..:";

    while (!worklist.empty()) {
        GraphNode* n = worklist.front();
        // errs()<<"\n before successor......:";
        if(n!=NULL)
        {
            neigh = n->getSuccessors();
            //    errs()<<"\n Collect current node taint set....:";
            parentTaintset = n->taintSet;

            for (std::map<GraphNode*, edgeType>::iterator i = neigh.begin(), e =
                 neigh.end(); i != e; ++i) {
                GraphNode* nextNode = i->first;
                if (!visited.count(nextNode)) {
                    worklist.push_back(nextNode);
                    visited.insert(nextNode);

                    //   nextNode->taintSet.insert(parentTaintset.begin(),parentTaintset.end());

                }
            }

        }
        else
            errs()<<"\n Worklist front node is null..";

        worklist.pop_front();
    }

    //set merge:
    //s1.insert(s2.begin(), s2.end());
}

void TaintFlow::HandleQueries(Module& M)
{

    for(set<QueryInput*>::iterator qit = queryinputs.begin();qit!=queryinputs.end();++qit)
    {
        bool constCheck;
        errs()<<"\n\n\n\n*******************************************************Query ";
        errs()<<"\nOperation : "<<(*qit)->operation;
        errs()<<"\nConstCheck : "<<(*qit)->constcheck;
        string operation = (*qit)->operation;
        set<string> labels = (*qit)->labels;
        set<Value*> vals = (*qit)->labVals;
        std::set<GraphNode*> taintedA;
        std::set<GraphNode*> taintedB;
        std::set<GraphNode*> intersectGraph;

        std::set<Value*> taintedA_val;
        std::set<Value*> taintedB_val;
        std::set<Value*> intersectGraph_val;


        for(set<string>::iterator label = labels.begin();label != labels.end();++label)
            errs()<<" - "<<*label;

        errs()<<"\n Val size:"<<vals.size();
        for(set<Value*>::iterator val = vals.begin();val != vals.end();++val)
        {
            (*val)->dump();
        }
        errs()<<"\n*******************************************************\n ";

        constCheck = (*qit)->constcheck;
        if(constCheck)
        {
            for(set<Value*>::iterator val = vals.begin();val != vals.end();++val)
            {
                //              errs()<<"\n In cons check :";
                taintedA = taintGraphMap[*val];
            }
            //  if(taintedA!=NULL)
            //            intersectGraph = taintedA;
            intersectGraph.insert(taintedA.begin(),taintedA.end());
            intersectGraph_val = getTaintedVals(intersectGraph);
        }
        else
        {

            if(strcmp(operation.c_str(),"intersect")==0)
            {
                intersectGraph.clear();
                intersectGraph_val.clear();
                for(set<Value*>::iterator val = vals.begin();val != vals.end();++val)
                {
                    bool round2 = false;
                    if(val==vals.begin())
                    {

                        taintedA = taintGraphMap[*val];
                        taintedA_val = getTaintedVals(taintedA);
                        ++val;
                        if(val!=vals.end())
                        {
                            taintedB = taintGraphMap[*val];
                            taintedB_val = getTaintedVals(taintedB);
                        }

                    }
                    else
                    {
                        taintedA_val.clear();
                        taintedA_val.insert(intersectGraph_val.begin(),intersectGraph_val.end());
                        intersectGraph_val.clear();
                        taintedB = taintGraphMap[*val];
                        taintedB_val = getTaintedVals(taintedB);
                    }

                    if(taintedA_val.empty() || taintedB_val.empty())
                        errs()<<"\n Val graphs null..";
                    else
                    {
                        for(set<Value*>::iterator gnode = taintedB_val.begin();gnode != taintedB_val.end();++gnode)
                        {
                            if(taintedA_val.count(*gnode) > 0)
                            {
                                intersectGraph_val.insert((*gnode));
                            }

                        }
                    }
                }
            }
        }

        //       int branches =0;
        errs()<<"\n Intersect graph size :"<<intersectGraph_val.size();
        errs()<<"\n--------------------------";

        for(set<Value*>::iterator gnode = intersectGraph_val.begin();gnode != intersectGraph_val.end();++gnode)
        {
            errs()<<"\n Intersected tainted ";
            Value * val = (*gnode);
            if(isa<Instruction>(val))
            {
                Instruction * inst = dyn_cast<Instruction>(val);
                string funcName = inst->getParent()->getParent()->getName();
                errs()<<"\n Function: "<<funcName;
            }
            val->dump();
        }
        //Print appropriate vals....:
        //        for (Module::iterator F = M.begin(), endF = M.end(); F != endF; ++F) {
        //            string funcName = F->getName();
        //            //  errs()<<"\nTaints in function: "<<funcName;
        //            for (Function::iterator BB = F->begin(), endBB = F->end(); BB != endBB; ++BB) {
        //                string bbName ="noName";
        //                if(BB->hasName())
        //                {
        //                    bbName = BB->getName();
        //                }
        //                //  errs()<<" - block: "<<bbName;
        //                NumberofBlocks++;
        //                for (BasicBlock::iterator I = BB->begin(), endI = BB->end(); I
        //                     != endI; ++I) {
        //                    GraphNode* g = depGraph->findNode(I);
        //                    if (intersectGraph.count(g)) {
        //                        errs()<<"\n Node found..:";
        //                        I->dump();
        //                        if(printall)
        //                        {
        //                            errs()<<"Taint in function : "<<funcName<<"  :";
        //                            I->dump();
        //                        }
        //                        else if(printcond)
        //                        {
        //                            if (BranchInst *BI = dyn_cast<BranchInst>(I))
        //                            {
        //                                if(constCheck)
        //                                {
        //                                    Value* conditional = BI->getCondition();

        //                                    for (unsigned int i = 0; i < cast<User> (conditional)->getNumOperands(); i++)
        //                                    {
        //                                        Value *v1 = cast<User> (conditional)->getOperand(i);
        //                                        if(isa<ConstantInt>(v1))
        //                                        {
        //                                            errs()<<"Branch Inst tainted in func : "<<funcName<<"  :";
        //                                            BI->dump();
        //                                            branches++;
        //                                        }

        //                                    }

        //                                }
        //                                else
        //                                {
        //                                    //    BI->getCondition()->dump();
        //                                    errs()<<"Branch Inst tainted : ";
        //                                    BI->dump();
        //                                    branches++;
        //                                }
        //                            }
        //                        }
        //                    }
        //                }
        //            }
        //        }


        //        errs()<<"\n Number of conditionals tainted : " <<branches;
    }

    errs()<<"\n*******************************************************\n ";
}


///Function:
// This function will get the sink values from the input file,
//and check the source dependent path for each of the sinks.
void TaintFlow::getSinkSourceDependence(){

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


void TaintFlow::getSinkSourceBlockDependence(){

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


void TaintFlow::getHookLocation(){
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

//void TaintFlow::PrintHooks(){	}

void testProcessing(){
    // depGraph->toDot("SomeString","DepGraph6.dot");
}


set<Value*> TaintFlow::getTaintedVals(std::set<GraphNode*> tainted)
{
    set<Value*> taintedVals;

    for(set<GraphNode*>::iterator taintNode = tainted.begin();taintNode != tainted.end();++taintNode)
    {
        if(isa<MemNode>(*taintNode))
        {
            MemNode * memNew = dyn_cast<MemNode>(*taintNode);
            Value * val = memNew->defLocation.second;
            std::set<Value*> aliases; // = memNew->getValue();
            aliases.insert(memNew->getValue());

            if(val)
            {
                taintedVals.insert(val);
            }
            for(set<Value*>::iterator alVal = aliases.begin(); alVal != aliases.end();++alVal)
            {
                taintedVals.insert(*alVal);
            }
        }
        if(isa<VarNode>(*taintNode))
        {
            VarNode * varNew = dyn_cast<VarNode>(*taintNode);
            Value * val = varNew->getValue(); //->defLocation.second;
            if(val)
            {
                taintedVals.insert(val);
            }
        }
    }

    return taintedVals;
}


void TaintFlow::PrintTainted(std::set<GraphNode*> tainted)
{
    //  errs()<<"\n\n Tainted Nodes: "<<tainted.size();


    for(set<GraphNode*>::iterator taintNode = tainted.begin();taintNode != tainted.end();++taintNode)
    {
        //errs()<<"\n Node Label : "<<(*taintNode)->getLabel();
        // errs()<<"--";
        if(isa<MemNode>(*taintNode))
        {
            // errs()<<"\n is mem node";
            string nodeLab = (*taintNode)->getLabel();
            string str = "sub_42BC4";
            std::size_t found = nodeLab.find(str);
            // if (found!=std::string::npos || 1)
            {

                MemNode * memNew = dyn_cast<MemNode>(*taintNode);
                Value * val = memNew->defLocation.second;
                std::set<Value*> aliases;
                aliases.insert(memNew->getValue());

                if(val)
                {
                    errs()<<"\n Sink Node Tainted : "<<(*taintNode)->getLabel();
                    if(isa<Instruction>(val))
                    {
                        Instruction * inst = dyn_cast<Instruction>(val);
                        string funcName = inst->getParent()->getParent()->getName();
                        errs()<<"\n Function: "<<funcName;
                    }
                    val->dump();
                }
                if(aliases.size()>0)
                    errs()<<"\n Sink Node Tainted : "<<(*taintNode)->getLabel();
                for(set<Value*>::iterator alVal = aliases.begin(); alVal != aliases.end();++alVal)
                {
                    if(isa<Instruction>(*alVal))
                    {
                        Instruction * inst = dyn_cast<Instruction>(*alVal);
                        string funcName = inst->getParent()->getParent()->getName();
                        errs()<<"\n Function: "<<funcName;
                    }
                    (*alVal)->dump();
                }
            }

        }
        if(isa<VarNode>(*taintNode))
        {
            VarNode * varNew = dyn_cast<VarNode>(*taintNode);
            Value * val = varNew->getValue(); //->defLocation.second;
            if(val)
            {
                errs()<<"\n Sink Node Tainted : "<<(*taintNode)->getLabel();
                if(isa<Instruction>(val))
                {
                    Instruction * inst = dyn_cast<Instruction>(val);
                    string funcName = inst->getParent()->getParent()->getName();
                    errs()<<"\n Function: "<<funcName;
                }
                val->dump();
            }
        }
        //if
    }
}


bool TaintFlow::isValueTainted(Value* v) {
    GraphNode* g = depGraph->findNode(v);
    return tainted.count(g);
}

std::set<GraphNode*> TaintFlow::getTaintedValues() {
    return tainted;
}

void TaintFlow::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    //AU.addRequired<blockAssign> ();
    //  AU.addRequired<InputValues> ();
    AU.addRequired<moduleDepGraph> ();
    AU.addRequired<InputDep> ();
    //  AU.addRequired<DominatorTree> ();
    // AU.addRequired<AndersAA> ();
    //AU.addRequired<hookPlacement> ();
    AU.addRequired<ChopControlModule>();
}

char TaintFlow::ID = 0;
static RegisterPass<TaintFlow> P("TaintPDG", "Tainted Flow Analysis for Source Code");
