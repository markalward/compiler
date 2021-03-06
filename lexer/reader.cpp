
#include <lexer/reader.h>

Reader::Reader(std::istream *in) :
	in(in),
	charbuf(0),
	charbuf_full(false),
	quotemode(false),
	readbuf(),
	pos(0),
	line(1),
	startline(line)
{}

char Reader::getChar()
{
	char c;
	
	if(charbuf_full) {
		charbuf_full = false;
		c = charbuf;
	}
	else {
		in->get(c);
		if(in->eof()) c = 0;
	}
		
	if(c == '\n') line++;

    if(pos >= BUFSIZE) c = 0;
	else readbuf[pos++] = c;
	return c;
}

void Reader::putChar()
{
	if(charbuf_full)
		throw std::exception();
	charbuf = readbuf[pos-1];
	charbuf_full = true;
	if(charbuf == '\n') line--;
	pos--;
}

const char *Reader::getLexeme()
{
	// null terminate the buffer
	readbuf[pos] = 0;
	return readbuf;
}

void Reader::clearLexeme()
{
	startline = line;
	pos = 0;
}
