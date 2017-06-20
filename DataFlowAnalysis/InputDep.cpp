#define DEBUG_TYPE "inputdep"
#include "InputDep.h"

STATISTIC(NumInputValues, "The number of input values");
STATISTIC(NumStores, "The number of store instructions in the program.");
STATISTIC(UniqueStores,"Number of stores to unique mem Location");

#define debug false
#define defaultInput false
#define globalStrings true

//Command prompt options:
cl::opt<std::string> SourceFile("taintSource", cl::desc("<Taint Source file>"), cl::init("-"));
cl::opt<std::string> LookupFile("lookup", cl::desc("<Lookup file>"), cl::init("-"));

cl::opt<std::string> fpFile("fp", cl::desc("<FP Function targets file>"), cl::init("-"));
cl::opt<std::string> mediatorFile("mediator", cl::desc("<Mediator Source file>"), cl::init("-"));
cl::opt<std::string> sinkFile("sinkFile", cl::desc("<sink function file>"), cl::init("-"));
cl::opt<std::string> queriesFile("query", cl::desc("<Query input file>"), cl::init("-"));

cl::opt<std::string> ConfigFile("configFile", cl::desc("<Config file for taint analysis tool>"), cl::init("-"));

cl::opt<bool> consString("consString",cl::desc("Set to compare constant strings with inputs."),cl::init("-"));

cl::opt<std::string> extTaintFile("extTaint", cl::desc("<External Taint file>"), cl::init("-"));

//TODO: Verify signature

/*
 * Main args are always input
 * Functions currently considered as input functions:
 * scanf
 * fscanf
 * gets
 * fgets
 * fread
 * fgetc
 * getc
 * getchar
 * recv
 * recvmsg
 * read
 * recvfrom
 * fread
 */





// get type after n'th index in getelementptr //adding temp
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





bool InputDep::runOnModule(Module &M) {
    //	DEBUG (errs() << "Function " << F.getName() << "\n";);
    NumInputValues = 0;
    NumStores = 0;
    UniqueStores = 0;
    bool inserted;
    //config = new ConfigInfo();
    Function* main = M.getFunction("main");

    //Add the default command prompt arguments from maina as input depepndent values
    if(defaultInput)
    {
        if (main) {
            MDNode *mdn = main->begin()->begin()->getMetadata("dbg");

            for (Function::arg_iterator Arg = main->arg_begin(), aEnd =
                 main->arg_end(); Arg != aEnd; Arg++) {
                inputDepValues.insert(Arg);
                ValLabelmap[Arg] = (*Arg).getName();
                NumInputValues++;
                if (DILocation *Loc = main->begin()->begin()->getDebugLoc()) { // Here I is an LLVM instruction
                    unsigned Line = Loc->getLine();
                    lineNo[Arg] = Line-1;
                }

//                if (mdn) {
//                    DILocation Loc(mdn);
//                    unsigned Line = Loc.getLineNumber();
//                    lineNo[Arg] = Line-1; //educated guess (can only get line numbers from insts, suppose main is declared one line before 1st inst
//                }
            }
        }
    }

    //Read the input files:
    // ReadSources();
    ReadTargets();
    ReadFPTargets();
    ReadTaintInput();
    ReadMediatorInput();
    ReadSinkInput();
    ReadQueryInput();
    ReadConfigFile();
    //ReadRelevantFields();

    errs()<<"\nRead queries: ";
    for(set<QueryInput*>::iterator qit =queryInputs.begin();qit!=queryInputs.end();++qit)
    {
        errs()<<"\n\tOperation : "<<(*qit)->operation;
        set<string> labels = (*qit)->labels;
        for(set<string>::iterator label = labels.begin();label != labels.end();++label)
            errs()<<"  "<<*label;
    }

    errs() << "\n\nTaint Inputs: " << taintSources.size();

    set<Value*> storedVals;
    set<Value*> repeatVals;
    set<pair<StructType*, int> > allStructs;

    //Taint Global Vars;


    // errs()<<"\n\n ******* TaintSource Size :.."<<taintSources.size();
    //    for(std::set<TaintSource*>::iterator ts = taintSources.begin();ts != taintSources.end(); ++ts)
    //    {
    //        errs()<<"\n\n Function :"<<(*ts)->functionName<<" var :  "<<(*ts)->variable<<"  label : "<<(*ts)->label;
    //    }


    for(std::set<TaintSource*>::iterator ts = taintSources.begin();ts != taintSources.end(); ++ts)
    {
        //  std::string funcName= F->getName();
        // errs()<<"\n Processing globals.. entry: " << (*ts)->functionName;
        std::string glob = "global";
        //   if(std::strcmp(glob.c_str(),(*ts)->functionName.c_str())==0)
        {
            //       errs()<<"\n\n *******Op Name from file.. .. for gloabl " <<(*ts)->variable;
            for (Module::global_iterator GVI = M.global_begin(), E = M.global_end();
                 GVI != E; ) {
                GlobalVariable *GV = GVI++;
                // Global variables without names cannot be referenced outside this module.
                if (GV->hasName())
                {
                    //Process the GV to add in tainted vars..
                    Value* GV_Val = GV;
                    //  int temp;
                    //    Value* operand = I->getOperand(i);
                    std::string opName = GV->getName();

                    if(consString>10)
                    {
                        if(opName.find("str")!=string::npos)
                        {
                            string globLabel = "global_"+opName;
                            inputDepValues.insert((GV_Val));
                            ValLabelmap[GV_Val] = globLabel;

                            for(GlobalValue::user_iterator GV_UI = GV->user_begin(); GV_UI != GV->user_end(); ++GV_UI)
                            {
                                // Value * GVUser = (*GV_UI);
                                //User *U = GV_   getUser();
                                //  (*GV_UI)->dump();
                                if(isa<StoreInst>(*GV_UI))
                                {
                                    //Handle appropriately..
                                    //    errs()<<"\n Is Stored..in a ptr.. ";
                                    StoreInst *SI = dyn_cast<StoreInst>(*GV_UI);
                                    //  Value* assignedVal = SI->getValueOperand();
                                    Value* pointerOperand = SI->getPointerOperand();
                                    inputDepValues.insert(pointerOperand);
                                    ValLabelmap[pointerOperand] = globLabel;
                                    NumInputValues++;

                                }
                                else if(isa<LoadInst>(*GV_UI))
                                {
                                    //    errs()<<"\n Is loaded from a ptr.. ";
                                    LoadInst * LI = dyn_cast<LoadInst>(*GV_UI);
                                    //  Value* assignedVal = SI->getValueOperand();
                                    // Value* pointerOperand = LI->get
                                    inputDepValues.insert(LI);
                                    ValLabelmap[LI] = globLabel;
                                    NumInputValues++;
                                }

                                else if (!isa<CallInst>(*GV_UI) && !isa<InvokeInst>(*GV_UI)) {
                                    if((*GV_UI)->use_empty())
                                        continue;
                                    //  errs()<<"\n Is used in call instruction..";
                                    //Todo: handle at the function def, as the param will be const and no lookup.
                                    //Quick fix.. taint call return val since global constants mostly used in lib calls.
                                    Value * UserVal = (*GV_UI);
                                    inputDepValues.insert(UserVal);
                                    ValLabelmap[UserVal] = globLabel;
                                    NumInputValues++;

                                }
                                else
                                {
                                    //all other instructions.. Apply taint to the result of the statement.
                                    //   errs()<<"\n Is used in statement.. ";
                                    // (*GV_UI)->dump();
                                    Value * UserVal = (*GV_UI);
                                    inputDepValues.insert(UserVal);
                                    ValLabelmap[UserVal] = (*ts)->label;
                                    NumInputValues++;

                                }

                            }


                        }
                    }
                    else
                    {
                        if(opName==(*ts)->variable)
                        {
                            // errs()<<"\nOp Name "<<  opName <<" and " <<(*ts)->variable;
                            inputDepValues.insert(GV_Val);
                            ValLabelmap[GV_Val] = (*ts)->label;

                            //Also add all the uses of the global in the tainted set..
                            //GV->use_begin();
                            // errs()<<"\n Getting all uses of the globals.. ";
                            for(GlobalValue::user_iterator GV_UI = GV->user_begin(); GV_UI != GV->user_end(); ++GV_UI)
                            {
                                // Value * GVUser = (*GV_UI);
                                //User *U = GV_   getUser();
                                (*GV_UI)->dump();
                                if(isa<StoreInst>(*GV_UI))
                                {
                                    //Handle appropriately..
                                    //    errs()<<"\n Is Stored..in a ptr.. ";
                                    StoreInst *SI = dyn_cast<StoreInst>(*GV_UI);
                                    //  Value* assignedVal = SI->getValueOperand();
                                    Value* pointerOperand = SI->getPointerOperand();
                                    inputDepValues.insert(pointerOperand);
                                    ValLabelmap[pointerOperand] = (*ts)->label;
                                    NumInputValues++;

                                }
                                else if(isa<LoadInst>(*GV_UI))
                                {
                                    //    errs()<<"\n Is loaded from a ptr.. ";
                                    LoadInst * LI = dyn_cast<LoadInst>(*GV_UI);
                                    //  Value* assignedVal = SI->getValueOperand();
                                    // Value* pointerOperand = LI->get
                                    inputDepValues.insert(LI);
                                    ValLabelmap[LI] = (*ts)->label;
                                    NumInputValues++;
                                }

                                else if (!isa<CallInst>(*GV_UI) && !isa<InvokeInst>(*GV_UI)) {
                                    if((*GV_UI)->use_empty())
                                        continue;
                                    //  errs()<<"\n Is used in call instruction..";
                                    //Todo: handle at the function def, as the param will be const and no lookup.
                                    //Quick fix.. taint call return val since global constants mostly used in lib calls.
                                    Value * UserVal = (*GV_UI);
                                    inputDepValues.insert(UserVal);
                                    ValLabelmap[UserVal] = (*ts)->label;
                                    NumInputValues++;

                                }
                                else
                                {
                                    //all other instructions.. Apply taint to the result of the statement.
                                    //   errs()<<"\n Is used in statement.. ";
                                    // (*GV_UI)->dump();
                                    Value * UserVal = (*GV_UI);
                                    inputDepValues.insert(UserVal);
                                    ValLabelmap[UserVal] = (*ts)->label;
                                    NumInputValues++;

                                }

                            }

                            //   ListAllUses(operand,F);

                            // GV_Val->
                            NumInputValues++;
                            //                    if (MDNode *mdn = I->getMetadata("dbg")) {

                            //                        DILocation Loc(mdn);
                            //                        unsigned Line = Loc.getLineNumber();
                            //                        lineNo[operand] = Line;
                            //                    }

                            // errs()<<"\nVar map found "<<opName;
                            //errs()<<"  Name " <<I->getName();
                        } //if match found.

                    }

                }
            } //iterate over gloabls.
        } //if global

        ///Get the type of the structure specified as input.

        std::string structString = "struct";
        if(std::strcmp(structString.c_str(),(*ts)->functionName.c_str())==0)
        {
            if(StructType* allStruct = M.getTypeByName("struct."+(*ts)->variable))
            {
                pair<StructType*, int> structfield;
                structfield.first = allStruct;
                structfield.second = (*ts)->fieldIndex;
                allStructs.insert(structfield);
                // errs()<<"\n";
                // allStruct->dump();
            }
        }


    } //iterate over taint sources.



    for (Module::iterator F = M.begin(), eM = M.end(); F != eM; ++F) {
        // errs()<<"\nFunction name: " << F->getName();
        // errs() << "\n---------------------\n";
        for (Function::iterator BB = F->begin(), e = F->end(); BB != e; ++BB) {
            // BB->dump();
            //   errs() << BB->getName() <<"\n";
            // errs() << "\n---------------------\n";

            for (BasicBlock::iterator I = BB->begin(), ee = BB->end(); I != ee; ++I) {

                //Quick test of the number of stores in a program;
                if(StoreInst *SI =dyn_cast<StoreInst>(I) )
                {
                    NumStores++;
                    Value * storePointer = SI->getPointerOperand();
                    int size = storedVals.size();
                    storedVals.insert(storePointer);
                    int size2 = storedVals.size();
                    if(size!=size2)
                    {
                        UniqueStores++;
                        //Can u check how many defs for the same variable"?
                    }
                    else
                    {
                        repeatVals.insert(storePointer);
                    }
                }

                //Map the input sources from the input files: function and variables:
                for(std::set<TaintSource*>::iterator ts = taintSources.begin();ts != taintSources.end(); ++ts)
                {
                    std::string funcName= F->getName();

                    if(strcmp(funcName.c_str(),(*ts)->functionName.c_str())==0)
                    {
                        //                  errs()<<"\nFunction Name "<<  funcName;
                        if(I->hasName())
                        {
                            std::string iName = I->getName();
                            if(iName==(*ts)->variable)
                            {
                                //  errs()<<"Match found for the declarations.." <<iName;
                                //  I->dump();
                                //Add the I in the input dep values.
                                //See if alloc instruction to value conversion possible.
                                // inputDepValues.insert((*I));

                            }
                        }

                        //If we need to add sources of wherever the operand is occuring.
                        int temp;
                        int opNum = I->getNumOperands();
                        for(int i =0;i<opNum;i++)
                        {
                            Value* operand = I->getOperand(i);
                            std::string opName = operand->getName();
                            //                          errs()<<"\nOp Name "<<  opName <<" and " <<(*ts)->variable;
                            if(opName==(*ts)->variable)
                            {
                                inputDepValues.insert(operand);
                                ValLabelmap[operand] = (*ts)->label;
                                //   ListAllUses(operand,F);
                                if (DILocation *Loc = I->getDebugLoc()) { // Here I is an LLVM instruction
                                    NumInputValues++;
                                    unsigned Line = Loc->getLine();
                                    lineNo[operand] = Line;
                                }


                                // errs()<<"\nVar map found "<<opName;
                                //errs()<<"  Name " <<I->getName();


                            }
                        }
                        //  std::cin>>temp;
                    }

                    ///If the input is structure type then find the instnce of the field and taint each instance.
                    string structstring = "struct";
                    if(strcmp(structstring.c_str(),(*ts)->functionName.c_str())==0)
                    {
                        if(GetElementPtrInst* GI = dyn_cast<GetElementPtrInst>(I))
                        {
                            Function * parentFunction = GI->getParent()->getParent();
                            Value* baseptr = GI->getOperand(0);
                            Type* ty = baseptr->getType();

                            if(ty->isPointerTy())
                            {
                                // ty->dump();
                                // errs()<<" ..elem ty-> ";
                                Type* structty = ty->getPointerElementType(); //->dump();
                                // ty->getPointerElementType()->dump();
                                // errs()<<"\n The baseptr is pointer to struct if true.. : "<<ty->getPointerElementType()->isStructTy()<<"\n";
                                // if(baseptr->hasName())
                                //    errs()<<"\n baseptr has name:  "<<baseptr->getName();
                                if(structty->isStructTy())
                                {
                                    for(set<pair<StructType*, int> >::iterator sourcest = allStructs.begin();sourcest != allStructs.end();++sourcest)
                                    {
                                        //                                        (*sourcest).first->dump();
                                        //                                        errs()<<"\n---comp to ---\n";
                                        //                                        ty->dump();
                                        //                                        StructType* structt = (*sourcest).first;
                                        //                                           string sourcestructName =structt->getName(); //getStructName();
                                        //                                        string structName = ty->getStructName();
                                        //   if((*sourcest).first->getStructName() == ty->getStructName())
                                        if((*sourcest).first == structty)
                                        {
                                            int index= -1;
                                            //   errs()<<"\n baseptr struct name:  "<<structty->getStructName();
                                            for (int i = GI->getNumOperands() - 2; i > 0; i--) {
                                                Type *ty = typeOfGetElementPtr(GI, i);
                                                assert(ty->isArrayTy() || ty->isStructTy());
                                                if (ty->isStructTy()) { // structure field
                                                    // t.ta_type = TAINT_FLD;
                                                    // t.ta_un.ta_fld.f_struct = ty;
                                                    index = cast<ConstantInt>
                                                            (GI->getOperand(i + 1))->getZExtValue();

                                                    //   errs()<<"\n\n At operand no: "<<i<<" with type : ";
                                                    //    ty->dump();
                                                    //   errs()<<" index is : "<<index;
                                                }
                                            }
                                            if(index!=-1 && index == (*sourcest).second)
                                            {
                                                errs()<<"\n]\nFound the Source field index...: in func:  "<<parentFunction->getName();
                                                GI->dump();

                                                inputDepValues.insert(GI);
                                                ValLabelmap[GI] = (*ts)->label;
                                                //   ListAllUses(operand,F);

                                                if (DILocation *Loc = I->getDebugLoc()) { // Here I is an LLVM instruction
                                                    NumInputValues++;
                                                    unsigned Line = Loc->getLine();
                                                    lineNo[GI] = Line;
                                                }
//                                                if (MDNode *mdn = I->getMetadata("dbg")) {
//                                                    NumInputValues++;
//                                                    DILocation Loc(mdn);
//                                                    unsigned Line = Loc.getLineNumber();
//                                                    lineNo[GI] = Line;
//                                                }


                                            }
                                        }
                                    }

                                }
                            }

                        }
                    }



                }

                // errs()<<"\n\nSource Type relations found : " << sourceTypes.size();

                //Getting input from the called functions, that are considered as input functions for C.

                // /*

                if (CallInst *CI = dyn_cast<CallInst>(I)) {
                    //  errs()<<"\n----Adding call inst in sources..:\n"; //------Call to foo de
                    // CI->dump();


                    // if(CI->getCalledFunction()==NULL)
                    // {         errs()<<"\n Indirect Call Found: in function: "<<  F->getName();;
                    //          CI->dump();
                    // }

                    //  errs() << "\n---------------------\n";

                    Function *Callee = CI->getCalledFunction();
                    if (Callee) {
                        Value* V;
                        inserted = false;
                        StringRef Name = Callee->getName();
                        //		errs()<<"\n\n Callee Function Name: " <<Name;

                        //Identify the target functions and add in target function set.
                        for(std::set<std::string>::iterator t = targetNames.begin(), en =
                            targetNames.end(); t != en; ++t)
                        {
                            // errs() << "Checking functions " << *t << "=="<< Name << "\n";
                            if(Name.compare(*t)==0){
                                targetFunctions.insert(CI);
                                targetBlocks.insert(BB);
                                if(debug) errs()<<" Inserted the function : "<< Callee->getName() <<"\n";
                            }
                        }


                        if(defaultInput)
                        {
                            if (Name.equals("main")) {
                                errs() << "main\n";
                                V = CI->getArgOperand(1); //char* argv[]
                                inputDepValues.insert(V);
                                ValLabelmap[V] = V->getName();
                                inserted = true;
                                if(debug) errs() << "Input  main args  " << *V << "\n";
                                if(debug) errs() << "In Function  " <<F->getName()<<"\n";
                            }

                            //GEtting the return value from socket call as the input..!!
                            if (Name.equals("socket")) {
                                inputDepValues.insert(CI);
                                ValLabelmap[CI] = CI->getName();
                                inserted = true;
                                if(debug) errs() << "Input fgetc,getchar   " << *CI << "\n";
                                if(debug) errs() << "In Function  " <<F->getName()<<"\n";
                            }
                        }


                        ///------------------------------------automatically check for some input functions as taint source..--------------
                        /*
                        if (Name.equals("__isoc99_scanf") || Name.equals(
                                "scanf")) {
                            for (unsigned i = 1, eee = CI->getNumArgOperands(); i
                                    != eee; ++i) { // skip format string (i=1)
                                V = CI->getArgOperand(i);
                                if (V->getType()->isPointerTy()) {
                                    inputDepValues.insert(V);
                                    inserted = true;
                                    if(debug) errs() << "Input scanf   " << *V << "\n";
                                    if(debug) errs() << "In Function  " <<F->getName()<<"\n";
                                }
                            }
                        } else if (Name.equals("__isoc99_fscanf")
                                || Name.equals("fscanf") || Name.equals("scanf")) {
                            for (unsigned i = 2, eee = CI->getNumArgOperands(); i
                                    != eee; ++i) { // skip file pointer and format string (i=1)
                                V = CI->getArgOperand(i);
                                if (V->getType()->isPointerTy()) {
                                    inputDepValues.insert(V);
                                    inserted = true;
                                    if(debug) errs() << "Input scanf   " << *V << "\n";
                                     if(debug) errs() << "In Function  " <<F->getName()<<"\n";
                                }
                            }
                        } else if ((Name.equals("gets") || Name.equals("fgets")
                                || Name.equals("fread"))
                                || Name.equals("getwd")
                                || Name.equals("getcwd")) {
                            V = CI->getArgOperand(0); //the first argument receives the input for these functions
                            if (V->getType()->isPointerTy()) {
                                inputDepValues.insert(V);
                                inserted = true;
                                if(debug) errs() << "Input gets,fread,getwd,getcwd   " << *V << "\n";
                                 if(debug) errs() << "In Function  " <<F->getName()<<"\n";
                            }
                        } else if ((Name.equals("fgetc") || Name.equals("getc")
                                || Name.equals("getchar"))) {
                            inputDepValues.insert(CI);
                            inserted = true;
                             if(debug) errs() << "Input fgetc,getchar   " << *CI << "\n";
                              if(debug) errs() << "In Function  " <<F->getName()<<"\n";
                        } else if (Name.equals("recv")
                                || Name.equals("recvmsg")
                                || Name.equals("read")) {
                            Value* V = CI->getArgOperand(1);
                            if (V->getType()->isPointerTy()) {
                                inputDepValues.insert(V);
                                inserted = true;
                                 if(debug) errs() << "Input recv,recvmsg,read   " << *V << "\n";
                                  if(debug) errs() << "In Function  " <<F->getName()<<"\n";
                            }
                        } else if (Name.equals("recvfrom")) {
                            V = CI->getArgOperand(1);
                            if (V->getType()->isPointerTy()) {
                                inputDepValues.insert(V);
                                inserted = true;
                                if(debug) errs() << "Input recvfrom    " << *V << "\n";
                                 if(debug) errs() << "In Function  " <<F->getName()<<"\n";
                            }
                            V = CI->getArgOperand(4);
                            if (V->getType()->isPointerTy()) {
                                inputDepValues.insert(V);
                                inserted = true;
                                if(debug) errs() << "Input    " << *V << "\n";
                                 if(debug) errs() << "In Function  " <<F->getName()<<"\n";
                            }
                        }
                        else if(Name.equals("foo")) {
                            errs()<<"------------Call to foo detected.---\n\n";
                        }
*/
                        ///------------------------------------automatically check for some input functions as taint source..--------------

                        if (inserted) {
                            if (DILocation *Loc = I->getDebugLoc()) { // Here I is an LLVM instruction
                                NumInputValues++;
                                unsigned Line = Loc->getLine();
                                lineNo[V] = Line;
                            }
//                            if (MDNode *mdn = I->getMetadata("dbg")) {
//                                NumInputValues++;
//                                DILocation Loc(mdn);
//                                unsigned Line = Loc.getLineNumber();
//                                lineNo[V] = Line;
//                            }
                        }
                    }
                }

                // */
            }
        }

    }



    //map labels to values for queries.
    for(set<QueryInput*>::iterator qit =queryInputs.begin();qit!=queryInputs.end();++qit)
    {
        //errs()<<"\n Operation : "<<(*qit)->operation;
        set<string> labels = (*qit)->labels;
        set<Value*> vals;
        for(set<string>::iterator labit = labels.begin();labit != labels.end();++labit)
        {
            for(std::map<Value*, std::string>::iterator valit =  ValLabelmap.begin(); valit != ValLabelmap.end();++valit)
            {
                string lab = (*valit).second;
                if(strcmp(lab.c_str(),(*labit).c_str())==0)
                {
                    vals.insert((*valit).first);
                    errs()<<"\n\t"<<lab<<" value :";
                    (*valit).first->dump();
                }
            }

        }
        (*qit)->labVals = vals;
    }

    DEBUG(printer());
    printer();
    return false;
}





///Functions to handle External Taint in libraries..
string InputDep::getProgramName(string progPath)
{
    //string progPath = externalTaint.programPath;
    progPath.erase( std::remove_if( progPath.begin(), progPath.end(), ::isspace ), progPath.end() );
    //errs()<<" \n Line :-  ",line;
    char delimiter = '/';
    string acc = "";
    for(int i = 0; i < progPath.size(); i++)
    {
        if(progPath[i] == delimiter)
        {
            acc = "";
        }
        else
            acc += progPath[i];
    }
    string progName = acc;
    // errs()<<"\n Program Name "<<progName;

    return progName;
}


//Function to read the taint coming from different modules.. ext calls, or returning taint or global taint.
void InputDep::ReadExtTaintInput(){
    //Read external calls taint..
    std::ifstream srcFile (extTaintFile.c_str(),std::ifstream::in);
    std::string line;
    if(!srcFile)
    {
        //errs() << " Could not open the external taint Input file \n";
    }
    else
    {
        while(srcFile >> line)
        {
            line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
            //errs()<<" \n Line :-  ",line;
            char delimiter = ',';
            vector<string> str;
            string acc = "";
            for(int i = 0; i < line.size(); i++)
            {
                if(line[i] == delimiter)
                {
                    str.push_back(acc);
                    acc = "";
                }
                else
                    acc += line[i];
            }
            str.push_back(acc);

            //Read the tokens in sequence and add in appropriate stuct..
            extTaint externalTaint;
            externalTaint.programPath = str.at(0);
            externalTaint.caller = str.at(1);
            externalTaint.callee = str.at(2);
            externalTaint.argNum = atoi(str.at(3).c_str());
            externalTaint.t_type = str.at(4);
            //externalTaint.taintLabel = str.at(5);


            //Get the program name only...
            string progName = getProgramName(externalTaint.programPath);
            externalTaint.program = progName;
            if(strcmp(externalTaint.t_type.c_str(),"ret")==0)
                externalTaint.taintLabel = str.at(5);
            else
                externalTaint.taintLabel = str.at(5) + "___"+progName;
            extTaint_set.push_back(externalTaint);

            //str.push_back(acc);
        }
    }
}


//Function that adds the new taint sources based on the external taint input...
void InputDep::AddExtTaint(Module &M)
{
    //iterate through the module.. when a match for the ext param is found


    /*1.	if caller not present then look for function, if present, add the taints on formal arguments of the externally called function. (taint in library)
      2.	If caller present then look for call to (external) function and add updated taint on the arguments. (returning taint on arguments....!)
      3.	If caller present and is of type - return type, look for call to function and add updated taint label to the ret val of the call stmt.(returning taint on ret val)
      4.	If caller not present and is return type…  (Invalid option)
    */

    for (Module::iterator F = M.begin(), E = M.end(); F != E; F++) {
        string funcName = F->getName();
        //check if caller or calleee.. both cannot be in the same module anyways..what happens when func with same name.. e.g main.!
        //For now process ext call... process returning taint later..
        for(list<extTaint>::iterator extT = extTaint_set.begin(); extT != extTaint_set.end();++extT)
        {
            if((strcmp(extT->t_type.c_str(),"ext")==0) && funcName==extT->callee)
            {
                //nt  errs()<<"\n ext match found...";
                //Record for outputting returning taints..
                pair<string,Function*> extCall;
                extCall.first = extT->caller;
                extCall.second = F;
                externallyCalled.push_back(extCall);

                Function::arg_iterator argptr;
                Function::arg_iterator e;
                unsigned i;

                for (i = 0, argptr = F->arg_begin(), e = F->arg_end(); argptr != e; ++i, ++argptr)
                {
                    if(i==extT->argNum)
                    {
                        Value *argValue = dyn_cast<Value>(argptr);
                        inputDepValues.insert(argValue);
                        ValLabelmap[argValue] = extT->taintLabel.c_str();
                    }
                }


            }
            else if((strcmp(extT->t_type.c_str(),"ret")==0) && funcName==extT->caller)
            {

                //Caller matched.. attach the returning taint on the call instruction
                //iterate over instructions to find the call stmt to the callee...
                //   errs()<<"\n Returning taint detected.. for caller :"<<funcName;
                int retArgnum = extT->argNum;
                for (inst_iterator I = inst_begin(F), J = inst_end(F); I != J; I++) {
                    if (CallInst *ci = dyn_cast<CallInst>(&*I)) {

                        ///Check if extrenal call with a pointer param, if so run the taint check for each taint on the param and write in the file.
                        //errs()<<"\nExternal call Check.....";
                        Function * calledFunc = ci->getCalledFunction();
                        if(calledFunc && calledFunc->isDeclaration())
                        {
                            if(calledFunc->getName()==extT->callee)
                            {
                                //callins to the ext call found.. add the returning taint here..
                                //     errs()<<"\n found the ext call to map ret edges.."<<retArgnum;
                                //    ci->dump();
                                if(retArgnum==-1)
                                {
                                    //Add taint label to return value.
                                    //      errs()<<"\n IDentifying taint for return value..";

                                    Value* retValue = dyn_cast<Value>(ci);
                                    inputDepValues.insert(retValue);
                                    ValLabelmap[retValue] = extT->taintLabel.c_str();


                                }
                                else
                                { //Add taint label to the argument.
                                    //   errs()<<"\n IDentifying taint for argument value..";
                                    int argNum = ci->getNumArgOperands();
                                    if(retArgnum<argNum)
                                    {
                                        Value* taintedArg = ci->getArgOperand(retArgnum);
                                        inputDepValues.insert(taintedArg);
                                        ValLabelmap[taintedArg] = extT->taintLabel.c_str();

                                    }

                                }


                            }

                        }
                    }
                } //end of inst iterator.
                //------
            }
        }
    }

}


/// End of functions to handle external Taint.





//Function to list all the uses of the tainted var found recursively:

void InputDep::ListAllUses(Value* Input, Function* F)
{
    int numUses = Input->getNumUses();
    int in;
    // errs()<<"Number of Uses : " << numUses;
    std::pair<std::set<Value*>::iterator,bool> ret;
    ret = inputUses.insert(Input);
    if(ret.second==false)
    {
        return;
    }

    for (Value::user_iterator UI = Input->user_begin(), E = Input->user_end(); UI != E; ++UI) {

        //    errs()<<"\nUse : "<< **UI;
        //Try to get the line number:
        if(Instruction *I = dyn_cast<Instruction>(*UI))
        {
            if (DILocation *Loc = I->getDebugLoc()) { // Here I is an LLVM instruction
                NumInputValues++;
                unsigned Line = Loc->getLine();
                errs()<<"  Line Number : "<< Line;
            }
//            if (MDNode *mdn = I->getMetadata("dbg")) {
//                NumInputValues++;
//                DILocation Loc(mdn);
//                unsigned Line = Loc.getLineNumber();
//                // lineNo[operand] = Line;
//                errs()<<"  Line Number : "<< Line;
//            }
        }
        //  errs()<<"  In function : "<<F->getName();

        //Process the call instruction...
        if(CallInst *call = dyn_cast<CallInst>(*UI))
        {
            if(call->getCalledFunction()==NULL)
            {
                errs()<<"\n Indirect call";
                errs()<<"\nUse : "<< **UI;
                errs()<<"  In function : "<<F->getName();
            }
            /*     else
                    {
                      errs()<<"\n It is a call inst..."<<call->getCalledFunction()->getName();
                      errs()<<"\nUse : "<< **UI;
                      errs()<<"  In function : "<<F->getName();
                      //Get the use and add that use and the function in it..
                       Function *func = call->getCalledFunction();
                       //store the function and param number and then in the function taint that paramt.
                        //.get uses iteratively.. add the taint label along witht the flow here.
                        Use *UsedParam = (*UI)->geto
                    } */

        }



        //   std::cin >> in;
        /*  if (LoadInst *load = dyn_cast<LoadInst>(*UI)) { // value from LOAD
                     Value *op = load->getOperand(0);
                     test = prepareTaint(op);
                   } else if (isa<Argument>(*UI)) { // value from argument
                     test = prepareTaint(*UI);
                   } else if (CallInst *ci = dyn_cast<CallInst>(*UI)) { // value from function ret
                     test = prepareTaint(ci->getCalledFunction());
                   } else if (isa<IntToPtrInst>(*UI) { // ...
                     return false;
                   } else {
                     errs() << "[ERROR] " << *val << "\n";
                     assert(false && "what is the value operand??");
                   } */
        ListAllUses(*UI, F);
        /*   Instruction *I =  dyn_cast<Instruction>(*UI);
                int opNum = I->getNumOperands();
                for(int i =0;i<opNum;i++)
                {
                    Value* operand = I->getOperand(i);
                    ListAllUses(operand);
                } */

        // errs()<<"   Instruction Val: " << (*(*UI)->getType());

    }


    //REcursivly keep adding in the set.. for one input.. and then return if the element already in the list..so don't go further types.


    //   for(int i =0;i<numUses;i++)
    // {
    //errs()<<"Used in : "<<Input->uses(i);
    // }
}



void InputDep::printer() {
    errs() << "\n\n===Input dependant values:====\n";
    for (std::set<Value*>::iterator i = inputDepValues.begin(), e =
         inputDepValues.end(); i != e; ++i) {
        errs() << **i << "\n";
    }
    errs() << "=============Target Functions==============\n";
    for (std::set<std::string>::iterator t = targetNames.begin(), en =
         targetNames.end(); t != en; ++t) {
        //			errs() << *t << "\n";
    }
    //errs() << "==================================\n";
}

void InputDep::ReadSources(){

    std::ifstream inFile (SourceFile.c_str(), std::ifstream::in);
    std::string file_Data,line;
    if(!inFile)
    {
        errs() << "Could not open the Sources file \n";
    }
    else
    {
        while (getline(inFile, line))
        {
            file_Data += line;
            errs() << line << "\n";
        }

    }
}


void InputDep::ReadTargets(){

    std::ifstream tgFile (LookupFile.c_str(), std::ifstream::in);
    std::string line;
    if(!tgFile)
    {
        errs() << "Could not open the targets file \n";
    }
    else
    {
        while (getline(tgFile, line))
        {
            line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
            targetNames.insert(line);
        }
    }
}

void InputDep::ReadFPTargets(){

    std::ifstream tgFile (fpFile.c_str(), std::ifstream::in);
    std::string line;
    if(!tgFile)
    {
        errs() << "Could not open the FP targets file \n";
    }
    else
    {
        while (getline(tgFile, line))
        {
            line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
            targetNames.insert(line);
        }
    }
}

void InputDep::ReadTaintInput(){

    std::ifstream srcFile (SourceFile.c_str(), std::ifstream::in);
    std::string line;
    if(!srcFile)
    {
        errs() << "Could not open the taint Input file \n";
    }
    else
    {
        while(srcFile >> line)
        {
            line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
            TaintSource * ts = new TaintSource();
            ts->functionName = line;
            if(line=="struct")
            {
                if(srcFile >> line)
                {
                    line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
                    ts->variable = line;
                    if(srcFile >> line)
                    {
                        line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
                        ts->fieldName = line;
                        ts->fieldIndex = atoi(line.c_str());
                        if(srcFile >> line)
                        {
                            line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
                            ts->label = line;
                            taintSources.insert(ts);
                        }
                        else
                        {
                            ts->label = ts->variable+ts->fieldName;
                            taintSources.insert(ts);
                        }

                    }
                }
            }
            else
            {
                if(srcFile >> line)
                {
                    line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
                    ts->variable = line;
                    if(srcFile >> line)
                    {
                        line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
                        ts->label = line;
                        taintSources.insert(ts);
                    }
                    else
                    {
                        ts->label = ts->variable;
                        taintSources.insert(ts);
                    }

                }
            }
            //.insert(line);
        }
    }
}



void InputDep::ReadConfigFile(){

    std::ifstream srcFile (ConfigFile.c_str(), std::ifstream::in);
    std::string line;
    if(!srcFile)
    {
        errs() << "Could not open the Config file \n";
    }
    else
    {
        while(srcFile >> line)
        {
            line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
            if(line==":")
            {
                //new config parameter:
                if(srcFile >> line)
                {

                    line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );

                    string key = line;
                    if(srcFile >> line)
                    {
                        line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
                        string value = line;

                        if(strcmp(key.c_str(),"Input")==0)
                        {
                            if(!value.empty() && value=="Source")
                                config.Source = true;
                            else
                                config.Source = false;
                        }
                        else if(key=="Context")
                        {
                            if(!value.empty())
                                if(value=="sensitive")
                                    config.ContextSensitive = true;
                                else
                                    config.ContextSensitive=false;
                        }
                        else if(key=="FlowSense")
                        {
                            if(!value.empty())
                                if(value=="true")
                                    config.full = true;
                                else
                                    config.full =false;
                        }
                        else if(key=="CallStrings")
                        {
                            if(!value.empty() && value=="true")
                                config.useCallString = true;
                            else
                                config.useCallString =false;
                        }
                    }
                }

            }
        }
    }
}


void InputDep::ReadMediatorInput(){

    std::ifstream srcFile (mediatorFile.c_str(), std::ifstream::in);
    std::string line;
    if(!srcFile)
    {
        errs() << "Could not open the Mediator Input file \n";
    }
    else
    {
        while(srcFile >> line)
        {
            line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
            mediatorFunctions.insert(line);
        }
    }
}


void InputDep::ReadSinkInput(){

    std::ifstream srcFile (sinkFile.c_str(), std::ifstream::in);
    std::string line;
    if(!srcFile)
    {
        errs() << "Could not open the Sink Input file \n";
    }
    else
    {
        while(srcFile >> line)
        {
            line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
            sinkFunctions.insert(line);
        }
    }
}

void InputDep::ReadQueryInput(){

    std::ifstream srcFile (queriesFile.c_str(), std::ifstream::in);
    std::string line;
    if(!srcFile)
    {
        errs() << "Could not open the query Input file \n";
    }
    else
    {
        set<string> labels;
        bool end = false;
        if(srcFile >> line)
        {
            while(!end)
            {

                line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
                //check if new record is to be created.
                if((strcmp(line.c_str(),"union")==0) || (strcmp(line.c_str(),"intersect")==0))
                {
                    QueryInput * qi = new QueryInput();
                    qi->constcheck = false;
                    labels.clear();
                    qi->operation = line;
                    end = true;

                    while(srcFile >> line)
                    {
                        line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
                        if((strcmp(line.c_str(),"union")!=0) && (strcmp(line.c_str(),"intersect")!=0))
                        {
                            labels.insert(line);
                            if(strcmp(line.c_str(),"constant")==0)
                                qi->constcheck = true;
                            //    qi->labels.insert(line);
                        }
                        else
                        {
                            end = false;
                            break;
                        }


                    }
                    qi->labels = labels;
                    queryInputs.insert(qi);
                    //.insert(line);
                }
            } //end while
        } //end if
    }
}


///Constructors for supplimentary classes:

InputDep::InputDep() :
    ModulePass(ID) {
}


SourceType::SourceType()
{

}

TaintSource::TaintSource()
{

}

QueryInput::QueryInput()
{

}

ConfigInfo::ConfigInfo()
{

}

///Functions for the suplimentary classes:

void TaintSource::Print()
{
    errs()<<"\n InputValues:in Function :  "<<functionName<< "   Variable " << variable;
}

void SourceType::Print()
{
    errs()<<"\n-------------------------------";
    errs()<<"\nField Name: "<<fieldName;
    errs()<<"\nField Type: "<<*fieldType;
    errs()<<"\nStruct Name: "<<structName;
    // if(structType != NULL)
    //   errs()<<"\nStruct Type: "<<*structType;
}




void InputDep::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
}

std::set<Value*> InputDep::getInputDepValues() {
    return inputDepValues;
}

std::set<std::string> InputDep::getMediators() {
    return mediatorFunctions;
}

std::set<std::string> InputDep::getSinks() {
    return sinkFunctions;
}

std::map<Value*,std::string> InputDep::getValueLabelMap()
{
    return ValLabelmap;
}

//getQueryVals

std::set<QueryInput*> InputDep::getQueryVals()
{
    return queryInputs;
}

std::set<std::string> InputDep::getTargetNames() {
    return targetNames;
}

std::set<CallInst*> InputDep::getTargetFunctions(){
    return targetFunctions;
}

std::set<BasicBlock*> InputDep::getTargetBlock(){
    return targetBlocks;
}

std::set<SourceType*> InputDep::getSourceTypes(){
    return sourceTypes;
}

ConfigInfo InputDep::getConfigInfo()
{
    return config;
}

char InputDep::ID = 0;
static RegisterPass<InputDep> X("inputDep",
                                "Input Dependency Pass: looks for values that are input-dependant");
