#include "ChopControlModule.h"


ChopControlModule::ChopControlModule() :
  ModulePass(ID){

}

typedef enum{
  cdPreMediator = 0, cdIntraMediator = 1, cdPostMediator = 2
} chopType;

struct Chop
{

  chopType chop_type;
  Value* source;
  Value* sink;
  
  std::list<GraphNode*> nodeList;
};



bool ChopControlModule::runOnModule(Module &M)
{
  return false;
}

/* Insert necessary functions here */
void ChopControlModule::processChops(Module &M, Graph *depGraph, std::set<Value*> source_vals, std::list<string> mediators){

 
  //std::vector<std::string> mediator_functions;

  Graph *dataDependenceGraph = depGraph;

	//errs() << mediators.size() << "\n";
	mediators.pop_back();
  
  for(std::list<std::string>::const_iterator i = mediators.begin(); i != mediators.end(); i++)
    {
      Function *med_function = M.getFunction(*i);
      std::vector<Value*> intra_arg_list;
      Value* intra_return_val;

      // Report which function we are generating chops for.
			errs() << "\n\n\n================Analyzing Candidate Mediator: "<<med_function->getName()<<"===============\n";
      errs() << "================Gathering Intra-Mediator parameters and return values===============\n\n";
      errs() << med_function->getName();
      errs() << "\n";

      /************** Gather values to compute intramediator chops ***************/
      
      //errs() << "Gathering intramediator parameters and return values.\n";
      intra_arg_list = getArgVars(*med_function);
      intra_return_val = getReturnVar(*med_function);

      if (intra_arg_list.size() == 0)
      {
	errs() << "Arguments within the function: ";
	errs() << med_function->getName();
	errs() << " were either not identified or the function has no arguments. Cannot create chop.\n";
      }

      // Attempt to get dependence graph nodes for corresponding values

      GraphNode *intra_return_node;
      std::list<GraphNode*> intra_arg_node;
      
      intra_return_node = dataDependenceGraph->findNode(intra_return_val);

      std::vector<Value*>::iterator argIt;

      for (argIt  = intra_arg_list.begin(); argIt != intra_arg_list.end(); ++argIt)
      {
	GraphNode* arg_temp_node = dataDependenceGraph->findNode(*argIt);

	if(arg_temp_node)
	{
	  intra_arg_node.push_back(arg_temp_node);
	}
      }

      /*****************    Create Intra-Mediator Chops Based on Found Arguments and Return Value *****/
      
      if (intra_return_node)
      {

	errs() << "================Processing Intra-Mediator Chop===============\n\n";


	std::list<GraphNode*> intra_return_slice;
	//intra_return_slice = computeReverseTaintSlice(intra_return_node, 100);

	std::list<GraphNode*> intra_arg_slice;
	//intra_arg_slice = computeForwardTaintSlice(intra_arg_node.back(), 100);
	std::list<Instruction*> i_list;
	i_list = test_function(*med_function, depGraph);
	//errs() << "Return slice size: ";
	//errs() << intra_return_slice.size();
	//errs() << "\n";

	
	errs() << "\n::::::::::::   Running queries on intra mediator chop  :::::::::::::::::\n\n\n";
	runQueries(i_list);
	
	for (std::list<GraphNode*>::iterator node_it = intra_return_slice.begin(), node_end_it = intra_return_slice.end(); node_it != node_end_it; ++node_it)
	  {
	    //errs() << "Node name: ";
	    //errs() << (*node_it)->getName();
	    //errs() << "Node label: ";
	    //errs() << (*node_it)->getLabel();
	    //errs() << "\n\n\n";
	  }

	//errs() << "Arg slice size for arg ";
	//errs() << intra_arg_slice.size();
	//errs() << "\n";
		
      	for (std::list<GraphNode*>::iterator node_source_it = intra_arg_slice.begin(), node_source_end = intra_arg_slice.end(); node_source_it != node_source_end; ++node_source_it)
	  {
	    //errs() << "Node name: ";
	    //errs() << (*node_source_it)->getName();
	    //errs() << "Node label: ";
	    //errs() << (*node_source_it)->getLabel();
	    //errs() << "\n\n\n";
	  }	
      }
      else
      {
	errs() << "Return value for function: ";
	errs() << med_function->getName();
	errs() << " could not be identified. Cannot create chop. \n";
      }



      

      /*****************    Compute Values for Before Mediator Analysis ***************/
      //errs() << M.getModuleIdentifier();

      std::set<Value*>::iterator value_it = source_vals.begin();
      std::set<Value*>::iterator value_end_it = source_vals.end();
      std::list<string> taint_input_list;
      
      for (; value_it != value_end_it; ++value_it)
      {
        Value* tempValue = (*value_it);
	std::string v_string = "v";
	std::string input_string = tempValue->getName();
	if (input_string.find(v_string) != std::string::npos)
	  {
	    taint_input_list.push_back(input_string);
	    errs() << input_string << "\n";
	  }
      }

	// Find call site for mediator

      std::list<Instruction*> pre_mediator_instructions;
      std::list<Instruction*> post_mediator_instructions;
      bool pre_med = true;
      bool post_med = false;
      /*     

      for (Module::iterator mod = M.begin(), mod_e = M.end(); mod != mod_e; ++mod)
      {
	  if(!mod->isDeclaration())
	  {
	    for (Function::iterator func = mod->begin(), func_e = mod->end(); func != func_e; ++func)
	    {
		for (BasicBlock::iterator block = func->begin(), block_e = func->end(); block != block_e; ++block)
		{
		    if (isa<CallInst>(block))
		    {
		      std::string temp_inst;
		      llvm::raw_string_ostream rso(temp_inst);
		      (&*block)->print(rso);

		      if (temp_inst.find(med_function->getName()) != std::string::npos)
			{
			  pre_med = false;
			  post_med = true;
			  errs() << *block;
			  errs() << "\n";
			}

		      
		      
		    }

		    std::string string2;
		    llvm::raw_string_ostream rso_2(string2);
		    (&*block)->print(rso_2);
		    std::string taint = "TAINT_";

		    if ((string2.find(taint) != std::string::npos) and (func->getName() != med_function->getName()))
		      {
			errs() << *block;
			errs() << "\n";
			if (pre_med)
			  pre_mediator_instructions.push_back(block);
			if(post_med)
			  post_mediator_instructions.push_back(block);
		      }
		    
		}

	      
	    }
	  }
	
      }
*/
       
    }
}


int ChopControlModule::runQueries(std::list<Instruction*> inst_list)
{

  std::list<Instruction*>::iterator inst_it = inst_list.begin();
  std::list<Instruction*>::iterator inst_end_it = inst_list.end();


  // Record all call instructions on chop
  for (; inst_it != inst_end_it; ++inst_it)
    {

      Instruction* temp_inst = (*inst_it);

      if (isa<CallInst>(temp_inst))
			{
	  		errs() << "Call Inst: " << *temp_inst << "\n";
	  
			}
			else if (isa<StoreInst>(temp_inst))
			{
				errs() << "Write Op: " << *temp_inst << "\n";
			}
			else
			{
				std::string string2;
	   		llvm::raw_string_ostream rso_2(string2);
	   		(&*temp_inst)->print(rso_2);
	   		std::string str_string = "zext";
				std::string str_string2 = "lshr";
				std::string str_string3 = "xor";
				std::string str_string4 = "and";
				std::string str_string5 = "sub nsw";

	   		if (
						(string2.find(str_string) != std::string::npos) ||
						(string2.find(str_string2) != std::string::npos) ||
						(string2.find(str_string3) != std::string::npos) ||
						(string2.find(str_string4) != std::string::npos) ||
						(string2.find(str_string5) != std::string::npos)
					 )
				{	
	     		errs() << "Modification Op: " << *temp_inst << "\n";
				}
			}


    }

  errs() << "\n";

   inst_it = inst_list.begin();
   inst_end_it = inst_list.end();
   // Check for strcmp specifically
   for (; inst_it != inst_end_it; ++inst_it)
    {

      Instruction* temp_inst = (*inst_it);

      if (isa<CallInst>(temp_inst))
	{
	   std::string string2;
	   llvm::raw_string_ostream rso_2(string2);
	   (&*temp_inst)->print(rso_2);
	   std::string str_string = "strcmp";

	   if (string2.find(str_string) != std::string::npos)
	     errs() << "String compare on chop: SEVERE " << *temp_inst << "\n";
	  
	}

    }

   errs() << "\n";

   inst_it = inst_list.begin();
   inst_end_it = inst_list.end();

  

   return 0;
  








}

std::list<GraphNode*> ChopControlModule::computeForwardTaintSlice(GraphNode* node, int height)
{
  std::list<GraphNode*> forward_taint_slice;
  std::list<GraphNode*> worklist;

  // Populate worklist with initial successors
  std::map<GraphNode*, edgeType> initialSuccs = node->getSuccessors();
  std::map<GraphNode*, edgeType>::iterator map_it = initialSuccs.begin();
  std::map<GraphNode*, edgeType>::iterator map_end_it = initialSuccs.end();

  errs() << "Size of successors list: ";

  for (; map_it != map_end_it; ++map_it)
  {
    worklist.push_back(map_it->first);
    forward_taint_slice.push_back(map_it->first);
  }

  while (height < 0)
  {
    GraphNode* tempNode = worklist.front();

    std::map<GraphNode*, edgeType> tempSuccs = tempNode->getSuccessors();
    std::map<GraphNode*, edgeType>::iterator temp_it = tempSuccs.begin();
    std::map<GraphNode*, edgeType>::iterator temp_end_it = tempSuccs.end();


    for (; temp_it != temp_end_it; ++temp_it)
    {
      worklist.push_back(temp_it->first);
      forward_taint_slice.push_back(temp_it->first);
    }

    worklist.pop_front();
    height--;

  }

  return forward_taint_slice;

}


std::list<GraphNode*> ChopControlModule::computeReverseTaintSlice(GraphNode* node, int height)
{
  std::list<GraphNode*> reverse_taint_slice;
  std::list<GraphNode*> worklist;

  // Populate worklist with initial predecessors
  std::map<GraphNode*, edgeType> initialPreds = node->getPredecessors();
  std::map<GraphNode*, edgeType>::iterator map_it = initialPreds.begin();
  std::map<GraphNode*, edgeType>::iterator map_end_it = initialPreds.end();

  for(; map_it != map_end_it; ++map_it)
  {
    worklist.push_back(map_it->first);
    reverse_taint_slice.push_back(map_it->first);
  }

  while(height > 0)
  {
    GraphNode* tempNode = worklist.front();

    std::map<GraphNode*, edgeType> tempPreds = tempNode->getPredecessors();
    std::map<GraphNode*, edgeType>::iterator temp_it = tempPreds.begin();
    std::map<GraphNode*, edgeType>::iterator temp_end_it = tempPreds.end();

    for (; temp_it != temp_end_it; ++temp_it)
    {
      GraphNode* temp_pred_node = temp_it->first;
      worklist.push_back(temp_pred_node);
      reverse_taint_slice.push_back(temp_pred_node);
    }

    
    worklist.pop_front();
    height--;
  }
  
  
  return reverse_taint_slice;
}

std::list<Instruction*> ChopControlModule::test_function(Function &F, Graph *depGraph)
{

  Graph *ddp_graph = depGraph;
  // Iterate through each basic block
  StringRef s2 = "TAINT_";
  std::list<Instruction*> instruction_list;

  for (Function::iterator BB = F.begin(), endBB = F.end(); BB!= endBB; ++BB)
  {
    // Iterate through each instruction
    for (BasicBlock::iterator I = BB->begin(), endI = BB->end(); I != endI; ++I)
    {
      std::string temp_inst;
      llvm::raw_string_ostream rso(temp_inst);
      (&*I)->print(rso);

      if (temp_inst.find(s2) != std::string::npos)
			{
				if (
						(temp_inst.find("and i32 %18,") != std::string::npos) ||
						(temp_inst.find("sub nsw") != std::string::npos) ||
						(temp_inst.find("xor") != std::string::npos) ||
						(temp_inst.find("%12,") != std::string::npos) ||
						(temp_inst.find("%11,") != std::string::npos) ||
						(temp_inst.find("%24 =") != std::string::npos) ||
						(temp_inst.find("%24,") != std::string::npos) ||
						(temp_inst.find("%25,") != std::string::npos) ||
						(temp_inst.find("%25 =") != std::string::npos) ||
						(temp_inst.find("%21,") != std::string::npos) ||
						(temp_inst.find("%21 ") != std::string::npos) ||
						(temp_inst.find("%424 ") != std::string::npos) ||
						(temp_inst.find("%424,") != std::string::npos) ||
						(temp_inst.find("%17,") != std::string::npos) ||
						(temp_inst.find("%17 ") != std::string::npos) ||
						(temp_inst.find("%23 ") != std::string::npos)
					 )
				{
					instruction_list.push_back(I);
					errs() << *I<<"\n";

				}
			}
			else if (
					(temp_inst.find("%708") != std::string::npos)
					)
			{
				instruction_list.push_back(I);
				errs() << *I;
				errs() << "\n";
			}
    }
  }

  return instruction_list;

}


std::vector<Value*> ChopControlModule::getArgVars(Function &F)
{

  std::vector<Value*> argValList;
  
  Function::ArgumentListType &sArgs = F.getArgumentList();
  Function::ArgumentListType::iterator psArg = sArgs.begin();
  Function::ArgumentListType::iterator psEndArg = sArgs.end();

  for ( ;psArg != psEndArg; ++psArg)
  {
    Argument::user_iterator psUSe = psArg->user_begin();
    Argument::user_iterator psUSeEnd = psArg->user_end();
    for (; psUSe != psUSeEnd ; ++psUSe)
    {
      //psUSe->print(errs());
      //errs() << "\n";

      Value *argVal = psUSe->getOperand(0);
      argValList.push_back(argVal);
      
      // We will want to return a list of these
      //errs() << psUSe->getOperand(0)->getName();
      //errs() << "\n";
    }
  }

  return argValList;

}




Value* ChopControlModule::getReturnVar(Function &F)
{
  Instruction *return_var;
  Instruction *prior;
  Value *returnValue;
  
  // Iterate through each basic block
  for (Function::iterator BB = F.begin(), endBB = F.end(); BB!= endBB; ++BB)
  {
    // Iterate through each instruction
    for (BasicBlock::iterator I = BB->begin(), endI = BB->end(); I != endI; ++I)
    {

      // Check if it is a return instruction
      if (ReturnInst *returnInst = dyn_cast<ReturnInst>(&*I))
      {
	//returnInst->print(errs());
	//errs() << "\n";

	if (LoadInst *loadInst = dyn_cast<LoadInst>(prior))
	{
	  //loadInst->print(errs());
	  //errs() << "\n";
	  //errs() << loadInst->getOperand(0)->getName();
	  //errs() << "\n";
	  returnValue = loadInst->getOperand(0);

	}
	
	//errs() << returnInst->getReturnValue()->getName();
	//errs() << "\n";

	// Return a copy of the return instruction
	return_var = I;
      }

      prior = &*I;
      
    }
  }

  return returnValue;
}

void ChopControlModule::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char ChopControlModule::ID = 0;
static RegisterPass<ChopControlModule> S("-create-chops", "Controls all necessary modules to generate chops");
