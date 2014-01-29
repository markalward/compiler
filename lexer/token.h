
#ifndef TOKEN_H
#define TOKEN_H

#include <unordered_map> // c++11 only
#include <string>
#include "../parser/basenodes.h"

enum TokenName {
	TK_EOF=0,
	TK_STR=2,
	TK_INT=3,
	TK_REAL=4,
	TK_LT=5,
	TK_LE=6,
	TK_GT=7,
	TK_GE=8,
	TK_EQ=9,
	TK_NE=10,
	TK_ID=11,
	TK_PLUS=12,
	TK_MINUS=13,
	TK_MULT=14,
	TK_DIV=15,
	TK_EXP=16,
	TK_MOD=17,
	TK_OBRAK=18,
	TK_CBRAK=19,
	TK_IF=20,
	TK_WHILE=21,
	TK_LET,
	TK_PRINT,
	TK_KBOOL,
	TK_T,
	TK_F,
	TK_KSTR,
	TK_KINT,
	TK_KREAL,
	TK_ASSIGN
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
		table["bool"] = TK_KBOOL;
		table["true"] = TK_T;
		table["false"] = TK_F;
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
		case TK_INT: return "int";
		case TK_STR: return "str";
		case TK_REAL: return "real";
		case TK_LT: return "<";
		case TK_LE: return "<=";
		case TK_GT: return ">";
		case TK_GE: return ">=";
		case TK_EQ: return "=";
		case TK_NE: return "!=";
		case TK_ID: return "id";
		case TK_PLUS: return "+";
		case TK_MINUS: return "-";
		case TK_MULT: return "*";
		case TK_DIV: return "/";
		case TK_EXP: return "^";
		case TK_MOD: return "%";
		case TK_OBRAK: return "[";
		case TK_CBRAK: return "]";
		case TK_IF: return "if";
		case TK_WHILE: return "while";
		case TK_KBOOL: return "bool";
		case TK_T: return "true";
		case TK_F: return "false";
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




#endif
