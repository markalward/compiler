
#ifndef TOKEN_H
#define TOKEN_H

#include <unordered_map> // c++11 only
#include <string>
#include <sstream>

class Token;

enum TokenName {
	TK_EOF = 0,

	TK_CONSTANT,	// strings, reals, ints, bools
	TK_ID,		// identifiers
	TK_BINOP,	// binary operators
	TK_UNOP,	// unary operators
	TK_MINUS,	// minus has to sit in a corner by itself because
			// it has ambiguous meaning during parsing
	TK_ASSIGN,

	TK_OBRAK,	// [
	TK_CBRAK,	// ]
	TK_TYPE,	// bool, string, int, real
	
	TK_IF,
	TK_WHILE,
	TK_LET,
	TK_PRINT
};

enum TokenAttr {
	AT_NONE = 0,

	// TK_CONSTANT attributes
	AT_STR,
	AT_INT_DEC,
	AT_INT_OCT,
	AT_INT_HEX,
	AT_REAL,	
	AT_T,
	AT_F,

	// TK_BINOP attributes
	AT_LT,
	AT_LE,
	AT_GT,
	AT_GE,
	AT_EQ,		
	AT_NE,
	AT_PLUS,
	AT_MULT,
	AT_DIV,
	AT_EXP,
	AT_MOD,
	AT_AND,
	AT_OR,
	
	// TK_UNOP attributes
	AT_SIN,
	AT_COS,
	AT_TAN,
	AT_NOT,

	// TK_TYPE attributes
	AT_KBOOL,
	AT_KSTR,
	AT_KINT,
	AT_KREAL
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


struct Token
{
	TokenName 	name;
	TokenAttr 	attr;
	SymbolEntry 	*ptr;

	Token(TokenName name, TokenAttr attr = AT_NONE, SymbolEntry *ptr = NULL) :
		name(name),
		attr(attr),
		ptr(ptr)
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


#endif
