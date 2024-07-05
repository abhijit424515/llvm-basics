#include "sema.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

namespace {
	class DeclCheck: public AstVisitor {
		llvm::StringSet<> scope;
		bool has_error;

		enum ErrorType { Twice, Not };

		void error(ErrorType et, llvm::StringRef v) {
			llvm::errs() << "Variable " << v << " " << (et == Twice ? "already" : "not") << " declared\n";
			has_error = true;
		}

	public:
		DeclCheck(): has_error(false) {}
		bool hasError() { return has_error; }
		virtual void visit(Factor &node) override {
			if (node.getKind() == Factor::Ident) {
				if (scope.find(node.getVal()) == scope.end()) {
					error(Not, node.getVal());
				}
			}	
		}
		virtual void visit(BinaryOp &node) override {
			if (node.getLeft()) node.getLeft()->accept(*this);
			else has_error = true;

			if (node.getRight()) node.getRight()->accept(*this);
			else has_error = true;
		}
		virtual void visit(WithDecl &node) override {
			for (auto i = node.begin(), e = node.end(); i != e; i++) {
				if (!scope.insert(*i).second) error(Twice, *i);
			}
			if (node.getExp()) node.getExp()->accept(*this);
			else has_error = true;
		}
	};
}

bool Sema::semantic(Ast *tree) {
	if (!tree) return false;
	DeclCheck dc;
	tree->accept(dc);
	return dc.hasError();
}