
#include <parser/newparser.h>

void toStream(std::ostream &str, Node &node, int indent)
{
	std::string ind(indent, '\t');
	if(node.isToken)
		str << ind << node.name() << std::endl;
	else {
		str << ind << "[" << node.name() << std::endl;
		for(auto iter = node.children.cbegin(); iter != node.children.cend();
		    iter++)
			toStream(str, **iter, indent+1);
		str << ind << "]" << std::endl;
	}
}

std::ostream &operator <<(std::ostream &str, Node &node)
{
	toStream(str, node, 0);
	return str;
}

std::ostream &operator <<(std::ostream &str, TokNode &tok)
{
	return (str << tok.token);
}


// scope (S)

ProgramNode *IBTLParser::T() {
	discard(TK_OBRAK);
	ProgramNode *t = new ProgramNode(scopelist());
	discard(TK_CBRAK);
	return t;
}

ContainerScopeNode *IBTLParser::scopelist() {
	return scopelist(new ContainerScopeNode);
}

ContainerScopeNode *IBTLParser::scopelist(ContainerScopeNode *sc) {
	sc->children.push_back(scope());
	return scopelist_p(sc);
}

ContainerScopeNode *IBTLParser::scopelist_p(ContainerScopeNode *sc) {
	if(is(TK_CBRAK)) return sc;
	if(is(TK_OBRAK) || is(TK_ID) || is(TK_CONSTANT))
		return scopelist(sc);
	err("unterminated list of scopes");
}

ScopeNode *IBTLParser::scope() {
	if(is(TK_CONSTANT) || is(TK_ID))
		return expr(); // TODO: might want different rule here
	if(is(TK_OBRAK))
		return scope1();
	err("invalid scope");
}

ScopeNode *IBTLParser::scope1() {
	discard(TK_OBRAK);
	return scope_p();
}

ScopeNode *IBTLParser::scope_p() {
	if(is(TK_OBRAK) || is(TK_CONSTANT) || is(TK_ID))
		return scope_p3();		
	if(is(TK_CBRAK))	return scope_p2();
	return scope_p1();
}

ContainerScopeNode *IBTLParser::scope_p3() {
	ContainerScopeNode *t = scopelist();
	discard(TK_CBRAK);
	return t;
}

ScopeNode *IBTLParser::scope_p2() {
	discard(TK_CBRAK);
	return new ContainerScopeNode;
}

ScopeNode *IBTLParser::scope_p1() {
	return exprsuffix();
}

// expr

ExprNode *IBTLParser::expr() {
	if(is(TK_OBRAK)) {
		discard(TK_OBRAK);
		return exprsuffix();
	}
	if(is(TK_CONSTANT)) return constant();
	if(is(TK_ID)) return id();
	err("invalid expr");
}

ExprNode *IBTLParser::exprsuffix() {
	if(is(TK_BINOP) || is(TK_UNOP) || is(TK_MINUS) || is(TK_ASSIGN))
		return opersuffix();
	if(is(TK_LET) || is(TK_PRINT) || is(TK_IF) || is(TK_WHILE))
		return stmtsuffix();
	err("invalid expr");
}

// oper

OperNode *IBTLParser::oper() {
	if(is(TK_OBRAK)) 	return oper1();
	if(is(TK_CONSTANT)) 	return constant();
	if(is(TK_ID)) 		return id(); 
}

OperNode *IBTLParser::oper1() {
	discard(TK_OBRAK);
	return opersuffix();
} 

OperNode *IBTLParser::opersuffix() {
	if(is(TK_BINOP))	return binop();
	if(is(TK_UNOP))		return unop();
	if(is(TK_MINUS))	return minus();
	if(is(TK_ASSIGN))	return assign();
	err("invalid oper");
}

BinopNode *IBTLParser::binop() {
	BinopNode *t = new BinopNode(take(TK_BINOP), oper(), oper());
	discard(TK_CBRAK);
	return t;
}

UnopNode *IBTLParser::unop() {
	UnopNode *t = new UnopNode(take(TK_UNOP), oper());
	discard(TK_CBRAK);
	return t;
}

OperNode *IBTLParser::minus() {
	TokNode *op = take(TK_MINUS);
	OperNode *left = oper();
	OperNode *right = minus_p();
	if(right) return new BinopNode(op, left, right);
	else return new UnopNode(op, left);
}

OperNode *IBTLParser::minus_p() {
	if(is(TK_CBRAK))	return minus_p2();
	return minus_p1();
}

OperNode *IBTLParser::minus_p1() {
	OperNode *right = oper();
	discard(TK_CBRAK);
	return right;
}

OperNode *IBTLParser::minus_p2() {
	discard(TK_CBRAK);
	return NULL;
}

TokNode *IBTLParser::constant() {
	return take(TK_CONSTANT);
}

TokNode *IBTLParser::id() {
	return take(TK_ID);
}

AssignNode *IBTLParser::assign() {
	discard(TK_ASSIGN);
	AssignNode *t = new AssignNode(id(), oper());
	discard(TK_CBRAK);
	return t;
}

// stmts

StmtNode *IBTLParser::stmtsuffix() {
	if(is(TK_IF)) 		return ifstmts();
	if(is(TK_WHILE)) 	return whilestmts();
	if(is(TK_LET)) 		return letstmts();
	if(is(TK_PRINT)) 	return printstmts();
	err("invalid statement");	
}

IfNode *IBTLParser::ifstmts() {
	discard(TK_IF);
	IfNode *t = new IfNode(expr(), expr(), ifstmts_p());
	discard(TK_CBRAK);
	return t;
}

ExprNode *IBTLParser::ifstmts_p() {
	if(is(TK_OBRAK)) return expr();
	if(is(TK_CBRAK)) return NULL;
	err("invalid if statement");
}

WhileNode *IBTLParser::whilestmts() {
	discard(TK_WHILE);
	WhileNode *t = new WhileNode(expr(), exprlist());
	discard(TK_CBRAK);
	return t;
}

LetNode *IBTLParser::letstmts() {
	discard(TK_LET);
	discard(TK_OBRAK);
	LetNode *t = new LetNode(varlist());
	discard(TK_CBRAK);
	discard(TK_CBRAK);
	return t;
}

PrintNode *IBTLParser::printstmts() {
	discard(TK_PRINT);
	PrintNode *t = new PrintNode(oper());
	discard(TK_CBRAK);
	return t;
}

// exprlist & varlist

ExprListNode *IBTLParser::exprlist() {
	return exprlist(new ExprListNode);
}

ExprListNode *IBTLParser::exprlist(ExprListNode *list) {
	list->children.push_back(expr());
	return exprlist_p(list);
}

ExprListNode *IBTLParser::exprlist_p(ExprListNode *list) {
	if(is(TK_OBRAK))	return exprlist(list);
	if(is(TK_CBRAK))	return list;
	err("invalid exprlist");
}

VarListNode *IBTLParser::varlist() {
	return varlist(new VarListNode);
}

VarListNode *IBTLParser::varlist(VarListNode *list) {
	discard(TK_OBRAK);
	list->children.push_back(take(TK_ID));
	list->children.push_back(take(TK_TYPE));
	discard(TK_CBRAK);
	return varlist_p(list);
}

VarListNode *IBTLParser::varlist_p(VarListNode *list) {
	if(is(TK_OBRAK))	return varlist(list);
	if(is(TK_CBRAK))	return list;
	err("invalid varlist");
}
