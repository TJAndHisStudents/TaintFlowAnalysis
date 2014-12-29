#ifndef DEPGRAPH_H_
#define DEPGRAPH_H_

#ifndef DEBUG_TYPE
#define DEBUG_TYPE "buildGraph"
#endif

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/User.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/ilist.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/Support/CallSite.h"
#include "llvm/IR/Metadata.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"
#include "llvm/DebugInfo.h"
#include "llvm/ADT/StringRef.h"
#include <deque>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include "llvm/DebugInfo.h"
#include "InputPass.h"
#include "libcallSummary.h"



using namespace std;

namespace llvm
{

STATISTIC(NrOpNodes, "Number of operation nodes");
STATISTIC(NrVarNodes, "Number of variable nodes");
STATISTIC(NrMemNodes, "Number of memory nodes");
STATISTIC(NrEdges, "Number of edges");

typedef enum {
        etypeData = 0, etypeControl = 1
} edgeType;



class GraphNode {
private:
        std::map<GraphNode*, edgeType> successors;
        std::map<GraphNode*, edgeType> predecessors;

        static int currentID;
        int ID;


protected:
        int Class_ID;
public:
        bool tainted;
        GraphNode();
        GraphNode(GraphNode &G);

        virtual ~GraphNode();

        static inline bool classof(const GraphNode *N) {
                return true;
        }
        ;
        std::map<GraphNode*, edgeType> getSuccessors();
        bool hasSuccessor(GraphNode* succ);

        std::map<GraphNode*, edgeType> getPredecessors();
        bool hasPredecessor(GraphNode* pred);

        void connect(GraphNode* dst, edgeType type = etypeData);
        int getClass_Id() const;
        int getId() const;
        std::string getName();
        string label;
        virtual std::string getLabel() = 0;
        virtual std::string getShape() = 0;
        virtual std::string getStyle();

        virtual GraphNode* clone() = 0;
};

/*
 * Class OpNode
 *
 * This class represents the operation nodes, opcode compatible with llvm:instruction opcodes.
 *
 */
class OpNode: public GraphNode {
private:
        unsigned int OpCode;
        Value* value;
public:
        OpNode(int OpCode) :
                GraphNode(), OpCode(OpCode), value(NULL) {
                this->Class_ID = 1;
                NrOpNodes++;
        }
        ;
        OpNode(int OpCode, Value* v) :
                GraphNode(), OpCode(OpCode), value(v) {
                this->Class_ID = 1;
                NrOpNodes++;
        }
        ;
        ~OpNode() {
                NrOpNodes--;
        }
        ;
        static inline bool classof(const GraphNode *N) {
                return N->getClass_Id() == 1 || N->getClass_Id() == 3;
        }
        ;
        unsigned int getOpCode() const;
        void setOpCode(unsigned int opCode);
        Value* getValue();

        std::string getLabel();
        std::string getShape();

        GraphNode* clone();
};


/*
 * Class CallNode
 *
 * This class represents operation nodes of llvm::Call instructions:
 *              - It stores the pointer to the called function
 */
class CallNode: public OpNode {
private:
        CallInst* CI;
public:
        CallNode(CallInst* CI) :
                OpNode(Instruction::Call, CI), CI(CI) {
                this->Class_ID = 3;
        }
        ;
        static inline bool classof(const GraphNode *N) {
                return N->getClass_Id() == 3;
        }
        ;
        Function* getCalledFunction() const;
	
	CallInst* getCallInst() const;

        std::string getLabel();
        std::string getShape();

        GraphNode* clone();
};


/*
 * Class VarNode
 *
 * This class represents variables and constants which are not pointers:
 *              It stores the pointer to the corresponding Value*
 */
class VarNode: public GraphNode {
private:
        Value* value;
public:
        VarNode(Value* value) :
                GraphNode(), value(value) {
                this->Class_ID = 2;
                NrVarNodes++;
        }
        ;
        ~VarNode() {
                NrVarNodes--;
        }
        static inline bool classof(const GraphNode *N) {
                return N->getClass_Id() == 2;
        }
        ;
        Value* getValue();

        std::string getLabel();
        std::string getShape();

        GraphNode* clone();
};


/*
 * Class MemNode
 *
 * This class represents set of alias pointers (although not yet updated):
 *             
 */
class MemNode: public GraphNode {
private:
        int aliasSetID;
public:
        MemNode(int aliasSetID) :
                aliasSetID(aliasSetID) {
                this->Class_ID = 4;
                NrMemNodes++;
        }
        ;
        ~MemNode() {
                NrMemNodes--;
        }
        ;
        static inline bool classof(const GraphNode *N) {
                return N->getClass_Id() == 4;
        }
        ;
        
       // std::set<Value*> getAliases();

        std::string getLabel();
        std::string getShape();
        GraphNode* clone();
        std::string getStyle();

        int getAliasSetId() const;
};


/*
 * Class Graph
 *
 * Stores a set of nodes. Each node knows how to go to other nodes.
 *
 * The class provides methods to:
 *              - Find specific nodes
 *              - Delete specific nodes
 *              - Print the graph
 *
 */
//Dependence Graph
class Graph {
private:

        llvm::DenseMap<Value*, GraphNode*> opNodes;
        llvm::DenseMap<Value*, GraphNode*> callNodes;

        llvm::DenseMap<Value*, GraphNode*> varNodes;
        llvm::DenseMap<int, GraphNode*> memNodes;

        std::set<GraphNode*> nodes;


        bool isMemoryPointer(Value *v); //Return true if the value is a memory pointer

public:

	typedef std::set<GraphNode*>::iterator iterator;

        std::set<GraphNode*>::iterator begin();
        std::set<GraphNode*>::iterator end();

        Graph()
             {
                NrEdges = 0;
        }
        ; //Constructor
        ~Graph(); //Destructor 

        std::set<GraphNode*> getDepValues(
        			std::set<llvm::Value*> sources, bool forward=true);
	    int getTaintedEdges ();
        int getTaintedNodesSize ();
        
        bool isValidInst(Value *v); //Return true if the instruction is valid for dependence graph construction
        GraphNode* addInst(Value *v); //Add an instruction into Dependence Graph
		GraphNode* addInst(Value *v, string context);
		
		
        void addEdge(GraphNode* src, GraphNode* dst, edgeType type = etypeData);

        GraphNode* findNode(Value *op); //Return the pointer to the node or NULL if it is not in the graph
        std::set<GraphNode*> findNodes(std::set<Value*> values);

        OpNode* findOpNode(Value *op); //Return the pointer to the node or NULL if it is not in the graph

        std::set<GraphNode*> getNodes();

        void toDot(std::string s, std::string fileName); //print in a file
        void toDot(std::string s, raw_ostream *stream);

        Graph generateSubGraph(Value *src, Value *dst); //Take a source value and a destination value and find a Connecting Subgraph from source to destination
        void findTaintedNodes (std::vector<Value *> src, std::vector<Value *> dst); //Take a source value and a destination value and mark all nodes from source to destination

        void dfsVisit(GraphNode* u, GraphNode* u2, std::set<GraphNode*> &visitedNodes); //Used by findConnectingSubgraph() method
        void dfsVisitBack(GraphNode* u, GraphNode* u2, std::set<GraphNode*> &visitedNodes); //Used by findConnectingSubgraph() method
        void dfsVisitMemo(GraphNode* u, std::set<GraphNode*> &visitedNodes); //Used by findConnectingSubgraph() method
        void dfsVisitBackMemo(GraphNode* u, std::set<GraphNode*> &visitedNodes); //Used by findConnectingSubgraph() method

        void deleteCallNodes(Function* F);

        /*
         * Function getNearestDependence
         *
         * Given a sink, returns the nearest source in the graph and the distance to the nearest source
         */
        std::pair<GraphNode*, int> getNearestDependency(Value* sink,
                        std::set<Value*> sources, bool skipMemoryNodes);

        /*
         * Function getEveryDependency
         *
         * Given a sink, returns shortest path to each source (if it exists)
         */
        std::map<GraphNode*, std::vector<GraphNode*> > getEveryDependency(
                        llvm::Value* sink, std::set<llvm::Value*> sources,
                        bool skipMemoryNodes);

        int getNumOpNodes();
        int getNumCallNodes();
        int getNumMemNodes();
        int getNumVarNodes();
        int getNumDataEdges();
        int getNumControlEdges();
        int getNumEdges(edgeType type);
};


/*
 * Class CallReturnInfo
 *
 * Class to store the call chains
 *
 */
class CallReturnInfo {
	
	public:
		
		CallReturnInfo();
	
		BasicBlock* Call_Block;
		BasicBlock* Start_Block;
		BasicBlock* Returnto_Block;
		std::string end_block;
};



class BuildGraph : public ModulePass {
	private:
		std::set<Value*> inputDepValues;
	//	std::set<Function*> targetFunctions;
		std::map<Value*, int> lineNo;
	//	std::set<std::string>  targetNames;
	//	bool runOnModule(Module &M);
	public:
		static char ID; // Pass identification, replacement for typeid.
		
		  BuildGraph() :
                ModulePass(ID), depGraph(NULL) {
        }
		void getAnalysisUsage(AnalysisUsage &AU) const;
        bool runOnModule(Module&);

        void matchParametersAndReturnValues(Function &F);
        void Process_BB(BasicBlock* BB, string end_funct,string context);
     //   void Process_wBB(set<BasicBlock*> w_BB, string end_funct);
        void deleteCallNodes(Function* F);

		std::set<CallReturnInfo*> CallRet_Info;
		std::stack<CallReturnInfo*> callStack;
		std::vector<FunctionInfo*> FunctionInfos;
		set<std::string> visitedBlocks;
        Graph* depGraph;
	
	//	bool isInputDependent(Value* V);
	//	std::set<Value*> getInputDepValues();
	//	std::set<std::string> getTargetNames();
	//	std::set<Function*> getTargetFunctions();
	//	int getLineNo(Value*);
	//	void printer();
	//	void ReadSources();
	//	void ReadTargets();
};

}

#endif
