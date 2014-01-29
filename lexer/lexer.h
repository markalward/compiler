
#ifndef LEXER_H
#define LEXER_H

#include "reader.h"
#include "token.h"

class Lexer
{
protected:
	Reader &input;

public:
	Lexer(Reader &input) :
		input(input)
	{}

	virtual Token *getToken() = 0;
};


class IBTLLexer : Lexer
{
private:
	KeywordTable keywordTable;

public:
	
	Token *makeIdToken();
	Token *makeLiteralToken(TokenName name);
	Token *makeOpToken(TokenName name);
	Token *readId();
	Token *readString();
	Token *readNumber();
	Token *readRelop(char c);
	Token *readOp(char c);

public:
	IBTLLexer(Reader &input) :
		Lexer(input)
	{}

	Token *getToken();
};




#endif
