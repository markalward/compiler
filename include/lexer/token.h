
#ifndef TOKEN_H
#define TOKEN_H

#include <unordered_map> // c++11 only
#include <string>
#include <iostream>
#include <sstream>


struct Token;

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


struct Token
{
	TokenName 	name;
	TokenAttr 	attr;
	std::string 	val;

	// for tokens that store a lexeme value
	Token(const char *val, TokenName name, TokenAttr attr = AT_NONE) :
		name(name),
		attr(attr),
		val(val)
	{}

	// for tokens that do not store a lexeme value
	Token(TokenName name, TokenAttr attr = AT_NONE) :
		name(name),
		attr(attr),
		val()
	{}

	Token() :
		name(),
		attr(),
		val()
	{}

	/*
		the 'val' field of a token can consume a lot of space. The
		move ctor ensures that only the underlying pointer to the 
		string (and not the string itself) is copied when a token
		is moved

		Note: when a non-temporary Token is copied, we still need 
		a deep copy of the 'val' field
	*/
	Token(Token &&other) :
		name(other.name),
		attr(other.attr),
		val(std::move(other.val))
	{}

	Token &operator =(Token &&other)
	{
		name = other.name;
		attr = other.attr;
		val = std::move(other.val);
	}


	static std::string nameToString(TokenName name)
	{
		switch(name) {
		case TK_EOF: return "eof";
		case TK_CONSTANT: return "constant";
		case TK_ID: return "id";
		case TK_BINOP: return "binop";
		case TK_UNOP: return "unop";
		case TK_MINUS: return "minus";
		case TK_ASSIGN: return "assign";
		case TK_OBRAK: return "[";
		case TK_CBRAK: return "]";
		case TK_TYPE: return "type";
		case TK_IF: return "if";
		case TK_WHILE: return "while";
		case TK_LET: return "let";
		case TK_PRINT: return "print";
		default: return "";
		}
	}

	static std::string attrToString(TokenAttr attr)
	{
		switch(attr) {
		case AT_STR: return "str";
		case AT_INT_DEC: return "int-dec";
		case AT_INT_OCT: return "int-oct";
		case AT_INT_HEX: return "int-hex";
		case AT_REAL: return "real";
		case AT_T: return "true";
		case AT_F: return "false";
		case AT_LT: return "<";
		case AT_LE: return "<=";
		case AT_GT: return ">";
		case AT_GE: return ">=";
		case AT_EQ: return "=";
		case AT_NE: return "!=";
		case AT_PLUS: return "+";
		case AT_MULT: return "*";
		case AT_DIV: return "/";
		case AT_EXP: return "^";
		case AT_MOD: return "%";
		case AT_AND: return "and";
		case AT_OR: return "or";
		case AT_SIN: return "sin";
		case AT_COS: return "cos";
		case AT_TAN: return "tan";
		case AT_NOT: return "not";
		case AT_KBOOL: return "bool";
		case AT_KSTR: return "str";
		case AT_KINT: return "int";
		case AT_KREAL: return "real";
		default: return "";
		}
	}

};

std::ostream &operator <<(std::ostream &str, Token &tok);


#endif
