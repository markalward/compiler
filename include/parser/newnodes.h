
#ifndef NODES_H
#define NODES_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include <lexer/token.h>
#include <generator/generator.h>
#include <symtable.h>

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

std::string typeString(Type tp);

Type tokenToType(TokenName name, TokenAttr attr);

typedef std::ostream    Stream;

class Node
{
	friend std::ostream &operator<<(std::ostream &, const Node &);

public:
	typedef std::vector<Node *> NodeArray;
	NodeArray children;
	bool isToken;
    int mline;

protected:
	
	Node(NodeArray children) :
		children(children),
		isToken(false)
	{}

	Node(int line, bool isToken = false) :
		children(),
		isToken(isToken),
        mline(line)
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

    void error(const std::string &msg)
    {
        std::ostringstream str;
        str << "line " << line() << ": " << msg;
        throw GenException(str.str());
    }

    void typeError(const std::string &msg) {error(msg); }
    

    int line() {return mline; }
    
    /*
        generates gforth code for this node. The code is written to
        the output stream 'str' and indented by 'indent' tabs. The
        default implementation simply calls generate() on all child
        nodes and returns TP_NONE.
    */
    virtual Type generate(Stream &str, SymbolTable &sym, int indent);
	
};

class ScopeNode : public Node
{
public:
	ScopeNode(int line) :
		Node(line)
	{}

	virtual std::string name() {return std::string("scope"); }
};

class ContainerScopeNode : public ScopeNode
{
public:
	ContainerScopeNode(int line) :
		ScopeNode(line)
	{}

    Type generate(Stream &str, SymbolTable &sym, int indent);

};


class ProgramNode : public Node
{
public:
	ProgramNode(ContainerScopeNode *sc, int line) :
		Node(line)
	{
		children.push_back(sc);
	}

    inline ContainerScopeNode *scope()
    {
        return dynamic_cast<ContainerScopeNode *>(children[0]);
    }

	std::string name() {return std::string("program"); }

    Type generate(Stream &str, SymbolTable &sym, int indent);
};


class ExprNode : public ScopeNode
{
protected:
	ExprNode(int line) :
		ScopeNode(line)
	{}
public:

	virtual ~ExprNode() {}
};

class StmtNode : public ExprNode
{
protected:
	StmtNode(int line) :
		ExprNode(line)
	{}
public:
	virtual ~StmtNode() {}
};

class OperNode : public ExprNode
{
protected:
	OperNode(int line) :
		ExprNode(line)
	{}
public:
	virtual ~OperNode() {}

};

class TokNode : public OperNode
{
	friend std::ostream &operator <<(std::ostream &str, TokNode &tok);
	tok token;

    void genBool(Stream &str);
    void genReal(Stream &str);
    Type genVariable(Stream &str, SymbolTable &sym);

public:
	TokNode(tok token, int line) :
		OperNode(line),
		token(token)
	{
		isToken = true;
	}

    inline TokenAttr attr() 
    {
        // temporary
        if(token.name == TK_MINUS) return AT_MINUS;
        return token.attr; 
    }

    inline TokenName type() {return token.name; }

    inline std::string &val() {return token.val; }

	std::string name() {
		std::ostringstream str;
		str << token;
		return str.str();
	}

    Type generate(Stream &str, SymbolTable &sym, int indent);

};

class ExprListNode : public Node
{
public:
	ExprListNode(int line) :
		Node(line)
	{}

	std::string name() {return std::string("exprlist"); }
};

class VarListNode : public Node
{
public:
	VarListNode(int line) :
		Node(line)
	{}

    int varCount() {return children.size() / 2; }
    
    std::pair<std::string, Type> item(int i)
    {
        i = i * 2;
        TokNode *id = dynamic_cast<TokNode *>(children[i]);
        TokNode *type = dynamic_cast<TokNode *>(children[i+1]);
        return std::pair<std::string, Type>(
            id->val(),
            tokenToType(type->type(), type->attr())
            );
    }

	std::string name() {return std::string("varlist"); }
};

class IdListNode : public Node
{
public:
	IdListNode(int line) :
		Node(line)
	{}

    int count() {return children.size(); }
    std::string item(int i) {
        return (dynamic_cast<TokNode *>(children[i]))->val();
    }

	std::string name() {return std::string("idlist"); }
};

class TypeListNode : public Node
{
public:
	TypeListNode(int line) :
		Node(line)
	{}

    int count() {return children.size(); }
    Type item(int i) {
        TokNode *t = dynamic_cast<TokNode *>(children[i]);
        return tokenToType(t->type(), t->attr());
    }

	std::string name() {return std::string("typelist"); }
};



// opers

class BinopNode : public OperNode
{
    void genReal(Type l, Type r, Stream &str);
    void genInt(Stream &str);
    void genBool(Type l, Type r, Stream &str);
    void genStr(Stream &str);
    Type typeCheckBoolOp(Type l, Type r);
    Type typeCheckCompareOp(Type l, Type r);
    Type typeCheckNumOp(Type l, Type r);
    Type typeCheckExpOp(Type l, Type r);
    Type typeCheck(Type l, Type r);

public:
	BinopNode(TokNode *op, OperNode *l, OperNode *r, int line) :
		OperNode(line)
	{
		children.push_back(op);
		children.push_back(l);
		children.push_back(r);
	}

    inline TokNode *op()        {return dynamic_cast<TokNode *>(children[0]); }
    inline OperNode *left()     {return dynamic_cast<OperNode *>(children[1]); }
    inline OperNode *right()    {return dynamic_cast<OperNode *>(children[2]); }

    inline void binopError(const std::string &msg, Type l, Type r)
    {
        std::ostringstream str;
        str << "line " << line() << ": " << msg << " (have " <<
            typeString(l) << ", " << typeString(r) << ")";
        throw GenException(str.str());
    }

    std::string opString() {return Token::attrToString(op()->attr()); }

    Type generate(Stream &str, SymbolTable &sym, int indent);

	std::string name() {return std::string("binop"); }
};


class UnopNode : public OperNode
{
    void genReal(Type l, Stream &str);
    void genInt(Stream &str);
    void genBool(Stream &str);
    Type typeCheck(Type l);

public:
	UnopNode(TokNode *op, OperNode *l, int line) :
		OperNode(line)
	{
		children.push_back(op);
		children.push_back(l);
	}
    
    inline TokNode *op()        {return dynamic_cast<TokNode *>(children[0]); }
    inline OperNode *left()     {return dynamic_cast<OperNode *>(children[1]); }

    inline void unopError(const std::string &msg, Type l)
    {
        std::ostringstream str;
        str << "line " << line() << ": " << msg << " (have " <<
            typeString(l) << ")";
        throw GenException(str.str());
    }

    std::string opString() {return Token::attrToString(op()->attr()); }

    Type generate(Stream &str, SymbolTable &sym, int indent);

	std::string name() {return std::string("unop"); }
};

class AssignNode : public OperNode
{

    Type typeCheck(Type ltype, Type rtype);

public:
	AssignNode(TokNode *id, OperNode *oper, int line) :
		OperNode(line)
	{
		children.push_back(id);
		children.push_back(oper);
	}
	
    inline TokNode *id() {return dynamic_cast<TokNode *>(children[0]); }
    inline OperNode *oper() {return dynamic_cast<OperNode *>(children[1]); }

    Type generate(Stream &str, SymbolTable &sym, int indent);

	std::string name() {return std::string("assign"); }
};

class CallNode : public OperNode
{

    void typeCheck(SymbolData &dat, int paramIndex, Type paramType);

public:
	CallNode(int line) :
		OperNode(line)
	{}
	
    inline TokNode *funcId() {return dynamic_cast<TokNode *>(children[0]); }
    int paramCount() {return children.size()-1; }
    inline OperNode *param(int i) {return dynamic_cast<OperNode *>(children[i+1]); }

    Type generate(Stream &str, SymbolTable &sym, int indent);

	std::string name() {return std::string("call"); }
};




// statements

class IfNode : public StmtNode
{
public:
	IfNode(int line, ExprNode *condExpr, ExprNode *thenStmt, ExprNode *elseStmt = NULL) :
		StmtNode(line)
	{
		children.push_back(condExpr);
		children.push_back(thenStmt);
		if(elseStmt) children.push_back(elseStmt);
	}

    inline ExprNode *condExpr() {return dynamic_cast<ExprNode *>(children[0]); }
    inline ExprNode *thenExpr() {return dynamic_cast<ExprNode *>(children[1]); }
    inline ExprNode *elseExpr() {
        if(children.size() == 3)
            return dynamic_cast<ExprNode *>(children[2]);
        else
            return NULL;
     }

    Type generate(Stream &str, SymbolTable &sym, int indent);

	std::string name() {return std::string("if"); }	
};

class WhileNode : public StmtNode
{
public:
	WhileNode(ExprNode *condExpr, ExprListNode *bodyList, int line) :
		StmtNode(line)
	{
		children.push_back(condExpr);
		children.push_back(bodyList);
	}

    inline ExprNode *condExpr() {return dynamic_cast<ExprNode *>(children[0]); }
    inline ExprListNode *bodyList() {return dynamic_cast<ExprListNode *>(children[1]); }

    Type generate(Stream &str, SymbolTable &sym, int indent);

	std::string name() {return std::string("while"); }
};

class LetNode : public StmtNode
{
    void genVar(Stream &str, const std::string &varname, Type type);

public:
	LetNode(VarListNode *varlist, int line) :
		StmtNode(line)
	{
		children.push_back(varlist);
	}

    inline VarListNode *varlist() {return dynamic_cast<VarListNode *>(children[0]); }

    Type generate(Stream &str, SymbolTable &sym, int indent);

	std::string name() {return std::string("let"); }
};

class FunctionNode : public StmtNode
{

public:
    FunctionNode(int line) :
        StmtNode(line)
    {}

    inline IdListNode *idlist() {return dynamic_cast<IdListNode *>(children[0]); }
    inline TypeListNode *typelist() {return dynamic_cast<TypeListNode *>(children[1]); }
    inline ContainerScopeNode *body() {return dynamic_cast<ContainerScopeNode *>(children[2]); }
    
    Type generate(Stream &str, SymbolTable &sym, int indent);

    std::string name() {return std::string("function"); }
};

class PrintNode : public StmtNode
{
public:
	PrintNode(OperNode *oper, int line) :
		StmtNode(line)
	{
		children.push_back(oper);
	}

    inline OperNode *oper() {return dynamic_cast<OperNode *>(children[0]); }

    Type generate(Stream &str, SymbolTable &sym, int indent);

	std::string name() {return std::string("print"); }
};

#endif

