
#ifndef READER_H
#define READER_H

#include <iostream>

class Reader
{
private:
	enum {
		BUFSIZE=4096
	};

	std::istream *in;
	char charbuf;
	bool charbuf_full;
	bool quotemode;
	char readbuf[BUFSIZE];
	int pos;
	int line;
	
public:
	Reader(std::istream *in);

	char getChar();

	void putChar();

	const char *getLexeme();

	void clearLexeme();

	int getLine() {return line; }
};

#endif
