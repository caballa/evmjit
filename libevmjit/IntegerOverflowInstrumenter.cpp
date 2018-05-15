#include "IntegerOverflowInstrumenter.h"
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>


namespace dev
{
namespace eth
{
namespace jit
{

using namespace llvm;

/* 
   For better way of checking overflow, read "Understanding Integer
   Overflow in C/C++" by Dietz et al.
*/  
Value* IntegerOverflowInstrumenter::instrumentBinOpHelper(Instruction *I) {
  assert (I->getType()->isIntegerTy());
  
  unsigned w = I->getType()->getIntegerBitWidth();
  Value *Op1 = I->getOperand(0);
  Value *Op2 = I->getOperand(1);
  
  IRBuilder<> B(m_Ctx);
  B.SetInsertPoint(I);
  IntegerType *biggerTy = IntegerType::get(m_Ctx, w+1); 
  Value *newOp1 = B.CreateSExt(Op1, biggerTy);
  Value *newOp2 = B.CreateSExt(Op2, biggerTy);
  Value *res = nullptr;
  switch (I->getOpcode()) {
  case BinaryOperator::Add:
    res = B.CreateAdd(newOp1, newOp2);
    break;
  case BinaryOperator::Sub:
    res = B.CreateSub(newOp1, newOp2);
    break;
  case BinaryOperator::Mul:
    res = B.CreateMul(newOp1, newOp2);
    break;
  default:
    errs () << "TODO: no instrumentation for " << *I << "\n";
    assert(false);
  }

  // min = 0...0 and max = 01...1 over w+1 bits
  /////////////////////////////////////////////////////////////////
  // FIXME: we don't now if the operation is signed or not. We are
  // assuming the operation is unsigned.
  // 
  // XXX: I think it's better to add "assert" in the solidity code
  // using e.g., SafeMath library and translate the "assert" into its
  // corresponding LLVM bitcode.
  /////////////////////////////////////////////////////////////////  
  Constant *min = ConstantInt::get(biggerTy, APInt::getMinValue(w+1));
  Constant *max = ConstantInt::get(biggerTy, APInt::getMaxValue(w+1).lshr(1));
  Value *lower = B.CreateICmpUGE(res, min, "int_overflow.min");
  Value *upper = B.CreateICmpULE(res, max, "int_overflow.max");  
  Value *cond = B.CreateAdd(lower, upper);
  return cond;  
}

  
Value* IntegerOverflowInstrumenter::instrumentAdd(Instruction *I){
  return instrumentBinOpHelper(I);
}
  
Value* IntegerOverflowInstrumenter::instrumentSub(Instruction *I){
  return instrumentBinOpHelper(I);  
}
  
Value* IntegerOverflowInstrumenter::instrumentMul(Instruction *I) {
  return instrumentBinOpHelper(I);    
}

}
}
}
