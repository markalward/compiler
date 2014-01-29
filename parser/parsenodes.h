
#ifndef PARSENODES_H
#define PARSENODES_H

#include "../lexer/token.h"
#include "tokenstream.h"

#include "basenodes.h"

class BinOp;
class UnOp;
class UnOrBinOp;
class VarList;
class ExprList;
class VarDec;

// oper

class AssignOp : public Oper
{
public:
	unique_ptr<Oper> left;
	unique_ptr<Oper> right;

	~AssignOp() {}
	
	static unique_ptr<AssignOp> parse(TokenStream &in);
	string toString();
};

class BinOp : public Oper
{
public:
	unique_ptr<Token> op;
	unique_ptr<Oper> first;
	unique_ptr<Oper> second;

	~BinOp() {}

	static unique_ptr<BinOp> parse(TokenStream &in);
	string toString();
};

class UnOp : public Oper
{
public:
	unique_ptr<Token> op;
	unique_ptr<Oper> first;

	~UnOp() {}
	
	static unique_ptr<UnOp> parse(TokenStream &in);
	string toString();
};

class UnOrBinOp : public Oper
{
public:
	unique_ptr<Token> op;
	unique_ptr<Oper> left;
	unique_ptr<Oper> right;

	static unique_ptr<Oper> parse(TokenStream &in);
};


// statements

class WhileStmt : public Statement
{
public:
	unique_ptr<Expr> cond;
	unique_ptr<ExprList> statements;

	static unique_ptr<WhileStmt> parse(TokenStream &in);
	string toString();
};

class IfStmt : public Statement
{
public:
	unique_ptr<Expr> cond;
	unique_ptr<Expr> thenexpr;
	unique_ptr<Expr> elseexpr;
	
	static unique_ptr<IfStmt> parse(TokenStream &in);
	string toString();
};

class PrintStmt : public Statement
{
public:
	unique_ptr<Oper> value;

	static unique_ptr<PrintStmt> parse(TokenStream &in);
	string toString();
};

class LetStmt : public Statement
{
public:
	unique_ptr<VarList> varlist;
	
	static unique_ptr<LetStmt> parse(TokenStream &in);
	string toString();
};

// exprlist

class ExprList : public ParseNode
{
public:
	vector< unique_ptr<Expr> > list;

	static unique_ptr<ExprList> parse(TokenStream &in);
	string toString();
};

// varlist

class VarList : public ParseNode
{
public:
	vector< unique_ptr<VarDec> > list;

	static unique_ptr<VarList> parse(TokenStream &in);
};

class VarDec : public ParseNode
{
public:
	unique_ptr<Token> name;
	unique_ptr<Token> type;

	static unique_ptr<VarDec> parse(TokenStream &in);
};



#endif
