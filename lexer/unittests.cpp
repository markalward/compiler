
#include "reader.h"
#include "lexer.h"
#include "token.h"
#include <iostream>
#include <sstream>
#include <assert.h>

using namespace std;

class ReaderTest
{
public:

	void getChar()
	{
		string in = "hello        world";
		istringstream str(in);
		Reader obj(&str);
		
		string exp = "helloworld";
		char c;
		const char *lex;

		for(int i = 0; i < exp.size(); i++) {
			c = obj.getChar();
			assert(c == exp[i]);
		}

		assert(obj.getChar() == 0);

		lex = obj.getLexeme();
		assert(string(lex) == exp);	
	}

	void putChar()
	{
		string in = "abcdefg";
		istringstream str(in);
		Reader obj(&str);
		
		
		for(int i = 0; i < 5; i++)
			obj.getChar();
		obj.putChar();

		const char *lex = obj.getLexeme();
		assert(string(lex) == string("abcd"));
		obj.clearLexeme();
		
		obj.getChar();
		obj.getChar();
		obj.getChar();
		lex = obj.getLexeme();
		assert(string(lex) == string("efg"));
	}
};


class IBTLLexerTest
{
public:

	void readString()
	{
		string in = "\"hello world\"\"next\"";
		istringstream str(in);
		Reader read(&str);
		read.getChar();

		IBTLLexer lex(read);
		LiteralToken *tok = (LiteralToken *) lex.readString();
		read.clearLexeme();
		read.getChar();
		LiteralToken *next = (LiteralToken *) lex.readString();
		assert(tok->val == string("\"hello world\""));
		assert(next->val == string("\"next\""));
	}
	

	void readNumber()
	{
		string in = "123+4.75";
		istringstream str(in);
		Reader read(&str);
		read.getChar();

		IBTLLexer lex(read);
		LiteralToken *tok = (LiteralToken *) lex.readNumber();
		read.clearLexeme();
		Token *op = lex.readOp(read.getChar());
		read.clearLexeme();
		LiteralToken *next = (LiteralToken *) lex.readNumber();
		cout << tok->val << endl;
		cout << next->val << endl;
		assert(op->name == TK_PLUS);
	}


	void readRelop()
	{
		string in = "<==>=!=";
		istringstream str(in);
		Reader read(&str);
		char c;

		IBTLLexer lex(read);
		c = read.getChar();
		Token *one = lex.readRelop(c);
		read.clearLexeme();
		c = read.getChar();
		Token *two = lex.readRelop(c);
		read.clearLexeme();
		c = read.getChar();
		Token *three = lex.readRelop(c);
		read.clearLexeme();
		c = read.getChar();
		Token *four = lex.readRelop(c);
		read.clearLexeme();

		assert(one->name == TK_LE);
		assert(two->name == TK_EQ);
		assert(three->name == TK_GE);
		assert(four->name == TK_NE);
	}

	void getToken1()
	{
		string in = "[[if a b c<=5+7.563]+1]";
		istringstream str(in);
		Reader read(&str);
		
		IBTLLexer lex(read);
		Token *tok;
		while(tok = lex.getToken()) {
			cout << tok->toString() << endl;
		}
	}
	
};

int main(int argc, char *argv[])
{
	IBTLLexerTest t;
	t.getToken1();
}

