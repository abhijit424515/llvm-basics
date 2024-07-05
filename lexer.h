#ifndef LEXER_H
#define LEXER_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"

class Lexer;

class Token {
	friend class Lexer;

public:
	enum TokenKind : unsigned short {
		eoi, unknown, ident, number, comma, colon, plus,
		minus, star, slash, l_paren, r_paren, KW_with
	};

private:
	TokenKind kind;
	llvm::StringRef text;

public:
	TokenKind getKind() const { return kind; }
	llvm::StringRef getText() const { return text; }
	bool is(TokenKind k) const { return kind == k; }
	bool isOneOf(TokenKind k1, TokenKind k2) const {
		return is(k1) || is(k2);
	}
	template <typename... Ts>
	bool isOneOf(TokenKind k1, TokenKind k2, Ts... ks) const {
		return is(k1) || isOneOf(k2, ks...);
	}
};

class Lexer {
	const char *buffer_start;
	const char *buffer_ptr;
public:
	Lexer(const llvm::StringRef &buffer) {
		buffer_start = buffer.begin();
		buffer_ptr = buffer_start;
	}
	void next(Token &token);
private:
	void formToken(Token &result, const char *tok_end, Token::TokenKind kind);
};

#endif