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

	//errs() << "\n\nMediator Analysis\n"; 

  //std::vector<std::string> mediator_functions;

  Graph *dataDependenceGraph = depGraph;

	//errs() << mediators.size() << "\n";
	mediators.pop_back();

  
  for(std::list<std::string>::const_iterator i = mediators.begin(); i != mediators.end(); i++)
  {

		// Boolean flag that states whether we have identifies arguments and return variable for mediator
		int have_all_inputs_bool = 0;

    Function *med_function = M.getFunction(*i);
    std::vector<Value*> intra_arg_list;
    Value* intra_return_val;


		//errs() << "For Loop\n\n";



    // Report which function we are generating chops for.
		errs() << "\n\n\n================Analyzing Candidate Mediator: " << med_function->getName() << "===============\n";
    errs() << "================Gathering Intra-Mediator parameters and return values===============\n\n";


		// User functions to gather arguments and return values for candidate mediator function
    intra_arg_list = getArgVars(*med_function);
    intra_return_val = getReturnVar(*med_function);

	  if ((intra_arg_list.size() > 0) && (intra_return_val))
		{
			have_all_inputs_bool = 1;
		}


		if (have_all_inputs_bool == 1)
		{

			// Boolean flag that states whether we have found all necessary nodes.
			int have_all_nodes_bool = 0;

			// Identify nodes in the program dependence graph that correspond to the arguments and return variable
			GraphNode *intra_return_node;
      std::list<GraphNode*> intra_arg_node;
      
			// Find the return variable node in the PDG
      intra_return_node = dataDependenceGraph->findNode(intra_return_val);

      std::vector<Value*>::iterator argIt;

			// For each argument identified above, find its corresponding node in the PDG
      for (argIt  = intra_arg_list.begin(); argIt != intra_arg_list.end(); ++argIt)
      {
				GraphNode* arg_temp_node = dataDependenceGraph->findNode(*argIt);

				if(arg_temp_node)
				{
	  			intra_arg_node.push_back(arg_temp_node);
				}
      }

			if ((intra_arg_node.size() == intra_arg_list.size()) && (intra_return_node))
			{

				errs() << "================Processing Intra-Mediator Chop===============\n\n";

				std::list<Instruction*> i_list;
				i_list = identify_instructions(*med_function, depGraph);

				

			}

		}
		else
		{
			// If there are no arguments, then we cannot compute chops, so we should report the problem
      if (intra_arg_list.size() == 0)
      {
				errs() << "Arguments within the function: ";
				errs() << med_function->getName();
				errs() << " were either not identified or the function has no arguments. Cannot create chop.\n";
      }
		}

		
		//Perform Intra_medator chop analysis


		errs() << "================Gathering Call Sites For Mediator Function==================\n\n";

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
	
								
								Instruction *temp = block;

								int numOps = block->getNumOperands();
								//pre_med = false;
				  			//post_med = true;
				  			//errs() << "Mediator Called from instruction: " << *block << "\n";

								for (int i = 0; i < numOps - 1; i++)
								{

									Value* val = block->getOperand(i);


									/*
										Not entirely intuitive, but getting the operand of a call instruction actually gives you
										an instruction where that value is loaded. Since an intruction is a child of a Value
										it is safe to cast it to an instruction and get the operand from the corresponding 
										load instruction.
									*/
									errs() << "Mediator operand instruction: " << *block->getOperand(i) << "\n";
									if (isa<LoadInst>(*block->getOperand(i)))
									{
											Instruction* load_inst = (Instruction*)block->getOperand(i);
											//errs() << "It is a load inst " << load_inst->getOperand(0)->getName() << "\n";
											Value* passedVar = load_inst->getOperand(0);
											GraphNode* param_var = dataDependenceGraph->findNode(passedVar);


									}
								}
	
	
								// Query for the next instruction
								++block;
								// Check if it is a store instruction
								if (isa<StoreInst>(block))
								{
									errs() << "Mediator Result Store Instruction: " << *block << "\n";
									errs() << "Value: " << block->getOperand(0)->getName() << "\n";

									Value* med_result = block->getOperand(0);

									GraphNode* med_result_node = dataDependenceGraph->findNode(med_result);
				
									if (med_result_node)
									{
										//errs() << "Node name: ";
	  								//errs() << med_result_node->getName();
	  								//errs() << "Node label: ";
	  								//errs() << med_result_node->getLabel();
	  								//errs() << "\n\n\n";


										if (med_result_node->getSuccessors().size() == 0)
										{
												//errs() << "NO SUCCS\n";
										}
										else
										{
												//errs() << "SUCCS: " << med_result_node->getSuccessors().size() << "\n";
										}

										errs() << "========================Pre-Mediator Chop=====================\n";
										std::list<GraphNode*> pre_med_slice;
							
										pre_med_slice = computeReverseTaintSlice(med_result_node, 100);
	
										errs() << "========================Post-Mediator Chop=====================\n";
										
										//errs() << "Node: " << med_result_node;
										std::list<GraphNode*> post_med_slice;

										post_med_slice = computeForwardTaintSlice(med_result_node, 100);
										
									}
	
								}
								// If not, then our mediator isn't a mediator
								// Otherwise, pull the operand from the store instruction to get the variable that is used to store the result.
								
		
				  			errs() << "\n";
							} // If correct call
			    	} // If call instruction
					} // For loop iterating over instructions
				} // For loop iterating over basic blocks in function
			} 
		}	// For loop iterating over functions in module
	} // End for-loop iterating over mediators
}

/*
	
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



      

      //****************    Compute Values for Before Mediator Analysis **************
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

       
    }
}

*/

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

std::list<Instruction*> ChopControlModule::identify_instructions(Function &F, Graph *depGraph)
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
				instruction_list.push_back(I);
				errs() << *I<<"\n";
			}
    }
  }

  return instruction_list;

}



std::list<GraphNode*> ChopControlModule::computeForwardTaintSlice(GraphNode* node, int height)
{
  std::list<GraphNode*> forward_taint_slice;
  std::list<GraphNode*> worklist;
	std::list<string> labels;

  // Populate worklist with initial successors
  std::map<GraphNode*, edgeType> initialSuccs = node->getSuccessors();
  std::map<GraphNode*, edgeType>::iterator map_it = initialSuccs.begin();
  std::map<GraphNode*, edgeType>::iterator map_end_it = initialSuccs.end();

  

  for (; map_it != map_end_it; ++map_it)
  {
    worklist.push_back(map_it->first);
    forward_taint_slice.push_back(map_it->first);
  }

	//errs() << "Worklist size: " << worklist.size();

  while (worklist.size() != 0)
  {
    GraphNode* tempNode = worklist.front();

    std::map<GraphNode*, edgeType> tempSuccs = tempNode->getSuccessors();
    std::map<GraphNode*, edgeType>::iterator temp_it = tempSuccs.begin();
    std::map<GraphNode*, edgeType>::iterator temp_end_it = tempSuccs.end();


    for (; temp_it != temp_end_it; ++temp_it)
    {
      worklist.push_back(temp_it->first);
      forward_taint_slice.push_back(temp_it->first);
			labels.push_back(temp_it->first->getLabel());


	  	errs() << temp_it->first->getLabel();
	  	errs() << "\n";
    }

    worklist.pop_front();
    height--;

  }





	errs() << "======================Analyzing Post-Mediator Chop=====================\n";

	analyzeChops(labels);

	for (auto s_label = labels.begin(); s_label != labels.end(); s_label++)
	{

		//errs() << *s_label << "\n";
	}







  return forward_taint_slice;

}


std::list<GraphNode*> ChopControlModule::computeReverseTaintSlice(GraphNode* node, int height)
{
  std::list<GraphNode*> reverse_taint_slice;
  std::list<GraphNode*> worklist;
	std::list<string> labels;

  // Populate worklist with initial predecessors
  std::map<GraphNode*, edgeType> initialPreds = node->getPredecessors();
  std::map<GraphNode*, edgeType>::iterator map_it = initialPreds.begin();
  std::map<GraphNode*, edgeType>::iterator map_end_it = initialPreds.end();

	//errs() << "Initial Predecessors: " << initialPreds.size() << "\n";

  for(; map_it != map_end_it; ++map_it)
  {
    worklist.push_back(map_it->first);
    reverse_taint_slice.push_back(map_it->first);

		//errs() << "Node name: ";
	  //errs() << map_it->first->getName();
	  //errs() << "Node label: ";
	  //errs() << map_it->first->getLabel();
	  //errs() << "\n\n\n";
  }



  while(worklist.size() != 0)
  {
    GraphNode* tempNode = worklist.front();

    std::map<GraphNode*, edgeType> tempPreds = tempNode->getPredecessors();
    std::map<GraphNode*, edgeType>::iterator temp_it = tempPreds.begin();
    std::map<GraphNode*, edgeType>::iterator temp_end_it = tempPreds.end();

    for (; temp_it != temp_end_it; ++temp_it)
    {
      //GraphNode* temp_pred_node = temp_it->first;
      worklist.push_back(temp_it->first);
      reverse_taint_slice.push_back(temp_it->first);
			labels.push_back(temp_it->first->getLabel());


			errs() << temp_it->first->getLabel();
	  	errs() << "\n";

    }

    
    worklist.pop_front();
    height--;
  }

	errs() << "======================Analyzing Pre-Mediator Chop=====================\n";

	analyzeChops(labels);

	for (auto s_label = labels.begin(); s_label != labels.end(); s_label++)
	{

		//errs() << *s_label << "\n";



	}
  
  
  return reverse_taint_slice;
}

int ChopControlModule::analyzeChops(std::list<string> inst_list)
{
	
	std::list<string> write_ops;
	std::list<string> modification_ops;
	std::list<string> string_manip;
	std::list<string> memory_ops;
	
	std::list<string> high_priority;

	
	for (auto s_label = inst_list.begin(); s_label != inst_list.end(); s_label++)
	{


		// Handle write operations
		std::string s = *s_label;
		if (s.find("store") != std::string::npos)
		{
			if (s.find("TAINT_") != std::string::npos)
			{			
				write_ops.push_back(s);
				high_priority.push_back(s);
			}
			else
			{
				write_ops.push_back(s);
			}
		}


		// Handle bit-wise modification and operations 
		if 
		( 
			(s.find("and ") != std::string::npos) ||
			(s.find("xor ") != std::string::npos) ||
			(s.find("zext ") != std::string::npos) ||
			(s.find("lshr ") != std::string::npos) ||
			(s.find("sub nsw ") != std::string::npos)
		)
		{

			modification_ops.push_back(s);

		}

		// Handle strcmp
		if (s.find("strcmp") != std::string::npos)
		{
			string_manip.push_back(s);
			high_priority.push_back(s);
		}

		// Handle other string manipulation calls
		if 
		(
			(s.find("strcpy") != std::string::npos) ||
			(s.find("strcat") != std::string::npos) ||
			(s.find("strlen") != std::string::npos) ||
			(s.find("strlwr") != std::string::npos) ||
			(s.find("strupr") != std::string::npos) ||
			(s.find("strchr") != std::string::npos) ||
			(s.find("strtok") != std::string::npos) ||
			(s.find("sprintf") != std::string::npos) ||
			(s.find("vsprintf") != std::string::npos) ||
			(s.find("strstr") != std::string::npos)
		)
		{
			string_manip.push_back(s);
		}

		// Handle memory manipulation calls
		if 
		(
			(s.find("memcpy") != std::string::npos) ||
			(s.find("memset") != std::string::npos) ||
			(s.find("mprotect") != std::string::npos)
		)
		{
			memory_ops.push_back(s);
		}
	}


	// Print risky operations to analyst
	errs() << "Number of write operations: " << write_ops.size() << "\n";
	for (auto inst = write_ops.begin(); inst != write_ops.end(); inst++)
	{
		errs() << *inst << "\n";
	}

	errs() << "\n";
	errs() << "Number of bit operations: " << modification_ops.size() << "\n";
	for (auto inst = modification_ops.begin(); inst != modification_ops.end(); inst++)
	{
		errs() << *inst << "\n";
	}

	errs() << "\n";
	errs() << "Number of string operations: " << string_manip.size() << "\n";
	for (auto inst = string_manip.begin(); inst != string_manip.end(); inst++)
	{
		errs() << *inst << "\n";
	}

	errs() << "\n";
	errs() << "Number of memory operations: " << memory_ops.size() << "\n";
	for (auto inst = memory_ops.begin(); inst != memory_ops.end(); inst++)
	{
		errs() << *inst << "\n";
	}



	return 0;
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


std::vector<Instruction*> ChopControlModule::getMedCallRetVar(Module &M, Function &F)
{

	std::vector<Instruction*> inst_list;

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

		      	if (temp_inst.find(F.getName()) != std::string::npos)
						{

							// This will get me a list of all of the operands for the call.

							Instruction *temp = block;
							int x = block->getNumOperands();
							//pre_med = false;
			  			//post_med = true;
			  			errs() << "Mediator Called from instruction: " << *block << "\n";
							errs() << x << "\n";
							errs() << *block->getOperand(0) << "\n";
							errs() << *block->getOperand(1) << "\n";

							


							// Query for the next instruction
							++block;
							// Check if it is a store instruction
							if (isa<StoreInst>(block))
							{
								errs() << "Mediator Result Store Instruction: " << *block << "\n";
								inst_list.push_back(temp);
								inst_list.push_back(block);

							}
							// If not, then our mediator isn't a mediator
							// Otherwise, pull the operand from the store instruction to get the variable that is used to store the result.
							
	
			  			errs() << "\n";
						}
		    	}
				}
			}
		}
	}
	return inst_list;

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


      Value *argVal = psUSe->getOperand(0);
      argValList.push_back(argVal);
      
      // We will want to return a list of these
      errs() << "\t Argument: " << psUSe->getOperand(0)->getName() << "\n";

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

		errs() << "\t Return variable: " << returnValue->getName() << "\n";

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
