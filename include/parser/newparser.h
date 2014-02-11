
#ifndef NEWPARSER_H
#define NEWPARSER_H

#include <parser/nodes.h> //TODO: change name of file

class IBTLParser
{
	IBTLLexer &lexer;
	tok cur;

	inline void require(TokenName expect)
	{
		if(cur != expect) throw ParseException(); // TODO: exception details
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
	inline bool is(TokenName name) {return cur == name; }
	
	
	// scope (S)

	ProgramNode *T() {
		return new ProgramNode(scopelist());
	}

	ScopeListNode *scopelist() {
		return scopelist(new ScopeListNode);
	}

	ScopeListNode *scopelist(ScopeListNode *list) {
		list->children.push_back(scope());
		return scopelist_p(list);
	}

	ScopeListNode *scopelist_p(ScopeListNode *list) {
		if(is(TK_CBRAK)) return list;
		return scopelist(list);
	}

	ScopeNode *scope() {
		if(is(TK_CONSTANT) || is(TK_ID))
			return expr();
		if(is(TK_OBRAK))
			return scope1();
		err("invalid scope");
	}

	ScopeNode *scope1() {
		discard(TK_OBRAK);
		return scope_p();
	}

	ScopeNode *scope_p() {
		if(is(TK_OBRAK))	return scope_p3();		
		if(is(TK_CBRAK))	return scope_p2();
		return scope_p1();
	}

	ContainerScopeNode *scope_p3() {
		ContainerScopeNode *t = new ContainerScopeNode(scopelist());
		discard(TK_CBRAK);
		return t;
	}

	ScopeNode *scope_p2() {
		discard(TK_CBRAK);
		return new ContainerScopeNode;
	}

	ScopeNode *scope_p1() {
		return exprsuffix();
	}

	// expr

	ExprNode *expr() {
		discard(TK_OBRAK);
		return exprsuffix();
	}

	ExprNode *exprsuffix() {
		if(is(TK_BINOP) || is(TK_UNOP) || is(TK_MINUS) || is(TK_ASSIGN))
			return opersuffix();
		if(is(TK_LET) || is(TK_PRINT) || is(TK_IF) || is(TK_WHILE))
			return stmtsuffix();
		err("invalid expr");
	}

	// oper

	OperNode *oper() {
		if(is(TK_OBRAK)) 	return oper1();
		if(is(TK_CONSTANT)) 	return constant();
		if(is(TK_ID)) 		return id(); 
	}

	OperNode *oper1() {
		discard(TK_OBRAK);
		return opersuffix();
	} 

	OperNode *opersuffix() {
		if(is(TK_BINOP))	return binop();
		if(is(TK_UNOP))		return unop();
		if(is(TK_MINUS))	return minus();
		if(is(TK_ASSIGN))	return assign();
		err("invalid oper");
	}

	BinopNode *binop() {
		BinopNode *t = new BinopNode(take(TK_BINOP), oper(), oper());
		discard(TK_CBRAK);
		return t;
	}

	UnopNode *unop() {
		UnopNode *t = new UnopNode(take(TK_UNOP), oper());
		discard(TK_CBRAK);
		return t;
	}

	OperNode *minus() {
		TokNode *op = take(TK_MINUS);
		OperNode *left = oper();
		OperNode *right = minus_p();
		if(right) return new BinopNode(op, left, right);
		else return new UnopNode(op, left);
	}

	OperNode *minus_p() {
		if(is(TK_CBRAK))	return minus_p2;
		return minus_p1();
	}

	OperNode *minus_p1() {
		OperNode *right = oper();
		discard(TK_CBRAK);
		return right;
	}

	OperNode *minus_p2() {
		discard(TK_CBRAK);
		return NULL;
	}

	TokNode *constant() {
		return take(TK_CONSTANT);
	}

	TokNode *id() {
		return take(TK_ID);
	}

	AssignNode *assign() {
		discard(TK_ASSIGN);
		AssignNode *t = new AssignNode(id(), oper());
		discard(TK_CBRAK);
		return t;
	}

	// stmts

	StmtNode *stmtsuffix() {
		if(is(TK_IF)) 		return ifstmts();
		if(is(TK_WHILE)) 	return whilestmts();
		if(is(TK_LET)) 		return letstmts();
		if(is(TK_PRINT)) 	return printstmts();
		err("invalid statement");	
	}
	
	IfNode *ifstmts() {
		discard(TK_IF);
		IfNode *t = new IfNode(expr(), expr(), ifstmts_p());
		discard(TK_CBRAK);
		return t;
	}

	ExprNode *ifstmts_p() {
		if(is(TK_OBRAK)) return expr();
		if(is(TK_CBRAK)) return NULL;
		err("invalid if statement");
	}

	WhileNode *whilestmts() {
		discard(TK_WHILE);
		WhileNode *t = new WhileNode(expr(), exprlist());
		discard(TK_CBRAK);
		return t;
	}

	LetNode *letstmts() {
		discard(TK_LET);
		discard(TK_OBRAK);
		LetNode *t = new LetNode(varlist());
		discard(TK_CBRAK);
		discard(TK_CBRAK);
		return t;
	}

	PrintNode *printstmts() {
		discard(TK_PRINT);
		PrintNode *t = new PrintNode(oper());
		discard(TK_CBRAK);
		return t;
	}

	// exprlist & varlist

	ExprListNode *exprlist() {
		return exprlist(new ExprListNode);
	}

	ExprListNode *exprlist(ExprListNode *list) {
		discard(TK_OBRAK);
		list->children.push_back(expr());
		return exprlist_p(list);
	}

	ExprListNode *exprlist_p(ExprListNode *list) {
		if(is(TK_OBRAK))	return exprlist(list);
		if(is(TK_CBRAK))	return list;
		err("invalid exprlist");
	}

	VarListNode *varlist(VarListNode *list) {
		discard(TK_OBRAK);
		list->children.push_back(take(TK_ID));
		list->children.push_back(take(TK_TYPE));
		discard(TK_CBRAK);
		return varlist_p(list);
	}

	VarListNode *varlist_p(VarListNode *list) {
		if(is(TK_OBRAK))	return varlist(list);
		if(is(TK_CBRAK))	return list;
		err("invalid varlist");
	}

};

#endif
