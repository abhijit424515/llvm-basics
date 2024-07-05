#include "lexer.h"

namespace charinfo {
	LLVM_READNONE inline bool isWhitespace(char c) {
		return c == ' ' || c == '\t' || c == '\f' || c == '\v' || c == '\r' || c == '\n';
	}
	LLVM_READNONE inline bool isDigit(char c) {
		return c >= '0' && c <= '9';
	}
	LLVM_READNONE inline bool isLetter(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}
}

void Lexer::next(Token &token) {
	while (*buffer_ptr && charinfo::isWhitespace(*buffer_ptr)) 
		++buffer_ptr;

	if (!*buffer_ptr) {
		token.kind = Token::eoi;
		return;
	}

	if (charinfo::isLetter(*buffer_ptr)) {
		const char* end = buffer_ptr+1;
		while (charinfo::isLetter(*end)) end++;

		llvm::StringRef name(buffer_ptr, end-buffer_ptr);
		Token::TokenKind kind = name == "with" ? Token::KW_with : Token::ident;

		formToken(token, end, kind);
	} else if (charinfo::isDigit(*buffer_ptr)) {
		const char* end = buffer_ptr+1;
		while (charinfo::isDigit(*end)) end++;

		formToken(token, end, Token::number);
	} else {
		switch (*buffer_ptr) {
			#define CASE(ch, tok) \ 
			case ch: formToken(token, buffer_ptr + 1, tok); break;
			CASE('+', Token::plus);
			CASE('-', Token::minus);
			CASE('*', Token::star);
			CASE('/', Token::slash);
			CASE('(', Token::l_paren);
			CASE(')', Token::r_paren);
			CASE(':', Token::colon);
			CASE(',', Token::comma);
			#undef CASE
			default: formToken(token, buffer_ptr + 1, Token::unknown);
		}
	}
	return;
}

void Lexer::formToken(Token &tok, const char* tok_end, Token::TokenKind kind) {
	tok.kind = kind;
	tok.text = llvm::StringRef(buffer_ptr, tok_end - buffer_ptr);
	buffer_ptr = tok_end;
}