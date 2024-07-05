#ifndef SEMA_HH
#define SEMA_HH

#include "ast.h"
#include "lexer.h"

class Sema {
public:
	bool semantic(Ast *tree);
};

#endif