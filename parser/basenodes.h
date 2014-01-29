
#ifndef BASENODES_H
#define BASENODES_H

class TokenStream;

using namespace std;

class ParseNode
{
public:
	virtual ~ParseNode() {}

	virtual string toString() {return ""; }
};

class Scope : public ParseNode
{
public:
	vector<	unique_ptr<ParseNode> > children;

	virtual ~Scope() {}

	static unique_ptr<Scope> parse(TokenStream &in);
	virtual string toString();
};

class Expr : public ParseNode
{
public:
	virtual ~Expr() {}

	static unique_ptr<Expr> parse(TokenStream &in);
};

class Oper : public Expr
{
public:
	virtual ~Oper() {}

	static unique_ptr<Oper> parse(TokenStream &in);
};

class Statement : public Expr
{
public:
	virtual ~Statement() {}

	static unique_ptr<Statement> parse(TokenStream &in);
};



#endif
