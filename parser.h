#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include "llvm/Support/raw_ostream.h"

class Parser {
	Lexer &lex;
	Token tok;
	bool has_error;

	void error() {
		llvm::errs() << "Unexpected: " << tok.getText() << "\n";
		has_error = true;
	}

	void advance() { lex.next(tok); }

	bool expect(Token::TokenKind kind) {
		if (tok.getKind() != kind) {
			error();
			return true;
		}
		return false;
	}

	bool consume(Token::TokenKind kind) {
		if (expect(kind)) return true;
		advance();
		return false;
	}

	Ast* parseCalc();
	Exp* parseExp();
	Exp* parseTerm();
	Exp* parseFactor();

public:
	Parser(Lexer &lex): lex(lex), has_error(false) { advance(); }
	bool has_error() { return has_error; }
	Ast* parse();
};

#endif