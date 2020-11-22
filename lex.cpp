/*
 * lex.cpp
 *
 * CS280 - Fall 2020
 */

#include <cctype>
#include <map>

using std::map;
using namespace std;

#include "lex.h"

static map<string,Token> kwmap = {
		{ "print", PRINT },
		{ "begin", BEGIN },
		{ "end", END },
		{ "if", IF },
		{ "then", THEN },
};

LexItem id_or_kw(const string& lexeme, int linenum) {
	Token tt = IDENT;

	auto kIt = kwmap.find(lexeme);
	if( kIt != kwmap.end() )
		tt = kIt->second;

	return LexItem(tt, lexeme, linenum);
}

LexItem getNextToken(istream& in, int& linenum) {
	enum TokState { START, INID, INSTRING, ININT, INREAL, INCOMMENT, SIGN } lexstate = START;
	string lexeme;
	char ch;
    bool sawEscape;

    while(in.get(ch)) {
		switch( lexstate ) {
		case START:
			if( ch == '\n' )
				linenum++;
                
			if( isspace(ch) )
				continue;

			lexeme = ch;

			if( isalpha(ch) ) {
				lexstate = INID;
			}
			else if( ch == '"' ) {
				lexstate = INSTRING;
				sawEscape = false;
			}
			else if( isdigit(ch) ) {
				lexstate = ININT;
			}
			else if( ch == '/' && in.peek() == '/' ) {
				lexstate = INCOMMENT;
			}		
			
			else {
				Token tt = ERR;
				switch( ch ) {
				case '+':
                        tt = PLUS;
                        break;   
				case '-':
                    tt = MINUS;
					break;   
				case '*':
					tt = MULT;
					break;
				case '/':
					tt = DIV;
					break;
				case '=':
					tt = EQ;
					break;
				case '(':
					tt = LPAREN;
					break;
				case ')':
					tt = RPAREN;
					break;
				case ';':
					tt = SCOMA;
					break;
				case ',':
					tt = COMA;
					break;
				case '.':
					char nextch = in.peek();
					if(isdigit(nextch)){
						lexstate = INREAL;
						continue;
					}
					else {
						lexeme += nextch;
						return LexItem(ERR, lexeme, linenum);
						cout << "Here what?" << endl;
					}
					
				}
				
				return LexItem(tt, lexeme, linenum);
			}
			break;

		case INID:
			if( isalpha(ch) || isdigit(ch) ) {
				lexeme += ch;
			}
			else {
				in.putback(ch);
				return id_or_kw(lexeme, linenum);
			}
			break;

		case INSTRING:
            if( sawEscape ) {
                // last character was an escape, what is this one??
                sawEscape = false;
                if( ch == 'n' ) ch = '\n';
                // otherwise... it's unchanged
                lexeme += ch;
                break;
           }
                
            if( ch == '\\' ) {
                sawEscape = true;
                break;
            }
                
			lexeme += ch;
			if( ch == '\n' ) {
				return LexItem(ERR, lexeme, linenum);
			}
			if( ch == '"' ) {
				lexeme = lexeme.substr(1, lexeme.length()-2);
				return LexItem(SCONST, lexeme, linenum);
			}
			break;

		case ININT:
			if( isdigit(ch) ) {
				lexeme += ch;
			}
			else if(ch == '.') {
				lexstate = INREAL;
				in.putback(ch);
				//lexeme += ch;
				//continue;
			}
			else {
				in.putback(ch);
				return LexItem(ICONST, lexeme, linenum);
			}
			break;
		
		case INREAL:
			if( ch == '.' && isdigit(in.peek()) ) {
				lexeme += ch;
				
			}
			else if(isdigit(ch)){
				lexeme += ch;
			}
			else if(ch == '.' && !isdigit(in.peek())){
				lexeme += ch;
				return LexItem(ERR, lexeme, linenum);
			}
			else {
				in.putback(ch);
				return LexItem(RCONST, lexeme, linenum);
			}
			break;
			
		case INCOMMENT:
			if( ch == '\n' ) {
                ++linenum;
				lexstate = START;
			}
			break;
		}

	}//end of while loop

	if( in.eof() )
		return LexItem(DONE, "", linenum);
	return LexItem(ERR, "some strange I/O error", linenum);
}

ostream& operator<<(ostream& out, const LexItem& tok) {
	Token tt = tok.GetToken();
	
	if( tt == IDENT || tt == ICONST || tt == SCONST || tt == RCONST || tt == ERR ) {
		out << "(" << tok.GetLexeme() << ")" << tok.GetToken() <<endl;;
	}
	return out;
}
