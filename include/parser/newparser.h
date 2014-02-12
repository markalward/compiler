
#ifndef NEWPARSER_H
#define NEWPARSER_H

#include <lexer/lexer.h>
#include <parser/newnodes.h> //TODO: change name of file
#include <exception>
#include <string>


class IBTLParser
{
	IBTLLexer &lexer;
	tok cur;

	// forward declarations
	ProgramNode *T();
	ContainerScopeNode *scopelist();
	ContainerScopeNode *scopelist(ContainerScopeNode *sc);
	ContainerScopeNode *scopelist_p(ContainerScopeNode *sc);
	ScopeNode *scope();
	ScopeNode *scope1();
	ScopeNode *scope_p();
	ScopeNode *scope_p1();
	ScopeNode *scope_p2();
	ContainerScopeNode *scope_p3();
	ExprNode *expr();
	ExprNode *exprsuffix();
	OperNode *oper();
	OperNode *oper1();
	OperNode *opersuffix();
	BinopNode *binop();
	UnopNode *unop();
	OperNode *minus();
	OperNode *minus_p();
	OperNode *minus_p1();
	OperNode *minus_p2();
	TokNode *constant();
	TokNode *id();
	AssignNode *assign();
	StmtNode *stmtsuffix();
	IfNode *ifstmts();
	ExprNode *ifstmts_p();
	WhileNode *whilestmts();
	LetNode *letstmts();
	PrintNode *printstmts();
	ExprListNode *exprlist();
	ExprListNode *exprlist(ExprListNode *list);
	ExprListNode *exprlist_p(ExprListNode *list);
	VarListNode *varlist();
	VarListNode *varlist(VarListNode *list);
	VarListNode *varlist_p(VarListNode *list);
	

	inline void require(TokenName expect)
	{
		if(cur.name != expect) throw std::exception(); // TODO: exception details
	}

	/*
		take current token and advance to next.
		throw exception if token does not match 'expect'.
	*/
	TokNode *take(TokenName expect)
	{
		require(expect);
		TokNode *t = new TokNode(cur);
		cur = lexer.getToken();
		return t;
	}

	/*
		discard current token and advance to next.
		throw exception if token does not match 'expect'.
	*/
	void discard(TokenName expect)
	{
		require(expect);
		cur = lexer.getToken();
	}

	/*
		return true if current token matches 'name'.
	*/
	inline bool is(TokenName name) {return cur.name == name; }
	
	void err(const char *msg)
	{
		throw std::exception();
	}

public:
	IBTLParser(IBTLLexer &lexer) :
		lexer(lexer),
		cur()
	{}

	ProgramNode *parse() {
		cur = lexer.getToken();
		return T();
	}
};

#endif

