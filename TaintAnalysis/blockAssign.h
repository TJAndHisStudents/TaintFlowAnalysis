//===- blockAssign.h - It makes a program intermediate representation which is a dependence graph with data and control edges ------------------*- C++ -*-===//
#ifndef LLVM_BSSA2_H
#define LLVM_BSSA2_H

#include "llvm/Pass.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Dominators.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Analysis/DominanceFrontier.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/SetOperations.h"
#include "llvm/Analysis/DominatorInternals.h"  //replacing this with DominanceFrontierImpl.h
//#include "llvm/Analysis/DominanceFrontierImpl.h"

//#include "llvm/Assembly/Writer.h" trying out a different header
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CFG.h"
#include "llvm/Support/Debug.h"
#include <deque>
#include <algorithm>
#include <vector>
#include <string>
#include "BuildGraph.h"
#include <sstream>
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include <stack>




namespace llvm {


	STATISTIC(numSources, "Number of secret information sources");
	STATISTIC(numSink, "Number of public channels (printf etc.)");
	STATISTIC(numControlEdges, "Number control edges in dependence graph");
	STATISTIC(numDirtyNodes, "Number of tainted nodes");


	// Auxiliar class for store one predicate and its gated instructions
	// This Class is used just for non-optimized version of algorithm. It find the IR for each basic block in CFG, using a flooding approach and gate the respective instructions
	// with basic block's predicate.
	class Pred {

		public:
			// \brief Gate an instruction
			//
			// \param i Instruction to be stored
			void addInst (Instruction *i);

			// \brief Return the respective stored predicate
			//
			// \return A value relate the predicate
			Value *getPred ();

			// \brief Return the gated instruction indexed by param i
			//
			// \param i index
			// \return The instruction
			Instruction *getInst (int i);

			// \brief Check if the Function content is gated
			//
			// \param The function
			// \return True if yes, false if not.
			bool isFGated (Function *f);

			// \brief Gate a function
			//
			// \param The function
			void addFunc (Function *f);

			// \brief Return the number of gated instructions
			//
			// \return The numer of gated instructions
			int getNumInstrucoes ();

			// \brief Constructor of the class.
			//
			// \param p The predicate
			Pred(Value *p);

			// \brief Return the gated function indexed by param i
			//
			// \param i The index
			// \return The function
			Function *getFunc (int i);

			// \brief Return the number of gated functions
			//
			// \return The number of gated functions
			int getNumFunctions ();

			// Vector for store gated instructions
			std::vector<Instruction *> insts;
		private:

			// Store the predicate
			Value *predicate;

			// Store the gated functions
			std::vector<Function *> funcs;

			// \brief Check if the instruction is gated
			//
			// \param i Instruction
			// \return True if yes, false if not
			bool isGated (Instruction *i);

	};



	class blockAssign : public ModulePass {

		public:

				//runOnModule - makes a depGraph with control edges using an optimized algorithm
				bool runOnModule(Module&);

				//Method constructor
				blockAssign() : ModulePass(ID) { }
					//optimized = IsOptimized;}

				//Pass identification
				static char ID;

				// Complete depGraph including data and control edges
				Graph *newGraph;




		private:

				bool optimized;

				//src stores the instructions which are source of secret information
				//dst stores the instructions  which are public output channel like printf()
				std::vector<Value *> src, dst;

				void getAnalysisUsage(AnalysisUsage &AU) const;

				// \brief Print the predicates and its respective gated instructions
				void printGate ();

				// \brief Increase graph including control edges
				//
				// \param g The dependence graph
				void incGraph (Graph *g);



				// \brief Return a set of all tainted values
				//
				// \return A set of tainted values
				std::set<Value *> getLeakedValues ();


				// \brief Return a vector of pairs (print instructions, parameters)
				//
				// \return The vector of pairs (instruction, vector)
				std::vector<std::pair<Instruction*, std::vector<Value*> > > getPrintfLeaks ();

				// Vector of predicates objects
				std::vector<Pred *> predicatesVector;

				// \brief It receives a BasicBLock and makes table of predicates and its respective gated instructions
				// \param b the initial BasicBlock
				// \param PD PostDominator Tree
				void makeTable (BasicBlock *b,  PostDominatorTree& PD );

				// \brief Flooding until reach a posDominator node and gate all instruction with p predicate
				//
				// \param borigin Start Basic Block
				// \param bdst Successor basic block
				// \para PD PostDominator tree in order to know the imediate post dominator
				// zpara p Predicate which is used to gate instructions found on IR
				void findIR (BasicBlock *borigin, BasicBlock *bdst, PostDominatorTree &PD, Pred *p);

				// \brief All instrutions of function F are gated with predicate p
				//
				// \param F The function
				// \param p The predicate
				void gateFunction (Function *F, Pred *p);

				// \brief Implement the optimization on gating instruction. Now, we make a pre-order in Dominator Tree in order to get transitivity
				//
				// \param node Initial Basic Block
				// \param DT Dominator Tree
				// \param PD Post Dominator Tree
                void processNode (BasicBlock *node, DominatorTree &DT, PostDominatorTree &PD );

				// \brief Gate the instructions in basic block children
				//
				// \param father Basic Block Father
				// \param children Basic Block children
				// \param p  predicate that terminates basic block father
				// \param PD Post Dominator Tree
				void gateChildren (BasicBlock *father, BasicBlock *children, Pred *p, PostDominatorTree &PD);

				// \brief If the instruction is gated return it, else return null
				//
				// \\param i The instruction
				Pred * getPredFromInst (Instruction *i);
 	};


}

#endif
