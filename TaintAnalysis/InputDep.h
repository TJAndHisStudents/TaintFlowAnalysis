#ifndef __INPUTDEP_H__
#define __INPUTDEP_H__

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/User.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/ilist.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/IR/Metadata.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"
#include "FlowGraph.h"
#include "llvm/ADT/StringRef.h"
#include<set>
#include<string>

using namespace llvm;

//Class to store the input source files:
class TaintSource {
public:
    std::string functionName;
    std::string variable;
    std::string label;
    TaintSource();
    void Print();
};

class QueryInput {
public:
    std::string operation;
    set<std::string> labels;
    set<Value*> labVals;
    bool cond;
    bool constcheck;
   // std::string label;
    QueryInput();
 //   void Print();
};



//Class to store the structs identofoed to be assigned the fp value:
class SourceType {
	private:
	//	std::set<Value*> 
		
	public:
		Value * fieldVal;
		Value * structVal;
	    Type * fieldType;
		Type * structType;
		std::string fieldName;
		std::string structName;
		SourceType();
		void Print();
};

class InputDep : public ModulePass {
	private:
        std::set<Value*> inputDepValues;
        std::map<Value*, std::string> ValLabelmap;
        std::map< std::string, Value*> LabelValmap;

		std::set<CallInst*> targetFunctions;
		std::set<BasicBlock*>  targetBlocks;
		std::map<Value*, int> lineNo;
		std::set<std::string>  targetNames;
		std::set<SourceType*> sourceTypes;
        std::set<TaintSource*> taintSources;
        std::set<QueryInput*> queryInputs;


        std::set<std::string> mediatorFunctions;
        std::set<std::string> sinkFunctions;

//        std::set<RelevantFields*> relevantFields;
        std::set<Value*> inputUses;
		bool runOnModule(Module &M);
	public:
		static char ID; // Pass identification, replacement for typeid.
		InputDep();
		void getAnalysisUsage(AnalysisUsage &AU) const;
		std::set<std::string> getTargetNames();
		std::set<CallInst*> getTargetFunctions();
		std::set<BasicBlock*> getTargetBlock();
		bool isInputDependent(Value* V);
        std::set<Value*> getInputDepValues();
        std::map<Value*,std::string> getValueLabelMap();
		std::set<SourceType*> getSourceTypes();
        std::set<std::string> getMediators();
        std::set<std::string> getSinks();
        std::set<QueryInput*> getQueryVals();
//        std::set<RelevantFields*>  getFields();
        void ListAllUses(Value* Input, Function *F);
		int getLineNo(Value*);
		void printer();
		void ReadSources();
		void ReadTargets();
		void ReadFPTargets();
        void ReadTaintInput();
        void ReadMediatorInput();
        void ReadSinkInput();
         void ReadQueryInput();
//        void ReadRelevantFields();
};

#endif
