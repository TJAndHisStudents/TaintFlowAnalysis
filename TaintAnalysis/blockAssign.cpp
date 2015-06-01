#include "blockAssign.h"

#define debug false

// Global variables
std::vector<BasicBlock *> ProcessedBB; //Stores processed basic block used in makeTable and findIR methods

// opt -o parameter in order to force the optimization (new style of control dependence graph construction)
    cl::opt<bool> IsOptimized("optimized_taint", cl::desc("Specify whether to use optimized version (getting transitivity on control edges"), cl::desc("is optmized"));


using namespace llvm;

//Receive a predicate and include it on predicate attribute
Pred::Pred(Value *p) {
	predicate = p;
}

//Receive a instruction and include it on insts vector
void Pred::addInst(Instruction *i) {
	insts.push_back(i);
}

//Receive a function pointer and include it on funcs vector
void Pred::addFunc(Function *f) {
	funcs.push_back(f);
}

//Return the total instruction count
int Pred::getNumInstrucoes() {
	return (insts.size());
}

int Pred::getNumFunctions() {
	return (funcs.size());
}

//Return the predicate
Value *Pred::getPred() {
	return (predicate);
}

//Return the instruction stored on insts vector, pointed for parameter i
Instruction *Pred::getInst(int i) {
	if (i < (signed int) insts.size())
		return (insts[i]);
	else
		return NULL;
}

Function *Pred::getFunc(int i) {
	if (i < (signed int) funcs.size())
		return (funcs[i]);
	else
		return NULL;
}

//Return true of *op instruction is gated (if it is stored on insts vector) for the predicate
bool Pred::isGated(Instruction *op) {
	unsigned int i;

	for (i = 0; i < insts.size(); i++) {
		if (op == insts[i])
			return true;
	}

	return false;
}

bool Pred::isFGated(Function *f) {
	unsigned int i;
	for (i = 0; i < funcs.size(); i++) {
		if (f == funcs[i])
			return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//Passes which are used by bSSA pass
void blockAssign::getAnalysisUsage(AnalysisUsage &AU) const {

	AU.addRequired<PostDominatorTree>();
    AU.addRequired<DominatorTreeWrapperPass>();
	AU.addRequired<moduleDepGraph>();

	// This pass will not modifies the program nor CFG
	AU.setPreservesAll();

}

//Return a set of all tainted nodes
std::set<Value *> blockAssign::getLeakedValues() {
	std::set<Value *> s;
	OpNode *op;
	VarNode *va;
	MemNode *mem;
	std::set<Value*> aliases;
	for (Graph::iterator gS = this->newGraph->begin(), gE =
			this->newGraph->end(); gS != gE; ++gS) {

		if ((*gS)->tainted) {
			if ((op = dyn_cast<OpNode>((*gS)))) {
				if (op->tainted)
					s.insert(op->getValue());
			} else if ((va = dyn_cast<VarNode>((*gS)))) {
				if (va->tainted)
					s.insert(va->getValue());
			} else if ((mem = dyn_cast<MemNode>((*gS)))) {
				if (mem->tainted) {
					aliases = mem->getAliases();
					for (std::set<Value *>::iterator mIt = aliases.begin(), mE =
							aliases.end(); mIt != mE; ++mIt) {
						s.insert((*mIt));
					}
				}
			}
		}
	}

	return (s);
}

//Return a vector of pair (print instruction, parameter vector)
std::vector<std::pair<Instruction*, std::vector<Value*> > > blockAssign::getPrintfLeaks() {

	std::vector<std::pair<Instruction*, std::vector<Value*> > > v;

	std::vector<Value *> args;

	for (std::vector<Value *>::iterator dstIt = dst.begin(), dstE = dst.end();
			dstIt != dstE; ++dstIt) {
		if (Instruction *I = dyn_cast<Instruction>(*dstIt)) {
			if (CallInst *cI = dyn_cast<CallInst>(*dstIt)) {
				for (unsigned int i = 0; i < cI->getNumArgOperands(); i++) {
					args.push_back(cI->getArgOperand(i));
				}
				v.push_back(
						std::pair<Instruction*, std::vector<Value*> >(I, args));
			}
		}
	}
	return (v);
}

//find and return the predicated used to gate the inst parameter.
Pred * blockAssign::getPredFromInst(Instruction *inst) {
	for (unsigned int i = 0; i < predicatesVector.size(); i++) {
		for (unsigned int j = 0; j < predicatesVector[i]->insts.size(); j++) {
			if (predicatesVector[i]->insts[j] == inst)
				return (predicatesVector[i]);
		}
	}
	return NULL;
}


//For each module, this method is executed
bool blockAssign::runOnModule(Module &M) {
	moduleDepGraph& DepGraph = getAnalysis<moduleDepGraph>();

	Function *F;


	//Getting dependency graph
	Graph *g = DepGraph.depGraph;

	for (Module::iterator Mit = M.begin(), Mend = M.end(); Mit != Mend; ++Mit) {
		F = Mit;
		if (F->begin() != F->end()) {
			if (IsOptimized) { 
				// Iterate over all Basic Blocks of the Function
				if (F->begin() != F->end()) {
                    DominatorTreeWrapperPass &DTW = getAnalysis<DominatorTreeWrapperPass>(*F);
                    DominatorTree &DT = DTW.getDomTree();
					PostDominatorTree &PD = getAnalysis<PostDominatorTree>(*F);
					for (Function::iterator Fit = F->begin(), Fend = F->end();
							Fit != Fend; ++Fit) {
						processNode(Fit, DT, PD);
					}
				}

			} else {
				// Iterate over all Basic Blocks of the Function
				if (F->begin() != F->end()) {
					PostDominatorTree &PD = getAnalysis<PostDominatorTree>(*F);
					for (Function::iterator Fit = F->begin(), Fend = F->end();
							Fit != Fend; ++Fit) {
						makeTable(Fit, PD); //Creating in memory the table with predicates and gated instructions
					}
				}

			}
		}
	}

    if(debug) printGate();
	//Including control edges into dependence graph.
//	incGraph(g);

	//Interates on all source code in order to get the sources of address (secret information) and sinks (instructions like printf)
	for (Module::iterator F = M.begin(), eM = M.end(); F != eM; ++F) {
		for (Function::iterator BB = F->begin(), e = F->end(); BB != e; ++BB) {
			for (BasicBlock::iterator I = BB->begin(), ee = BB->end(); I != ee;
					++I) {
				if (dyn_cast<Instruction>(I)->getOpcode()
						== Instruction::PtrToInt) {
					src.push_back(I);
				} else if (dyn_cast<Instruction>(I)->getOpcode()
						== Instruction::Alloca) {
					src.push_back(I);
				}

				//If is a function call
				if (CallInst *CI = dyn_cast<CallInst>(I)) {
					Function *Callee = CI->getCalledFunction();
					if (Callee) {
						StringRef Name = Callee->getName();
						//if is a print function
						if (Name.equals("printf")) {
							dst.push_back(I);
						} else if (Name.equals("fiprintf")) {
							dst.push_back(I);
						} else if (Name.equals("fprintf")) {
							dst.push_back(I);
						} else if (Name.equals("iprintf")) {
							dst.push_back(I);
						} else if (Name.equals("vfprintf")) {
							dst.push_back(I);
						} else if (Name.equals("vprintf")) {
							dst.push_back(I);
						} else if (Name.equals("fputc")) {
							dst.push_back(I);
						} else if (Name.equals("fputs")) {
							dst.push_back(I);
						} else if (Name.equals("putc")) {
							dst.push_back(I);
						} else if (Name.equals("putchar")) {
							dst.push_back(I);
						} else if (Name.equals("puts")) {
							dst.push_back(I);
						} else if (Name.equals("fwrite")) {
							dst.push_back(I);
						} else if (Name.equals("pwrite")) {
							dst.push_back(I);
						} else if (Name.equals("write")) {
							dst.push_back(I);
						}

						//If is a source of address
						if (Name.equals("malloc")) {
							src.push_back(I);
						} else if (Name.equals("calloc")) {
							src.push_back(I);
						} else if (Name.equals("realloc")) {
							src.push_back(I);
						} else if (Name.equals("realloccf")) {
							src.push_back(I);
						}
						if (Name.equals("valloc")) {
							src.push_back(I);
						}
					}
				}
			}
		}
	}

	//Stats about sources and sinks
	numSources = src.size();
	numSink = dst.size();
	numControlEdges = g->getNumControlEdges();

	unsigned int countWarning = 0;
	unsigned int totalControlEdges = 0, totalDataEdges = 0;
	unsigned int totalNodes = 0;

	g->findTaintedNodes(src, dst);

	//numDirtyNodes = g->countTaintedNodes();
	this->newGraph = g;

	/*
	 Graph::Guider * guider = new Graph::Guider(g);
	 std::string s;

	 for (Graph::iterator gS = g->begin(), gE = g->end(); gS != gE; ++gS) {
	 if ((*gS)->tainted) {
	 s = "[label=\"";
	 s += (*gS)->getLabel() + "\" shape=\"" + (*gS)->getShape()
	 + "\" color=\"red\", style=solid]";
	 guider->setNodeAttrs((*gS), s);
	 std::map<GraphNode*, edgeType> suc = (*gS)->getSuccessors();
	 for (std::map<GraphNode*, edgeType>::iterator sucS = suc.begin(),
	 sucE = suc.end(); sucS != sucE; ++sucS) {
	 s = "[color=red ";
	 if (sucS->second == etControl)
	 s += "style=dashed]";
	 else
	 s += "]";
	 guider->setEdgeAttrs((*gS), sucS->first, s);
	 }
	 } else {
	 s = "[label=\"";
	 s += (*gS)->getLabel() + "\" shape=\"" + (*gS)->getShape()
	 + "\" , style=solid]";
	 guider->setNodeAttrs((*gS), s);
	 std::map<GraphNode*, edgeType> suc = (*gS)->getSuccessors();
	 for (std::map<GraphNode*, edgeType>::iterator sucS = suc.begin(),
	 sucE = suc.end(); sucS != sucE; ++sucS) {
	 s = "[ ";
	 if (sucS->second == etControl)
	 s += "style=dashed]";
	 else
	 s += "]";
	 guider->setEdgeAttrs((*gS), sucS->first, s);
	 }
	 }
	 }
	 std::string name = "fullPath";
	 std::string ErrorInfo("");
	 std::string fileName = name + ".dot";
	 raw_fd_ostream File(fileName.c_str(), ErrorInfo);
	 g->toDot(name, &File, guider);
	 */

	return false;
}

void blockAssign::processNode(BasicBlock * node, DominatorTree &DT,
		PostDominatorTree &PD) {
	//BasicBlock *BB = node->getBlock();
	BasicBlock *BB = node;
	Value *condition;
	TerminatorInst *ti = BB->getTerminator();
	BranchInst *bi = NULL;
	SwitchInst *si = NULL;
	Pred *predicate;
	DomTreeNode *nodeTemp = DT.getNode(node);

	if ((bi = dyn_cast<BranchInst>(ti)) && bi->isConditional()) { //If the terminator instruction is a conditional branch
		condition = bi->getCondition();
		//Including the predicate on the predicatesVector
		predicatesVector.push_back(new Pred(condition));
		//Gate childrens in the dominance tree
		for (unsigned int i = 0; i < nodeTemp->getNumChildren(); i++) {
			gateChildren(node, nodeTemp->getChildren()[i]->getBlock(),
					predicatesVector.back(), PD);
		}

	} else if ((si = dyn_cast<SwitchInst>(ti))) { //If the termination instruction is a switch instruction
		condition = si->getCondition();
		//Including the predicate on the predicatesVector
		predicatesVector.push_back(new Pred(condition));
		//Gate childrens in the dominance tree
		for (unsigned int i = 0; i < nodeTemp->getNumChildren(); i++) {
			gateChildren(node, nodeTemp->getChildren()[i]->getBlock(),
					predicatesVector.back(), PD);
		}
	} else if ((bi = dyn_cast<BranchInst>(ti)) && bi->isUnconditional()) { //If the terminator instruction is not a conditional branch
		if ((predicate = getPredFromInst(bi)) != NULL) {
			//Gate childrens in the dominance tree
			for (unsigned int i = 0; i < nodeTemp->getNumChildren(); i++) {
				gateChildren(node, nodeTemp->getChildren()[i]->getBlock(),
						predicate, PD);
			}
		}
	}
}

void blockAssign::gateChildren(BasicBlock *father, BasicBlock *child, Pred *p,
		PostDominatorTree &PD) {

	BasicBlock *bBOring = father;
	BasicBlock *bBSuss = child;
	BasicBlock *temp;

	
	if (dyn_cast<Instruction>(p->getPred()) != NULL)
		temp = dyn_cast<Instruction>(p->getPred())->getParent();
	else
		return;

	//If the actual basic block post dominates the predicate's basic block
	if (PD.dominates(bBSuss, temp) && (bBSuss != temp)) {
		//Find PHI instructions
		for (BasicBlock::iterator bBIt = bBSuss->begin(), bBEnd = bBSuss->end();
				bBIt != bBEnd; ++bBIt) {
			if (dyn_cast<Instruction>(bBIt)->getOpcode() == Instruction::PHI) {
				//for (unsigned int k=0; k<dyn_cast<Instruction>(bBIt)->getNumOperands(); k++) {
				//if (p->isGated(dyn_cast<Instruction>(dyn_cast<Instruction>(bBIt)->getOperand(k)))) {
				p->addInst(bBIt);
				//break;
				//}
				//}
			}
		}
		return;

	} else {
		//Instruction will be gated whit the bBOring predicate
		for (BasicBlock::iterator bBIt = bBSuss->begin(), bBEnd = bBSuss->end();
				bBIt != bBEnd; ++bBIt) {
			//If is a function call which is defined on the same module
			if (CallInst *CI = dyn_cast<CallInst>(&(*bBIt))) {
				Function *F = CI->getCalledFunction();
				if (F != NULL)
					if (!F->isDeclaration() && !F->isIntrinsic()) {
						//Gate function just if it has not gated by this predicate
						if (!p->isFGated(F))
							gateFunction(F, p);
					}
			}

			//Gate the other instructions
			p->addInst(bBIt);

		}
	}
}

//Increase graph including control edges
void blockAssign::incGraph(Graph *g) {
	unsigned int i;
	int j;

	//For all predicates in predicatesVector
	for (i = 0; i < predicatesVector.size(); i++) {
		//Locates the predicate (icmp instrution) from the graph
		GraphNode *predNode = g->findNode(predicatesVector[i]->getPred());

		//For each predicate, iterates on the list of gated INSTRUCTIONS
		for (j = 0; j < predicatesVector[i]->getNumInstrucoes(); j++) {
			GraphNode *instNode = g->findNode(predicatesVector[i]->getInst(j));
			if (predNode != NULL && instNode != NULL) {	//If the instruction is on the graph, make a edge
				g->addEdge(predNode, instNode, etControl);
			}
		}


		 //For each predicate, iterates on the list of gated FUNCTIONS
		for (j=0; j<predicatesVector[i]->getNumFunctions(); j++) {
			Function *F = predicatesVector[i]->getFunc(j);
			//For each function, iterates on its basic blocks
			for (Function::iterator Fit = F->begin(), Fend = F->end(); Fit != Fend; ++Fit) {
				//For each basic block, iterates on its instructions
				for (BasicBlock::iterator bBIt = Fit->begin(), bBEnd = Fit->end(); bBIt != bBEnd; ++bBIt) {
					GraphNode *instNode = g->findNode(bBIt);
					if (predNode != NULL && instNode != NULL)
						g->addEdge(predNode, instNode, etControl);
				}
			}
		}


	/*	for (j = 0; j < predicatesVector[i]->getNumFunctions(); j++) {
			Function *F = predicatesVector[i]->getFunc(j);
			g->addJoin(F);
			GraphNode *jn = g->findJoinNode(F);
			g->addEdge(predNode, jn, etControl);
		} */

	}

}

//It receives a BasicBLock and makes table of predicates and its respective gated instructions
void blockAssign::makeTable(BasicBlock *BB, PostDominatorTree &PD) {
	Value *condition;
	TerminatorInst *ti = BB->getTerminator();
	BranchInst *bi = NULL;
	SwitchInst *si = NULL;
	Pred *predicate;

	ProcessedBB.clear();
	if ((bi = dyn_cast<BranchInst>(ti)) && bi->isConditional()) { //If the terminator instruction is a conditional branch
		condition = bi->getCondition();
		//Including the predicate on the predicatesVector
		predicate = new Pred(condition);
		predicatesVector.push_back(predicate);
		//Make a "Flooding" on each sucessor gated the instruction on Influence Region of the predicate
		for (unsigned int i = 0; i < bi->getNumSuccessors(); i++) {
			findIR(BB, bi->getSuccessor(i), PD, predicate);
		}
	} else if ((si = dyn_cast<SwitchInst>(ti))) {
		condition = si->getCondition();
		//Including the predicate on the predicatesVector
		predicate = new Pred(condition);
		predicatesVector.push_back(predicate);
		//Make a "Flooding" on each sucessor gated the instruction on Influence Region of the predicate
		for (unsigned int i = 0; i < si->getNumSuccessors(); i++) {
			findIR(BB, si->getSuccessor(i), PD, predicate);
		}
	}
}

//Flooding until reach a posdominator node
void blockAssign::findIR(BasicBlock *bBOring, BasicBlock *bBSuss,
		PostDominatorTree &PD, Pred *p) {
	BasicBlock *temp;
	TerminatorInst *ti = bBSuss->getTerminator();

	
	if (dyn_cast<Instruction>(p->getPred()) != NULL)
		temp = dyn_cast<Instruction>(p->getPred())->getParent();
	else
		return;

	//If the basic block has been processed, do not advance
	for (unsigned int x = 0; x < ProcessedBB.size(); x++) {
		if (ProcessedBB[x] == bBSuss) {
			return;
		}
	}

	//Including the basic block in the processed vector
	ProcessedBB.push_back(bBSuss);

	//If the basic block is a post dominator and is not the start basic block, just return
	if (PD.dominates(bBSuss, bBOring) && bBSuss != bBOring) {
		//Find PHI instructions
		for (BasicBlock::iterator bBIt = bBSuss->begin(), bBEnd = bBSuss->end();
				bBIt != bBEnd; ++bBIt) {
			if (dyn_cast<Instruction>(bBIt)->getOpcode() == Instruction::PHI) {
				//if there is a PHI's argument gated, gate the PHI instruction
				//for (unsigned int k=0; k<dyn_cast<Instruction>(bBIt)->getNumOperands(); k++) {
				//if (p->isGated(dyn_cast<Instruction>(dyn_cast<Instruction>(bBIt)->getOperand(k)))) {
				p->addInst(bBIt);
				//break;
				//}
				//}
			}
		}
		return;
	} else { //Advance the flooding
		//Instruction will be gated whit the bBOring predicate
		for (BasicBlock::iterator bBIt = bBSuss->begin(), bBEnd = bBSuss->end();
				bBIt != bBEnd; ++bBIt) {

			//If is a function call which is defined on the same module
			if (CallInst *CI = dyn_cast<CallInst>(&(*bBIt))) {
				Function *F = CI->getCalledFunction();
				if (F != NULL)
					if (!F->isDeclaration() && !F->isIntrinsic()) {
						//Gate just if this predicate not yet dominates this call
						if (!p->isFGated(F))
							gateFunction(F, p);
					}
			}

			//Gate the other instructions
			p->addInst(bBIt);
		}
		//If there is successor, go there
		for (unsigned int i = 0; i < ti->getNumSuccessors(); i++) {
			findIR(bBOring, ti->getSuccessor(i), PD, p);
		}
	}

}

//All instrutions of function F are gated with predicate p
void blockAssign::gateFunction(Function *F, Pred *p) {
	p->addFunc(F);
}

//Print all predicates and its respective gated instructions
void blockAssign::printGate() {
	for (unsigned int i = 0; i < predicatesVector.size(); i++) {
		errs() << "\n\n" << predicatesVector[i]->getPred()->getName() << "\n";
		for (int j = 0; j < predicatesVector[i]->getNumInstrucoes(); j++) {
			errs() << "  " << predicatesVector[i]->getInst(j)->getOpcodeName()
					<< "\n";

		}
	}

}

char blockAssign::ID = 0;
static RegisterPass<blockAssign> X("blockAssign",
		"Block Assignment");

