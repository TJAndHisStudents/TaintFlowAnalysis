#include "BuildGraph.h"

#define DEBUG_TYPE "buildGraph"

#define debug true

using namespace llvm;


static cl::opt<bool, false>
includeAllInstsInDepGraph("includeAllInstsInDepGraph", cl::desc("Include All Instructions In DepGraph."), cl::NotHidden);


/*
STATISTIC(nodeCount, "The number of nodes in the Graph");
STATISTIC(edgeCount, "The number of edges in the Graph");
STATISTIC(branchNodeCount, "The number of branch nodes in the Graph");
STATISTIC(NumInputValues, "The number of input values");
*/


/*
 * Class GraphNode
 */

GraphNode::GraphNode() {
        Class_ID = 0;
        ID = currentID++;
        tainted=false;
}


GraphNode::GraphNode(GraphNode &G) {
        Class_ID = 0;
        ID = currentID++;
        tainted = false;
}

//Destructor frees the successor and pred info for the given node.
GraphNode::~GraphNode() {

        for (std::map<GraphNode*, edgeType>::iterator pred = predecessors.begin(); pred
                        != predecessors.end(); pred++) {
                (*pred).first->successors.erase(this);
                NrEdges--;
        }

        for (std::map<GraphNode*, edgeType>::iterator succ = successors.begin(); succ
                        != successors.end(); succ++) {
                (*succ).first->predecessors.erase(this);
                NrEdges--;
        }

        successors.clear();
        predecessors.clear();
}

std::map<GraphNode*, edgeType> llvm::GraphNode::getSuccessors() {
        return successors;
}

std::map<GraphNode*, edgeType> llvm::GraphNode::getPredecessors() {
        return predecessors;
}

void llvm::GraphNode::connect(GraphNode* dst, edgeType type) {

        unsigned int curSize = this->successors.size();
        this->successors[dst] = type;
        dst->predecessors[this] = type;

        if (this->successors.size() != curSize) //Only count new edges
                NrEdges++;
}

int llvm::GraphNode::getClass_Id() const {
        return Class_ID;
}

int llvm::GraphNode::getId() const {
        return ID;
}

bool llvm::GraphNode::hasSuccessor(GraphNode* succ) {
        return successors.count(succ) > 0;
}

bool llvm::GraphNode::hasPredecessor(GraphNode* pred) {
        return predecessors.count(pred) > 0;
}

std::string llvm::GraphNode::getName() {
        std::ostringstream stringStream;
        stringStream << "node_" << getId();
        return stringStream.str();
}

std::string llvm::GraphNode::getStyle() {
        return std::string("solid");
}

int llvm::GraphNode::currentID = 0;

/*
 * Class OpNode
 */
unsigned int OpNode::getOpCode() const {
        return OpCode;
}

void OpNode::setOpCode(unsigned int opCode) {
        OpCode = opCode;
}

std::string llvm::OpNode::getLabel() {

        std::ostringstream stringStream;
        stringStream << Instruction::getOpcodeName(OpCode);
        if(label != "none")
        	return label+"_"+stringStream.str();
        else 
        	return stringStream.str();

}

std::string llvm::OpNode::getShape() {
        return std::string("octagon");
}

GraphNode* llvm::OpNode::clone() {

	OpNode* R = new OpNode(*this);
	R->Class_ID = this->Class_ID;
	return R;

}

llvm::Value* llvm::OpNode::getValue() {
        return value;
}


/*
 * Class CallNode
 */
Function* llvm::CallNode::getCalledFunction() const {
        return CI->getCalledFunction();
}

std::string llvm::CallNode::getLabel() {
        std::ostringstream stringStream;

        stringStream << "Call ";
        if (Function* F = getCalledFunction())
                stringStream << F->getName().str();
        else if (CI->hasName())
                stringStream << "*(" << CI->getName().str() << ")";
        else
                stringStream << "*(Unnamed)";

        return stringStream.str();
}

std::string llvm::CallNode::getShape() {
        return std::string("doubleoctagon");
}

GraphNode* llvm::CallNode::clone() {
	CallNode* R = new CallNode(*this);
	R->Class_ID = this->Class_ID;
	return R;
}

CallInst* llvm::CallNode::getCallInst() const {
	return this->CI;
}


/*
 * Class VarNode
 */
llvm::Value* VarNode::getValue() {
        return value;
}

std::string llvm::VarNode::getShape() {

        if (!isa<Constant> (value)) {
                return std::string("ellipse");
        } else {
                return std::string("box");
        }

}

std::string llvm::VarNode::getLabel() {

        std::ostringstream stringStream;

        if (!isa<Constant> (value)) {

                stringStream << value->getName().str();

        } else {

                if ( ConstantInt* CI = dyn_cast<ConstantInt>(value)) {
                        stringStream << CI->getValue().toString(10, true);
                } else {
                        stringStream << "Const:" << value->getName().str();
                }
        }

        return stringStream.str();

}

GraphNode* llvm::VarNode::clone() {
	VarNode* R = new VarNode(*this);
    	R->Class_ID = this->Class_ID;
    	return R;
}


/*
 * Class MemNode
 */
std::string llvm::MemNode::getLabel() {
        std::ostringstream stringStream;
        stringStream << "Memory " << aliasSetID;
        return stringStream.str();
}

std::string llvm::MemNode::getShape() {
        return std::string("ellipse");
}

GraphNode* llvm::MemNode::clone() {
	MemNode* R = new MemNode(*this);
    	R->Class_ID = this->Class_ID;
    	return R;
}

std::string llvm::MemNode::getStyle() {
        return std::string("dashed");
}

int llvm::MemNode::getAliasSetId() const {
        return aliasSetID;
}

/*
 * Class Graph
 */
std::set<GraphNode*>::iterator Graph::begin(){
	return(nodes.begin());
}

std::set<GraphNode*>::iterator Graph::end(){
	return(nodes.end());
}

Graph::~Graph() {
	for (std::set<GraphNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		GraphNode* g = *it;
		delete g;
	}

    nodes.clear();

}

llvm::DenseMap<GraphNode*, bool > taintedMap; 

int Graph::getTaintedEdges () {
	int countEdges=0;

	for (llvm::DenseMap<GraphNode*, bool>::iterator it = taintedMap.begin(); it != taintedMap.end(); ++it) {
		std::map<GraphNode*, edgeType> succs = it->first->getSuccessors();
		for (std::map<GraphNode*, edgeType>::iterator succ = succs.begin(), s_end = succs.end(); succ != s_end; succ++) {
			if (taintedMap.count(succ->first) > 0) {
				countEdges++;
			}
		}
	}
	return (countEdges);
}

int Graph::getTaintedNodesSize() {
	return (taintedMap.size());
}


void Graph::findTaintedNodes(std::vector<Value *> src, std::vector<Value *> dst) {

	        std::set<GraphNode*> visitedNodes1;
	        std::set<GraphNode*> visitedNodes2;

	        for (unsigned int i=0; i<src.size(); i++) {
	        	GraphNode* source = findOpNode(src[i]);
	        	if (!source)
	        		source = findNode(src[i]);
	        	if (source != NULL)
	        		dfsVisitMemo(source, visitedNodes1);
	        }


	        for (unsigned int i=0; i<dst.size(); i++) {
	        	GraphNode* destination = findNode(dst[i]);
	        	if (destination != NULL)
	        		dfsVisitBackMemo(destination, visitedNodes2);
	        }


	        //check the nodes visited in both directions
	        for (std::set<GraphNode*>::iterator it = visitedNodes1.begin(); it != visitedNodes1.end(); ++it) {
	                if (visitedNodes2.count(*it) > 0) {
	                	(*it)->tainted = true;
	                }
	        }

}

void Graph::dfsVisitMemo(GraphNode* u, std::set<GraphNode*> &visitedNodes) {

        visitedNodes.insert(u);

        std::map<GraphNode*, edgeType> succs = u->getSuccessors();

        for (std::map<GraphNode*, edgeType>::iterator succ = succs.begin(), s_end = succs.end(); succ != s_end; succ++) {
                if (visitedNodes.count(succ->first) == 0) {
                        dfsVisitMemo(succ->first, visitedNodes);
                }
        }

}

void Graph::dfsVisitBackMemo(GraphNode* u, std::set<GraphNode*> &visitedNodes) {

        visitedNodes.insert(u);

        std::map<GraphNode*, edgeType> preds = u->getPredecessors();

        for (std::map<GraphNode*, edgeType>::iterator pred = preds.begin(), s_end = preds.end(); pred != s_end; pred++) {
                if (visitedNodes.count(pred->first) == 0) {
                        dfsVisitBackMemo(pred->first, visitedNodes);
                }
        }

}

Graph Graph::generateSubGraph(Value *src, Value *dst) {
        Graph G; //= new Graph();

        std::map<GraphNode*, GraphNode*> nodeMap;

        std::set<GraphNode*> visitedNodes1;
        std::set<GraphNode*> visitedNodes2;


        GraphNode* source = findOpNode(src);
        if (!source) source = findNode(src);

        GraphNode* destination = findNode(dst);

        if (source == NULL || destination == NULL) {
                return G;
        }

        dfsVisit(source, destination, visitedNodes1);
        dfsVisitBack(destination, source, visitedNodes2);

        //check the nodes visited in both directions
        for (std::set<GraphNode*>::iterator it = visitedNodes1.begin(); it != visitedNodes1.end(); ++it) {
                if (visitedNodes2.count(*it) > 0) {
                        nodeMap[*it] = (*it)->clone();
                        if (taintedMap.count(*it)==0) {
                        	taintedMap[*it] = true;
                        }
                }
        }

        //connect the new vertices
        for (std::map<GraphNode*, GraphNode*>::iterator it = nodeMap.begin(); it != nodeMap.end(); ++it) {

                std::map<GraphNode*, edgeType> succs = it->first->getSuccessors();

                for (std::map<GraphNode*, edgeType>::iterator succ = succs.begin(), s_end = succs.end(); succ != s_end; succ++) {
                        if (nodeMap.count(succ->first) > 0) {
                                it->second->connect(nodeMap[succ->first], succ->second);
                        }
                }

                if ( !G.nodes.count(it->second)) {
                	G.nodes.insert(it->second);

                	if (isa<VarNode>(it->second)) {
                		G.varNodes[dyn_cast<VarNode>(it->second)->getValue()] = dyn_cast<VarNode>(it->second);
                	}

                	if (isa<MemNode>(it->second)) {
                		G.memNodes[dyn_cast<MemNode>(it->second)->getAliasSetId()] = dyn_cast<MemNode>(it->second);
                	}

                	if (isa<OpNode>(it->second)) {
                		G.opNodes[dyn_cast<OpNode>(it->second)->getValue()] = dyn_cast<OpNode>(it->second);

                		if (isa<CallNode>(it->second)) {
                    		G.callNodes[dyn_cast<CallNode>(it->second)->getCallInst()] = dyn_cast<CallNode>(it->second);

                    	}
                	}

                }

        }


        return G;
}


void Graph::dfsVisit(GraphNode* u, GraphNode* u2, std::set<GraphNode*> &visitedNodes) {


        visitedNodes.insert(u);

        if (u->getId() == u2->getId()) return;

        std::map<GraphNode*, edgeType> succs = u->getSuccessors();


        for (std::map<GraphNode*, edgeType>::iterator succ = succs.begin(), s_end =
                        succs.end(); succ != s_end; succ++) {
                if (visitedNodes.count(succ->first) == 0) {
                        dfsVisit(succ->first, u2, visitedNodes);
                }
        }

}

void Graph::dfsVisitBack(GraphNode* u, GraphNode* u2, std::set<GraphNode*> &visitedNodes) {

        visitedNodes.insert(u);

        if (u->getId() == u2->getId()) return;

        std::map<GraphNode*, edgeType> preds = u->getPredecessors();

        for (std::map<GraphNode*, edgeType>::iterator pred = preds.begin(), s_end =
                        preds.end(); pred != s_end; pred++) {
                if (visitedNodes.count(pred->first) == 0 && pred->first != u2) {
                        dfsVisitBack(pred->first, u2, visitedNodes);
                }
        }

}

void Graph::toDot(std::string s, const std::string fileName) {

        std::string ErrorInfo;

        raw_fd_ostream File(fileName.c_str(), ErrorInfo);

        if (!ErrorInfo.empty()) {
                errs() << "Error opening file " << fileName
                                << " for writing! Error Info: " << ErrorInfo << " \n";
                return;
        }

        this->toDot(s, &File);

}

void Graph::toDot(std::string s, raw_ostream *stream) {

        (*stream) << "digraph \"DFG for \'" << s << "\'  \"{\n";
        (*stream) << "label=\"DFG for \'" << s << "\' \";\n";



        std::map<GraphNode*, int> DefinedNodes;

        for (std::set<GraphNode*>::iterator node = nodes.begin(), end = nodes.end(); node
                        != end; node++) {

                if (DefinedNodes.count(*node) == 0) {
                        (*stream) << (*node)->getName() << "[shape=" << (*node)->getShape()
                                        << ",style=" << (*node)->getStyle() << ",label=\""
                                        << (*node)->getLabel() << "\"]\n";
                        DefinedNodes[*node] = 1;
                }

                std::map<GraphNode*, edgeType> succs = (*node)->getSuccessors();

                for (std::map<GraphNode*, edgeType>::iterator succ = succs.begin(),
                                s_end = succs.end(); succ != s_end; succ++) {

                        if (DefinedNodes.count(succ->first) == 0) {
                                (*stream) << (succ->first)->getName() << "[shape="
                                                << (succ->first)->getShape() << ",style="
                                                << (succ->first)->getStyle() << ",label=\""
                                                << (succ->first)->getLabel() << "\"]\n";
                                DefinedNodes[succ->first] = 1;
                        }

                        //Source
                        (*stream) << "\"" << (*node)->getName() << "\"";

                        (*stream) << "->";

                        //Destination
                        (*stream) << "\"" << (succ->first)->getName() << "\"";

                        if (succ->second == etypeControl)
                                (*stream) << " [style=dashed]";

                        (*stream) << "\n";

                }

        }

        (*stream) << "}\n\n";

}

GraphNode* Graph::addInst(Value *v) {

        GraphNode *Op, *Var, *Operand;

        CallInst* CI = dyn_cast<CallInst> (v);
        bool hasVarNode = true;

        if (isValidInst(v)) { //If is a data manipulator instruction
        
        //NT: added
        if(debug) {errs()<<"\nAdding inst :- " << *v;}
        
                Var = this->findNode(v);

                /*
                 * If Var is NULL, the value hasn't been processed yet, so we must process it
                 *
                 * However, if Var is a Pointer, maybe the memory node already exists but the
                 * operation node aren't in the graph, yet. Thus we must process it.
                 */
                if (Var == NULL || (Var != NULL && findOpNode(v) == NULL)) { //If it has not processed yet

                        //If Var isn't NULL, we won't create another node for it
                        if (Var == NULL) {

                                if (CI) {
                                        hasVarNode = !CI->getType()->isVoidTy();
                                }

                                if (hasVarNode) {
                                    if (StoreInst* SI = dyn_cast<StoreInst>(v))
                                                Var = addInst(SI->getOperand(1)); 
                                        else if ((!isa<Constant> (v)) && isMemoryPointer(v)) {
                                                Var = new MemNode(0); //adding the default memory node.. need to update this based on the stack offset computation
                                                memNodes[0] = Var;
                                        } else {
                                                Var = new VarNode(v);
                                                varNodes[v] = Var;
                                        }
                                        nodes.insert(Var);
                                        if(debug){errs()<<"     Node added :- " << (*Var).getLabel() << " size  " <<nodes.size();}
                                }

                        }

                        if (isa<Instruction> (v)) {

                                if (CI) {
                                        Op = new CallNode(CI);
                                        callNodes[CI] = Op;
                                } else {
                                        Op = new OpNode(dyn_cast<Instruction> (v)->getOpcode(), v);
                                }
                                opNodes[v] = Op;

                                nodes.insert(Op);
                                if (hasVarNode)
                                        Op->connect(Var);

                                //Connect the operands to the OpNode
                                for (unsigned int i = 0; i < cast<User> (v)->getNumOperands(); i++) {

                                      if (isa<StoreInst> (v) && i == 1)
                                               continue; 
										
                                        Value *v1 = cast<User> (v)->getOperand(i);
                                        Operand = this->addInst(v1);

                                        if (Operand != NULL)
                                                Operand->connect(Op);
                                }
                        }
                }

                return Var;
        }
        return NULL;
}

GraphNode* Graph::addInst(Value *v, string context) {

        GraphNode *Op, *Var, *Operand;

        CallInst* CI = dyn_cast<CallInst> (v);
        bool hasVarNode = true;

        if (isValidInst(v)) { //If is a data manipulator instruction
        
        //NT: added
        if(debug) {errs()<<"\nAdding inst :- " << *v;}
        
                Var = this->findNode(v);

                /*
                 * If Var is NULL, the value hasn't been processed yet, so we must process it
                 *
                 * However, if Var is a Pointer, maybe the memory node already exists but the
                 * operation node aren't in the graph, yet. Thus we must process it.
                 */
                if (Var == NULL || (Var != NULL && findOpNode(v) == NULL)) { //If it has not processed yet

                        //If Var isn't NULL, we won't create another node for it
                        if (Var == NULL) {

                                if (CI) {
                                        hasVarNode = !CI->getType()->isVoidTy();
                                }

                                if (hasVarNode) {
                                    if (StoreInst* SI = dyn_cast<StoreInst>(v))
                                                Var = addInst(SI->getOperand(1)); 
                                        else if ((!isa<Constant> (v)) && isMemoryPointer(v)) {
                                                Var = new MemNode(0); //adding the default memory node.. need to update this based on the stack offset computation
                                                memNodes[0] = Var;
                                        } else {
                                                Var = new VarNode(v);
                                                varNodes[v] = Var;
                                        }
                                        if(context != "none")
                                        	(*Var).label =context;
                                        nodes.insert(Var);
                                        if(debug){errs()<<"     Node added :- " << (*Var).getLabel() << " size  " <<nodes.size();}
                                }

                        }

                        if (isa<Instruction> (v)) {

                                if (CI) {
                                        Op = new CallNode(CI);
                                        callNodes[CI] = Op;
                                } else {
                                        Op = new OpNode(dyn_cast<Instruction> (v)->getOpcode(), v);
                                }
                                opNodes[v] = Op;

                                nodes.insert(Op);
                                if (hasVarNode)
                                        Op->connect(Var);

                                //Connect the operands to the OpNode
                                for (unsigned int i = 0; i < cast<User> (v)->getNumOperands(); i++) {

                                      if (isa<StoreInst> (v) && i == 1)
                                               continue; 
										
                                        Value *v1 = cast<User> (v)->getOperand(i);
                                        Operand = this->addInst(v1);

                                        if (Operand != NULL)
                                                Operand->connect(Op);
                                }
                        }
                }

                return Var;
        }
        return NULL;
}

void Graph::addEdge(GraphNode* src, GraphNode* dst, edgeType type) {

        nodes.insert(src);
        nodes.insert(dst);
        src->connect(dst, type);

}

//It verify if the instruction is valid for the dependence graph, i.e. just data manipulator instructions are important for dependence graph
bool Graph::isValidInst(Value *v) {

        if ((!includeAllInstsInDepGraph) && isa<Instruction> (v)) {

                //List of instructions that we don't want in the graph
                switch (cast<Instruction>(v)->getOpcode()) {

                        case Instruction::Br:
                        case Instruction::Switch:
                        case Instruction::Ret:
                                return false;

                }

        }

        if (v) return true;
        return false;

}

bool llvm::Graph::isMemoryPointer(llvm::Value* v) {
        if (v && v->getType())
                return v->getType()->isPointerTy();
        return false;
}

//Return the pointer to the node related to the operand.
//Return NULL if the operand is not inside map.
GraphNode* Graph::findNode(Value *op) {

		//errs()<<"\n Inside Find Node ... " << *op;
        if ((!isa<Constant> (op)) && isMemoryPointer(op)) {
                int index = 0; //Use the getAlias set id.. 
                if (memNodes.count(index))
                        return memNodes[index];
        } else {
                if (varNodes.count(op))
                        return varNodes[op];
        }

        return NULL;
}

std::set<GraphNode*> Graph::findNodes(std::set<Value*> values) {

        std::set<GraphNode*> result;

        for (std::set<Value*>::iterator i = values.begin(), end = values.end(); i
                        != end; i++) {
                if (GraphNode* node = findNode(*i)) {
                        result.insert(node);
                }

        }

        return result;
}

OpNode* llvm::Graph::findOpNode(llvm::Value* op) {

        if (opNodes.count(op))
                return dyn_cast<OpNode> (opNodes[op]);
        return NULL;
}

std::set<GraphNode*> llvm::Graph::getNodes() {
        return nodes;
}

void llvm::Graph::deleteCallNodes(Function* F) {

        for (Value::use_iterator UI = F->use_begin(), E = F->use_end(); UI != E; ++UI) {
                User *U = *UI;

                // Ignore blockaddress uses
                if (isa<BlockAddress> (U))
                        continue;

                // Used by a non-instruction, or not the callee of a function, do not
                // match.

                //FIXME: Deal properly with invoke instructions
                if (!isa<CallInst> (U))
                        continue;

                Instruction *caller = cast<Instruction> (U);

                if (callNodes.count(caller)) {
                        if (GraphNode* node = callNodes[caller]) {
                                nodes.erase(node);
                                delete node;
                        }
                        callNodes.erase(caller);
                }

        }

}

std::pair<GraphNode*, int> llvm::Graph::getNearestDependency(llvm::Value* sink,
                std::set<llvm::Value*> sources, bool skipMemoryNodes) {

        std::pair<llvm::GraphNode*, int> result;
        result.first = NULL;
        result.second = -1;

        if (GraphNode* startNode = findNode(sink)) {

                std::set<GraphNode*> sourceNodes = findNodes(sources);

                std::map<GraphNode*, int> nodeColor;

                std::list<std::pair<GraphNode*, int> > workList;

                for (std::set<GraphNode*>::iterator Nit = nodes.begin(), Nend =
                                nodes.end(); Nit != Nend; Nit++) {

                        if (skipMemoryNodes && isa<MemNode> (*Nit))
                                nodeColor[*Nit] = 1;
                        else
                                nodeColor[*Nit] = 0;
                }

                workList.push_back(pair<GraphNode*, int> (startNode, 0));

                /*
                 * we will do a breadth search on the predecessors of each node,
                 * until we find one of the sources. If we don't find any, then the
                 * sink doesn't depend on any source.
                 */

                while (workList.size()) {

                        GraphNode* workNode = workList.front().first;
                        int currentDistance = workList.front().second;

                        nodeColor[workNode] = 1;

                        workList.pop_front();

                        if (sourceNodes.count(workNode)) {

                                result.first = workNode;
                                result.second = currentDistance;
                                break;

                        }

                        std::map<GraphNode*, edgeType> preds = workNode->getPredecessors();

                        for (std::map<GraphNode*, edgeType>::iterator pred = preds.begin(),
                                        pend = preds.end(); pred != pend; pred++) {

                                if (nodeColor[pred->first] == 0) { // the node hasn't been processed yet

                                        nodeColor[pred->first] = 1;

                                        workList.push_back(
                                                        pair<GraphNode*, int> (pred->first,
                                                                        currentDistance + 1));

                                }

                        }

                }

        }

        return result;
}

std::map<GraphNode*, std::vector<GraphNode*> > llvm::Graph::getEveryDependency(
                llvm::Value* sink, std::set<llvm::Value*> sources, bool skipMemoryNodes) {

        std::map<llvm::GraphNode*, std::vector<GraphNode*> > result;
        DenseMap<GraphNode*, GraphNode*> parent;
        std::vector<GraphNode*> path;

              errs() << "--- Get every dep --- \n";
        if (GraphNode* startNode = findNode(sink)) {
                             errs() << "found sink\n";
		              errs() << "Starting search from " << startNode->getLabel() << "\n";
                std::set<GraphNode*> sourceNodes = findNodes(sources);
                errs() << "Source nodes size " << sourceNodes.size() << "\n";
                std::map<GraphNode*, int> nodeColor;
                std::list<GraphNode*> workList;
                //              int size = 0;
                for (std::set<GraphNode*>::iterator Nit = nodes.begin(), Nend =
                                nodes.end(); Nit != Nend; Nit++) {
                        //                      size++;
                        if (skipMemoryNodes && isa<MemNode> (*Nit))
                                nodeColor[*Nit] = 1;
                        else
                                nodeColor[*Nit] = 0;
                }

                workList.push_back(startNode);
                nodeColor[startNode] = 1;
                /*
                 * we will do a breadth search on the predecessors of each node,
                 * until we find one of the sources. If we don't find any, then the
                 * sink doesn't depend on any source.
                 */
                //              int pb = 1;
                while (!workList.empty()) {
                	 errs() << "Worklist not empty.. "<< workList.size()<<"\n";
                        GraphNode* workNode = workList.front();
                        workList.pop_front();
                        if (sourceNodes.count(workNode)) {
                        	 errs() << "sourceNodes.count(workNode) .. \n";
                                //Retrieve path
                                path.clear();
                                GraphNode* n = workNode;
                                path.push_back(n);
                                while (parent.count(n)) {
                                        path.push_back(parent[n]);
                                        n = parent[n];
                                }
//                                std::reverse(path.begin(), path.end());
                                //                              errs() << "Path: ";
                                //                              for (std::vector<GraphNode*>::iterator i = path.begin(), e = path.end(); i != e; ++i) {
                                //                                      errs() << (*i)->getLabel() << " | ";
                                //                              }
                                //                              errs() << "\n";
                                result[workNode] = path;
                        }
                        std::map<GraphNode*, edgeType> preds = workNode->getPredecessors();
                        errs()<<"The size of the predecessors  :" <<preds.size();
                        for (std::map<GraphNode*, edgeType>::iterator pred = preds.begin(),
                                        pend = preds.end(); pred != pend; pred++) {
                                if (nodeColor[pred->first] == 0) { // the node hasn't been processed yet
                                        nodeColor[pred->first] = 1;
                                        workList.push_back(pred->first);
                                        //                                      pb++;
                                        parent[pred->first] = workNode;
                                }
                        }
                                        //      errs() << pb << "/" << size << "\n";
                }
        }
        return result;
}


int llvm::Graph::getNumOpNodes() {
        return opNodes.size();
}

int llvm::Graph::getNumCallNodes() {
        return callNodes.size();
}

int llvm::Graph::getNumMemNodes() {
        return memNodes.size();
}

int llvm::Graph::getNumVarNodes() {
        return varNodes.size();
}

int llvm::Graph::getNumEdges(edgeType type){

        int result = 0;

    for (std::set<GraphNode*>::iterator node = nodes.begin(), end = nodes.end(); node
                     != end; node++) {

             std::map<GraphNode*, edgeType> succs = (*node)->getSuccessors();

             for (std::map<GraphNode*, edgeType>::iterator succ = succs.begin(),
                             s_end = succs.end(); succ != s_end; succ++) {

                     if (succ->second == type) result++;

             }

     }

    return result;

}

int llvm::Graph::getNumDataEdges() {
        return getNumEdges(etypeData);
}

int llvm::Graph::getNumControlEdges() {
        return getNumEdges(etypeControl);
}

//Class CallReturnInfo
CallReturnInfo::CallReturnInfo()
{
}

void BuildGraph::Process_BB(BasicBlock* BB, string end_funct, string context)
{
	string BB_Name = BB->getName();
	bool functendBlock = false;

	if((visitedBlocks.find(BB_Name) == visitedBlocks.end()) && (BB_Name!="invalid_address")) // && (BB->getName() != "switch")
	{
	  //Check if it is a call or return block.. if so then for calls ignore the switch
	   std::vector<FunctionInfo*>::iterator FI;
	  
	 // if(BB_Name == 
	 
	 
		visitedBlocks.insert(BB_Name);
  		errs() << "\nProcessing Block " << BB_Name; 
		for (BasicBlock::iterator Iit = BB->begin(), Iend = BB->end(); Iit!= Iend; ++Iit) {
			if(context != "none")
				depGraph->addInst(Iit,context);
			else
            	depGraph->addInst(Iit);
    	}
    	
    	//Check if it the end of any function call .. if so then check the top element of stack and pop if matched. to get to the successor
    	for(FI= FunctionInfos.begin(); FI!=FunctionInfos.end(); ++FI)
		{
		   if(BB->getName() == (*FI)->end_block) 
			{
		//		errs()<<"\n Found the end block\n";
				//check for match in stack..
				CallReturnInfo *retBlock = new CallReturnInfo();
				if(callStack.size()!=0)
				 {retBlock = callStack.top();
				
			//	errs()<<"\n Got the top block assigned\n";
				if(retBlock != NULL) {
				if((*FI)->end_block == retBlock->end_block)
				{
				//	errs()<<"\n End block matches with the stack top.\n";
					functendBlock = true;
					callStack.pop();
					BasicBlock *rettoBlock = retBlock->Returnto_Block;
					Process_BB(rettoBlock,end_funct,context);
					
				}
				} // retBlock not null
				} //stack  size not 0
    		}
    	}

    //	else
       if(BB_Name=="BB_196")  //FIXMe.. this is due to the undeterministic branching at the return of main function. 
    	{ return; }
    	//Dont process successors if the block is end of function block...
    	else if((BB_Name != end_funct) && (functendBlock != true))
    	{
			TerminatorInst* TI = BB->getTerminator();
			int n = TI->getNumSuccessors();
		     CallReturnInfo * CRI;
		       
			for(int i=0;i<n;i++)
			{
			    bool call_statement = false;
				BasicBlock* SB = TI->getSuccessor(i);
				//If the successor is a start of a function.. then it is a call inst.. store return inst
				if(BB->getName() != "switch")  //&& (visitedBlocks.find(SB->getName()) == visitedBlocks.end())
				{
				for(FI= FunctionInfos.begin(); FI!=FunctionInfos.end(); ++FI)
					{
					   if(SB->getName() == (*FI)->begin_block) 
						{
						  call_statement = true;
					//	  errs() << "\n\n ***** Got the CALL INST : "  <<BB->getName() << " for " << SB->getName() << "funct begin "<< (*FI)->begin_block;
					      errs() <<" \n\n Processing FUNCTION ... "<<(*FI)->FunctionName <<"  "<<(*FI)->begin_block <<"  to  "<<(*FI)->end_block;
						 //store the return block
						 Function::iterator BBit(BB);
						 --BBit;
					//	 errs() << " Return to... " << BBit->getName() << "\n";
						 BasicBlock *RB = &*BBit;
						
						 CRI = new CallReturnInfo();
						 CRI->Call_Block = BB;
						 CRI->Start_Block = SB;
						 CRI->Returnto_Block = RB;
						 CRI->end_block = (*FI)->end_block;
						 CallRet_Info.insert(CRI);
						//  errs()<<"\n call chain info.." << CRI->Call_Block->getName() << " " <<CRI->Start_Block->getName()<<" " <<CRI->end_block<<" "<<CRI->Returnto_Block->getName();
						}

					}
				//	Process_BB(SB,end_funct);
				 }
				
			//	errs()<<"\nCalling rec for block: "<<SB->getName() << " Successor of: " <<BB_Name;
			    if(call_statement)
			    	{
			    		callStack.push(CRI);
			    		context = CRI->Call_Block->getName();
			    	}
				Process_BB(SB,end_funct,context);
				
			}
			
   	 	}
   //	 errs()<< "\n Call Stack Size .. " <<callStack.size();
    }
    else
		return;

}

bool BuildGraph::runOnModule(Module &M) {

       // AliasSets* AS = NULL;

//if (USE_ALIAS_SETS)
  //              AS = &(getAnalysis<AliasSets> ());
                
        InputPass &IV = getAnalysis<InputPass> ();
		FunctionInfos = IV.getFunctionInfo();
	    
	    //Making dependency graph
        depGraph = new Graph();
        
		 depGraph->toDot(M.getModuleIdentifier(), "fromAS.dot");
	//	 int read_block =0;
		 set<BasicBlock*> worklist;
		// set<std::string> visitedFunctions;
		 string end_function;
	//	 set<std::string> visitedBlocks;
        //Insert instructions in the graph
        for (Module::iterator Fit = M.begin(), Fend = M.end(); Fit != Fend; ++Fit) {
                for (Function::iterator BBit = Fit->begin(), BBend = Fit->end(); BBit
                                != BBend; ++BBit) {
                    //   errs() << "\n At Block " << BBit->getName();
                    
                    std::vector<FunctionInfo*>::iterator FI;
					for(FI= FunctionInfos.begin(); FI!=FunctionInfos.end(); ++FI)
 					{
                        if((BBit->getName() == (*FI)->begin_block) && ((*FI)->FunctionName=="main")) {  //&& (BBit->getName()=="BB_107")
                        	//read_block=1;
                        	end_function = (*FI)->end_block;
                        	errs() <<" \n\n Processing main FUNCTION ... "<<(*FI)->FunctionName <<"  "<<(*FI)->begin_block <<"  to  "<<(*FI)->end_block;
                        	string context = "none";
                        	Process_BB(BBit,end_function,context); 
                        	
                       /* 	errs() << "\nvisited blocks: ";
   	 						for(set<std::string>::iterator sb = visitedBlocks.begin();sb != visitedBlocks.end();++sb)
   	 							errs() << *sb << "  ";
   	 						//	cin>>n; */
   	 						
                        	visitedBlocks.clear();
                        	}
                    }
                        
                    
                }
           
             //   errs()<< "\n Worklist Size -- "<< worklist.size();
        }
          errs()<< "\nNumber of calls and returns.."<<  CallRet_Info.size() << "\n";
        for(std::set<CallReturnInfo*>::iterator cr = CallRet_Info.begin();cr != CallRet_Info.end();++cr)
         {
           errs()<<"\n call chain info.." << (*cr)->Call_Block->getName() << " " <<(*cr)->Start_Block->getName()<<" " <<(*cr)->end_block<<" "<<(*cr)->Returnto_Block->getName();
         }
        
        depGraph->toDot(M.getModuleIdentifier(), "afteradInst.dot");
/*
        //Connect formal and actual parameters and return values
        for (Module::iterator Fit = M.begin(), Fend = M.end(); Fit != Fend; ++Fit) {

                // If the function is empty, do not do anything
                // Empty functions include externally linked ones (i.e. abort, printf, scanf, ...)
                if (Fit->begin() == Fit->end())
                        continue;

                matchParametersAndReturnValues(*Fit);

        }
*/
        //We don't modify anything, so we must return false
        return false;
}



void BuildGraph::matchParametersAndReturnValues(Function &F) {

        // Only do the matching if F has any use
        if (F.isVarArg() || !F.hasNUsesOrMore(1)) {
                return;
        }

        // Data structure which contains the matches between formal and real parameters
        // First: formal parameter
        // Second: real parameter
        SmallVector<std::pair<GraphNode*, GraphNode*>, 4> Parameters(F.arg_size());

        // Fetch the function arguments (formal parameters) into the data structure
        Function::arg_iterator argptr;
        Function::arg_iterator e;
        unsigned i;

        //Create the PHI nodes for the formal parameters
        for (i = 0, argptr = F.arg_begin(), e = F.arg_end(); argptr != e; ++i, ++argptr) {

                OpNode* argPHI = new OpNode(Instruction::PHI);
                GraphNode* argNode = NULL;
                argNode = depGraph->addInst(argptr);

                if (argNode != NULL)
                        depGraph->addEdge(argPHI, argNode);

                Parameters[i].first = argPHI;
        }

        // Check if the function returns a supported value type. If not, no return value matching is done
        bool noReturn = F.getReturnType()->isVoidTy();

        // Creates the data structure which receives the return values of the function, if there is any
        SmallPtrSet<llvm::Value*, 8> ReturnValues;

        if (!noReturn) {
                // Iterate over the basic blocks to fetch all possible return values
                for (Function::iterator bb = F.begin(), bbend = F.end(); bb != bbend; ++bb) {
                        // Get the terminator instruction of the basic block and check if it's
                        // a return instruction: if it's not, continue to next basic block
                        Instruction *terminator = bb->getTerminator();
						//Todo :Match it with the end block of a function 
                        ReturnInst *RI = dyn_cast<ReturnInst> (terminator);

                        if (!RI)
                                continue;

                        // Get the return value and insert in the data structure
                        ReturnValues.insert(RI->getReturnValue());
                }
        }

        for (Value::use_iterator UI = F.use_begin(), E = F.use_end(); UI != E; ++UI) {
                User *U = *UI;

                // Ignore blockaddress uses
                if (isa<BlockAddress> (U))
                        continue;

                // Used by a non-instruction, or not the callee of a function, do not
                // match.
                if (!isa<CallInst> (U) && !isa<InvokeInst> (U))
                        continue;

                Instruction *caller = cast<Instruction> (U);

                CallSite CS(caller);
                if (!CS.isCallee(UI))
                        continue;

                // Iterate over the real parameters and put them in the data structure
                CallSite::arg_iterator AI;
                CallSite::arg_iterator EI;

                for (i = 0, AI = CS.arg_begin(), EI = CS.arg_end(); AI != EI; ++i, ++AI) {
                        Parameters[i].second = depGraph->addInst(*AI);
                }

                // Match formal and real parameters
                for (i = 0; i < Parameters.size(); ++i) {

                        depGraph->addEdge(Parameters[i].second, Parameters[i].first);
                }

                // Match return values
                if (!noReturn) {

                        OpNode* retPHI = new OpNode(Instruction::PHI);
                        GraphNode* callerNode = depGraph->addInst(caller);
                        depGraph->addEdge(retPHI, callerNode);

                        for (SmallPtrSetIterator<llvm::Value*> ri = ReturnValues.begin(),
                                        re = ReturnValues.end(); ri != re; ++ri) {
                                GraphNode* retNode = depGraph->addInst(*ri);
                                depGraph->addEdge(retNode, retPHI);
                        }

                }

                // Real parameters are cleaned before moving to the next use (for safety's sake)
                for (i = 0; i < Parameters.size(); ++i)
                        Parameters[i].second = NULL;
        }

        depGraph->deleteCallNodes(&F);
}

void llvm::BuildGraph::deleteCallNodes(Function* F) {
        depGraph->deleteCallNodes(F);
}


std::set<GraphNode*> llvm::Graph::getDepValues(std::set<llvm::Value*> sources,
		bool forward) {
	unsigned long nnodes = nodes.size();
	std::set<GraphNode*> visited;
	
/*	for(std::set<llvm::Value*>::iterator si = sources.begin();si!=sources.end();++si)
	{
		errs()<<"\n Sources "<< *si;
	} */
	
	std::set<GraphNode*> sourceNodes = findNodes(sources);
	std::list<GraphNode*> worklist;
	std::map<GraphNode*, edgeType> neigh;
	for (std::set<GraphNode*>::iterator i = sourceNodes.begin(), e =
			sourceNodes.end(); i != e; ++i) {
		worklist.push_back(*i);
	}
	while (!worklist.empty()) {
		GraphNode* n = worklist.front();
	//	DEBUG(errs() << worklist.size() << "/" << visited.size() << "/" << nnodes << "\n");
		DEBUG(assert(worklist.size() <= nnodes && "Problem with nodes"));
		if (forward)
			neigh = n->getSuccessors();
		else
			neigh = n->getPredecessors();
		for (std::map<GraphNode*, edgeType>::iterator i = neigh.begin(), e =
				neigh.end(); i != e; ++i) {
			if (!visited.count(i->first)) {
				worklist.push_back(i->first);
				visited.insert(i->first);
			}
		}
		worklist.pop_front();
	}
	return visited;
}



/*

bool BuildGraph::runOnModule(Module &M) {
	//	DEBUG (errs() << "Function " << F.getName() << "\n";);
	nodeCount = 0;
	edgeCount = 0;
	branchNodeCount = 0;
	NumInputValues =0;
	bool inserted;
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
 }

*/


void BuildGraph::getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
      AU.addRequired<InputPass> ();
     // AU.addRequired<InputPass> ();
    }
    
char BuildGraph::ID = 0;
static RegisterPass<BuildGraph> Y("buildGraph",
                "Build Dependence Graph");

