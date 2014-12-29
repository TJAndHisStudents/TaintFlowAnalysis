//===- InputPass.cpp - Mark the inputs for taint analysis pass. ---------------===//
//
//                     
// 
//
//===----------------------------------------------------------------------===//
//
// Description: The file implements a pass to identify the user input taints for taint tracking.
// Input files: 
// Output : 
//
//===----------------------------------------------------------------------===//


#define DEBUG_TYPE "InputPass"
#include "InputPass.h"


STATISTIC(NumInputValues, "The number of input values");

InputPass::InputPass() :
	ModulePass(ID) {
}



cl::opt<std::string> SourceFile("src_file", cl::desc("<Taint Source file>"), cl::init("-"));
cl::opt<std::string> LookupFile("lookup", cl::desc("<Lookup file>"), cl::init("-"));
cl::opt<std::string> FunctionFile("funcInfo", cl::desc("<Function Info file>"), cl::init("-"));
cl::opt<std::string> defUseFile("def-use", cl::desc("<Def-use Info file>"), cl::init("-"));




//TODO: Have specific input functions behavior for the following cases.. make it generic. (read from File)
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
 
bool InputPass::runOnModule(Module &M) {
	//	DEBUG (errs() << "Function " << F.getName() << "\n";);
	NumInputValues = 0;
	bool inserted;
	errs() << "File name: " <<FunctionFile <<"\n";
	//TODO : get the starting function name from user again specified in file or passed as arg.
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
	 ReadSources();
	 ReadFunctionInfo();
	 ReadDefUse();
	// ReadTargets();
	//FS.print();
//	int one =0;
	
	
	///Iterate over the module to read the input dependent values from the input functions
for (Module::iterator F = M.begin(), eM = M.end(); F != eM; ++F) {
	   //errs() << F.getName();
	   
	   std::vector<FunctionInfo*>::iterator FI;
	   for(FI = FunctionInfos.begin(); FI != FunctionInfos.end() ; ++ FI)
	   {
		for (Function::iterator BB = F->begin(), e = F->end(); BB != e; ++BB) {
			//errs() << BB->getName() << "\n";
	//		int func_block =0;
	//		if((*FI)->begin_block ==BB->getName())
	//			errs() << BB->getName() <<" for function "<< (*FI)->FunctionName << "\n";
		//	else
			//	errs() << BB->getName() << "  and func begin block "<<(*FI)->begin_block << " func " <<(*FI)->FunctionName <<"\n";
		/*	FunctionInfo * funcI = FunctionInfos.front();
			if (funcI->begin_block == BB->getName())
				{
				errs() << "Found begin block    " << BB->getName();
				func_block =1;
				}
			
				
				
		//	while(func_block ==1)
			{
			for (BasicBlock::iterator I = BB->begin(), ee = BB->end(); I != ee; ++I) {
			
			//	errs() << *I;
			 //generate dep grap for each function.
			//For Binary - modify the following to get the taint on the initial instruction identified.
			}
			if (funcI->end_block == BB->getName())
				{
				errs() << "Found End block    " << BB->getName();
				func_block = 0;
				}
			
		  } */
		}
		} //end loop functioninfos.
	} 
	
	
	
	//Iterate Over the module to generate the data flow graph for the given basic blocks.
	for (Module::iterator F = M.begin(), eM = M.end(); F != eM; ++F) {
	   //errs() << F.getName();
		for (Function::iterator BB = F->begin(), e = F->end(); BB != e; ++BB) {
			//errs() << BB.getName();
		
				for (BasicBlock::iterator I = BB->begin(), ee = BB->end(); I != ee; ++I) {
				//	errs() << *I;
				
				if (CallInst *CI = dyn_cast<CallInst>(I)) {
                         Function *Callee = CI->getCalledFunction();
                         if (Callee) {
                              Value* V;
                              inserted = false;
                              StringRef Name = Callee->getName();
                              errs() << "\n\n--- Function name from the call instructions:  "  <<Name;
                              //Identify the target functions and add in target function set.
                               for(std::set<std::string>::iterator t = targetNames.begin(), en =
                                             targetNames.end(); t != en; ++t)
                               {
                                    if(Name.equals(*t)){
                                         targetFunctions.insert(Callee);
                                    }
                               }



                              if (Name.equals("main")) {
                                   errs() << "main\n";
                                   V = CI->getArgOperand(1); //char* argv[]
                                   inputDepValues.insert(V);
                                   inserted = true;
                                   //errs() << "Input    " << *V << "\n";
                              }
                              if (Name.equals("__isoc99_scanf") || Name.equals(
                                        "scanf")) {
                                   for (unsigned i = 1, eee = CI->getNumArgOperands(); i
                                             != eee; ++i) { // skip format string (i=1)
                                        V = CI->getArgOperand(i);
                                        if (V->getType()->isPointerTy()) {
                                             inputDepValues.insert(V);
                                             inserted = true;
                                             //errs() << "Input    " << *V << "\n";
                                        }
                                   }
                              } else if (Name.equals("__isoc99_fscanf")
                                        || Name.equals("fscanf")) {
                                   for (unsigned i = 2, eee = CI->getNumArgOperands(); i
                                             != eee; ++i) { // skip file pointer and format string (i=1)
                                        V = CI->getArgOperand(i);
                                        if (V->getType()->isPointerTy()) {
                                             inputDepValues.insert(V);
                                             inserted = true;
                                             //errs() << "Input    " << *V << "\n";
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
                                        //errs() << "Input    " << *V << "\n";
                                   }
                              } else if ((Name.equals("fgetc") || Name.equals("getc")
                                        || Name.equals("getchar"))) {
                                   inputDepValues.insert(CI);
                                   inserted = true;
                                   //errs() << "Input    " << *CI << "\n";
                              } else if (Name.equals("recv")
                                        || Name.equals("recvmsg")
                                        || Name.equals("read")) {
                                   Value* V = CI->getArgOperand(1);
                                   if (V->getType()->isPointerTy()) {
                                        inputDepValues.insert(V);
                                        inserted = true;
                                        //errs() << "Input    " << *V << "\n";
                                   }
                              } else if (Name.equals("recvfrom")) {
                                   V = CI->getArgOperand(1);
                                   if (V->getType()->isPointerTy()) {
                                        inputDepValues.insert(V);
                                        inserted = true;
                                        //errs() << "Input    " << *V << "\n";
                                   }
                                   V = CI->getArgOperand(4);
                                   if (V->getType()->isPointerTy()) {
                                        inputDepValues.insert(V);
                                        inserted = true;
                                        //errs() << "Input    " << *V << "\n";
                                   }
                              }
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
				
				
				}
			
		}
	}
	

	
	DEBUG(printer());
	//printer();
	return false;
}



void InputPass::ReadFunctionInfo()
{
	std::string word;
    
    std::ifstream funcinFile (FunctionFile.c_str(), std::ifstream::in);
   // FunctionInfo * funcI;
  //  std::string file_Data,line;
    if(!funcinFile)
    {
        errs() << " Could not open the Function info file \n";
    }
    else
    {
    
        while(funcinFile >> word)
    	 {
    	    if(word.compare("Caller:") == 0)
    	    {
    	    	//funcI = new FunctionInfo;
    	    	 FunctionInfo * funcI = new FunctionInfo();
    	    	funcinFile >> word;
    	    	errs() << "\nCaller : " <<word;
    	    	funcI->FunctionName = word;
    	    	
    	    	//Read start of function
    	    	while(word != "BB_Min:")
    	    		funcinFile >> word;
    	    	funcinFile >> word;
    	    	funcI->begin = stoi(word);
    	    	funcI->begin_block = "BB_"+word;
    	    	errs() << "  begin block : " <<word;
    	    	
    	    	//Read end of function
    	    	while(word != "BB_Max:")
    	    		funcinFile >> word;
    	    	funcinFile >> word;
    	    	funcI->end = stoi(word);
    	    	funcI->end_block = "BB_"+word;
    	    	errs() << "  end block : " <<word;
    	    	errs() << "   " << funcI->FunctionName;
    	    	
    
    	        FunctionInfos.push_back(funcI);
    	        errs() <<  "  " <<FunctionInfos.size();
    	    //    delete funcI;
    	       
    	    /*   std::set<FunctionInfo*>::iterator FI;
			for(FI= FunctionInfos.begin(); FI != FunctionInfos.end(); ++FI)
 				{
 				errs() << "\n" << (*FI)->FunctionName << " begin  :" << (*FI)->begin << " end  :" << (*FI)->end;
 				errs() << "  " << " block  :" << (*FI)->begin_block << " e block " << (*FI)->end_block; 
 				} 
    	       */
    	       
    	      // errs() << "\nSet of fucntions:   " <<FunctionInfos.size() << "  " <<FunctionInfos.max_size();
    	      // free(funcI);
			}
			//file_Data+=word;
			//errs() << word << " ";
    	 }
    	 /*
        while (getline(funcinFile, line))
        {
        	 file_Data += line;
             errs() << line << "\n";
        } */
            
    }
  /*  vector<char> xml_copy(file_Data.begin(), file_Data.end());
    xml_copy.push_back('\0');
    rapidxml::xml_document<> doc;
    doc.parse<0>(&xml_copy[0]);
    xml_node<>* root = doc.first_node("sources");
    std::set<string> allSources;
    std::map<int,string> SourceloctoSource;

    // Parse lattice and create a lattice graph 
    for (xml_node<>* fnode = root->first_node("source"); fnode!= NULL; fnode = fnode->next_sibling("source"))
    {
            xml_attribute<> *number = fnode->first_attribute();
            xml_attribute<> *sourcename = number->next_attribute();
           
            int Instnumber = atoi(number->value());
            string source(sourcename->value());
            allSources.insert(source);
    }
    
    
    for(std::set<string>::iterator sname = allSources.begin(); sname !=allSources.end();++sname)
     {
       errs() << "Sources..........\n";
       errs() << *sname << " \n";
     } 
     */

}


void InputPass::ReadDefUse()
{
	std::string word;
    
    std::ifstream defuse_File (defUseFile.c_str(), std::ifstream::in);
   // FunctionInfo * funcI;
  //  std::string file_Data,line;
  
  int def_count=0;
  int noUse_count =0;
  bool emptydef =false;
  std::string def_block;
    if(!defuse_File)
    {
        errs() << " Could not open the Def-Use info file \n";
    }
    else
    {
    
    	DefUseInfo *def_use; // = new DefUseInfo();
    	DefUseInfo *use;
        while(defuse_File >> word)
    	 {
    	    if(word.compare("def:") == 0)
    	    {
    	    	if(def_count != 0)
    	    		{ 
    	    		if(def_use->UseInfo.size()!=0 && emptydef!=true)
    	    			DefUseInfos.push_back(def_use);
    	    		errs() <<"\n Size of uses "<<  def_use->UseInfo.size();
    	    		}
    	    	emptydef = false;
    	    	def_use = new DefUseInfo();
    	    	//get the defined block
    	    	defuse_File >> word;
    	    	def_use->defBlock = "BB_"+word;
    	    	def_block = "BB_"+word;
    	    	defuse_File >> word;
    	    	def_use->lineNumber = stoi(word);
    	    	std::string str1;
    	    	getline(defuse_File, str1);
    	    	if(str1==" ")
    	    		{errs() << "This def is empty..\n\n\n";
    	    		 emptydef= true;
    	    		}
    	    	errs() << "\n Def:" <<str1 <<"enddef";
    	    	def_count++;
    	    }
    	    else if(word.compare("use:") == 0)
    	    {
    	    	defuse_File >> word;
    	    	//errs()<< word <<"\n";
    	    	if(word != "none")
    	    	{
    	    		use = new DefUseInfo();
    	    		//errs()<<"In not nine \n";
					use->defBlock = def_block;
					use->useBlock = "BB_"+word;
					defuse_File >> word;
					use->lineNumber = stoi(word);
					def_use->UseInfo.insert(use);
					std::string str;
					getline(defuse_File, str);
					errs() << "\n      Use : " <<str;
    	    	}
    	    	else
    	    		noUse_count++;
    	    }	
 
    	 }
    	 
    	 //Add the last element.
    	 if(def_use->UseInfo.size()!=0)
    	    			DefUseInfos.push_back(def_use);
    	    			
    	 errs() <<"Defs and unused defs "<< def_count<< " "<<noUse_count;  
    	 errs() <<"\n Size of uses "<<  def_use->UseInfo.size();
    	 errs() <<"\n Def use Chains "<< DefUseInfos.size();
    }
}


void InputPass::ReadSources(){

	std::string word;
    
    if(!SourceFile.empty()) //!= NULL)
    {
     
     errs() << "\nTrying to read source file.. ";
    std::ifstream inFile (SourceFile.c_str(), std::ifstream::in);
    std::string file_Data,line;
    
    
    
    if(!inFile)
    {
        errs() << " Could not open the sources file \n";
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

}

void InputPass::ReadTargets(){

	 std::ifstream tgFile (LookupFile.c_str(), std::ifstream::in);
	    std::string line;
	    if(!tgFile)
	    {
	        errs() << " Could not open the lookup file \n";
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



void InputPass::printer() {
   	errs() << "===Input dependant values:====\n";
	for (std::set<Value*>::iterator i = inputDepValues.begin(), e =
			inputDepValues.end(); i != e; ++i) {
		errs() << **i << "\n";
	}
	errs() << "=============Target Functions==============\n";
	for (std::set<std::string>::iterator t = targetNames.begin(), en =
			targetNames.end(); t != en; ++t) {
			errs() << *t << "\n";
		}
	errs() << "==================================\n";
	
	errs()<< "\nSize of the Function set : " << FunctionInfos.size() <<"\n";
		
	std::vector<FunctionInfo*>::iterator FI;
	for(FI= FunctionInfos.begin(); FI!=FunctionInfos.end(); ++FI)
 	{
 		errs() << "\n" << (*FI)->FunctionName << " begin  :" << (*FI)->begin << " end  :" << (*FI)->end;
 		errs() << "  " << " block  :" << (*FI)->begin_block << " e block " << (*FI)->end_block; 
 	}
}


void InputPass::getAnalysisUsage(AnalysisUsage &AU) const {
	AU.setPreservesAll();
}

std::set<Value*> InputPass::getInputDepValues() {
	return inputDepValues;
}

std::set<std::string> InputPass::getTargetNames() {
	return targetNames;
}

std::set<Function*> InputPass::getTargetFunctions(){
	return targetFunctions;
}


std::vector<FunctionInfo*> InputPass::getFunctionInfo(){
	return FunctionInfos;
}



char InputPass::ID = 0;
static RegisterPass<InputPass> X("InputPass",
		"Input Dependency Pass: looks for values that are input-dependant");
