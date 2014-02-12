
#ifndef LEXER_H
#define LEXER_H

#include <sstream>
#include <lexer/reader.h>
#include <lexer/token.h>
#include <symtable.h>

class LexException : public std::exception
{
	std::string msg;
	int line;
public:
	
	LexException(const std::string &msg, Reader &rd) :
		msg(msg),
		line(rd.getStartLine())
	{}

	~LexException() throw() {}
	
	const char *what() const throw()
	{
		std::ostringstream str;
		str << "line " << line << ": " << msg;
		return str.str().c_str();
	}
};

class Lexer
{
public:

	virtual Token getToken() = 0;
};


class IBTLLexer : public Lexer
{
private:
	Reader &input;
	SymbolTable &symTable;

public:
	
	Token makeIdToken();
	Token makeLiteralToken(TokenAttr attr);
	Token makeOpToken(TokenName name, TokenAttr attr);
	Token readId();
	Token readString();
	Token readNumber(char c);
	Token readRelop(char c);
	Token readOp(char c);
	void readWs();

public:
	IBTLLexer(Reader &input, SymbolTable &symTable) :
		Lexer(),
		input(input),
		symTable(symTable)
	{}

	Token getToken();
};




#endif
