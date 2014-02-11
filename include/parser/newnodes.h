
#include <vector>
#include <string>
#include <initializer_list>

static std::ostream &operator <<(std::ostream &str, const Node &node);

class Node
{
	std::vector<Node *> children;
	
protected:
	Node(std::initializer_list<Node *> children) :
		children(children)
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

static std::ostream &operator <<(std::ostream &str, const Node &node)
{
	str << "[" << node.name();
	for(auto iter = node.children.cbegin(); iter != children.cend();
	    iter++)
		str << " " << **iter;
	return (str << "]");
}

class ProgramNode : public Node
{
public:
	ProgramNode(ScopeNode *scope) :
		Node({scope})
	{}

	std::string name() {return std::string("program"); }
};

class ScopeNode : public Node
{
public:
	ScopeNode() :
		Node({})
	{}
};

class ScopeListNode : public Node
{
public:
	ScopeListNode()
		Node({})
	{}

};

class ContainerScopeNode : public ScopeNode
{
public:
	ContainerScopeNode(ScopeList *list) :
		ScopeNode({list})
	{}

	ContainerScopeNode() :
		ScopeNode({new ScopeList})
	{}
};



class ExprNode : public ScopeNode
{
protected:
	ExprNode(std::initializer_list<Node *children) :
		Node(children)
	{}
public:
	virtual ~ExprNode() {}
};

class StmtNode : public ExprNode
{
protected:
	StmtNode(std::initializer_list<Node *> children) :
		ExprNode(children)
	{}
public:
	virtual ~StmtNode() {}
};

class OperNode : public ExprNode
{
protected:
	OperNode(std::initializer_list<Node *> children) :
		ExprNode(children)
	{}
public:
	virtual ~OperNode() {}
};

class TokNode : public Node
{
private:
	tok token;

public:
	TokNode(tok token) :
		Node({}),
		token(token)
	{}
};

// opers

class BinopNode : public OperNode
{
public:
	BinopNode(TokNode *op, OperNode *l, OperNode *r) :
		OperNode({op, l, r})
	{}

	std::string name() {return std::string("binop"); }
};

class UnopNode : public OperNode
{
public:
	UnopNode(TokNode *op, OperNode *l) :
		OperNode({op, l})
	{}

	std::string name() {return std::string("unop"); }
};

class AssignNode : public OperNode
{
public:
	AssignNode(TokNode *name, OperNode *oper) :
		OperNode({name, oper})
	{}
	
	std::string name() {return std::string("assign"); }
};

// statements

class IfNode : public StmtNode
{
public:
	IfNode(ExprNode *condExpr, StmtNode *thenStmt, StmtNode *elseStmt) :
		StmtNode({condExpr, thenStmt, elseStmt})
	{}

	std::string name() {return std::string("if"); }	
};

class WhileNode : public StmtNode
{
public:
	WhileNode(ExprNode *condExpr, ExprListNode *bodyList) :
		StmtNode({condExpr, bodyList})
	{}

	std::string name() {return std::string("while"); }
};

class LetNode : public StmtNode
{
public:
	LetNode(VarlistNode *varlist) :
		StmtNode({varList})
	{}

	std::string name() {return std::string("let"); }
};

class PrintNode : public StmtNode
{
public:
	PrintNode(OperNode *oper) :
		StmtNode({oper})
	{}

	std::string name() {return std::string("print"); }
};

class ExprListNode : public Node
{
public:
	ExprListNode(ExprNode *expr, ExprListNode *next) :
		Node({expr, next})
	{}

	std::string name() {return std::string("exprlist"); }
};

class VarListNode : public Node
{
	VarListNode() :
		Node({})
	{}

	std::string name() {return std::string("varlist"); }
};


