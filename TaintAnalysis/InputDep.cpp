#define DEBUG_TYPE "inputdep"
#include "InputDep.h"

STATISTIC(NumInputValues, "The number of input values");
STATISTIC(NumStores, "The number of store instructions in the program.");
STATISTIC(UniqueStores,"Number of stores to unique mem Location");

#define debug true

InputDep::InputDep() :
	ModulePass(ID) {
}


SourceType::SourceType()
{

}

TaintSource::TaintSource()
{

}

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


cl::opt<std::string> SourceFile("taintSource", cl::desc("<Taint Source file>"), cl::init("-"));
cl::opt<std::string> LookupFile("lookup", cl::desc("<Lookup file>"), cl::init("-"));

cl::opt<std::string> fpFile("fp", cl::desc("<FP Function targets file>"), cl::init("-"));


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
bool InputDep::runOnModule(Module &M) {
	//	DEBUG (errs() << "Function " << F.getName() << "\n";);
	NumInputValues = 0;
    NumStores = 0;
    UniqueStores = 0;
	bool inserted;
	Function* main = M.getFunction("main");
	if (main) {
		MDNode *mdn = main->begin()->begin()->getMetadata("dbg");
		for (Function::arg_iterator Arg = main->arg_begin(), aEnd =
				main->arg_end(); Arg != aEnd; Arg++) {
			inputDepValues.insert(Arg);
			NumInputValues++;
			if (mdn) {
				DILocation Loc(mdn);
				unsigned Line = Loc.getLineNumber();
				lineNo[Arg] = Line-1; //educated guess (can only get line numbers from insts, suppose main is declared one line before 1st inst
			}
		}
	}
	
	//Read the input files:
    // ReadSources();
	 ReadTargets();
	 ReadFPTargets();
     ReadTaintInput();
//     ReadRelevantFields();

     errs() << "Taint Inputs REad: " << taintSources.size();

    set<Value*> storedVals;
    set<Value*> repeatVals;

    //Taint Global Vars;


    //errs()<<"\n\n ******* Processing globals..";
    for(std::set<TaintSource*>::iterator ts = taintSources.begin();ts != taintSources.end(); ++ts)
    {
        //  std::string funcName= F->getName();
        // errs()<<"\n Processing globals.. entry: " << (*ts)->functionName;
        std::string glob = "global";
        if(std::strcmp(glob.c_str(),(*ts)->functionName.c_str())==0)
        {
           // errs()<<"\n\n *******Op Name from file.. .. for gloabl " <<(*ts)->variable;
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

                if(opName==(*ts)->variable)
                {
                    // errs()<<"\nOp Name "<<  opName <<" and " <<(*ts)->variable;
                    inputDepValues.insert(GV_Val);

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
                            NumInputValues++;

                        }
                        else if(isa<LoadInst>(*GV_UI))
                        {
                         //    errs()<<"\n Is loaded from a ptr.. ";
                             LoadInst * LI = dyn_cast<LoadInst>(*GV_UI);
                           //  Value* assignedVal = SI->getValueOperand();
                            // Value* pointerOperand = LI->get
                             inputDepValues.insert(LI);
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
                            NumInputValues++;

                        }
                        else
                        {
                            //all other instructions.. Apply taint to the result of the statement.
                          //   errs()<<"\n Is used in statement.. ";
                            // (*GV_UI)->dump();
                            Value * UserVal = (*GV_UI);
                            inputDepValues.insert(UserVal);
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
            } //iterate over gloabls.
        } //if global
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

                //Portion to add input sources for stores and loads for functions pointing to :
                /*
                 * std::string buff, buff2;
                 *  raw_string_ostream rso(type_str); // = new raw_string_ostream(&type_str);
                raw_string_ostream rso2(type_str2);
                std::string type_str,type_str2;
                 if(StoreInst *SI = dyn_cast<StoreInst>(I))
			        {
			        //	I->dump();
			        	Value* val = SI->getValueOperand();
			        	Value* pval = SI->getPointerOperand();
			        	val->print(rso);
			        	buff = rso.str();	
			        	pval->print(rso2);
			        	buff2 = rso2.str();
			        	
			         //Check for each function in the fpPointer File for tainting source.	
			         for(std::set<std::string>::iterator t = targetNames.begin(), en =
									targetNames.end(); t != en; ++t)
						 {
						  std::string fp_Name(*t);
						  std::size_t found = buff.find(fp_Name);
						   std::size_t found2 = buff2.find(fp_Name);
						  
						  if (found!=std::string::npos)
							{
							 SourceType * sType = new SourceType();
							 errs() << "\n\n  Assignment Found in Store: "<<*t; 
						//	 inputDepValues.insert(val);						 
							 inputDepValues.insert(pval);
							 sType->fieldVal = pval;
								if (MDNode *mdn = I->getMetadata("dbg")) {
									NumInputValues++;
									DILocation Loc(mdn);
									unsigned Line = Loc.getLineNumber();
									lineNo[pval] = Line;
								}
						//	errs() << "\n Pointer Var Name : " <<pval->getName();
							sType->fieldName = pval->getName();
							Type * ty = pval->getType();
							sType->fieldType = ty;
					//		errs() << "\n Pointer Type : " <<*ty;
							if (GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(&*pval)) {
								Value* firstOperand = gep->getOperand(0);
								Type* type = firstOperand->getType();

								// Figure out whether the first operand points to an array
								if (PointerType *pointerType = dyn_cast<PointerType>(type)) {
									Type* elementType = pointerType->getElementType();
	
									if (elementType->isArrayTy()) {					
										errs() << "\n --- points to an array!\n";
									//	errs()<< "First op  ..: "<< firstOperand <<"  full def :";
										gep->dump();
										elementType->dump();
									}
									else if(elementType->isStructTy ()) {
										errs() << "\n-------Is a Structure Field !";
										//errs()<< "First op  ..: "<< firstOperand <<"  full def :";
										errs()<<"\nStruct Name     : "<<elementType->getStructName();
										sType->structName =elementType->getStructName();
										errs()<<"\nStructure type  : ";
										elementType->dump();
										sType->structType = elementType;
										sType->structVal = firstOperand;
										//Also add the containing struct to the inputdep types...!!
										 inputDepValues.insert(firstOperand);
											if (MDNode *mdn = I->getMetadata("dbg")) {
												NumInputValues++;
												DILocation Loc(mdn);
												unsigned Line = Loc.getLineNumber();
												lineNo[pval] = Line;
											}
									}
								}
						   }
						   //Add the Found element in the set..
						   sourceTypes.insert(sType);
						   
						   }
						else if(found2!=std::string::npos)
						{
							 SourceType * sType = new SourceType();
							 errs() << "\n\n  Assignment Found in store 2: "<<*t; 
						//	 inputDepValues.insert(val);						 
							 inputDepValues.insert(val);
							 sType->fieldVal = pval;
								if (MDNode *mdn = I->getMetadata("dbg")) {
									NumInputValues++;
									DILocation Loc(mdn);
									unsigned Line = Loc.getLineNumber();
									lineNo[val] = Line;
								}
							inputDepValues.insert(pval);
							if (MDNode *mdn = I->getMetadata("dbg")) {
									NumInputValues++;
									DILocation Loc(mdn);
									unsigned Line = Loc.getLineNumber();
									lineNo[pval] = Line;
								}
						//	errs() << "\n Pointer Var Name : " <<val->getName();
							sType->fieldName = val->getName();
							Type * ty = val->getType();
							sType->fieldType = ty;
						//	errs() << "\n Pointer Type : " <<*ty;
							if (GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(&*val)) {
								Value* firstOperand = gep->getOperand(0);
								Type* type = firstOperand->getType();

								// Figure out whether the first operand points to an array
								if (PointerType *pointerType = dyn_cast<PointerType>(type)) {
									Type* elementType = pointerType->getElementType();
	
									if (elementType->isArrayTy()) {					
										errs() << "\n --- points to an array!\n";
									//	errs()<< "First op  ..: "<< firstOperand <<"  full def :";
										gep->dump();
										elementType->dump();
									}
									else if(elementType->isStructTy ()) {
										errs() << "\n-------Is a Structure Field !";
										//errs()<< "First op  ..: "<< firstOperand <<"  full def :";
										errs()<<"\nStruct Name     : "<<elementType->getStructName();
										sType->structName =elementType->getStructName();
										errs()<<"\nStructure type  : ";
										elementType->dump();
										sType->structType = elementType;
										sType->structVal = firstOperand;
										//Also add the containing struct to the inputdep types...!!
										 inputDepValues.insert(firstOperand);
											if (MDNode *mdn = I->getMetadata("dbg")) {
												NumInputValues++;
												DILocation Loc(mdn);
												unsigned Line = Loc.getLineNumber();
												lineNo[pval] = Line;
											}
									}
								}
						   }
						   //Add the Found element in the set..
						   sourceTypes.insert(sType);
						
						}
						   
					   }
			        }
			        
			        
			     if(LoadInst *LI = dyn_cast<LoadInst>(I))
			     	{
			     	//  errs()<<"\n\n--------Load instruction: " << *I;
			        	Value* pval = LI->getPointerOperand();
			        	pval->print(rso);
			        	buff = rso.str();
			        	bool structAvail = false;
			        	
			         //Check for each function in the fpPointer File for tainting source.	
			         for(std::set<std::string>::iterator t = targetNames.begin(), en =
									targetNames.end(); t != en; ++t)
						 {
						  std::string fp_Name(*t);
						  std::size_t found = buff.find(fp_Name);
						
						//  errs()<<"\nLoad var" <<buff;
						  if (found!=std::string::npos)
							{
							 SourceType * sType = new SourceType();
							 errs() << "\n\n  Assignment Found in Load : "<<*t; 
						//	 inputDepValues.insert(val);						 
							 inputDepValues.insert(I);
							 sType->fieldVal = pval;
								if (MDNode *mdn = I->getMetadata("dbg")) {
									NumInputValues++;
									DILocation Loc(mdn);
									unsigned Line = Loc.getLineNumber();
									lineNo[I] = Line;
								}
							//errs() << "\n Pointer Var Name : " <<pval->getName();
							sType->fieldName = pval->getName();
							Type * ty = pval->getType();
							sType->fieldType = ty;
							//errs() << "\n Pointer Type : " <<*ty;
							if (GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(&*pval)) {
								Value* firstOperand = gep->getOperand(0);
								Type* type = firstOperand->getType();

								// Figure out whether the first operand points to an array
								if (PointerType *pointerType = dyn_cast<PointerType>(type)) {
									Type* elementType = pointerType->getElementType();
	
									if (elementType->isArrayTy()) {					
										errs() << "\n --- points to an array!\n";
									//	errs()<< "First op  ..: "<< firstOperand <<"  full def :";
									//	gep->dump();
								//		elementType->dump();
									}
									else if(elementType->isStructTy ()) {
									   structAvail = true;
										errs() << "\n-------Is a Structure Field !";
										//errs()<< "First op  ..: "<< firstOperand <<"  full def :";
										errs()<<"\nStruct Name     : "<<elementType->getStructName();
										sType->structName =elementType->getStructName();
										errs()<<"\nStructure type  : ";
										elementType->dump();
										sType->structType = elementType;
										sType->structVal = firstOperand;
										//Also add the containing struct to the inputdep types...!!
										 inputDepValues.insert(firstOperand);
											if (MDNode *mdn = I->getMetadata("dbg")) {
												NumInputValues++;
												DILocation Loc(mdn);
												unsigned Line = Loc.getLineNumber();
												lineNo[pval] = Line;
											}
									}
								}
						   }
						   //Add the Found element in the set..
						   if(!structAvail)
						   {
						   	 Type *tp;
						   	 sType->structType = tp;
						   }
						   sourceTypes.insert(sType);
						   
						   }
						}
			     	   
			     	}
			        
                */
					
                //Map the input sources from the input files: function and variables:
                for(std::set<TaintSource*>::iterator ts = taintSources.begin();ts != taintSources.end(); ++ts)
                {
                    std::string funcName= F->getName();
                    if(strcmp(funcName.c_str(),(*ts)->functionName.c_str())==0)
                      {
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
                           // errs()<<"\nOp Name "<<  opName <<" and " <<(*ts)->variable;
                            if(opName==(*ts)->variable)
                            {
                                inputDepValues.insert(operand);
                             //   ListAllUses(operand,F);
                                if (MDNode *mdn = I->getMetadata("dbg")) {
                                    NumInputValues++;
                                    DILocation Loc(mdn);
                                    unsigned Line = Loc.getLineNumber();
                                    lineNo[operand] = Line;
                                }

                               // errs()<<"\nVar map found "<<opName;
                                //errs()<<"  Name " <<I->getName();


                           }
                        }
                       //  std::cin>>temp;
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


						if (Name.equals("main")) {
							errs() << "main\n";
							V = CI->getArgOperand(1); //char* argv[]
							inputDepValues.insert(V);
							inserted = true;
                            if(debug) errs() << "Input  main args  " << *V << "\n";
                             if(debug) errs() << "In Function  " <<F->getName()<<"\n";
						}

                        //GEtting the return value from socket call as the input..!!
                        if (Name.equals("socket")) {
                            inputDepValues.insert(CI);
                            inserted = true;
                            if(debug) errs() << "Input fgetc,getchar   " << *CI << "\n";
                             if(debug) errs() << "In Function  " <<F->getName()<<"\n";
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
							if (MDNode *mdn = I->getMetadata("dbg")) {
								NumInputValues++;
								DILocation Loc(mdn);
								unsigned Line = Loc.getLineNumber();
								lineNo[V] = Line;
							}
						}
					}
				}

               // */
            }
		}
		
	}

/*
    for(set<Value*>::iterator repVal = storedVals.begin();repVal != storedVals.end(); ++repVal)
    {
        errs()<<"\nStored Val :" << (**repVal);
    }
    */

    DEBUG(printer());
	printer();
	return false;
}


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
                    if (MDNode *mdn = I->getMetadata("dbg")) {
                        NumInputValues++;
                        DILocation Loc(mdn);
                        unsigned Line = Loc.getLineNumber();
                       // lineNo[operand] = Line;
                        errs()<<"  Line Number : "<< Line;
                    }
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
	errs() << "==================================\n";
}

void InputDep::ReadSources(){

    std::ifstream inFile (SourceFile.c_str(), std::ifstream::in);
    std::string file_Data,line;
    if(!inFile)
    {
        errs() << " Could not open the file \n";
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
	        errs() << " Could not open the file \n";
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
	        errs() << " Could not open the file \n";
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
            errs() << " Could not open the taint Input file \n";
        }
        else
        {
            while(srcFile >> line)
            {
                line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
                TaintSource * ts = new TaintSource();
                ts->functionName = line;
                if(srcFile >> line)
                { line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
                ts->variable = line;
                taintSources.insert(ts);
                }
                //.insert(line);
            }
        }
}




void InputDep::getAnalysisUsage(AnalysisUsage &AU) const {
	AU.setPreservesAll();
}

std::set<Value*> InputDep::getInputDepValues() {
	return inputDepValues;
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


char InputDep::ID = 0;
static RegisterPass<InputDep> X("inputDep",
		"Input Dependency Pass: looks for values that are input-dependant");
