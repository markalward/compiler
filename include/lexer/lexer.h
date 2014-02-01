
#ifndef LEXER_H
#define LEXER_H

#include <sstream>
#include <lexer/reader.h>
#include <lexer/token.h>

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
	Token *makeNumToken(TokenName name, NumAttr attr);
	Token *readId();
	Token *readString();
	Token *readNumber(char c);
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
