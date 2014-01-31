
#include <locale>
#include <lexer/lexer.h>
#include <lexer/token.h>
#include <lexer/reader.h>

using namespace std;

/*
	reads the current lexeme from the input. If it is a keyword, a keyword
	token is returned. Otherwise, an identifier token is 
	returned.
*/
Token *IBTLLexer::makeIdToken()
{
	const char *lexeme = input.getLexeme();
	TokenName keyword;
	if(keywordTable.isKeyword(lexeme, keyword)) {
		Token *ret = new Token(keyword);
		return ret;
	}
	else {
		IdToken *ret = new IdToken();
		ret->val = lexeme;
		return ret;
	}
}


/*
	reads the current lexeme from the input, and creates a new literal
	token for the lexeme. Note: ints/reals require that last input char
	be put back. This must be done prior to this call.
*/
Token *IBTLLexer::makeLiteralToken(TokenName name)
{
	const char *lexeme = input.getLexeme();
	// note: the lexeme string is copied into the token, which could be
	// expensive
	return new LiteralToken(name, lexeme);
}


Token *IBTLLexer::makeOpToken(TokenName name)
{
	return new Token(name);
}


Token *IBTLLexer::readId()
{
	char c;

	while(true) {
		c = input.getChar();
		if(std::isalnum(c) || c == '_') continue;
		else break;
	}

	input.putChar();
	return makeIdToken();
}


Token *IBTLLexer::readString()
{
	char c;
	while(true) {
		c = input.getChar();
		if(c == EOF || c == '\n')
			throw std::exception();
		else if(c == '\"')
			break;
	}

	return makeLiteralToken(TK_STR);
}


Token *IBTLLexer::readNumber()
{
	char c;
	int state = 0;
	const int acceptInt = 6,
		  acceptReal = 7,
		  reject = 8; //the accept/reject states
	
	while(true) {
		switch(state) {
		case 0:
			c = input.getChar();
			if(std::isdigit(c)) state = 0;
			else if(c == '.') state = 1;
			else state = acceptInt;
			break;
		case 1:
			c = input.getChar();
			if(std::isdigit(c)) state = 2;
			else state = reject;
			break;
		case 2:
			c = input.getChar();
			if(std::isdigit(c)) state = 2;
			else if(c == 'e' || c == 'E') state = 3;
			else state = acceptReal;
			break;
		case 3:
			c = input.getChar();
			if(std::isdigit(c))  state = 5;
			else if(c == '+' || c == '-') state = 4;
			else state = reject;
			break;
		case 4:
			c = input.getChar();
			if(std::isdigit(c)) state = 5;
			else state = reject;
			break;
		case 5:
			c = input.getChar();
			if(std::isdigit(c)) state = 5;
			else state = acceptReal;
			break;
		case acceptInt:
			input.putChar();
			return makeLiteralToken(TK_INT);
			break;
		case acceptReal:
			input.putChar();
			return makeLiteralToken(TK_REAL);
			break;
		case reject:
			throw std::exception();
		}
	}
}

Token *IBTLLexer::readRelop(char c)
{
	char next;
	switch(c) {
	case '<':
		next = input.getChar();
		if(next == '=')
			return makeOpToken(TK_LE);
		else {
			input.putChar();
			return makeOpToken(TK_LT);
		}
		break;
	case '>':
		next = input.getChar();
		if(next == '=')
			return makeOpToken(TK_GE);
		else
			input.putChar();
			return makeOpToken(TK_GT);
		break;
	case '=':
		return makeOpToken(TK_EQ);
		break;
	case '!':
		next = input.getChar();
		if(next == '=')
			return makeOpToken(TK_NE);
		else
			throw std::exception();
	}
}


Token *IBTLLexer::readOp(char c)
{
	switch(c) {
	case '+': return makeOpToken(TK_PLUS); break;
	case '-': return makeOpToken(TK_MINUS); break;
	case '*': return makeOpToken(TK_MULT); break;
	case '/': return makeOpToken(TK_DIV); break;
	case '^': return makeOpToken(TK_EXP); break;
	case '%': return makeOpToken(TK_MOD); break;
	case '[': return makeOpToken(TK_OBRAK); break;
	case ']': return makeOpToken(TK_CBRAK); break;
	case ':':
		c = input.getChar();
		if(c == '=') return makeOpToken(TK_ASSIGN);
		else
			throw std::exception();
		break;
	case '!':
	case '<':
	case '>':
	case '=':
		return readRelop(c);
		break;
	default:
		throw std::exception();
	}
}

void IBTLLexer::readWs()
{
	char c = input.getChar();
	while(std::isspace(c) && c != 0)
		c = input.getChar();
	input.putChar();
	input.clearLexeme();
}

Token *IBTLLexer::getToken()
{
	char c;
	Token *ret;

	// first consume any leading ws
	readWs();

	c = input.getChar();
	if(c == 0)
		ret = new Token(TK_EOF);
	else if(c == '\"')
		ret = readString();
	else if(std::isalpha(c) || c == '_')
		ret = readId();
	else if(std::isdigit(c))
		ret = readNumber();
	else
		ret = readOp(c);
	
	input.clearLexeme();
	return ret;
}

