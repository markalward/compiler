
#ifndef PARSER_H
#define PARSER_H

#include <parser/basenodes.h>
#include <parser/parsenodes.h>
#include <parser/tokenstream.h>
#include <memory>

using namespace std;

class Parser
{
public:
	virtual unique_ptr<ParseNode> parse(TokenStream &toks) = 0;	
};


class IBTLParser
{
public:
	
	unique_ptr<ParseNode> parse(TokenStream &toks)
	{
		unique_ptr<ParseNode> result(Scope::parse(toks));
		return result;
	}
};

#endif
