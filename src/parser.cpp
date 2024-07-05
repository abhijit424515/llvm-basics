#include "parser.h"

Ast* Parser::parse() {
	Ast* res = parseCalc();
	expect(Token::eoi);
	return res;
}

Ast* Parser::parseCalc() {
	Exp *e;
	llvm::SmallVector<llvm::StringRef, 8> vars;

	if (tok.is(Token::KW_with)) {
		advance();
		if (expect(Token::ident)) goto _error;
		vars.push_back(tok.getText());
		advance();

		while (tok.is(Token::comma)) {
			advance();
			if (expect(Token::ident)) goto _error;
			vars.push_back(tok.getText());
			advance();
		}

		if (consume(Token::colon)) goto _error;
	}

	e = parseExp();
	if (vars.empty()) return e;
	else return new WithDecl(vars, e);

_error:
	while (!tok.is(Token::eoi)) advance();
	return nullptr;
}

Exp* Parser::parseExp() {
	Exp *left = parseTerm();

	while (tok.isOneOf(Token::plus, Token::minus)) {
		BinaryOp::Operator op = tok.is(Token::plus) ? BinaryOp::Plus : BinaryOp::Minus;
		advance();
		Exp *right = parseTerm();
		left = new BinaryOp(op, left, right);
	}
	return left;
}

Exp* Parser::parseTerm() {
	Exp *left = parseFactor();

	while (tok.isOneOf(Token::star, Token::slash)) {
		BinaryOp::Operator op = tok.is(Token::star) ? BinaryOp::Mul : BinaryOp::Div;
		advance();
		Exp *right = parseFactor();
		left = new BinaryOp(op, left, right);
	}
	return left;
}

Exp* Parser::parseFactor() {
	Exp *res = nullptr;

	switch (tok.getKind()) {
		case Token::number:
			res = new Factor(Factor::Number, tok.getText());
			advance(); 
			break;
		case Token::ident:
			res = new Factor(Factor::Ident, tok.getText());
			advance();
			break;
		case Token::l_paren:
			advance();
			res = parseExp();
			if (!consume(Token::r_paren)) break;
		default:
			if (!res) error();
			while (!tok.isOneOf(Token::r_paren, Token::star, Token::plus, Token::minus, Token::slash, Token::eoi)) 
				advance();
	}
	return res;
}