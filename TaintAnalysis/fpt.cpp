//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Constants.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

struct FPT: public ModulePass {
  static char ID;
  FPT() : ModulePass(ID) {}

  static bool isFunctionPointerType(Type *t) {
    if (!t->isPointerTy())
      return false; // not even a pointer
    if (!t->getPointerElementType()->isFunctionTy())
      return false; // not a function pointer
    return true;
  }

  static bool isFunctionPointer(Value *v) {
    return isFunctionPointerType(v->getType());
  }

  static const char *constantFunctionPointerName(Value *val) {
    if (!val)
      return NULL;
    if (ConstantExpr *e = dyn_cast<ConstantExpr>(val))
      val = e->getOperand(0);
    if (!isFunctionPointer(val))
      return NULL;
    if (Constant *c = dyn_cast<Constant>(val))
      return c->getName().data();
    return NULL;
  }

  static bool constantTravel(Constant *c) {
    Type *ty = c->getType();
    if (ty->isArrayTy()) { // an array
      if (isFunctionPointerType(ty->getArrayElementType())) { // array of FPs
        for (unsigned i = 0; i < c->getNumOperands(); i++) {
          if (constantFunctionPointerName(c->getOperand(i))) {
            return true;
          }
        }
      } else { // array of anything else
        for (unsigned i = 0; i < c->getNumOperands(); i++) {
          if (constantTravel(cast<Constant>(c->getOperand(i))))
            return true;
        }
      }
    } else if (ty->isStructTy()) { // a struct
      for (unsigned i = 0; i < c->getNumOperands(); i++) {
        Constant *f = cast<Constant>(c->getOperand(i));
        if (constantFunctionPointerName(f)) {
          return true;
        } else {
          if (constantTravel(f))
            return true;
        }
      }
    }
    return false;
  }

  bool runOnModule(Module &M) override {
    // function pointer initialized at compile time
    for (Module::global_iterator G = M.global_begin(); G != M.global_end(); G++) {
      GlobalVariable *gv = &*G;
      if (!gv->hasDefinitiveInitializer())
        continue;
      Constant *initor = gv->getInitializer();
      if (constantFunctionPointerName(initor) || constantTravel(initor))
        errs() << *gv << "\n";
    }
    // function pointer initialized at runtime
    for (Module::iterator F = M.begin(); F != M.end(); F++) {
      for (inst_iterator I = inst_begin(F); I != inst_end(F); I++) {
        if (StoreInst *si = dyn_cast<StoreInst>(&*I)) {
          Value *val = si->getValueOperand();
          if (constantFunctionPointerName(val))
            errs() << *I << "\n";
        } else if (CallInst *ci = dyn_cast<CallInst>(&*I)) {
          for (unsigned i = 0; i < ci->getNumArgOperands(); i++) {
            Value *operand = ci->getArgOperand(i);
            if (constantFunctionPointerName(operand))
              errs() << *I << "\n";
          }
        } else if (ReturnInst *ri = dyn_cast<ReturnInst>(&*I)) {
          Value *ret = ri->getReturnValue();
          if (constantFunctionPointerName(ret))
            errs() << *I << "\n";
        }
      }
    }
    return false;
  }
};

char FPT::ID = 0;
static RegisterPass<FPT> fpt("fpt", "Function Pointer Targets");
