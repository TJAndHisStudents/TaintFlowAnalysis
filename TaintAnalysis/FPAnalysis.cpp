#include "FPAnalysis.h"

using namespace llvm;

FPAnalysis::FPAnalysis() :
	ModulePass(ID) {
}

bool FPAnalysis::runOnModule(Module &M) {
	
	InputDep &IV = getAnalysis<InputDep> ();
	inputDepValues = IV.getInputDepValues();
	sourceTypes = IV.getSourceTypes();
	
	
	//Check if the uses list for each input dep---do it recursively.. 	
	//and update the user type info in sources.
	//for(std::set<SourceType*>::iterator st = sourceTypes.begin(); st!=sourceTypes.end();++st)
	//{
	for(std::set<Value*>::iterator tv = inputDepValues.begin(); tv != inputDepValues.end(); ++tv)
    {

	//	errs()<<"\nUses for: "<<st->structName<<"->"<<st->fieldName;
		errs()<<"\nUses for : " <<**tv;
		for (Value::use_iterator UI = (*tv)->use_begin(), E = (*tv)->use_end(); UI != E; ++UI) {
			errs()<<"\nUse : "<< **UI;
			GetUses(*UI);
			
		}
	}
	//}
	
		
	for (Module::iterator F = M.begin(), eM = M.end(); F != eM; ++F) {
		for (Function::iterator BB = F->begin(), e = F->end(); BB != e; ++BB) {
			for (BasicBlock::iterator I = BB->begin(), ee = BB->end(); I != ee; ++I) {
			    
			    //Check if the type of the statement matches any of the types in
			    // the sourcetypes.. then add in tainted.
			      std::string buff;
		         std::string type_str;
				 raw_string_ostream rso(type_str); // = new raw_string_ostream(&type_str);

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
							 inputDepValues.insert(pval);
								if (MDNode *mdn = I->getMetadata("dbg")) {
									//NumInputValues++;
									DILocation Loc(mdn);
									unsigned Line = Loc.getLineNumber();
									lineNo[pval] = Line;
								}
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
			}
		}
	return false;
}


void FPAnalysis::GetUses(Value* v)
{
	for (Value::use_iterator UI = v->use_begin(), E = v->use_end(); UI != E; ++UI) {
			errs()<<"\nUse : "<< **UI;
			GetUses(*UI);
		}

}


void FPAnalysis::getAnalysisUsage(AnalysisUsage &AU) const {
	AU.setPreservesAll();
 	AU.addRequired<InputDep> ();

}

std::set<Value*> FPAnalysis::getSSOs() {
	return SSO;
}


std::set<BasicBlock*> FPAnalysis::getDefaultHooks(){
	return defaultHooks;
}


char FPAnalysis::ID = 0;
static RegisterPass<FPAnalysis> F_Analysis("fpAnalysis", "Function pointer Anlaysis",true,true);
