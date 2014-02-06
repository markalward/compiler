
#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <lexer/token.h>
#include <unordered_map>
#include <iostream>
#include <string>

class SymbolTable
{
	typedef std::unordered_map<std::string, Token *> SymbolMap;
	SymbolMap table;

public:
	SymbolTable() :
		table()
	{
		// add keywords to table
		table["if"] = new Token(TK_IF);
		table["while"] = new Token(TK_WHILE);

		table["let"] = new Token(TK_LET);
		table["stdout"] = new Token(TK_PRINT);

		table["bool"] = new Token(TK_KBOOL);
		table["int"] = new Token(TK_KINT);
		table["real"] = new Token(TK_KREAL);
		table["str"] = new Token(TK_KSTR);

		table["true"] = new Token(TK_T);
		table["false"] = new Token(TK_F);
		
		table["and"] = new Token(TK_AND);
		table["or"] = new Token(TK_OR);
		table["not"] = new Token(TK_NOT);
		table["sin"] = new Token(TK_SIN);
		table["cos"] = new Token(TK_COS);
		table["tan"] = new Token(TK_TAN);
	}

	/*
		installs the lexeme 'val' into the symbol table. If a 
		token with matching lexeme is already in the table, a 
		pointer to this token is returned.
	*/
	Token *install(const char *val) 
	{
		auto iter = table.find(val);
		if(iter == table.end()) {
			// add new token
			IdToken *tok = new IdToken;
			tok->val = val;
			table[tok->val] = tok;
			return tok;
		}
		else {
			// return existing token
			return iter->second;
		}
	}

	void display(std::ostream &str)
	{
		for(auto iter = table.begin(); iter != table.end(); iter++) {
			str << iter->second->toString() << endl;
		}
	}
};

#endif
