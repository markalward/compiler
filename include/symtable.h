
#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <lexer/token.h>
#include <unordered_map>
#include <iostream>
#include <string>

struct SymbolEntry
{
	TokenName name;
	TokenAttr attr;

	SymbolEntry(TokenName name, TokenAttr attr = AT_NONE) :
		name(name),
		attr(attr)
	{}
};

class SymbolTable
{
	typedef std::unordered_map<std::string, SymbolEntry *> SymbolMap;
	SymbolMap table;

public:
	SymbolTable() :
		table()
	{
		// add keywords to table
		table["if"] = new SymbolEntry(TK_IF);
		table["while"] = new SymbolEntry(TK_WHILE);
		table["let"] = new SymbolEntry(TK_LET);
		table["stdout"] = new SymbolEntry(TK_PRINT);

		table["bool"] = new SymbolEntry(TK_TYPE, AT_KBOOL);
		table["int"] = new SymbolEntry(TK_TYPE, AT_KINT);
		table["real"] = new SymbolEntry(TK_TYPE, AT_KREAL);
		table["str"] = new SymbolEntry(TK_TYPE, AT_KSTR);

		table["true"] = new Token(TK_CONSTANT, AT_T);
		table["false"] = new Token(TK_CONSTANT, AT_F);
		
		table["and"] = new SymbolEntry(TK_BINOP, AT_AND);
		table["or"] = new SymbolEntry(TK_BINOP, AT_OR);
		table["not"] = new SymbolEntry(TK_UNOP, AT_NOT);
		table["sin"] = new SymbolEntry(TK_UNOP, TK_SIN);
		table["cos"] = new SymbolEntry(TK_UNOP, TK_COS);
		table["tan"] = new SymbolEntry(TK_UNOP, TK_TAN);
	}

	/*
		installs the lexeme 'val' into the symbol table. If a 
		token with matching lexeme is already in the table, a 
		pointer to this token is returned.
	*/
	SymbolEntry *install(const char *val) 
	{
		auto iter = table.find(val);
		if(iter == table.end()) {
			// add new entry
			SymbolEntry *tok = new SymbolEntry;
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
