#include "codegen.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

namespace {
	class ToIRVisitor: public AstVisitor {
		Module *m;
		IRBuilder<> builder;
		Type *void_ty;
		Type *i32_ty;
		PointerType *ptr_ty;
		Constant *i32_zero;
		Value *v;
		StringMap<Value*> name_map;
	
	public:
		ToIRVisitor(Module *m): m(m), builder(m->getContext()) {
			void_ty = Type::getVoidTy(m->getContext());
			i32_ty = Type::getInt32Ty(m->getContext());
			ptr_ty = PointerType::getUnqual(m->getContext());
			i32_zero = ConstantInt::get(i32_ty, 0, true);
		}

		void run(Ast *tree) {
			FunctionType *main_fn_ty = FunctionType::get(i32_ty, { i32_ty, ptr_ty }, false);
			Function *main_fn = Function::Create(main_fn_ty, GlobalValue::ExternalLinkage, "main", m);

			BasicBlock *bb = BasicBlock::Create(m->getContext(), "entry", main_fn);
			builder.SetInsertPoint(bb);

			tree->accept(*this);
			FunctionType *calc_write_fn_ty = FunctionType::get(void_ty, { i32_ty }, false);
			Function *calc_write_fn = Function::Create(calc_write_fn_ty, GlobalValue::ExternalLinkage, "calc_write", m);
			builder.CreateCall(calc_write_fn_ty, calc_write_fn, { v });
			builder.CreateRet(i32_zero);
		}

		virtual void visit(WithDecl &node) override {
			FunctionType *read_fn_ty = FunctionType::get(i32_ty, { ptr_ty }, false);
			Function *read_fn = Function::Create(read_fn_ty, GlobalValue::ExternalLinkage, "read", m);

			for (auto i = node.begin(), e = node.end(); i != e; i++) {
				StringRef var = *i;
				Constant *str_text = ConstantDataArray::getString(m->getContext(), var);
				GlobalVariable *str = new GlobalVariable(*m, str_text->getType(), true, GlobalValue::PrivateLinkage, str_text, Twine(var).concat(".str"));
				CallInst *call = builder.CreateCall(read_fn_ty, read_fn, { str });
				name_map[var] = call;
			}

			node.getExp()->accept(*this);
		}

		virtual void visit(Factor &node) override {
			if (node.getKind() == Factor::Ident) v = name_map[node.getVal()];
			else {
				int ival;
				node.getVal().getAsInteger(10, ival);
				v = ConstantInt::get(i32_ty, ival, true);
			}
		}

		virtual void visit(BinaryOp &node) override {
			node.getLeft()->accept(*this);
			Value *left = v;

			node.getRight()->accept(*this);
			Value *right = v;

			switch (node.getOperator()) {
				case BinaryOp::Plus: v = builder.CreateNSWAdd(left, right); break;
				case BinaryOp::Minus: v = builder.CreateNSWSub(left, right); break;
				case BinaryOp::Mul: v = builder.CreateNSWMul(left, right); break;
				case BinaryOp::Div: v = builder.CreateSDiv(left, right); break;
			} 
		}
	};
}

void CodeGen::compile(Ast *tree) {
	LLVMContext ctx;
	Module *m = new Module("calc.expr", ctx);
	ToIRVisitor visitor(m);
	visitor.run(tree);
	m->print(outs(), nullptr);
}