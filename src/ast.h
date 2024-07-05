#ifndef AST_HH
#define AST_HH

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

class Ast;
class Exp;
class Factor;
class BinaryOp;
class WithDecl;

class AstVisitor {
public:
	virtual void visit(Ast &) {}
	virtual void visit(Exp &) {}
	virtual void visit(Factor &) = 0;
	virtual void visit(BinaryOp &) = 0;
	virtual void visit(WithDecl &) = 0;
};

class Ast {
public:
	virtual ~Ast() {}
	virtual void accept(AstVisitor &v) = 0;
};

class Exp : public Ast {
public:
	Exp() {}
};

class Factor: public Exp {
public:
	enum ValueKind { Ident, Number };

private:
	ValueKind kind;
	llvm::StringRef val;

public:
	Factor(ValueKind kind, llvm::StringRef val): kind(kind), val(val) {}
	ValueKind getKind() { return kind; }
	llvm::StringRef getVal() { return val; }
	virtual void accept(AstVisitor &v) override { v.visit(*this); }
};

class BinaryOp: public Exp {
public:
	enum Operator { Plus, Minus, Mul, Div };

private:
	Exp *left, *right;
	Operator op;

public:
	BinaryOp(Operator op, Exp *left, Exp *right): op(op), left(left), right(right) {}
	Exp* getLeft() { return left; }
	Exp* getRight() { return right; }
	Operator getOperator() { return op; }
	virtual void accept(AstVisitor &v) override { v.visit(*this); }
};

class WithDecl: public Ast {
	using VarVector = llvm::SmallVector<llvm::StringRef, 8>;
	VarVector vars;
	Exp *e;

public:
	WithDecl(VarVector vars, Exp *e): vars(vars), e(e) {}
	VarVector::const_iterator begin() { return vars.begin(); }
	VarVector::const_iterator end() { return vars.end(); }
	Exp* getExp() { return e; }
	virtual void accept(AstVisitor &v) override { v.visit(*this); }
};

#endif