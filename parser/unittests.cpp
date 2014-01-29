
#include <iostream>
#include <assert.h>
#include "tokenstream.h"
#include "parsenodes.h"

using namespace std;

struct OperTest
{

	TokenStream *create(vector<TokenName> names)
	{
		vector<Token *> tok;
		for(auto iter = names.cbegin(); iter != names.cend(); iter++)
			tok.push_back(new Token(*iter));

		TestLexer *lex = new TestLexer;
		lex->setTokens(tok);
		return new TokenStream(lex);
	}

	void scope1()
	{
		vector<TokenName> tok({
			TK_OBRAK,
				TK_OBRAK,
					TK_OBRAK, TK_PLUS, TK_STR, 
						TK_OBRAK, TK_MULT, TK_REAL, TK_REAL, TK_CBRAK,
					TK_CBRAK,
				TK_CBRAK,
			TK_CBRAK,
			TK_EOF
			});
		TokenStream *str = create(tok);
		unique_ptr<Scope> tree = Scope::parse(*str);
		cout << tree->toString() << endl;
	}

	void stmt1()
	{
		vector<TokenName> tok({
			
				TK_OBRAK, TK_IF, TK_STR,
					TK_OBRAK, TK_PLUS, TK_INT, TK_INT, TK_CBRAK,
				TK_CBRAK,
			
			TK_EOF
		});
		TokenStream *str = create(tok);
		unique_ptr<Statement> tree = Statement::parse(*str);
		cout << tree->toString() << endl;
	}

	void stmt2()
	{
		vector<TokenName> tok({
			TK_OBRAK, TK_WHILE,
				TK_OBRAK, TK_ASSIGN, TK_ID,
					TK_OBRAK, TK_PLUS, TK_ID, TK_INT, TK_CBRAK,
				TK_CBRAK,
				TK_OBRAK, TK_IF,
					TK_ID,
					TK_OBRAK, TK_PRINT, TK_STR, TK_CBRAK,
					TK_OBRAK, TK_PRINT, TK_STR, TK_CBRAK,
				TK_CBRAK,
			TK_CBRAK,
			TK_EOF
		});
		TokenStream *str = create(tok);
		unique_ptr<Statement> tree = Statement::parse(*str);
		cout << tree->toString() << endl;
	}
};

int main(int argc, char *argv[])
{
	OperTest t;
	t.stmt1();
	t.stmt2();
}
