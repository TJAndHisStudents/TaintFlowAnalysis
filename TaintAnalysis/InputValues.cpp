/*
 * InputDeps.cpp
 */

#define DEBUG_TYPE "InputDeps"

#include "InputValues.h"

using namespace llvm;

STATISTIC(NumInputValues, "Number of input values");

void llvm::InputValues::getAnalysisUsage(AnalysisUsage& AU) const {

	/*
	 * We won't modify anything, so we shall tell this to LLVM
	 * to allow LLVM to not invalidate other analyses.
	 */
	AU.setPreservesAll();
}

bool llvm::InputValues::isMarkedCallInst(CallInst* CI) {

	Function* F = CI->getCalledFunction();

	//CallInst has a dynamic function pointer
	if (!F) return true;

	 // Empty functions include externally linked ones (i.e. abort, printf, scanf, ...)
	if (F->begin() == F->end() && !whiteList.count(F)) return true;

	return false;

}

void llvm::InputValues::insertInInputDepValues(Value* V) {
	if (!inputValues.count(V))
		inputValues.insert(V);
}

bool llvm::InputValues::runOnModule(Module& M) {

	module = &M;

	initializeWhiteList();

    collectMainArguments();
int one=0;
 errs()<<"\n------------------------------------------------\n";
	for(Module::iterator Fit = M.begin(), Fend = M.end(); Fit != Fend; Fit++){

		for (Function::iterator BBit = Fit->begin(), BBend = Fit->end(); BBit != BBend; BBit++) {
			
			for (BasicBlock::iterator Iit = BBit->begin(), Iend = BBit->end(); Iit != Iend; Iit++) {
			
				if(one<2)
				{
			    Value* V_in;
			     //FIXME: Temporary assignment of the tainted source for testing...
			     if(BBit->getName()=="BB_146")
			     {
			    //   errs() << "Inside BB_0";
			      if(LoadInst *LI = dyn_cast<LoadInst>(Iit))  
			       {
			     //   LI->dump();
			        V_in = LI->getPointerOperand();
			        errs() <<"\ntaint source " << LI->getName() << " ";
			        errs() << V_in;
			        insertInInputDepValues(V_in);
			        //NumInputValues++;
			        one++;
			        }
			     }
			     
			     }



				if (CallInst *CI = dyn_cast<CallInst>(Iit)) {

					if (isMarkedCallInst(CI)){

						//Values returned by marked instructions
						insertInInputDepValues(CI);

						for(unsigned int i = 0; i < CI->getNumOperands(); i++){

							if (CI->getOperand(i)->getType()->isPointerTy()){
								//Arguments with pointer type of marked functions
						//		insertInInputDepValues(CI->getOperand(i));
							}

						}

					}

				}

			}

		}

	}

 errs()<<"\n------------------------------------------------\n";
	NumInputValues = inputValues.size();

	//We don't modify anything, so we must return false;
	return false;
}

void llvm::InputValues::initializeWhiteList() {

	//output functions (stdio.h)
	insertInWhiteList(module->getFunction("putc"));
	insertInWhiteList(module->getFunction("putchar"));
	insertInWhiteList(module->getFunction("puts"));

	insertInWhiteList(module->getFunction("fputc"));
	insertInWhiteList(module->getFunction("fputs"));

	insertInWhiteList(module->getFunction("printf"));
	insertInWhiteList(module->getFunction("fprintf"));
	insertInWhiteList(module->getFunction("sprintf"));
	insertInWhiteList(module->getFunction("snprintf"));

	insertInWhiteList(module->getFunction("vfprintf"));
	insertInWhiteList(module->getFunction("vprintf"));
	insertInWhiteList(module->getFunction("vsprintf"));


	//Conversion functions (stdlib.h)
	insertInWhiteList(module->getFunction("abs"));
	insertInWhiteList(module->getFunction("labs"));

	insertInWhiteList(module->getFunction("atoi"));
	insertInWhiteList(module->getFunction("itoa"));
	insertInWhiteList(module->getFunction("atol"));
	insertInWhiteList(module->getFunction("atoll"));
	insertInWhiteList(module->getFunction("atof"));

	insertInWhiteList(module->getFunction("div"));
	insertInWhiteList(module->getFunction("ldiv"));
	insertInWhiteList(module->getFunction("lldiv"));

	insertInWhiteList(module->getFunction("strtod"));
	insertInWhiteList(module->getFunction("strtof"));
	insertInWhiteList(module->getFunction("strtol"));
	insertInWhiteList(module->getFunction("strtold"));
	insertInWhiteList(module->getFunction("strtoll"));
	insertInWhiteList(module->getFunction("strtoul"));
	insertInWhiteList(module->getFunction("strtoull"));

}

void llvm::InputValues::insertInWhiteList(Function* F) {
	if(F && whiteList.count(F) == 0){
		whiteList.insert(F);
	}
}

/*
 * All the arguments of the function main depend on external data.
 *
 * This method let us collect these arguments and mark them as input-dependent
 */
void llvm::InputValues::collectMainArguments() {

    Function *main = module->getFunction("main");

    // Using fortran? ... this kind of works
    if (!main)
        main = module->getFunction("MAIN__");

    if (main) {
		for(Function::arg_iterator Arg = main->arg_begin(), aEnd = main->arg_end(); Arg != aEnd; Arg++) {
			if (!inputValues.count(Arg)) inputValues.insert(Arg);
		}
    }

}


bool llvm::InputValues::isInputDependent(Value* V) {
	return inputValues.count(V);
}

std::set<Value*> llvm::InputValues::getInputDepValues() {
	return inputValues;
}


char InputValues::ID = 0;
static RegisterPass<InputValues> Y("input-deps", "Track Input Dependencies");


