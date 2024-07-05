#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

class CodeGen {
public:
	void compile(Ast *tree);
};

#endif