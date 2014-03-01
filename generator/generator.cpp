
#include <parser/newnodes.h>
#include <generator/generator.h>
#include <assert.h>

std::string typeString(Type tp)
{
    switch(tp) {
    case TP_INT: return "int";
    case TP_REAL: return "real";
    case TP_BOOL: return "bool";
    case TP_STR: return "str";
    case TP_NONE: return "none";
    default:
        return "none";
    }
}


Type Node::generate(Stream &str, SymbolTable &sym, int indent)
{
    for(auto iter = children.begin(); iter != children.end(); iter++)
        (*iter)->generate(str, sym, indent);
    return TP_NONE;
}

Type ProgramNode::generate(Stream &str, SymbolTable &sym, int indent)
{
    std::string tabs(indent, '\t');
    str << ": prog" << std::endl;
    str << tabs << "\t";
    scope()->generate(str, sym, indent+1);
    str << std::endl << ";" << std::endl;
    str << "prog bye" << std::endl;
    return TP_NONE;
}

/********************************************************
 Statements
********************************************************/

Type PrintNode::generate(Stream &str, SymbolTable &sym, int indent)
{
    Type t = oper()->generate(str, sym, indent);
    switch(t) {
    case TP_BOOL:
    case TP_INT:    str << " ."; break;
    case TP_REAL:   str << " f."; break;
    case TP_STR:    str << " type"; break;
    default:        assert(0 && "invalid type in print stmt"); break;  
    }
    return TP_NONE;
}

Type IfNode::generate(Stream &str, SymbolTable &sym, int indent)
{
    std::string tabs(indent, '\t');
    Type t = condExpr()->generate(str, sym, indent);
    if(t != TP_BOOL)
        typeError(std::string("expected bool condition in if statement"));
                  
    str << " if" << std::endl;
    str << tabs << "\t";
    thenExpr()->generate(str, sym, indent+1);
    str << std::endl;
    if(elseExpr()) {
        str << tabs << "else" << std::endl;
        str << tabs << "\t";
        elseExpr()->generate(str, sym, indent+1);
    }
    str << std::endl;
    str << tabs << "endif" << std::endl;
    return TP_NONE;   
}

/********************************************************
 BinopNode
********************************************************/

void BinopNode::genReal(Type l, Type r, Stream &str)
{
    // if either operand is an int, cast to real
    // unless operator is exp
    if(op()->attr() != AT_EXP) {
        if(l == TP_INT) {
            assert(r == TP_REAL);
            str << " s>f fswap";
        } else if(r == TP_INT) {
            assert(l == TP_REAL);
            str << " s>f";
        }
    }

    switch(op()->attr()) {
    case AT_PLUS:   str << " f+"; break;
    case AT_MINUS:  str << " f-"; break;
    case AT_MULT:   str << " f*"; break;
    case AT_DIV:    str << " f/"; break;
    case AT_MOD:    str << " fmod"; break;
    case AT_EXP:    str << " 1e0 0 do fover f* loop fnip"; break;
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
    case AT_MINUS:  str << " -"; break; // TODO: make MINUS an attr
    case AT_MULT:   str << " *"; break;
    case AT_DIV:    str << " /"; break;
    case AT_MOD:    str << " mod"; break;
    case AT_EXP:    str << " 1 swap 0 do over * loop nip"; break;
    case AT_LT:     str << " <"; break;
    case AT_GT:     str << " >"; break;
    case AT_LE:     str << " <="; break;
    case AT_GE:     str << " >="; break;
    case AT_EQ:     str << " ="; break;
    case AT_NE:     str << " <>"; break;
    default:        assert(0); break;
    }
}

void BinopNode::genBool(Type l, Type r, Stream &str)
{
    switch(op()->attr()) {
    case AT_AND:    str << " and"; return;
    case AT_OR:     str << " or"; return;
    default: 
        break;  
    }

    // if we fall down here, we are looking at one of the 
    // comparison ops
    if(l == TP_REAL || r == TP_REAL) genReal(l, r, str);
    else                             genInt(str);
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
    binopError(std::string("expected bool operands to binary ") +
               opString(), l, r);
}

Type BinopNode::typeCheckCompareOp(Type l, Type r)
{
    if((l == TP_REAL || l == TP_INT) &&
       (r == TP_REAL || r == TP_INT))       return TP_BOOL;
    binopError(std::string("expected numeric operands to binary ") +
              opString(), l, r);
}

Type BinopNode::typeCheckNumOp(Type l, Type r)
{
    if(l == TP_REAL || r == TP_REAL) {
        if((l == TP_REAL || l == TP_INT) &&
           (r == TP_REAL || r == TP_INT))   return TP_REAL;
        binopError(std::string("expected numeric operands to binary ") +
                   opString(), l, r);
    }
    if(l == TP_INT && r == TP_INT)          return TP_INT;
    binopError(std::string("expected numeric operands to binary ") +
              opString(), l, r);
}

Type BinopNode::typeCheckExpOp(Type l, Type r)
{
    if(l == TP_INT && r == TP_INT)          return TP_INT;
    if(l == TP_REAL && r == TP_INT)         return TP_REAL;
    binopError(std::string("expected numeric left arg and int right arg to binary ") +
               opString(), l, r); 
}

Type BinopNode::typeCheck(Type l, Type r)
{
    switch(op()->attr()) {
    case AT_PLUS:
        if(l == TP_STR && r == TP_STR) return TP_STR;
        return typeCheckNumOp(l, r);
        break;
    case AT_MINUS:
    case AT_MULT:
    case AT_DIV:
    case AT_MOD:
        return typeCheckNumOp(l, r); 
        break;
    case AT_EXP:
        return typeCheckExpOp(l, r);
        break;
    case AT_LT:
    case AT_LE:
    case AT_GT:
    case AT_GE:
    case AT_EQ:
    case AT_NE:
        return typeCheckCompareOp(l, r);
        break;
    case AT_AND:
    case AT_OR:
        return typeCheckBoolOp(l, r);
        break;
    default: assert(0); break;
    }
}

Type BinopNode::generate(Stream &str, SymbolTable &sym, int indent)
{
    Type l = left()->generate(str, sym, indent);
    Type r = right()->generate(str, sym, indent);
    Type exprType = typeCheck(l, r);
    switch(exprType) {
    case TP_BOOL:       genBool(l, r, str); break;
    case TP_REAL:       genReal(l, r, str); break;
    case TP_INT:        genInt(str); break;
    case TP_STR:        genStr(str); break;
    default:            assert(0); break;
    }
    
    return exprType;
}

/********************************************************
 UnopNode
********************************************************/

void UnopNode::genReal(Type l, Stream &str)
{
    // cast int to real
    if(l == TP_INT) {
        str << " s>f";
    }

    switch(op()->attr()) {
    case AT_MINUS:      str << " fnegate"; break;
    case AT_SIN:        str << " fsin"; break;
    case AT_COS:        str << " fcos"; break;
    case AT_TAN:        str << " ftan"; break;
    default:            assert(0 && "unexpected case"); break;
    }
}

void UnopNode::genInt(Stream &str)
{
    switch(op()->attr()) {
    case AT_MINUS:      str << " negate"; break;
    default:            assert(0 && "unexpected case"); break;  
    }
}

void UnopNode::genBool(Stream &str)
{
    switch(op()->attr()) {
    case AT_NOT:        str << " invert"; break;
    default:            assert(0 && "unexpected case"); break;
    }
}

Type UnopNode::typeCheck(Type l)
{
    switch(op()->attr()) {
    case AT_NOT:
        if(l == TP_BOOL)    return TP_BOOL;
        unopError(std::string("expected bool operand to unary not"), l);
        break;
    case AT_MINUS:
        if(l == TP_INT)     return TP_INT;
        if(l == TP_REAL)    return TP_REAL;
        unopError(std::string("expected numeric operand to unary -"), l);
    case AT_SIN:
    case AT_COS:
    case AT_TAN:
        if(l == TP_REAL || l == TP_INT) return TP_REAL;
        unopError(std::string("expected numeric operand to unary ") + 
                  opString(), l);
    default:
        assert(0 && "unhandled unary operator in switch case");
        break;
    }
}

Type UnopNode::generate(Stream &str, SymbolTable &sym, int indent)
{
    Type l = left()->generate(str, sym, indent);
    Type exprType = typeCheck(l);
    switch(exprType) {
    case TP_BOOL:       genBool(str); break;
    case TP_INT:        genInt(str); break;
    case TP_REAL:       genReal(l, str); break;
    case TP_STR:
    default:
        assert(0 && "unexpected case");
        break;    
    }

    return exprType;
}


Type AssignNode::generate(Stream &str, SymbolTable &sym, int indent)
{
    assert(0 && "assign generate() not implemented");
}

/********************************************************
 TokNode
********************************************************/

void TokNode::genReal(Stream &str)
{
    const std::string &val = token.val;
    str << " " << val;
    // if the token value does not contain an 'e' or 'E',
    // append 'e0' to the value
    if(val.find('e') == std::string::npos &&
       val.find('E') == std::string::npos)
        str << "e0";
}

void TokNode::genBool(Stream &str)
{
    switch(token.attr) {
    case AT_T:      str << " true"; break;
    case AT_F:      str << " false"; break;
    default:        assert(0 && "unexpected case"); break;
    }
}

Type TokNode::generate(Stream &str, SymbolTable &sym, int indent)
{
    switch(token.attr) {
    case AT_INT_OCT:
    case AT_INT_HEX:
    case AT_INT_DEC:
        str << " " << token.val;
        return TP_INT;
    case AT_REAL:
        genReal(str);
        return TP_REAL;
    case AT_T:
    case AT_F:
        genBool(str);
        return TP_BOOL;
    case AT_STR:
        str << " s\" " << token.val.substr(1, std::string::npos);
        return TP_STR;
    default: assert(0 && "unknown literal type"); break;
    }
}


