#include "PropertyInstrumenterManager.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Support/raw_ostream.h>

#include <algorithm>

using namespace llvm;

namespace dev
{
namespace eth
{
namespace jit
{

PropertyInstrumenterManager::PropertyInstrumenterManager(Module &M)
  : m_M(M), m_error_fn(nullptr) {

  LLVMContext &Context = M.getContext ();
  AttrBuilder B;
  AttributeList as = AttributeList::get (Context,
				       AttributeList::FunctionIndex,
				       B);
  as = AttributeList::get (Context, AttributeList::FunctionIndex, B);
  m_error_fn =  dyn_cast<Function>
    (m_M.getOrInsertFunction ("__VERIFIER_error",
			    as,
			    Type::getVoidTy (Context),
			    NULL));

}

void PropertyInstrumenterManager::addAssertion(Value *Cond,
					       Instruction *InsPt) {
  // Check that InsPt cannot be the last instruction of its block
  assert(!isa<TerminatorInst>(InsPt));
  // Cond should be a boolean (e.g., from ICmp)
  assert(Cond->getType()->isIntegerTy(1));  
  
  BasicBlock *ErrorBB = nullptr;
  // Get error block
  {
    Function *F = InsPt->getParent()->getParent();    
    auto it = m_error_blocks.find(F);
    if (it != m_error_blocks.end()) {
      ErrorBB = it->second;
    } else {
      LLVMContext &Context = m_M.getContext ();
      ErrorBB = BasicBlock::Create(Context, "SeaHorn_Error", F);
      IRBuilder<> B(Context);
      B.SetInsertPoint(ErrorBB);
      B.CreateCall(m_error_fn);
      B.CreateUnreachable();
      m_error_blocks[F] = ErrorBB;
    }
  }

  auto it = InsPt->getIterator();
  it++;
  Instruction *SplitPt = &*it;
  BasicBlock *OldBB = InsPt->getParent();
  BasicBlock *NewBB = SplitBlock(OldBB, SplitPt);
  TerminatorInst *OldTI = OldBB->getTerminator();
  OldTI->eraseFromParent();
  BranchInst::Create(NewBB, ErrorBB, Cond, OldBB);
}

void PropertyInstrumenterManager::instrumentAdd(Instruction *res) {
  for (size_t i=0, e=m_instrumenters.size(); i<e; ++i) {
    if (auto cond = m_instrumenters[i]->instrumentAdd(res)) {
      addAssertion(cond, llvm::cast<Instruction>(cond));      
    }
  }
}
  
void PropertyInstrumenterManager::instrumentSub(Instruction *res) {
  for (size_t i=0, e=m_instrumenters.size(); i<e; ++i) {
    if (auto cond = m_instrumenters[i]->instrumentSub(res)) {
      addAssertion(cond, llvm::cast<Instruction>(cond));      
    }
  }
}
  
void PropertyInstrumenterManager::instrumentMul(Instruction *res) {
  for (size_t i=0, e=m_instrumenters.size(); i<e; ++i) {
    if (auto cond = m_instrumenters[i]->instrumentMul(res)) {
      addAssertion(cond, llvm::cast<Instruction>(cond));      
    }
  }  
}

void PropertyInstrumenterManager::printRegistered(llvm::raw_ostream &os) {
  if (m_instrumenters.empty()){
    os << "No instrumentation.\n";
  } else {
    os << "Instrumentations: \n";
    for (size_t i=0, e=m_instrumenters.size(); i<e; ++i) {
      os << "\t" << m_instrumenters[i]->getName() << "\n";
    }
  }
}
  
PropertyInstrumenterManager::~PropertyInstrumenterManager() {
  for (size_t i=0, e=m_instrumenters.size(); i<e; ++i) {  
    delete m_instrumenters[i];
  }
}

void PropertyInstrumenterManager::registerInstrumenter(PropertyInstrumenter *pi){
  auto it = std::find(m_instrumenters.begin(), m_instrumenters.end(), pi);
  if (it == m_instrumenters.end()) {
    m_instrumenters.push_back(pi);
  }
}

}
}
}
    
