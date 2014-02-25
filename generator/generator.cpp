
#include <parser/newnodes.h>
#include <assert.h>

void BinopNode::genReal(Type l, Type r, Stream &str)
{
    // if either operand is an int, cast to real
    if(l == TP_INT) {
        assert(r == TP_REAL);
        str << " 0 d>f fswap";
    } else if(r == TP_INT) {
        assert(l == TP_REAL);
        str << " 0 d>f";
    }

    switch(op()->attr()) {
    case AT_PLUS:   str << " f+"; break;
    //case AT_MINUS:  str << " f-"; break;
    case AT_MULT:   str << " f*"; break;
    case AT_DIV:    str << " f/"; break;
    case AT_MOD:    str << " fmod"; break;
    case AT_EXP:    break; // TODO: how to do exp?
    case AT_LT:     str << " f<"; break;
    case AT_LE:     str << " f<="; break;
    case AT_GT:     str << " f>"; break;
    case AT_GE:     str << " f>="; break;
    case AT_EQ:     str << " f="; break;
    case AT_NE:     str << " f<>"; break;
    default:        assert(0); break;
    }   
}

void BinopNode::genInt(Stream &str)
{
    switch(op()->attr()) {
    case AT_PLUS:   str << " +"; break;
    //case AT_MINUS:  str << " -"; break; // TODO: make MINUS an attr
    case AT_MULT:   str << " *"; break;
    case AT_DIV:    str << " /"; break;
    case AT_MOD:    str << " mod"; break;
    case AT_EXP:    break; // TODO: how to do exp?
    case AT_LT:     str << " <"; break;
    case AT_GT:     str << " >"; break;
    case AT_LE:     str << " <="; break;
    case AT_GE:     str << " >="; break;
    case AT_EQ:     str << " ="; break;
    case AT_NE:     str << " <>"; break;
    default:        assert(0); break;
    }
}

void BinopNode::genBool(Stream &str)
{
    switch(op()->attr()) {
    case AT_AND:    str << " and"; break;
    case AT_OR:     str << " or"; break;
    default:        assert(0); break;
    }
}

void BinopNode::genStr(Stream &str)
{
    switch(op()->attr()) {
    case AT_PLUS:
        assert(0 && "string concat not implemented"); // TODO 
        break;
    default:        assert(0); break;
    }
}

Type BinopNode::typeCheckBoolOp(Type l, Type r)
{
    if(l == TP_BOOL && r == TP_BOOL)        return TP_BOOL;
    typeError("expected bool", l, r);
}

Type BinopNode::typeCheckCompareOp(Type l, Type r)
{
    if((l == TP_REAL || l == TP_INT) &&
       (r == TP_REAL || r == TP_INT))       return TP_BOOL;
    typeError("expected numbers", l, r);
}

Type BinopNode::typeCheckNumOp(Type l, Type r)
{
    if(l == TP_REAL || r == TP_REAL) {
        if((l == TP_REAL || l == TP_INT) &&
           (r == TP_REAL || r == TP_INT))   return TP_REAL;
        typeError("expected numbers", l, r);
    }
    if(l == TP_INT && r == TP_INT)          return TP_INT;
    typeError("expected numbers", l, r);
}

Type BinopNode::typeCheck(Type l, Type r)
{
    switch(op()->attr()) {
    case AT_PLUS:
        if(l == TP_STR && r == TP_STR) return TP_STR;
        return typeCheckNumOp(l, r);
    //case AT_MINUS:
    case AT_MULT:
    case AT_DIV:
    case AT_MOD:
    case AT_EXP:
        return typeCheckNumOp(l, r);
    case AT_LT:
    case AT_LE:
    case AT_GT:
    case AT_GE:
    case AT_EQ:
    case AT_NE:
        return typeCheckCompareOp(l, r);
    case AT_AND:
    case AT_OR:
        return typeCheckBoolOp(l, r);
    default: assert(0); break;
    }
}

Type BinopNode::generate(Stream &str)
{
    Type l = left()->generate(str);
    Type r = right()->generate(str);
    Type exprType = typeCheck(l, r);
    switch(exprType) {
    case TP_BOOL:       genBool(str); break;
    case TP_REAL:       genReal(l, r, str); break;
    case TP_INT:        genInt(str); break;
    case TP_STR:        genStr(str); break;
    default:            assert(0); break;
    }
    
    return exprType;
}


Type UnopNode::generate(Stream &str)
{
    assert(0 && "unop generate() not implemented");
}

Type AssignNode::generate(Stream &str)
{
    assert(0 && "assign generate() not implemented");
}

Type TokNode::generate(Stream &str)
{
    // TODO: temporary
    str << " " << token.val;
    switch(token.attr) {
    case AT_INT_OCT:
    case AT_INT_HEX:
    case AT_INT_DEC:
        return TP_INT;
    case AT_REAL:
        return TP_REAL;
    case AT_T:
    case AT_F:
        return TP_BOOL;
    case AT_STR:
        return TP_STR;
    default: assert(0 && "unknown literal type"); break;
    }
}


