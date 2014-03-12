
#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <lexer/token.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <string>

enum Type {
    TP_INT,
    TP_REAL,
    TP_BOOL,
    TP_STR,
    TP_NONE
};

struct SymbolData;
class SymbolTable;

// the data associated with each identifier in the table
struct SymbolData
{
	TokenName name;
	TokenAttr attr;
    
    // the name of the identifier in the generated gforth code
    std::string outputName;
    
    // the type of the identifier  
    // TODO: make defn of Type visible  
    Type type;

    SymbolData(TokenName name, TokenAttr attr, const std::string &outputName, 
               Type type) :
        name(name),
        attr(attr),
        outputName(outputName),
        type(type)
    {}

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

    // returns the global scope, which is always at the bottom of the 
	// stack
	inline ScopeTable &globals() {return table[0]; }

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

	inline void enterScope() {table.push_back(ScopeTable()); }
	inline void exitScope() {table.pop_back(); }
    inline int scopeDepth() {return table.size(); }

    /* 
       declares the identifier 'id' in the current scope with type 
       'type'. returns true on success, false if the variable is already 
       defined in current scope
    */
    bool declare(const std::string &id, const std::string &outputName,
                 Type type)
    {
        ScopeTable &local = table.back();
        if(local.find(id) == local.end()) {
            local[id] = SymbolData(TK_ID, AT_NONE, outputName, type); 
            return true;
        }
        else
            return false;
    }

    /*
        finds the declaration of 'id' by searching scopes inner to outer.
        If id is declared, returns true and 'dat' contains the associated 
        SymbolData. Otherwise returns false.
    */
    bool find(const std::string &id, SymbolData &dat)
    {
        for(auto riter = table.rbegin(); riter != table.rend(); riter++) {
            ScopeTable &cur = *riter;
            auto entry = cur.find(id);
            if(entry != cur.end()) {
                dat = entry->second;
                return true;
            }
        }

        return false;
    }
	
	

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
