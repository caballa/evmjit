#pragma once

#include <llvm/ADT/DenseMap.h>
#include "PropertyInstrumenter.h"

namespace llvm {
  class Value;  
  class Module;
  class Function;
  class BasicBlock;
  class Instruction;
  class raw_ostream;
}


namespace dev
{
namespace eth
{
namespace jit
{

class PropertyInstrumenterManager {
  std::vector<PropertyInstrumenter*> m_instrumenters;
  llvm::Module &m_M;
  llvm::Function *m_error_fn;
  llvm::DenseMap<llvm::Function*, llvm::BasicBlock*> m_error_blocks;

  /* Insert the equivalent to "assert(Cond)" into the bitcode */
  void addAssertion(llvm::Value *Cond, llvm::Instruction *InsPt);
  
 public:
  
  PropertyInstrumenterManager(llvm::Module &M);
  
  ~PropertyInstrumenterManager();
  
  /* The manager takes ownership of PI so it will be in charge of
     deleting it */
  void registerInstrumenter(PropertyInstrumenter *PI);

  /* Add instrumentation for the result of addition */
  void instrumentAdd(llvm::Instruction *res);
  /* Add instrumentation for the result of subtraction */  
  void instrumentSub(llvm::Instruction *res);
  /* Add instrumentation for the result of multiplication */    
  void instrumentMul(llvm::Instruction *res);

  void printRegistered(llvm::raw_ostream &os);
  
};

}
}
}


    
