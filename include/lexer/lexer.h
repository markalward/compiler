
#ifndef LEXER_H
#define LEXER_H

#include <lexer/reader.h>
#include <lexer/token.h>

class Lexer
{
public:

	virtual Token *getToken() = 0;
};


class IBTLLexer : public Lexer
{
private:
	Reader &input;
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
	void readWs();

public:
	IBTLLexer(Reader &input) :
		Lexer(),
		input(input),
		keywordTable()
	{}

	Token *getToken();
};




#endif
