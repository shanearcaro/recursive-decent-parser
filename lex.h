/*
 * lex.h
 *
 * CS280
 * Fall 2020
 */

#ifndef LEX_H_
#define LEX_H_

#include <string>
#include <iostream>
using std::string;
using std::istream;
using std::ostream;

enum Token {
		// keywords
	PRINT, BEGIN, END, IF, THEN,

		// an identifier
	IDENT,

		// an integer and string constant
	ICONST, RCONST, SCONST,

		// the operators, parens, semicolon
	PLUS, MINUS, MULT, DIV, EQ, LPAREN, RPAREN, SCOMA, COMA,
		// any error returns this token
	ERR,

		// when completed (EOF), return this token
	DONE
};

class LexItem {
	Token	token;
	string	lexeme;
	int	lnum;

public:
	LexItem() {
		token = ERR;
		lnum = -1;
	}
	LexItem(Token token, string lexeme, int line) {
		this->token = token;
		this->lexeme = lexeme;
		this->lnum = line;
	}

	bool operator==(const Token token) const { return this->token == token; }
	bool operator!=(const Token token) const { return this->token != token; }

	Token	GetToken() const { return token; }
	string	GetLexeme() const { return lexeme; }
	int	GetLinenum() const { return lnum; }
};

extern ostream& operator<<(ostream& out, const LexItem& tok);

extern LexItem getNextToken(istream& in, int& linenum);


#endif /* LEX_H_ */
