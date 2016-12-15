#include "FlowGraph.h"

#define debug false
#define code false

#define printMem false
using namespace llvm;

static cl::opt<bool, false>
includeAllInstsInDepGraph("includeAllInstsInDepGraph", cl::desc("Include All Instructions In DepGraph."), cl::NotHidden);

/*
 * Class MemoryOperation
 */

MemoryOperation::MemoryOperation() {

}

CallSiteInfo::CallSiteInfo()
{

}

FuncParamInfo::FuncParamInfo()
{

}

void MemoryOperation::setVal()
{
    if(parentFunction!=NULL)
    {
        std::ostringstream stringStream;
        string funcName = parentFunction->getName();

        if(parentBlock != NULL  && !isParam)
        {
            string blockName = parentBlock->getName();
            stringStream <<funcName<<"_"<<blockName << "_"<<lineNumber;
        }
        else
        {
            stringStream <<funcName<<"_Param";
        }
        IdValue = stringStream.str();
        hasVal = true;
    }
}


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

void llvm::GraphNode::setLabel(string str)
{
    this->label = str;
}


void llvm::GraphNode::initialize(Value* v, nodeType ntype)
{
    this->value = v;
    if(Instruction * inst = dyn_cast<Instruction>(v))
    {
        this->nodeInst = inst;
        this->BB = inst->getParent();
        this->Func = this->BB->getParent();
    }
    else
    {
        this->nodeInst = nullptr;
        this->BB = nullptr;
        this->Func = nullptr;
    }

    //Todo NT: if getelementptr inst then add the fields and type and source etc info.
    //Todo NT: if debug info available add linenumber info.
    //add appropriate info with respect to type.


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
    string lab = stringStream.str()+"_"+label;
    return lab;

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

    return stringStream.str()+"_"+label;
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
 * Class Entry Node
 */
Function* llvm::EntryNode::getFunction() const {
    return this->F;
}

std::string llvm::EntryNode::getLabel() {
    std::ostringstream stringStream;

    stringStream << "Entry ";
    stringStream << F->getName().str();

    return stringStream.str()+"_"+label;
}

std::string llvm::EntryNode::getShape() {
    return std::string("doubleoctagon");
}

GraphNode* llvm::EntryNode::clone() {
    EntryNode* R = new EntryNode(*this);
    R->Class_ID = this->Class_ID;
    return R;
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

    return stringStream.str()+"_"+label;

}

GraphNode* llvm::VarNode::clone() {
    VarNode* R = new VarNode(*this);
    R->Class_ID = this->Class_ID;
    return R;
}

/*
 * Class MemNode
 */
//std::set<llvm::Value*> llvm::MemNode::getAliases() {
//   // return USE_ALIAS_SETS ? AS->getValueSets()[aliasSetID] : std::set<llvm::Value*>();

//}

std::string llvm::MemNode::getLabel() {
    std::ostringstream stringStream;

    bool labelSet = false;
    // if(aliasSetID)
//    if(USE_ALIAS_SETS)
//    {
//        set<Value *> aliasValSet = AS->getValueSets()[aliasSetID];
//        if(aliasValSet.size()==1)
//        {
//            Value * ptrVal = (*aliasValSet.begin());
//            if(ptrVal->hasName())
//            {
//                string valName = ptrVal->getName();
//                if(defLoc->hasVal)
//                {
//                    stringStream << valName << "_" << aliasSetID << "_" << defLoc->IdValue;
//                    labelSet = true;
//                }
//                else
//                {
//                    stringStream << valName << "_" << aliasSetID;
//                    labelSet = true;
//                }
//            }
//        }
//    }

//    if(!labelSet)
//        if(defLoc->hasVal)
//        {
//            //string blockName = defLoc->parentBlock->getName();
//            //string funcName = defLoc->parentFunction->getName();

//            // stringStream << "Memory " << aliasSetID << "_" << funcName<<"_"<<blockName << "_"<<defLoc->lineNumber;
//            stringStream << "Memory " << aliasSetID << "_" <<defLoc->IdValue;

//        }
//        else
//            stringStream << "Memory " << aliasSetID;

    return stringStream.str()+"_"+label;
}


llvm::Value* MemNode::getValue() {
    return value;
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

//int llvm::MemNode::getAliasSetId() const {
//    return aliasSetID;
//}

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

llvm::DenseMap<GraphNode*, bool > taintedMap; //For statistics on how many edges of the original graph are tainted generated by generateSubgraph()

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
    Graph G;

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
            //Stores the original on static map
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
                G.memValNodes[dyn_cast<MemNode>(it->second)->getValue()] = dyn_cast<MemNode>(it->second);
               // G.memNodes[0] = dyn_cast<MemNode>(it->second);
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


void Graph::updateMemNodeLabels()
{
//          llvm::DenseMap<int, GraphNode*> memNodes;
//    DenseMap<int, set<Value*> > AliasMap;
//    for(DenseMap<int, GraphNode*>::iterator memNodeIT = memNodes.begin(); memNodeIT != memNodes.end(); ++memNodeIT)
//    {
//        int id = (*memNodeIT)->aliasSetID
//        set<Value*> ALiasValSet = AliasMap[]
//    }
}


//Print the graph (.dot format) in the stderr stream.
void Graph::toDot(std::string s) {

    this->toDot(s, &errs());

}

void Graph::toDot(std::string s, const std::string fileName) {

    // errs()<<"\n in function todot";
    std::string ErrorInfo;
    sys::fs::OpenFlags Flags;
    std::error_code errcode;

    this->updateMemNodeLabels();
    raw_fd_ostream File(fileName.c_str(), errcode,sys::fs::F_None);

    if (errcode.value() != 0) {
        errs() << "Error opening file " << fileName
               << " for writing! Error code: " << errcode.value() << " \n";
        return;
    }

    this->toDot(s, &File);

}


void Graph::toDotLines(std::string s, const std::string fileName) {

   // std::string ErrorInfo;

    sys::fs::OpenFlags Flags;
    std::error_code errcode;

    raw_fd_ostream File(fileName.c_str(), errcode, Flags);

    if (errcode.value() != 0) {
        errs() << "Error opening file " << fileName
               << " for writing! Error Info: " << errcode.value() << " \n";
        return;
    }

    this->toDotLines(s, &File);

}



void Graph::toDotLines(std::string s, raw_ostream *stream) {

    Instruction *A;
    StringRef File;
    OpNode *op;
    VarNode *va;
    CallNode *ca;
    ostringstream label;
    MDNode *N;

    unsigned Line;


    (*stream) << "digraph \"DFG for \'" << s << "\'  \"{\n";
    (*stream) << "label=\"DFG for \'" << s << "\' \";\n";

    std::map<GraphNode*, int> DefinedNodes;

    for (std::set<GraphNode*>::iterator node = nodes.begin(), end = nodes.end(); node != end; node++) {
        label.str("");
        if ((op = dyn_cast<OpNode>((*node)))) {
            if (op->getValue() != NULL) {
                if ((A = dyn_cast<Instruction>(op->getValue()))) {
                    if (DILocation *Loc = A->getDebugLoc()) {
                        //DILocation Loc(N);
                        File = Loc->getFilename();
                        Line = Loc->getLine();
                        label << File.str() << " " << Line;
                    } else {
                        label << "null";
                    }
                }
            }else
                label << "null";
        }else if ((va = dyn_cast<VarNode>((*node)))) {
            if (va->getValue() != NULL) {
                if ((A = dyn_cast<Instruction>(va->getValue()))) {
                    if (DILocation *Loc = A->getDebugLoc()) {
                       // DILocation Loc(N);
                        File = Loc->getFilename();
                        Line = Loc->getLine();
                        label << File.str() << " " << Line;
                    } else {
                        label << "null";
                    }
                }
            }else
                label << "null";
        }else if ((ca = dyn_cast<CallNode>((*node)))) {
            if (ca->getValue() != NULL) {
                if ((A = dyn_cast<Instruction>(ca->getValue()))) {
                    if (DILocation *Loc = A->getDebugLoc()) {
                        //DILocation Loc(N);
                        File = Loc->getFilename();
                        Line = Loc->getLine();
                        label << File.str() << " " << Line;
                    } else {
                        label << "null";
                    }
                }
            }else
                label << "null";
        }


        if (DefinedNodes.count(*node) == 0) {
            string nodeLabel = label.str();
            nodeLabel.erase (std::remove(nodeLabel.begin(), nodeLabel.end(), '"'), nodeLabel.end());
            (*stream) << (*node)->getName() << "[shape=" << (*node)->getShape() << ",style=" << (*node)->getStyle() << ",label=\"" << nodeLabel << "\"]\n";
            DefinedNodes[*node] = 1;
        }


        std::map<GraphNode*, edgeType> succs = (*node)->getSuccessors();

        for (std::map<GraphNode*, edgeType>::iterator succ = succs.begin(), s_end = succs.end(); succ != s_end; succ++) {
            label.str("");
            if ((op = dyn_cast<OpNode>((succ->first)))) {
                if (op->getValue() != NULL) {
                    if ((A = dyn_cast<Instruction>(op->getValue()))) {
                        if (DILocation *Loc = A->getDebugLoc()) {
                           // DILocation Loc(N);
                            File = Loc->getFilename();
                            Line = Loc->getLine();
                            label << File.str() << " " << Line;
                        } else {
                            label << "null";
                        }
                    }
                }else
                    label << "null";
            }else if ((va = dyn_cast<VarNode>((succ->first)))) {
                if (va->getValue() != NULL) {
                    if ((A = dyn_cast<Instruction>(va->getValue()))) {
                        if (DILocation *Loc = A->getDebugLoc()) {
                            //DILocation Loc(N);
                            File = Loc->getFilename();
                            Line = Loc->getLine();
                            label << File.str() << " " << Line;
                        } else {
                            label << "null";
                        }
                    }
                }else
                    label << "null";
            }else if ((ca = dyn_cast<CallNode>((succ->first)))) {
                if (ca->getValue() != NULL) {
                    if ((A = dyn_cast<Instruction>(ca->getValue()))) {
                        if (DILocation *Loc = A->getDebugLoc()) {
                            //DILocation Loc(N);
                            File = Loc->getFilename();
                            Line = Loc->getLine();
                            label << File.str() << " " << Line;
                        } else {
                            label << "null";
                        }
                    }
                }else
                    label << "null";
            }


            if (DefinedNodes.count(succ->first) == 0) {
                string nodeLabel = label.str();
                nodeLabel.erase (std::remove(nodeLabel.begin(), nodeLabel.end(), '"'), nodeLabel.end());
                (*stream) << (succ->first)->getName() << "[shape="
                          << (succ->first)->getShape() << ",style="
                          << (succ->first)->getStyle() << ",label=\""
                          << nodeLabel << "\"]\n";
                DefinedNodes[succ->first] = 1;
            }

            //Source
            (*stream) << "\"" << (*node)->getName() << "\"";

            (*stream) << "->";

            //Destination
            (*stream) << "\"" << (succ->first)->getName() << "\"";

            if (succ->second == etControl || succ->second == etCall)
                (*stream) << " [style=dashed]";

            (*stream) << "\n";

        }

    }

    (*stream) << "}\n\n";

}


void Graph::toDot(std::string s, raw_ostream *stream) {

    (*stream) << "digraph \"DFG for \'" << s << "\'  \"{\n";
    (*stream) << "label=\"DFG for \'" << s << "\' \";\n";



    std::map<GraphNode*, int> DefinedNodes;

    for (std::set<GraphNode*>::iterator node = nodes.begin(), end = nodes.end(); node
         != end; node++) {

        if (DefinedNodes.count(*node) == 0) {
            //remove special chars from label, name..
            string nodeLabel = (*node)->getLabel();
          //  char chars[] = "\"";

//               for (unsigned int i = 0; i < strlen(chars); ++i)
//               {
                 // nodeLabel.erase (std::remove(nodeLabel.begin(), nodeLabel.end(), '\\'), nodeLabel.end());
                                    nodeLabel.erase (std::remove(nodeLabel.begin(), nodeLabel.end(), '"'), nodeLabel.end());
            //   }

            (*stream) << (*node)->getName() << "[shape=" << (*node)->getShape()
                      << ",style=" << (*node)->getStyle() << ",label=\""
                      << nodeLabel << "\"]\n";
            DefinedNodes[*node] = 1;
        }

        std::map<GraphNode*, edgeType> succs = (*node)->getSuccessors();

        for (std::map<GraphNode*, edgeType>::iterator succ = succs.begin(),
             s_end = succs.end(); succ != s_end; succ++) {

            if (DefinedNodes.count(succ->first) == 0) {
                string nodeLabel = (succ->first)->getLabel();
                nodeLabel.erase (std::remove(nodeLabel.begin(), nodeLabel.end(), '"'), nodeLabel.end());

                (*stream) << (succ->first)->getName() << "[shape="
                          << (succ->first)->getShape() << ",style="
                          << (succ->first)->getStyle() << ",label=\""
                          << nodeLabel << "\"]\n";
                DefinedNodes[succ->first] = 1;
            }

            //Source
            (*stream) << "\"" << (*node)->getName() << "\"";

            (*stream) << "->";

            //Destination
            (*stream) << "\"" << (succ->first)->getName() << "\"";

            if (succ->second == etControl || succ->second == etCall)
                (*stream) << " [style=dashed]";

            (*stream) << "\n";

        }

    }

    (*stream) << "}\n\n";

}

void llvm::Graph::toDot(std::string s, raw_ostream *stream, llvm::Graph::Guider* g) {
    (*stream) << "digraph \"DFG for \'" << s << "\' module \"{\n";
    (*stream) << "label=\"DFG for \'" << s << "\' module\";\n";


    // print every node
    for (std::set<GraphNode*>::iterator node = nodes.begin(), end = nodes.end(); node
         != end; node++) {
        (*stream) << (*node)->getName() << g->getNodeAttrs(*node) << "\n";

    }
    // print edges
    for (std::set<GraphNode*>::iterator node = nodes.begin(), end = nodes.end(); node
         != end; node++) {
        std::map<GraphNode*, edgeType> succs = (*node)->getSuccessors();
        for (std::map<GraphNode*, edgeType>::iterator succ = succs.begin(),
             s_end = succs.end(); succ != s_end; succ++) {
            //Source
            (*stream) << "\"" << (*node)->getName() << "\"";
            (*stream) << "->";
            //Destination
            (*stream) << "\"" << (succ->first)->getName() << "\"";
            (*stream) << g->getEdgeAttrs(*node, succ->first);
            (*stream) << "\n";
        }
    }
    (*stream) << "}\n\n";
}


GraphNode* Graph::addNode(Instruction *inst, nodeType nType)
{
    GraphNode *Var;

//    errs()<<"\n Adding node for : ";
//    inst->dump();

    if (isValidInst(inst)) { //If is a data manipulator instruction

        Value *v = inst;
        Var = this->findNode(v);

        if(Var==NULL)
        {
            if(isMemoryPointer(v))
            {
                Var = new MemNode();
                Var->node_type = nType;
                memNodes[0] = Var;
            }
            else
            {
                Var = new VarNode(v);
                Var->node_type = nType;
                varNodes[v] = Var;
            }

            //   GraphNode * newParamNode =
        }
        else if(Var->node_type != nType)
        {
            if(isMemoryPointer(v))
            {
                Var = new MemNode();
                Var->node_type = nType;
                memNodes[0] = Var;
            }
            else
            {
                Var = new VarNode(v);
                Var->node_type = nType;
                varNodes[v] = Var;
            }

        }


        if(isa<MemNode>(Var))
        {
            Var->node_type = nType;
            //   BasicBlock* parentBlock

            GraphNode* newVar = Var->clone();
            MemNode * memNew = dyn_cast<MemNode>(newVar);
            memNew->defLoc->parentBlock = NULL;
            //    errs()<<"\nFound a mem node..!! "<<parentBlock->getName() <<"\n";
            memNew->defLoc->assignedValue = v;
            //memNew->defLoc->lineNumber = Instcounter;
           // memNew->defLoc->parentFunction = func;
            memNew->defLoc->MemInst = v; //dyn_cast<Value*>(&SI);
            memNew->defLoc->isParam = true;
          //  memNew->defLoc->setVal();
            //errs()<<"\n IDVALUE Obtained: ---- "<< memNew->defLoc->IdValue;
            set<MemoryOperation* > defSets = RDSMap[v];
            defSets.insert(memNew->defLoc);
            RDSMap[v] = defSets;

            //   if(printMem)   errs()<<"\n  New node gen:    "<<memNew->getLabel()<<"   Line   :"<< Instcounter<<"\n";
            // if(printMem)  v->dump();
            //StoreNodeMap[SI] = memNew;
            //functionParamMap
            //funcParMapLocal
            memNodes[0] = memNew;
            Var->connect(memNew);
            Var = memNew;
            //nodes.insert(memNew);
        }

        //Update in the Function param info in the global data structure.
        //  BasicBlock* parentBlock = v->getParent();

        nodes.insert(Var);
    }

    std::string str;
    llvm::raw_string_ostream rso(str);
    inst->print(rso);
    Var->setLabel(str);

  //  errs()<<" Added -- "<<Var->getLabel();


    Var->initialize(inst,nType);
    this->instMap[inst] = Var;
    return Var;
}


GraphNode* Graph::addParamNode(Value *v, Function * func, nodeType nType)
{
    GraphNode  *Var;



    if (isValidInst(v)) { //If is a data manipulator instruction
        Var = this->findNode(v);


        if(Var==NULL)
        {
            if(isMemoryPointer(v))
            {
                Var = new MemNode();
                Var->node_type = nType;
                memNodes[0] = Var;
            }
            else
            {
                Var = new VarNode(v);
                Var->node_type = nType;
                varNodes[v] = Var;
            }

            //   GraphNode * newParamNode =
        }
        else if(Var->node_type != nType)
        {
            if(isMemoryPointer(v))
            {
                Var = new MemNode();
                Var->node_type = nType;
                memNodes[0] = Var;
            }
            else
            {
                Var = new VarNode(v);
                Var->node_type = nType;
                varNodes[v] = Var;
            }

        }


        if(isa<MemNode>(Var))
        {
            Var->node_type = nType;
            //   BasicBlock* parentBlock

            GraphNode* newVar = Var->clone();
            MemNode * memNew = dyn_cast<MemNode>(newVar);
            memNew->defLoc->parentBlock = NULL;
            //    errs()<<"\nFound a mem node..!! "<<parentBlock->getName() <<"\n";
            memNew->defLoc->assignedValue = v;
            //memNew->defLoc->lineNumber = Instcounter;
            memNew->defLoc->parentFunction = func;
            memNew->defLoc->MemInst = v; //dyn_cast<Value*>(&SI);
            memNew->defLoc->isParam = true;
            memNew->defLoc->setVal();
            //errs()<<"\n IDVALUE Obtained: ---- "<< memNew->defLoc->IdValue;
            set<MemoryOperation* > defSets = RDSMap[v];
            defSets.insert(memNew->defLoc);
            RDSMap[v] = defSets;

            //   if(printMem)   errs()<<"\n  New node gen:    "<<memNew->getLabel()<<"   Line   :"<< Instcounter<<"\n";
            // if(printMem)  v->dump();
            //StoreNodeMap[SI] = memNew;
            //functionParamMap
            //funcParMapLocal
            memNodes[0] = memNew;
            Var->connect(memNew);
            Var = memNew;
            //nodes.insert(memNew);
        }

        //Update in the Function param info in the global data structure.
        //  BasicBlock* parentBlock = v->getParent();
        if(func)
        {
            //  Function* Func = parentBlock->getParent();
            FuncParamInfo* funcParam;
            funcParam = functionParamMap[func];
            if(!funcParam)
                funcParam = new FuncParamInfo();

            if(nType == ntFormalIn)
                funcParam->FormalInMap[v] = Var;
            else if(nType == ntFormalOut)
                funcParam->FormalOutMap[v] = Var;
            functionParamMap[func]= funcParam;

        }


        nodes.insert(Var);
    }
    std::string str;
    llvm::raw_string_ostream rso(str);
    v->print(rso);
    Var->setLabel(str);


   // this->instMap[v] = Var;

    return Var;
}


GraphNode* Graph::addEntryNode(Function *F)
{
    EntryNode* enNode = new EntryNode(F);
    enNode->node_type = ntEntry;
    //instMap[F] = enNode;

    nodes.insert(enNode);

    return enNode;
}

GraphNode* Graph::addInst(Value *v, int Instcounter, Function * func) {

    GraphNode *Op, *Var, *Operand;

    CallInst* CI = dyn_cast<CallInst> (v);
    bool hasVarNode = true;

    if(code) errs()<<"\n-------------------------";
    if(code) v->dump();

    if (isValidInst(v)) { //If is a data manipulator instruction
        Var = this->findNode(v);
        /*
                 * If Var is NULL, the value hasn't been processed yet, so we must process it
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
                    //   errs()<<" in has var node";

                    if (StoreInst* SI = dyn_cast<StoreInst>(v))
                    {

                        Var = addInst(SI->getOperand(1),Instcounter,func); // We do this here because we want to represent the store instructions as a flow of information of a data to a memory node
                        BasicBlock* parentBlock = SI->getParent();
                        Value* assignedVal = SI->getValueOperand();
                        Value* pointerOperand = SI->getPointerOperand();

                        if(isa<GetElementPtrInst>(pointerOperand) && isa<MemNode>(Var))
                        {
                            for(set<Value*>::iterator fields = FieldsVal.begin(); fields != FieldsVal.end(); ++fields)
                            {
                                if(pointerOperand==(*fields))
                                {
                                    //match for field found .. create new node.
                                    GraphNode* newVar = Var->clone();
                                    MemNode * memNew = dyn_cast<MemNode>(newVar);
                                    memNew->defLoc->parentBlock = parentBlock;
                                    //    errs()<<"\nFound a mem node..!! "<<parentBlock->getName() <<"\n";
                                    memNew->defLoc->assignedValue = assignedVal;
                                    memNew->defLoc->lineNumber = Instcounter;
                                    memNew->defLoc->parentFunction = parentBlock->getParent();
                                    memNew->defLoc->MemInst = SI;//dyn_cast<Value*>(&SI);
                                    memNew->defLoc->setVal();
                                    StoreNodeMap[SI] = memNew;
                                    // Var->connect(memNew);
                                    Var = memNew;
                                    memNodes[0] = memNew;
                                    if(debug) errs()<<"\n ~~~~~~~~~~~~  Node size before inert...: "<<nodes.size()<<"  name: "<<memNew->getLabel();;
                                    nodes.insert(memNew);
                                      if(debug) errs()<<"\n ~~~~~~~~~~~~  Node size after inert...: "<<nodes.size();
                                }
                            }
                        }

                        if(isa<MemNode>(Var) && LIVENESS)
                        {

                            GraphNode* newVar = Var->clone();
                            MemNode * memNew = dyn_cast<MemNode>(newVar);
                            memNew->defLoc->parentBlock = parentBlock;
                            //    errs()<<"\nFound a mem node..!! "<<parentBlock->getName() <<"\n";
                            memNew->defLoc->assignedValue = assignedVal;
                            memNew->defLoc->lineNumber = Instcounter;
                            memNew->defLoc->parentFunction = parentBlock->getParent();
                            memNew->defLoc->MemInst = SI;//dyn_cast<Value*>(&SI);
                            memNew->defLoc->setVal();
                            //  errs()<<"\n IDVALUE Obtained: ---- "<< memNew->defLoc->IdValue;
                            set<MemoryOperation* > defSets = RDSMap[pointerOperand];
                            defSets.insert(memNew->defLoc);
                            RDSMap[pointerOperand] = defSets;
                            // RDSMap[pointerOperand].insert(memNew->defLoc);
                            if(printMem)   errs()<<"\n  New node gen:    "<<memNew->getLabel()<<"   Line   :"<< Instcounter<<"\n";
                            if(printMem)  v->dump();
                            StoreNodeMap[SI] = memNew;
                            Var->connect(memNew);
                            Var = memNew;
                            memNodes[0] = memNew;
                             if(debug) errs()<<"\n ~~~~~~~~~~~~  Node size before inert...: "<<nodes.size()<<"  name: "<<memNew->getLabel();;
                            nodes.insert(memNew);
                             if(debug) errs()<<"\n ~~~~~~~~~~~~  Node size after inert...: "<<nodes.size();

                        }
                        //  SI->dump();
                        //  errs()<<"  ONLY node gen:    "<<Var->getLabel(); //<<"  "<<instCast;

                    }
                    else if ((!isa<Constant> (v)) && isMemoryPointer(v)) {


                        Var = new MemNode();
                        memNodes[0] = Var;
                        if(printMem)          errs()<<"\nMEMNODE GENERATED  ---- : "<<Var->getName()<<"  "<< Var->getLabel() << " for ";
                        if(printMem)         v->dump();

                    }
                    else {  //if(!isa<Constant> (v))  .. dont generate the constant nodes.. makes it ugly plus adds no val.


                        Var = new VarNode(v);
                        varNodes[v] = Var;
                        if(code) errs()<<"\n Creating var node for :";
                        if(code) v->dump();
                    }
                    nodes.insert(Var);
                }

            }

            if (isa<Instruction> (v)) {

                if (CI) {
                    Op = new CallNode(CI);
                    callNodes[CI] = Op;
                    if(code) errs()<<"\n Creating call node";

                } else {
                    Op = new OpNode(dyn_cast<Instruction> (v)->getOpcode(), v);
                    if(code) errs()<<"\n Creating op node";
                }
                opNodes[v] = Op;

                nodes.insert(Op);
                if (hasVarNode)
                    Op->connect(Var);

                if(isa<LoadInst>(v) && LIVENESS)
                {
                    //Getting all the defs of the load instruction ptr operand.. (if store considered as def..)
                    //Here get the recent stores to that var and add edges...!!
                    LoadInst* LI = dyn_cast<LoadInst>(v);
                    Value* loadptr = LI->getPointerOperand();
                    Value* RootPtr = loadptr;

                    set<MemoryOperation*> memOps = LiveMap[loadptr];
                    if(isa<GetElementPtrInst>(loadptr))
                    {
                        GetElementPtrInst* GI = dyn_cast<GetElementPtrInst>(loadptr);
                        RootPtr =  GI->getPointerOperand();

                        set<MemoryOperation*> memOpsRoot = LiveMap[RootPtr];
                        memOps.insert(memOpsRoot.begin(),memOpsRoot.end());
                    }

                    //set<GraphNode*> relatedStores = StoreNodeMap[loadptr];

                    if(memOps.size()==0)
                    {
                        //TODO: Check if there is live param.. if so then add connection to it.

                        if(func)
                        {
                            // Function* Func = parentBlock->getParent();
                            FuncParamInfo* funcParam;
                            funcParam = functionParamMap[func];
                            if(funcParam)
                            {
                                GraphNode* paramNode = funcParam->ParamNodeMap[loadptr];
                                if(paramNode)
                                    paramNode->connect(Op);
                                else if(GraphNode* rootNode = funcParam->ParamNodeMap[RootPtr])
                                    rootNode->connect(Op);
                            }
                        }
                    }

                    for(set<MemoryOperation*>::iterator memIt = memOps.begin(); memIt != memOps.end();++memIt)
                    {
                        //            errs()<<"\n Gets values from..--- ";
                        //           (*memIt)->MemInst->dump();
                        GraphNode * storeNode = StoreNodeMap[(*memIt)->MemInst];
                        if(storeNode!=NULL)
                        {
                            storeNode->connect(Op);
                        }
                    }
                }
                if(isa<GetElementPtrInst>(v))

                {
                    //         errs()<<"Get Element Pointer: ";
                    //       v->dump();
                }

                //Process Call Instruction seperately..
                //For each param check if it is a pointer
                //if it is a pointer then consider it as a new store op and add connections from all the previous live stores to the pointer.
                // if(CI && LIVENESS)
                if(CI)
                {
                    CallSite CS(CI);
                    CallSite::arg_iterator AI;
                    CallSite::arg_iterator EI;

                    for (AI = CS.arg_begin(), EI = CS.arg_end(); AI != EI; ++AI) {

                        Use * useparam = &*AI;
                        Value * param =  useparam->get();

                        if(code) errs()<<"\n Creating nodes for call args.. node";
                        Operand = this->addInst(param,Instcounter,func);
                        BasicBlock* parentBlock = CI->getParent();
                        //Value* assignedVal = SI->getValueOperand();
                        //Value* pointerOperand = SI->getPointerOperand();

                        if(isa<MemNode>(Operand))
                        {

                            GraphNode* newVar = Operand->clone();
                            MemNode * memNew = dyn_cast<MemNode>(newVar);
                            memNew->defLoc->parentBlock = parentBlock;
                            //    errs()<<"\nFound a mem node..!! "<<parentBlock->getName() <<"\n";
                            //    memNew->defLoc->assignedValue = assignedVal;
                            memNew->defLoc->lineNumber = Instcounter;
                            memNew->defLoc->parentFunction = parentBlock->getParent();
                            memNew->defLoc->MemInst = param;//dyn_cast<Value*>(&SI);
                            memNew->defLoc->setVal();
                            //  errs()<<"\n IDVALUE Obtained: ---- "<< memNew->defLoc->IdValue;
                            set<MemoryOperation* > defSets = RDSMap[param];
                            defSets.insert(memNew->defLoc);
                            RDSMap[param] = defSets;
                            // RDSMap[pointerOperand].insert(memNew->defLoc);
                            //          errs()<<"\n  New node gen:    "<<memNew->getLabel()<<"   Line   :"<< Instcounter<<"\n";
                            //        v->dump();
                            StoreNodeMap[param] = memNew;
                            Operand->connect(memNew);
                            Operand = memNew;
                            if(debug) errs()<<"\n ~~~~~~~~~~~~  Node size before inert...: "<<nodes.size()<<"  name: "<<memNew->getLabel();
                            nodes.insert(memNew);
                            if(debug) errs()<<"\n ~~~~~~~~~~~~  Node size after inert...: "<<nodes.size();
                            memNodes[0] = memNew;


                            set<MemoryOperation*> memOps = LiveMap[param];
                            if(isa<GetElementPtrInst>(param))
                            {
                                GetElementPtrInst* GI = dyn_cast<GetElementPtrInst>(param);
                                Value * RootPtr =  GI->getPointerOperand();

                                set<MemoryOperation*> memOpsRoot = LiveMap[RootPtr];
                                memOps.insert(memOpsRoot.begin(),memOpsRoot.end());
                            }

                            //set<GraphNode*> relatedStores = StoreNodeMap[loadptr];


                            for(set<MemoryOperation*>::iterator memIt = memOps.begin(); memIt != memOps.end();++memIt)
                            {
                                GraphNode * storeNode = StoreNodeMap[(*memIt)->MemInst];
                                if(storeNode!=NULL)
                                {
                                    storeNode->connect(memNew);
                                    //  errs()<<"\nAdding Edge between store live map to arg "<< storeNode->getLabel()<<" -- "<<memNew->getLabel();
                                }
                            }

                            memNew->connect(Op);
                            //          errs()<<"\nAdding Edge between new node to call node.. "<< memNew->getLabel()<<" -- "<<Op->getLabel();

                            callParamNodeMap[&CS][param] = memNew;
                            //update the callsite info struct here..
                            CallSiteMap * paramMap =CallSiteParams[CI];
                            if(!paramMap)
                                paramMap = new CallSiteMap();
                            paramMap->ArgNodeMap[param] = memNew;
                            paramMap->cs = &CS;
                            paramMap->CI = CI;
                            CallSiteParams[CI] = paramMap;
                        }
                        else if (Operand != NULL)
                        {
                            Operand->connect(Op);
                            //   errs()<<"\nAdding Edge between "<< Operand->getLabel()<<" -- "<<Op->getLabel();
                            callParamNodeMap[&CS][param] = Operand;


                            CallSiteMap * paramMap =CallSiteParams[CI];
                            if(!paramMap)
                                paramMap = new CallSiteMap();

                            paramMap->ArgNodeMap[param] = Operand;
                            paramMap->cs = &CS;
                            paramMap->CI = CI;
                            CallSiteParams[CI] = paramMap;
                        }
                    }


                }

                //Connect the operands to the OpNode
                for (unsigned int i = 0; i < cast<User> (v)->getNumOperands(); i++) {

                    if (isa<StoreInst> (v) && i == 1)
                    {
                        //This case captures the storing in an already exisiting memory node...!
                        continue;
                    }
                    if(isa<CallInst> (v) )
                    {
                        //handled above.
                        continue;
                    }
                    if(isa<AllocaInst> (v))
                    {
                        continue;
                    }

                    Value *v1 = cast<User> (v)->getOperand(i);
                    Operand = this->addInst(v1,Instcounter,func);

                    if (Operand != NULL)
                        Operand->connect(Op);


                }

            }
        }

        return Var;
    }
    return NULL;
}

//Function :Overloaded for Data dep analysis:
//Processes each instruction based for data dependence.
GraphNode* Graph::addInst_new(Value *v) {

    GraphNode *Op, *Var, *Operand;

    CallInst* CI = dyn_cast<CallInst> (v);
    bool hasVarNode = true;

    if (isValidInst(v)) { //If is a data manipulator instruction (it is not br,switch, ret)
        //	if(debug) errs()<<"\n Adding Inst in Dep Graph :" <<*v;
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
                        Var = addInst_new(SI->getOperand(1)); // We do this here because we want to represent the store instructions as a flow of information of a data to a memory node
                    else if ((!isa<Constant> (v)) && isMemoryPointer(v)) {
                        Var = new MemNode();
                        memNodes[0] = Var;
                    } else {
                        Var = new VarNode(v);
                        varNodes[v] = Var;
                    }
                    nodes.insert(Var);
                }

            }

            if (isa<Instruction> (v)) {

                if (CI) {
                    Op = new CallNode(CI);
                    callNodes[CI] = Op;
                    //}
                    // else {
                    //        Op = new OpNode(dyn_cast<Instruction> (v)->getOpcode(), v);
                    // }
                    //opNodes[v] = Op;

                    nodes.insert(Op);
                    if (hasVarNode)
                        Op->connect(Var);

                    //Connect the operands to the OpNode
                    for (unsigned int i = 0; i < cast<User> (v)->getNumOperands(); i++) {

                        if (isa<StoreInst> (v) && i == 1)
                            continue; // We do this here because we want to represent the store instructions as a flow of information of a data to a memory node

                        Value *v1 = cast<User> (v)->getOperand(i);
                        Operand = this->addInst_new(v1);

                        if (Operand != NULL)
                            Operand->connect(Op);
                    }
                }
            }
        }

        //Print the instruction getting added:


        return Var;

    }
    return NULL;
}


GraphNode* Graph::addGlobalVal(GlobalVariable *gv)
{
    GraphNode *Var;
    Value *v = gv;

    if ((!isa<Constant> (v)) && isMemoryPointer(v)) {
        Var = new MemNode();
        memNodes[0] = Var;
        if(printMem)          errs()<<"\nMEMNODE GENERATED  ---- : "<<Var->getName()<<"  "<< Var->getLabel() << " for ";
        if(printMem)         v->dump();
    } else {  //if(!isa<Constant> (v))  .. dont generate the constant nodes.. makes it ugly plus adds no val.
        Var = new VarNode(v);
        varNodes[v] = Var;
    }

    //REplacing with simple add var node to remove memory segmentation
    Var = new VarNode(v);
    varNodes[v] = Var;

    nodes.insert(Var);

    return Var;
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
        case Instruction::Call:
            return true;

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

    //GraphNode* ConstNode;
    if ((!isa<Constant> (op)) && isMemoryPointer(op)) {
        int index =  0;
        if (memNodes.count(index))
            return memNodes[index];
    } else {
        if (varNodes.count(op))
            return varNodes[op];
        else if (opNodes.count(op))
            return opNodes[op];
        //else if(nodes.count(op))
        //    return ;
    }

    return NULL;
}


GraphNode* Graph::findInstNode(Value *op) {

    if(op)
    if(Instruction* inst = dyn_cast<Instruction>(op))
            return this->instMap[inst];
    else
        return NULL;
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

    for (Value::user_iterator UI = F->user_begin(), E = F->user_end(); UI != E; ++UI) {
        User *U = *UI;

        // Ignore blockaddress uses
        //NT: TODO
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
    //errs()<<
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
    return getNumEdges(etData);
}

int llvm::Graph::getNumControlEdges() {
    return getNumEdges(etControl);
}

std::set<GraphNode*> llvm::Graph::getDepValues(std::set<llvm::Value*> sources,
                                               bool forward) {
    unsigned long nnodes = nodes.size();
    std::set<GraphNode*> visited;
    std::set<GraphNode*> sourceNodes = findNodes(sources);
    std::list<GraphNode*> worklist;
    std::map<GraphNode*, edgeType> neigh;

    //NT: debug..
    // errs()<<"Source nodes count.  : "<<sourceNodes.size();

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



std::set<GraphNode*> llvm::Graph::getDataDepValues(std::set<llvm::Value*> sources,
                                               bool forward) {
    unsigned long nnodes = nodes.size();
    std::set<GraphNode*> visited;
    std::set<GraphNode*> sourceNodes = findNodes(sources);
    std::list<GraphNode*> worklist;
    std::map<GraphNode*, edgeType> neigh;

    //NT: debug..
    // errs()<<"Source nodes count.  : "<<sourceNodes.size();

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
            if(i->second == etData || i->second == etParam){
            if (!visited.count(i->first)) {
                worklist.push_back(i->first);
                visited.insert(i->first);
            }
            }
        }
        worklist.pop_front();
    }
    return visited;
}




llvm::Graph::Guider::Guider(Graph* graph) {
    this->graph = graph;
    std::set<GraphNode*> nodes = graph->getNodes();
    for (std::set<GraphNode*>::iterator i = nodes.begin(), e = nodes.end(); i != e; ++i) {
        string nodeLabel = (*i)->getLabel();
                nodeLabel.erase (std::remove(nodeLabel.begin(), nodeLabel.end(), '"'), nodeLabel.end());
        nodeAttrs[*i] = "[label=\"" + nodeLabel + "\" shape=\""+ (*i)->getShape() +"\" style=\""+ (*i)->getStyle()+"\"]";
    }
}

void llvm::Graph::Guider::setNodeAttrs(GraphNode* n, std::string attrs) {
    nodeAttrs[n] = attrs;
}

void llvm::Graph::Guider::setEdgeAttrs(GraphNode* u, GraphNode* v,
                                       std::string attrs) {
    edgeAttrs[std::make_pair(u, v)] = attrs;
}

void llvm::Graph::Guider::clear() {
    nodeAttrs.clear();
    edgeAttrs.clear();
}

std::string llvm::Graph::Guider::getNodeAttrs(GraphNode* n) {
    if (nodeAttrs.count(n))
        return nodeAttrs[n];
    return "";
}

std::string llvm::Graph::Guider::getEdgeAttrs(GraphNode* u, GraphNode* v) {
    std::pair<GraphNode*, GraphNode*> edge = std::make_pair(u, v);
    if (edgeAttrs.count(edge))
        return edgeAttrs[edge];
    return "";
}


