
#include <iostream>
#include <memory>
#include <sstream>
#include <parser/tokenstream.h>
#include <parser/parsenodes.h>

using namespace std;

struct TypePred : public TokenPredicate
{
	bool operator() (TokenName n) const
	{
		return n & TK_TYPENAME;
	}
} isType;

struct UnopPred : public TokenPredicate
{
	bool operator() (TokenName n) const
	{
		return n & TK_UNOP;
	}
} isUnop;

struct BinopPred : public TokenPredicate
{
	bool operator() (TokenName n) const
	{
		return n & TK_BINOP;
	}
} isBinop;

struct AmbigPred : public TokenPredicate
{
	bool operator() (TokenName n) const
	{
		return (n & TK_BINOP) && (n & TK_UNOP);
	}
} isAmbiguousOp;

struct ConstPred : public TokenPredicate
{
	bool operator() (TokenName n) const
	{
		return n & TK_CONST;
	}
} isConstant;

struct StmtPred : public TokenPredicate
{
	bool operator() (TokenName n) const
	{
		return n & TK_STATEMENT;
	}
} isStatement;

bool isOper(TokenPair p)
{
	return (p.first == TK_OBRAK && (
		isUnop(p.second) || isBinop(p.second) || p.second == TK_ASSIGN))
		|| isConstant(p.first) || p.first == TK_ID;
}


bool isExpr(TokenPair p)
{
	return (p.first == TK_OBRAK && isStatement(p.second) ||
		isOper(p));
			
}

unique_ptr<Scope> Scope::parse(TokenStream &in)
{
	unique_ptr<Scope> obj(new Scope);
	in.discard(TK_OBRAK);
	TokenPair p = in.peekTwo();
	while(p.first != TK_CBRAK) {
		if(isExpr(p))
			obj->children.push_back(Expr::parse(in));
		else if(p.first == TK_OBRAK)
			obj->children.push_back(Scope::parse(in));
		else
			throw ParseException();
		p = in.peekTwo();
	}
	
	in.discard(TK_CBRAK);
	return obj;
}

string Scope::toString()
{
	ostringstream str;
	str << "[Scope ";
	for(auto iter = children.begin(); iter != children.end();
	    iter++)
		str << (*iter)->toString();
	str << "]";
	return str.str();
}

unique_ptr<Expr> Expr::parse(TokenStream &in)
{
	TokenPair p = in.peekTwo();
	if(p.first == TK_OBRAK) {
		if(isUnop(p.second) || isBinop(p.second) || p.second == TK_ASSIGN)
			return Oper::parse(in);
		else if(isStatement(p.second))
			return Statement::parse(in);
		else
			throw ParseException();
	}
	else if(isConstant(p.first) || p.first == TK_ID)
		return Oper::parse(in);
	else
		throw ParseException();
}

// oper

unique_ptr<Oper> Oper::parse(TokenStream &in)
{
	TokenPair p = in.peekTwo();
	if(p.first == TK_OBRAK) {
		if(p.second == TK_ASSIGN)
			return AssignOp::parse(in);
		else if(isUnop(p.second) && isBinop(p.second))
			return UnOrBinOp::parse(in);
		else if(isBinop(p.second))
			return BinOp::parse(in);
		else if(isUnop(p.second))
			return UnOp::parse(in);
		else
			throw ParseException();
	}
	else if(isConstant(p.first))
		return in.take();
	else if(p.first == TK_ID)
		return in.take();
	else
		throw ParseException();
}

unique_ptr<AssignOp> AssignOp::parse(TokenStream &in)
{
	unique_ptr<AssignOp> obj(new AssignOp);
	in.discard(TK_OBRAK);	
	in.discard(TK_ASSIGN);
	obj->left = Oper::parse(in); //TODO: should this actually be oper?
	obj->right = Oper::parse(in);
	in.discard(TK_CBRAK);
	return obj;
}

string AssignOp::toString()
{
	ostringstream str;
	str << "[ Assign " << left->toString() << " " << right->toString() << "]";
	return str.str();
}

unique_ptr<BinOp> BinOp::parse(TokenStream &in)
{
	unique_ptr<BinOp> obj(new BinOp);
	in.discard(TK_OBRAK);
	obj->op = in.take(isBinop);
	obj->first = Oper::parse(in);
	obj->second = Oper::parse(in);
	in.discard(TK_CBRAK);
	return obj;
}

string BinOp::toString()
{
	ostringstream str;
	str << "[ Binop " << op->toString() << " " << first->toString() <<
		" " << second->toString() << "]";
	return str.str();
}

unique_ptr<UnOp> UnOp::parse(TokenStream &in)
{
	unique_ptr<UnOp> obj(new UnOp);
	in.discard(TK_OBRAK);
	obj->op = in.take(isUnop);
	obj->first = Oper::parse(in);
	in.discard(TK_CBRAK);
	return obj;
}

string UnOp::toString()
{
	ostringstream str;
	str << "[ Unop " << op->toString() << " " << first->toString() << "]";
	return str.str();
}

unique_ptr<Oper> UnOrBinOp::parse(TokenStream &in)
{
	unique_ptr<Token> op;
	unique_ptr<Oper> first;
	unique_ptr<Oper> second;

	in.discard(TK_OBRAK);
	op = in.take(isAmbiguousOp);
	first = Oper::parse(in);
	if(in.peekOne() == TK_CBRAK) {
		UnOp *obj = new UnOp;
		obj->op = std::move(op); 
		obj->first = std::move(first);
		return unique_ptr<Oper>(obj);
	} else {
		second = Oper::parse(in);
		BinOp *obj = new BinOp;
		obj->op = std::move(op); obj->first = std::move(first); 
		obj->second = std::move(second);
		return unique_ptr<Oper>(obj);
	}
}

// statements

unique_ptr<Statement> Statement::parse(TokenStream &in)
{
	TokenPair p = in.peekTwo();
	if(p.first == TK_OBRAK) {
		switch(p.second) {
		case TK_WHILE:
			return WhileStmt::parse(in);
			break;
		case TK_IF:
			return IfStmt::parse(in);
			break;
		case TK_LET:
			return LetStmt::parse(in);
			break;
		case TK_PRINT:
			return PrintStmt::parse(in);
			break;
		default:
			throw ParseException();	
		}
	}
	else
		throw ParseException();
}

unique_ptr<WhileStmt> WhileStmt::parse(TokenStream &in)
{
	unique_ptr<WhileStmt> obj(new WhileStmt);
	in.discard(TK_OBRAK);
	in.discard(TK_WHILE);
	obj->cond = Expr::parse(in);
	obj->statements = ExprList::parse(in);
	in.discard(TK_CBRAK);
	return obj;
}

string WhileStmt::toString()
{
	ostringstream str;
	str << "[while " << cond->toString() << " " <<
		statements->toString() << "]";
	return str.str();
}

unique_ptr<IfStmt> IfStmt::parse(TokenStream &in)
{
	unique_ptr<IfStmt> obj(new IfStmt);
	in.discard(TK_OBRAK);
	in.discard(TK_IF);
	obj->cond = Expr::parse(in);
	obj->thenexpr = Expr::parse(in);
	if(in.peekOne() == TK_CBRAK)
		obj->elseexpr = unique_ptr<Expr>();
	else
		obj->elseexpr = Expr::parse(in);
	in.discard(TK_CBRAK);
	return obj;
}

string IfStmt::toString()
{
	ostringstream str;
	str << "[if " << cond->toString() << " " << thenexpr->toString();
	if(elseexpr) str << elseexpr->toString();
	str << "]";
	return str.str();
}

unique_ptr<PrintStmt> PrintStmt::parse(TokenStream &in)
{
	unique_ptr<PrintStmt> obj(new PrintStmt);
	in.discard(TK_OBRAK);
	in.discard(TK_PRINT);
	obj->value = Oper::parse(in);
	in.discard(TK_CBRAK);
	return obj;
}

string PrintStmt::toString()
{
	ostringstream str;
	str << "[print " << value->toString() << "]";
	return str.str();
}

unique_ptr<LetStmt> LetStmt::parse(TokenStream &in)
{
	unique_ptr<LetStmt> obj(new LetStmt);
	in.discard(TK_OBRAK);
	in.discard(TK_LET);
	obj->varlist = VarList::parse(in);
	in.discard(TK_CBRAK);
	return obj;
}

string LetStmt::toString()
{
	ostringstream str;
	str << "[let " << varlist->toString() << "]";
	return str.str();
}

// exprlist

unique_ptr<ExprList> ExprList::parse(TokenStream &in)
{
	unique_ptr<ExprList> obj(new ExprList);
	obj->list.push_back(Expr::parse(in));
	while(in.peekOne() != TK_CBRAK)
		obj->list.push_back(Expr::parse(in));
	return obj;
}

string ExprList::toString()
{
	ostringstream str;
	str << "[ExprList ";
	for(auto iter = list.begin(); iter != list.end(); iter++)
		str << (*iter)->toString() << " ";
	str << "]";
	return str.str(); 
}

// varlist

unique_ptr<VarList> VarList::parse(TokenStream &in)
{
	unique_ptr<VarList> obj(new VarList);
	obj->list.push_back(VarDec::parse(in));
	while(in.peekOne() != TK_CBRAK)
		obj->list.push_back(VarDec::parse(in));
	return obj;
}

string VarList::toString()
{
	ostringstream str;
	str << "[VarList ";
	for(auto iter = list.begin(); iter != list.end(); iter++)
		str << (*iter)->toString() << " ";
	str << "]";
	return str.str();
}

unique_ptr<VarDec> VarDec::parse(TokenStream &in)
{
	unique_ptr<VarDec> obj(new VarDec);
	in.discard(TK_OBRAK);
	obj->name = in.take(TK_ID);
	obj->type = in.take(isType);
	in.discard(TK_CBRAK);
	return obj;
}

string VarDec::toString()
{
	ostringstream str;
	str << "[VarDec " << name->toString() << " " << type->toString() << "]";
	return str.str();
}




