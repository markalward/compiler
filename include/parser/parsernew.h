
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

class ExprNode : public Node
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

// opers



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


