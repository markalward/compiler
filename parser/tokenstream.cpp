
#include <parser/tokenstream.h>

TokenStream::TokenStream(Lexer *lexer) :
	lex(lexer),
	buf(),
	pos(0)
{
	buf[0] = lex->getToken();
	buf[1] = lex->getToken();
}

void TokenStream::discard()
{
	delete buf[pos];
	buf[pos] = lex->getToken();
	pos = (pos+1) % 2;
}

void TokenStream::discard(TokenName expected)
{
	if(buf[pos]->name != expected)
		throw ParseException();
	else
		discard();
}

void TokenStream::discard(const TokenPredicate &pred)
{
	if(!pred(buf[pos]->name))
		throw ParseException();
	else
		discard();
}

unique_ptr<Token> TokenStream::take()
{
	Token *temp = buf[pos];
	buf[pos] = lex->getToken();
	pos = (pos+1) % 2;
	return unique_ptr<Token>(temp);
}

unique_ptr<Token> TokenStream::take(TokenName expected)
{
	if(buf[pos]->name != expected)
		throw ParseException();
	else
		return take();
}

unique_ptr<Token> TokenStream::take(const TokenPredicate &pred)
{
	if(!pred(buf[pos]->name))
		throw ParseException();
	else
		return take();
}

TokenName TokenStream::peekOne()
{
	return buf[pos]->name;
}

TokenPair TokenStream::peekTwo()
{
	TokenPair p;
	p.first = buf[pos]->name;
	p.second = buf[(pos+1) % 2]->name;
	return p;
}

