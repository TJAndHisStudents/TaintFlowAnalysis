#include "hookPlacement.h"

#define debug true

using namespace llvm;


hookPlacement::hookPlacement() :
	ModulePass(ID) {
}


template <typename T> static std::string ToString(const T *Obj) {
  std::string S;
  raw_string_ostream OS(S);
  Obj->print(OS);
  return OS.str();
}

bool hookPlacement::runOnModule(Module &M) {

	errs()<<"---getting from InputDep--------------\n";	
		InputDep &IV = getAnalysis<InputDep> ();
		inputDepValues = IV.getInputDepValues();
		targetFunctions = IV.getTargetFunctions();
		targetBlocks = IV.getTargetBlock();
		sourceTypes = IV.getSourceTypes();
		ControlDependenceGraphs  &CDGs = getAnalysis<ControlDependenceGraphs> ();
		//conDepGraph = CDGs.graphFor
	
	
	//Trial iteration for determining the type of the pointer .. 
	for (Module::iterator F = M.begin(), endF = M.end(); F != endF; ++F) {
		for (Function::iterator BB = F->begin(), endBB = F->end(); BB != endBB; ++BB) {
			for (BasicBlock::iterator I = BB->begin(), endI = BB->end(); I
					!= endI; ++I) {	
					errs()<<"\nAt Statement: " ;
					I->dump();		
					if (GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(&*I)) {
					// Dump the GEP instruction
					gep->dump();
					Value* firstOperand = gep->getOperand(0);
					Type* type = firstOperand->getType();


				    
        errs() << "Found GEP:\n";
        gep->dump();
        errs() << "  The type is: " << ToString(gep->getType()) << "\n";
        errs() << "  The pointer operand is: "
               << ToString(gep->getPointerOperand()) << "\n";
        errs() << "  Indices: ";
        for (auto Idx = gep->idx_begin(), IdxE = gep->idx_end(); Idx != IdxE;
             ++Idx) {
          errs() << "[" << ToString(Idx->get()) << "] ";
          }
    
    
					// Figure out whether the first operand points to an array
					if (PointerType *pointerType = dyn_cast<PointerType>(type)) {
						Type* elementType = pointerType->getElementType();
						errs() << "The element type is: " << *elementType << "\n";

						if (elementType->isArrayTy()) {
							errs() << "\n .. points to an array!   .....  "<<*firstOperand;
							SSO.insert(firstOperand);
							
						}
						if (elementType->isStructTy()) {
							errs() << "\n  .. points to an Srtruct!  ...... "<<*firstOperand;
							SSO.insert(firstOperand);
						}
					}
            }		
			}
		}
	}
	
	
	
	blockAssign &bssa = getAnalysis<blockAssign> ();
	depGraph = bssa.newGraph;
	//DEBUG( // display dependence graph
	string Error;
	std::string tmp = M.getModuleIdentifier();
	replace(tmp.begin(), tmp.end(), '\\', '_');
	std::string Filename = "/tmp/" + tmp + ".dot";

	//Print dependency graph (in dot format)
	depGraph->toDot(M.getModuleIdentifier(), "depGraphtest.dot");
	depGraph->toDotLines(M.getModuleIdentifier(), "depGraphtestlines.dot");
//	sys::Path Filename_path(Filename);
//	llvm::DisplayGraph(Filename_path, true, GraphProgram::DOT);
//	);
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

	Value* currentTaintVal = *tv;
	inputDepValuespart.insert(currentTaintVal);
	errs()<<"\n\nInputDep Values for "<<*currentTaintVal;
    tainted =  depGraph->getDepValues(inputDepValuespart);
    
   
    
    std::set<GraphNode *>::iterator G;
    std::set<GraphNode *>::iterator endG;
    for (G = tainted.begin(), endG = tainted.end(); G != endG; ++G)
      {
       //  errs()<<" The tainted graph node : "<<(*G)->getName()<<"\n";
      }
      
    errs()<<" \n \n";
    


	//DEBUG( // If debug mode is enabled, add metadata to easily identify tainted values in the llvm IR
	for (Module::iterator F = M.begin(), endF = M.end(); F != endF; ++F) {
		//Temporary take cdg for each func.. then modify it to take cdg for whole module.
		conDepGraphBase = CDGs.graphFor(&*F);
		for (Function::iterator BB = F->begin(), endBB = F->end(); BB != endBB; ++BB) {
			for (BasicBlock::iterator I = BB->begin(), endI = BB->end(); I
					!= endI; ++I) {
				GraphNode* g = depGraph->findNode(I);
				if (tainted.count(g)) {
					LLVMContext& C = I->getContext();
					MDNode* N = MDNode::get(C, MDString::get(C, "TaintFlow"));
				   // char numstr[21]; // enough to hold all numbers up to 64-bits
					//sprintf(numstr, "%d", inputDepVal_count);
					std::string taintVal = "TAINT_"+appendVal ;
					errs()<<"\nTainted Val " << *I <<"  val: " << appendVal;
					//std::string taintVal = std::strcat("tainted",numstr);
					I->setMetadata(taintVal, N);
					
					
					if (GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(&*I)) {
					// Dump the GEP instruction
				//	gep->dump();
					Value* firstOperand = gep->getOperand(0);
					Type* type = firstOperand->getType();

					// Figure out whether the first operand points to an array
					if (PointerType *pointerType = dyn_cast<PointerType>(type)) {
						Type* elementType = pointerType->getElementType();
					//	errs() << "The element type is: " << *elementType << "\n";

						if (elementType->isArrayTy()) {
						//	errs() << "\n .. points to an array!   .....  "<<*firstOperand;
							SSO.insert(firstOperand);
							
							//Generate a new operation info:
							SSOperations * ssOp = new SSOperations();
							ssOp->sObject = firstOperand;
							ssOp->objType = type;
							for (auto Idx = gep->idx_begin(), IdxE = gep->idx_end(); Idx != IdxE;
								 ++Idx) {
							  	ssOp->indexSet.push_back(Idx->get());
							  }
    						defaultHook *defHook = new defaultHook();
    						defHook->hookBlock = BB;
    						defHook->blockssOps.insert(ssOp);
    						defHooks.insert(defHook);
							//defaultHooks.insert(BB);
							
						}
						if (elementType->isStructTy()) {
						//	errs() << "\n  .. points to an Srtruct!  ...... "<<*firstOperand;
							SSO.insert(firstOperand);
							
							//Generate a new operation info:
							SSOperations * ssOp = new SSOperations();
							ssOp->sObject = firstOperand;
							ssOp->objType = type;
							for (auto Idx = gep->idx_begin(), IdxE = gep->idx_end(); Idx != IdxE;
								 ++Idx) {
							  	ssOp->indexSet.push_back(Idx->get());
							  }
    						defaultHook *defHook = new defaultHook();
    						defHook->hookBlock = BB;
    						defHook->blockssOps.insert(ssOp);
							defHooks.insert(defHook);
							//defaultHooks.insert(BB);
						}
					}
            }
					
	
					//for FPAnalysis: if the value is tainted then check for its type.. if in source fine.. else add.
					
				 
				/* for(std::set<SourceType*>::iterator st = sourceTypes.begin(); st!=sourceTypes.end();++st)
	 			{
					    std::string buff;
		         std::string type_str;
				 raw_string_ostream rso(type_str); // = new raw_string_ostream(&type_str);
				 bool add = false;
				 Type * taintTy = I->getType();
				 if(taintTy==(*st)->fieldType)
				 	{
				 //		errs()<<"\n\nMatch found for: "<<*I;
				 	}
				 else if(taintTy->isStructTy())
				 	{
				 	  //add in sources.
				 	 // SourceType sty = new SourceType();
				 	  if(debug)  errs()<<"\n\nIS Struct Type:";
				 	  //sourceTypes
				 	}
				  else if(taintTy->isPointerTy())
				 	{
				 	//   errs()<<"\n\nIS Pointer Type:";
				 	   
				 	    SourceType * sType = new SourceType();					 
						if(debug) errs() << "\n Pointer Var Name : " <<I->getName();
						sType->fieldName = I->getName();
						Type * ty = I->getType();
						sType->fieldType = ty;
						if(debug) errs() << "\n Pointer Type : " <<*ty;
						if (GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(&*I)) {
							Value* firstOperand = gep->getOperand(0);
							Type* type = firstOperand->getType();

							// Figure out whether the first operand points to an array
							if (PointerType *pointerType = dyn_cast<PointerType>(type)) {
								Type* elementType = pointerType->getElementType();

								if (elementType->isArrayTy()) {					
									if(debug) errs() << "\n --- points to an array!\n";
								//	errs()<< "First op  ..: "<< firstOperand <<"  full def :";
									if(debug) gep->dump();
									if(debug) elementType->dump();
								}
								else if(elementType->isStructTy ()) {
								   add = true;
									if(debug) errs() << "\n-------Is a Structure Field !";
									//errs()<< "First op  ..: "<< firstOperand <<"  full def :";
									if(debug) errs()<<"\nStruct Name     : "<<elementType->getStructName();
									sType->structName =elementType->getStructName();
									if(debug) errs()<<"\nStructure type  : ";
									if(debug) elementType->dump();
									sType->structType = elementType;
								}
							}
					   //Add the Found element in the set..
					   if(add)
					   	sourceTypesDerived.insert(sType);
					   }
				 	}
				   else if(taintTy->isArrayTy())
				 	{
				 		 if(debug) errs()<<"\n\nIS Araay Type:";
				 	  //add in sources.
				 	  //sourceTypes
				 	}


			     if(StoreInst *SI = dyn_cast<StoreInst>(I))
			        {
			        	Value* val = SI->getValueOperand();
			        	Value* pval = SI->getPointerOperand();
			        	val->print(rso);
			        	buff = rso.str();	
			         //Check for each function in the fpPointer File for tainting source.	
							 SourceType * sType = new SourceType();
							// errs() << "\n\n  Assignment Found : "<<*t; 
						//	 inputDepValues.insert(val);						 
							errs() << "\n Pointer Var Name : " <<pval->getName();
							sType->fieldName = pval->getName();
							Type * ty = pval->getType();
							sType->fieldType = ty;
							errs() << "\n Pointer Type : " <<*ty;
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
									}
								}
						   //Add the Found element in the set..
						//   sourceTypes.insert(sType);
						   
						   }
					   
			        } 
			    } 
			    */
							
				
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
	errs()<<"\nSource Types added  :"<<sourceTypes.size();
	
	errs()<<"\nSecurity Sensitive look up identified :"<<SSO.size();
	errs()<<"\n\n\n******************** SSO Identified  ***********************";
	for(std::set<Value*>::iterator ssot = SSO.begin(); ssot != SSO.end(); ++ssot)
	{
		errs()<<"\n"<<**ssot;
	}
	GenerateDefaultPlacement();
	
	errs()<<"\n\n\n******************** default placement blocks Identified  ***********************";
	errs()<<"\nDefault placement blocks identified :"<<defaultHooks.size();
	for(std::set<BasicBlock*>::iterator bbt = defaultHooks.begin(); bbt != defaultHooks.end(); ++bbt)
	{
		errs()<<"\n"<<(*bbt)->getName();
	}
	
	inputDepVal_count++;
	//appendVal = appendVal+ 1;
    
    } //closing the loop for inputdepVal

    //
  //  M.dump();

  //  getSinkSourceBlockDependence();
   // getSinkSourceDependence();
 //   getHookLocation();
    //testProcessing();

//	);
	GenControlDepHookNodes();
	errs()<<"\nDefault placement blocks from control dep:"<<controlDepHookNodes.size()<<"\n";

   for(std::set<ControlDependenceNode*>::iterator n = controlDepHookNodes.begin(); n != controlDepHookNodes.end(); ++n)
   {
   		if((*n) != NULL)
   		errs()<<"\nNode LAbel: "<<(*n)->getLabel();
   		else errs()<<"\nNode null";
   		//BasicBlock*  tb = (*n)->getBlock();
   		// if(tb != NULL)
//    			{
//    			  errs()<<"\n Block "<< tb->getName();
//    			}
//    		else
//    		{
//    			errs()<<"\n No Block ";
//    			
//    		}
	}



	return false;

}




void hookPlacement::GenerateDefaultPlacement()
{

for(std::set<Value*>::iterator ssot = SSO.begin(); ssot != SSO.end(); ++ssot)
	{
		//TODO:: update so that this can be populated outside the taint loop.
		//BasicBlock* bb;
		//bb = (*ssot)->getParent();
		//defaultHooks.insert(bb);
	}


}


void hookPlacement::GenControlDepHookNodes()
{
	
	for(std::set<BasicBlock*>::iterator bbt = defaultHooks.begin(); bbt != defaultHooks.end(); ++bbt)
	{
		errs()<<"\n inserting for block..-- "<<(*bbt)->getName();
		ControlDependenceNode  *node = conDepGraphBase.getNode(*bbt);
		if(node!=NULL)
			controlDepHookNodes.insert(node);
	}
}



void hookPlacement::getAnalysisUsage(AnalysisUsage &AU) const {
	AU.setPreservesAll();
	AU.addRequired<blockAssign> ();
	AU.addRequired<InputValues> ();
	AU.addRequired<InputDep> ();
	AU.addRequired<ControlDependenceGraphs> ();
}

std::set<Value*> hookPlacement::getSSOs() {
	return SSO;
}

std::set<Value*> hookPlacement::getLookups()
{
	return Lookups;

}


std::set<defaultHook*> hookPlacement::getDefaultHooks(){
	return defHooks;
}


char hookPlacement::ID = 0;
static RegisterPass<hookPlacement> hook("hookPlacement", "Hook Placement Pass",true,true);
