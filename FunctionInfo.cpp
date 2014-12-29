//===- InputPass.cpp - Mark the inputs for taint analysis pass. ---------------===//
//
//                     
// 
//
//===----------------------------------------------------------------------===//
//
// Description: This file defines the structure for reading function info.
//
//===----------------------------------------------------------------------===//



#include "FunctionInfo.h"



FunctionInfo::FunctionInfo()
{
}

DefUseInfo::DefUseInfo()
{
	isdef = false;
}

/*
void FunctionInfoSet::print()
{

errs()<< "\nSize of the Function set : " << FunctionInfos.size() <<"\n";
		
std::set<FunctionInfo*>::iterator FI;
for(FI= FunctionInfos.begin(); FI!=FunctionInfos.end(); ++FI)
 {
 	errs() << "\n" << (*FI)->FunctionName << " begin  :" << (*FI)->begin << " end  :" << (*FI)->end;
 	errs() << "  " << " block  :" << (*FI)->begin_block << " e block " << (*FI)->end_block; 
 }	
}
*/
