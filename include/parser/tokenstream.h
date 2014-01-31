
#ifndef TOKENSTREAM_H
#define TOKENSTREAM_H

#include <iostream>
#include <exception>
#include <vector>
#include <memory>
#include <lexer/lexer.h>
#include <lexer/token.h>

using namespace std;

class ParseException : public exception
{
public:
	
};

typedef pair<TokenName, TokenName> TokenPair;

class TokenPredicate
{
public:
	virtual ~TokenPredicate() {}

	virtual bool operator() (TokenName name) const = 0;
};

/*
	presents the output from a Lexer as a stream of
	tokens for use by the parser. Allows the parser
	to peek up to 2 tokens ahead before consuming 
	them.
*/
class TokenStream
{
private:
	Lexer *lex;
	Token *buf[2];
	int pos;

public:
	TokenStream(Lexer *lex);
	void discard();
	void discard(TokenName expected);
	void discard(const TokenPredicate &pred);
	unique_ptr<Token> take();	
	unique_ptr<Token> take(TokenName expected);
	unique_ptr<Token> take(const TokenPredicate &pred);
	TokenName peekOne();
	TokenPair peekTwo();
};


// TODO: move to lexer.h
class TestLexer : public Lexer
{
private:
	vector<Token *> tokens;
	int pos;

public:
	TestLexer() :
		tokens(),
		pos(0)
	{}

	void setTokens(vector<Token *> arr)
	{
		tokens = arr;
	}

	Token *getToken()
	{
		if(pos == tokens.size())
			return tokens[pos-1];
		else
			return tokens[pos++];
	}
};

#endif
