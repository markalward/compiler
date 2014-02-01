
#include <locale>
#include <lexer/lexer.h>
#include <lexer/token.h>
#include <lexer/reader.h>

using namespace std;

bool isoct(char c) {return '0' <= c && c <= '7'; }
bool ishex(char c) {return std::isdigit(c) || 'a' <= c && c <= 'f' ||
		    'A' <= c && c <= 'F'; }

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

Token *IBTLLexer::makeNumToken(TokenName name, NumAttr attr)
{
	const char *lexeme = input.getLexeme();
	return new NumToken(name, attr, lexeme);
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
		if(c == EOF)
			throw LexException("missing end \"", input);
		else if(c == '\"')
			break;
	}

	return makeLiteralToken(TK_STR);
}


Token *IBTLLexer::readNumber(char c)
{
	enum {
		q0, q1, q2, q3, q4, q5, q6, q7, q8, q9, q10, q11, q12,
		q1_12, q1_8, q1_9, acceptDec, acceptOct, acceptHex,
		acceptReal, reject
	} state = q0;
	
	while(true) {
		switch(state) {
		case q0:
			if(c == '0') state = q1_8;
			else if(std::isdigit(c)) state = q1_12;
			else if(c == '.') state = q3;
			else state = reject;
			break;
		case q1:
			c = input.getChar();
			if(std::isdigit(c)) state = q1;
			else if(c == '.') state = q2;
			else state = reject;
			break;
		case q2:
			c = input.getChar();
			if(std::isdigit(c)) state = q2;
			else if(c == 'e' || c == 'E') state = q5;
			else state = acceptReal;
			break;
		case q3:
			c = input.getChar();
			if(std::isdigit(c)) state = q4;
			else state = reject;
			break;
		case q4:
			c = input.getChar();
			if(std::isdigit(c)) state = q4;
			else if(c == 'e' || c == 'E') state = q5;
			else state = acceptReal;
			break;
		case q5:
			c = input.getChar();
			if(std::isdigit(c)) state = q6;
			else if(c == '+' || c == '-') state = q7;
			else state = reject;
			break;
		case q6:
			c = input.getChar();
			if(std::isdigit(c)) state = q6;
			else state = acceptReal;
			break;
		case q7:
			c = input.getChar();
			if(std::isdigit(c)) state = q6;
			else state = reject;
			break;
		case q1_8:
			c = input.getChar();
			if(c == 'x' || c == 'X') state = q10;
			else if(isoct(c)) state = q1_9;
			else if(std::isdigit(c)) state = q1;
			else if(c == '.') state = q2;
			else state = acceptDec;
			break;
		case q1_9:
			c = input.getChar();
			if(isoct(c)) state = q1_9;
			else if(std::isdigit(c)) state = q1;
			else if(c == '.') state = q2;
			else state = acceptOct;
			break;
		case q1_12:
			c = input.getChar();
			if(std::isdigit(c)) state = q1_12;
			else if(c == '.') state = q2;
			else state = acceptDec;
			break;
		case q10:
			c = input.getChar();
			if(ishex(c)) state = q11;
			else state = reject;
			break;
		case q11:
			c = input.getChar();
			if(ishex(c)) state = q11;
			else state = acceptHex;
			break;
		case acceptDec:
			input.putChar();
			return makeNumToken(TK_INT, ATTR_DEC);
			break;
		case acceptOct:
			input.putChar();
			return makeNumToken(TK_INT, ATTR_OCT);
			break;
		case acceptHex:
			input.putChar();
			return makeNumToken(TK_INT, ATTR_HEX);
			break;
		case acceptReal:
			input.putChar();
			return makeNumToken(TK_REAL, ATTR_NONE);
			break;
		case reject:
			throw LexException("invalid numeric constant", input);
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
			throw LexException("unrecognized operator", input);
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
			throw LexException("unrecognized operator", input);
		break;
	case '!':
	case '<':
	case '>':
	case '=':
		return readRelop(c);
		break;
	default:
		throw LexException("unrecognized operator", input);
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
	else if(std::isdigit(c) || c == '.')
		ret = readNumber(c);
	else
		ret = readOp(c);
	
	input.clearLexeme();
	return ret;
}

