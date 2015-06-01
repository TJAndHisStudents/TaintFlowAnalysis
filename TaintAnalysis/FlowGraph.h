#ifndef FLOWGRAPH_H_
#define FLOWGRAPH_H_

#ifndef DEBUG_TYPE
#define DEBUG_TYPE "Flowgraph"
#endif

#define USE_ALIAS_SETS true

#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/DominanceFrontier.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/IR/CallSite.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "AliasSets.h"
#include <deque>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include "llvm/IR/DebugInfo.h"
#include "InputDep.h"

using namespace std;

 namespace llvm
{

STATISTIC(NrOpNodes, "Number of operation nodes");
STATISTIC(NrVarNodes, "Number of variable nodes");
STATISTIC(NrMemNodes, "Number of memory nodes");
STATISTIC(NrEdges, "Number of edges");


typedef enum {
        etData = 0, etControl = 1
} edgeType;

/*
 * Class GraphNode
 *
 * This abstract class can do everything a simple graph node can do:
 *              - It knows the nodes that points to it
 *              - It knows the nodes who are pointed by it
 *              - It has a unique ID that can be used to identify the node
 *              - It knows how to connect itself to another GraphNode
 *
 * This class provides virtual methods that makes possible printing the graph
 * in a fancy .dot file, providing for each node:
 *              - Label
 *              - Shape
 *              - Style
 *
 */

class MemoryOperation
{
    public:
    BasicBlock* parentBlock;
    Function * parentFunction;
    int lineNumber;
    Value* assignedValue;
    string IdValue;
    Value * MemInst;
    bool hasVal = false;
    bool isParam;
    MemoryOperation();
    void setVal();

}
;


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
      //  std::string Name;
        MemoryOperation* defLoc = new MemoryOperation();

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

        void connect(GraphNode* dst, edgeType type = etData);
        int getClass_Id() const;
        int getId() const;
        std::string getName();
        virtual std::string getLabel() = 0;
        virtual std::string getShape() = 0;
        virtual std::string getStyle();

        virtual GraphNode* clone() = 0;
};

/*
 * Class OpNode
 *
 * This class represents the operation nodes:
 *              - It has a OpCode that is compatible with llvm::Instruction OpCodes
 *              - It may or may not store a value, that is the variable defined by the operation
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
 *              - It stores the pointer to the corresponding Value*
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
 * This class represents AliasSets of pointer values:
 *              - It stores the ID of the AliasSet
 *              - It provides a method to get access to all the Values contained in the AliasSet
 */
class MemNode: public GraphNode {
private:
        int aliasSetID;
        AliasSets *AS;
public:
        MemNode(int aliasSetID, AliasSets *AS) :
                aliasSetID(aliasSetID), AS(AS) {
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

        //Node stores the defined location..!
        std::pair<BasicBlock*,Value*> defLocation;
        std::set<Value*> getAliases();

        std::string getLabel();
        std::string getShape();
        GraphNode* clone();
        std::string getStyle();

        int getAliasSetId() const;
};




class  FuncParamInfo
{
public:
  //  Function * F;
     map<Value*,GraphNode*>  ParamNodeMap;
    FuncParamInfo();
};





class CallSiteMap{
public:
    CallSite * cs;
    Value * val;
    Function * CalledFunc;
    CallInst * CI;
    int frequency;
    int num_Params;
     map<Value*,set<MemoryOperation*> > LiveMap;
     map<Value*, GraphNode*> ArgNodeMap;
     CallSiteMap();
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

        AliasSets *AS;

        bool isValidInst(Value *v); //Return true if the instruction is valid for dependence graph construction



public:

	typedef std::set<GraphNode*>::iterator iterator;

        std::set<GraphNode*>::iterator begin();
        std::set<GraphNode*>::iterator end();

        map<Value*, GraphNode*> StoreNodeMap;

        map<Value*,set<MemoryOperation*> > RDSMap;
        map<Value*,set<MemoryOperation*> > LiveMap;
        map<Value*,set<MemoryOperation*> > BackupLiveMap;
        map<CallSite*, map<Value*,GraphNode*> > callParamNodeMap;
        
        map<CallInst *,CallSiteMap *>     CallSiteParams;

        map<Function*,FuncParamInfo*>   functionParamMap;
      //  map<Function*,FuncParamInfo*>   funcParMapLocal;



		Graph()
        {
                NrEdges = 0;
        }
        ;
        
        Graph(AliasSets *AS) :
                AS(AS) {
                NrEdges = 0;
        }
        ; //Constructor 
        ~Graph(); //Destructor - Free adjacent matrix's memory

        std::set<GraphNode*> getDepValues(
        			std::set<llvm::Value*> sources, bool forward=true);
	    int getTaintedEdges ();
        int getTaintedNodesSize ();

        GraphNode* addInst(Value *v,int Instcounter, Function* Func); //Add an instruction into Dependence Graph
        GraphNode* addInst_new(Value *v);  //Adds the instruction in dep graph..updated.

        void addEdge(GraphNode* src, GraphNode* dst, edgeType type = etData);

        GraphNode* findNode(Value *op); //Return the pointer to the node or NULL if it is not in the graph
        std::set<GraphNode*> findNodes(std::set<Value*> values);

        OpNode* findOpNode(Value *op); //Return the pointer to the node or NULL if it is not in the graph

        std::set<GraphNode*> getNodes();

        //print graph in dot format
        class Guider {
        public:
                Guider(Graph* graph);
                std::string getNodeAttrs(GraphNode* n);
                std::string getEdgeAttrs(GraphNode* u, GraphNode* v);
                void setNodeAttrs(GraphNode* n, std::string attrs);
                void setEdgeAttrs(GraphNode* u, GraphNode* v, std::string attrs);
                void clear();
        private:
                Graph* graph;
                DenseMap<GraphNode*, std::string> nodeAttrs;
                DenseMap<std::pair<GraphNode*, GraphNode*>, std::string> edgeAttrs;
        };
        void toDot(std::string s); //print in stdErr
        void toDot(std::string s, std::string fileName); //print in a file
        void toDotLines(std::string s, std::string fileName); //print in a file
        void toDot(std::string s, raw_ostream *stream); //print in any stream
        void toDotLines(std::string s, raw_ostream *stream); //print in any stream
        void toDot(std::string s, raw_ostream *stream, llvm::Graph::Guider* g);

        Graph generateSubGraph(Value *src, Value *dst); //Take a source value and a destination value and find a Connecting Subgraph from source to destination
        void findTaintedNodes (std::vector<Value *> src, std::vector<Value *> dst); //Take a source value and a destination value and mark all nodes from source to destination

        void dfsVisit(GraphNode* u, GraphNode* u2, std::set<GraphNode*> &visitedNodes); //Used by findConnectingSubgraph() method
        void dfsVisitBack(GraphNode* u, GraphNode* u2, std::set<GraphNode*> &visitedNodes); //Used by findConnectingSubgraph() method
        void dfsVisitMemo(GraphNode* u, std::set<GraphNode*> &visitedNodes); //Used by findConnectingSubgraph() method
        void dfsVisitBackMemo(GraphNode* u, std::set<GraphNode*> &visitedNodes); //Used by findConnectingSubgraph() method

           bool isMemoryPointer(Value *v); //Return true if the value is a memory pointer

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



//Remove this once everything is made consistent
class CallSiteInfo
{
    public:
    BasicBlock* parentBlock;
    Function * parentFunction;
    Function * CalledFunction; //could be empty for functions without body or library functions
    Value * Inst;
    CallSite * cs;
    CallInst * CI;
    map<Value*,GraphNode*>  ArgNodeMap;
    CallSiteInfo();
}
;




}

#endif

