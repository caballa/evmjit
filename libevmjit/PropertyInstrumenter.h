#pragma once

#include "llvm/IR/InstVisitor.h"

namespace llvm
{
  class Value;
  class StringRef;
}

namespace dev
{
namespace eth
{
namespace jit
{

class PropertyInstrumenter: public llvm::InstVisitor<PropertyInstrumenter> {
  friend class llvm::InstVisitor<PropertyInstrumenter>;
 public:
  virtual ~PropertyInstrumenter() {}
  // TODO: add more instructions
  
  /* InstVisitor methods */
  void visitAdd(llvm::BinaryOperator &I) { instrumentAdd(&I);}
  void visitSub(llvm::BinaryOperator &I) { instrumentSub(&I);}
  void visitMul(llvm::BinaryOperator &I) { instrumentMul(&I);}

  /* To be defined by derived classes */
  virtual llvm::StringRef getName () const = 0;
  
  virtual llvm::Value* instrumentAdd(llvm::Instruction *){
    return nullptr;
  }
  virtual llvm::Value* instrumentSub(llvm::Instruction *){
    return nullptr;
  }
  virtual llvm::Value* instrumentMul(llvm::Instruction *){
    return nullptr;
  }
};

}
}
}



    
