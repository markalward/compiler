
#ifndef NODES_H
#define NODES_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include <lexer/token.h>

typedef Token tok;
class Node;
class ProgramNode;
class ScopeNode;
class ScopeListNode;
class ContainerScopeNode;
class ExprNode;
class OperNode;
class StmtNode;
class TokNode;
class BinopNode;
class UnopNode;
class AssignNode;
class IfNode;
class WhileNode;
class LetNode;
class PrintNode;
class ExprListNode;
class VarListNode;

std::ostream &operator <<(std::ostream &str, Node &node);
std::ostream &operator <<(std::ostream &str, TokNode &tok);

class Node
{
	friend std::ostream &operator<<(std::ostream &, const Node &);

public:
	typedef std::vector<Node *> NodeArray;
	NodeArray children;
	bool isToken;

protected:
	

	Node(NodeArray children) :
		children(children),
		isToken(false)
	{}

	Node(bool isToken = false) :
		children(),
		isToken(isToken)
	{}

public:
	virtual ~Node()
	{
		for(auto iter = children.cbegin(); iter != children.cend(); 
		    iter++)
			delete *iter;
			
	}

	virtual std::string name()
	{
		return std::string();
	}

	
};

class ScopeNode : public Node
{
public:
	ScopeNode() :
		Node()
	{}

	virtual std::string name() {return std::string("scope"); }
};

class ContainerScopeNode : public ScopeNode
{
public:
	ContainerScopeNode() :
		ScopeNode()
	{}

};


class ProgramNode : public Node
{
public:
	ProgramNode(ContainerScopeNode *sc) :
		Node()
	{
		children.push_back(sc);
	}

	std::string name() {return std::string("program"); }
};


class ExprNode : public ScopeNode
{
protected:
	ExprNode() :
		ScopeNode()
	{}
public:
	virtual ~ExprNode() {}
};

class StmtNode : public ExprNode
{
protected:
	StmtNode() :
		ExprNode()
	{}
public:
	virtual ~StmtNode() {}
};

class OperNode : public ExprNode
{
protected:
	OperNode() :
		ExprNode()
	{}
public:
	virtual ~OperNode() {}
};

class TokNode : public OperNode
{
	friend std::ostream &operator <<(std::ostream &str, TokNode &tok);
	tok token;

public:
	TokNode(tok token) :
		OperNode(),
		token(token)
	{
		isToken = true;
	}

	std::string name() {
		std::ostringstream str;
		str << token;
		return str.str();
	}

};

class ExprListNode : public Node
{
public:
	ExprListNode() :
		Node()
	{}

	std::string name() {return std::string("exprlist"); }
};

class VarListNode : public Node
{
public:
	VarListNode() :
		Node()
	{}

	std::string name() {return std::string("varlist"); }
};



// opers

class BinopNode : public OperNode
{
public:
	BinopNode(TokNode *op, OperNode *l, OperNode *r) :
		OperNode()
	{
		children.push_back(op);
		children.push_back(l);
		children.push_back(r);
	}

	std::string name() {return std::string("binop"); }
};

class UnopNode : public OperNode
{
public:
	UnopNode(TokNode *op, OperNode *l) :
		OperNode()
	{
		children.push_back(op);
		children.push_back(l);
	}

	std::string name() {return std::string("unop"); }
};

class AssignNode : public OperNode
{
public:
	AssignNode(TokNode *name, OperNode *oper) :
		OperNode()
	{
		children.push_back(name);
		children.push_back(oper);
	}
	
	std::string name() {return std::string("assign"); }
};

// statements

class IfNode : public StmtNode
{
public:
	IfNode(ExprNode *condExpr, ExprNode *thenStmt, ExprNode *elseStmt = NULL) :
		StmtNode()
	{
		children.push_back(condExpr);
		children.push_back(thenStmt);
		if(elseStmt) children.push_back(elseStmt);
	}

	std::string name() {return std::string("if"); }	
};

class WhileNode : public StmtNode
{
public:
	WhileNode(ExprNode *condExpr, ExprListNode *bodyList) :
		StmtNode()
	{
		children.push_back(condExpr);
		children.push_back(bodyList);
	}

	std::string name() {return std::string("while"); }
};

class LetNode : public StmtNode
{
public:
	LetNode(VarListNode *varlist) :
		StmtNode()
	{
		children.push_back(varlist);
	}

	std::string name() {return std::string("let"); }
};

class PrintNode : public StmtNode
{
public:
	PrintNode(OperNode *oper) :
		StmtNode()
	{
		children.push_back(oper);
	}

	std::string name() {return std::string("print"); }
};

#endif

