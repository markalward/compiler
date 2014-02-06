
#ifndef TOKEN_H
#define TOKEN_H

#include <unordered_map> // c++11 only
#include <string>
#include <sstream>
#include <parser/basenodes.h>

class Token;

enum TokenClass
{
	TK_BINOP = 0x0100,
	TK_UNOP = 0x0200,
	TK_CONST = 0x0400,
	TK_STATEMENT = 0x0800,
	TK_TYPENAME = 0x1000
};	

enum TokenName {
	TK_EOF=0,

	TK_STR = 	TK_CONST | 1,
	TK_INT =	TK_CONST | 2,
	TK_REAL =	TK_CONST | 3,	
	TK_T =		TK_CONST | 4,
	TK_F =		TK_CONST | 5,

	TK_LT =		TK_BINOP | 10, 
	TK_LE =		TK_BINOP | 11,
	TK_GT = 	TK_BINOP | 12,
	TK_GE =		TK_BINOP | 13,
	TK_EQ =		TK_BINOP | 14,		
	TK_NE =		TK_BINOP | 15,
	TK_PLUS =	TK_BINOP | 16,
	TK_MINUS =	TK_BINOP | TK_UNOP | 17,
	TK_MULT =	TK_BINOP | 18,
	TK_DIV =	TK_BINOP | 19,
	TK_EXP =	TK_BINOP | 20,
	TK_MOD =	TK_BINOP | 21,
	TK_AND =	TK_BINOP | 22,
	TK_OR =		TK_BINOP | 23,
	
	TK_SIN =	TK_UNOP | 60,
	TK_COS =	TK_UNOP | 61,
	TK_TAN =	TK_UNOP | 62,
	TK_NOT =	TK_UNOP | 63,

	TK_OBRAK =	30,
	TK_CBRAK =	31,
	TK_ASSIGN =	32,
	TK_ID =		33,

	TK_IF =		TK_STATEMENT | 40,
	TK_WHILE =	TK_STATEMENT | 41,
	TK_LET = 	TK_STATEMENT | 42,
	TK_PRINT =	TK_STATEMENT | 43,
	
	TK_KBOOL =	TK_TYPENAME | 50,
	TK_KSTR =	TK_TYPENAME | 51,
	TK_KINT =	TK_TYPENAME | 52,
	TK_KREAL =	TK_TYPENAME | 53
};


class KeywordTable
{
	typedef std::unordered_map<std::string, TokenName> KeywordMap;
	KeywordMap table;

public:

	KeywordTable()
	{
		table["if"] = TK_IF;
		table["while"] = TK_WHILE;

		table["let"] = TK_LET;
		table["stdout"] = TK_PRINT;

		table["bool"] = TK_KBOOL;
		table["int"] = TK_KINT;
		table["real"] = TK_KREAL;
		table["str"] = TK_KSTR;

		table["true"] = TK_T;
		table["false"] = TK_F;
		
		table["and"] = TK_AND;
		table["or"] = TK_OR;
		table["not"] = TK_NOT;
		table["sin"] = TK_SIN;
		table["cos"] = TK_COS;
		table["tan"] = TK_TAN;
	}

	bool isKeyword(const char *val, TokenName &keyword)
	{
		KeywordMap::iterator iter = table.find(val);
		if(iter == table.end()) return false;		
		keyword = iter->second;
		return true;
	}

	
};


struct Token : public Oper
{
	TokenName name;
	
	Token(TokenName name) :
		name(name)
	{}

	static std::string nameToString(TokenName name)
	{
		switch(name) {
		case TK_EOF: return "eof";
		case TK_STR: return "str";
		case TK_INT: return "int";
		case TK_REAL: return "real";
		case TK_T: return "true";
		case TK_F: return "false";
		
		case TK_LT: return "<";
		case TK_LE: return "<=";
		case TK_GT: return ">";
		case TK_GE: return ">=";
		case TK_EQ: return "=";
		case TK_NE: return "!=";
		case TK_PLUS: return "+";
		case TK_MINUS: return "-";
		case TK_MULT: return "*";
		case TK_DIV: return "/";
		case TK_EXP: return "^";
		case TK_MOD: return "%";
		case TK_AND: return "and";
		case TK_OR: return "or";

		case TK_SIN: return "sin";
		case TK_COS: return "cos";
		case TK_TAN: return "tan";
		case TK_NOT: return "not";

		case TK_ID: return "id";
		case TK_OBRAK: return "[";
		case TK_CBRAK: return "]";
		case TK_ASSIGN: return "assign";

		case TK_IF: return "if";
		case TK_WHILE: return "while";
		case TK_LET: return "let";
		case TK_PRINT: return "print";

		case TK_KBOOL: return "type-bool";
		case TK_KSTR: return "type-str";
		case TK_KINT: return "type-int";
		case TK_KREAL: return "type-real";

		default: return "";
		}
	}

	virtual std::string toString()
	{
		return std::string("<") + nameToString(name) +
			std::string(">");	
	}

};


/*
	Represents an identifier (not including keywords)
*/
struct IdToken : Token
{
	std::string val;

	IdToken() :
		Token(TK_ID)
	{}

	std::string toString()
	{
		return std::string("<id, ") + val + std::string(">"); 
	}

};

/*
	Represents an int, real, or string. The token itself stores
	the value as a string.
*/
struct LiteralToken : Token
{
	std::string val;

	LiteralToken(TokenName name, const char *val) :
		Token(name),
		val(val)
	{}

	std::string toString()
	{
		return std::string("<") + nameToString(name) +
			std::string(", ") + val + std::string(">");
	}
};

enum NumAttr {
	ATTR_NONE=0,
	ATTR_DEC,
	ATTR_OCT,
	ATTR_HEX
};

struct NumToken : public LiteralToken
{
	NumAttr attr;
	
	NumToken(TokenName name, NumAttr attr, const char *val) :
		LiteralToken(name, val),
		attr(attr)
	{}

	std::string toString()
	{
		ostringstream str;
		str << "<" << nameToString(name) << ", ";
		if(attr == ATTR_DEC) str << "dec";
		else if(attr == ATTR_OCT) str << "oct";
		else if(attr == ATTR_HEX) str << "hex";
		else str << "-";
		str << ", " << val << ">";
		return str.str();
	}
};



#endif
