#include "codegen.h"
#include "parser.h"
#include "sema.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

static llvm::cl::opt<std::string>
	input(llvm::cl::Positional, llvm::cl::desc("<input expression>"), llvm::cl::init(""));

int main(int argc, const char **argv) {
	llvm::InitLLVM X(argc, argv);
	llvm::cl::ParseCommandLineOptions(argc, argv, "calc - the expression compiler\n");

	Lexer lex(input);
	Parser parser(lex);
	Ast *tree = parser.parse();

	if (!tree || parser.hasError()) {
		llvm::errs() << "Syntax error\n";
		return 1;
	}

	Sema semantic;
	if (semantic.semantic(tree)) {
		llvm::errs() << "Semantic error\n";
		return 1;
	}

	CodeGen cg;
	cg.compile(tree);
	return 0;
}