#pragma once

#include <llvm/ADT/StringRef.h>
#include <llvm/IR/IRBuilder.h>

#include "PropertyInstrumenter.h"

namespace dev
{
namespace eth
{
namespace jit
{

class IntegerOverflowInstrumenter: public PropertyInstrumenter {
  llvm::LLVMContext &m_Ctx;
  llvm::Value* instrumentBinOpHelper(llvm::Instruction *);
  
public:
  
  IntegerOverflowInstrumenter(llvm::LLVMContext &Ctx): m_Ctx(Ctx) {}
  virtual llvm::StringRef getName() const override
  { return "Integer Overflow"; }
  virtual llvm::Value *instrumentAdd(llvm::Instruction *) override;
  virtual llvm::Value *instrumentSub(llvm::Instruction *) override;
  virtual llvm::Value *instrumentMul(llvm::Instruction *) override;
};

}
}
}
