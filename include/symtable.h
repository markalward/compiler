
#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <lexer/token.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <string>

struct SymbolData;
class SymbolTable;

typedef std::pair<const std::string, SymbolData *> SymbolEntry;

// the data associated with each identifier in the table
struct SymbolData
{
	TokenName name;
	TokenAttr attr;

	SymbolData(TokenName name, TokenAttr attr = AT_NONE) :
		name(name),
		attr(attr)
	{}

	SymbolData() :
		name(),
		attr()
	{}
};

typedef std::unordered_map<std::string, SymbolData> ScopeTable;

class SymbolTable
{
	std::vector<ScopeTable> table;

public:
	SymbolTable() :
		table()
	{
		// add outermost (global) scope to table
		table.push_back(ScopeTable());

		// add keywords to global scope
		ScopeTable &glob = table.front();

		glob["if"] = SymbolData(TK_IF);
		glob["while"] = SymbolData(TK_WHILE);
		glob["let"] = SymbolData(TK_LET);
		glob["stdout"] = SymbolData(TK_PRINT);

		glob["bool"] = SymbolData(TK_TYPE, AT_KBOOL);
		glob["int"] = SymbolData(TK_TYPE, AT_KINT);
		glob["real"] = SymbolData(TK_TYPE, AT_KREAL);
		glob["str"] = SymbolData(TK_TYPE, AT_KSTR);

		glob["true"] = SymbolData(TK_CONSTANT, AT_T);
		glob["false"] = SymbolData(TK_CONSTANT, AT_F);
		
		glob["and"] = SymbolData(TK_BINOP, AT_AND);
		glob["or"] = SymbolData(TK_BINOP, AT_OR);
		glob["not"] = SymbolData(TK_UNOP, AT_NOT);
		glob["sin"] = SymbolData(TK_UNOP, AT_SIN);
		glob["cos"] = SymbolData(TK_UNOP, AT_COS);
		glob["tan"] = SymbolData(TK_UNOP, AT_TAN);
	}

	
	inline ScopeTable &top() {return table.back(); }
	inline void push() {table.push_back(ScopeTable()); }
	inline void push(const ScopeTable &scope) {table.push_back(scope); }
	inline void pop() {table.pop_back(); }
	
	// returns the global scope, which is always at the bottom of the 
	// stack
	inline ScopeTable &globals() {return table[0]; }

	/*
		helper function for lexer to determine whether a lexeme is
		a keyword. If it is, 'name' and 'attr' describe the matching
		keyword.
	*/
	bool isKeyword(const char *lexeme, TokenName &name, TokenAttr &attr)
	{
		ScopeTable &glob = globals();
		auto iter = glob.find(lexeme);
		if(iter != glob.end()) {
			SymbolData &dat = iter->second;
			name = dat.name;
			attr = dat.attr;
			return true;
		}
		else
			return false;
	}
};

#endif
