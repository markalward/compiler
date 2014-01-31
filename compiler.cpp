
#include <lexer/lexer.h>
#include <parser/parser.h>
#include <getopt.h>
#include <iostream>
#include <fstream>

using namespace std;

extern char *optarg;
extern int optind, opterr, optopt;

const char *optstr = "tp";
const struct option longopts[] = {
	{"help", 0, NULL, 'h'}
};
	
const char *helpstr = "\
IBTL Compiler \n\
Usage: 	%s -[t|p] file \n\
	%s -[t|p] - \n\
Options: \n\
	-t	tokenize only \n\
	-p	tokenize & parse \n\
";

void printUsageAndDie(const char *prog)
{
	printf(helpstr, prog);
	exit(EXIT_FAILURE);
}

void printTokens(TokenStream &toks)
{
	unique_ptr<Token> cur;
	while(true) {
		cur = toks.take();
		if(cur->name == TK_EOF) break;
		else
			cout << cur->toString() << endl;
	}
}

void printParse(TokenStream &toks)
{
	unique_ptr<ParseNode> tree;
	IBTLParser parser;
	tree = parser.parse(toks);
	cout << tree->toString() << endl;
}

int main(int argc, char **argv)
{
	int opt;
	bool tokens_only = false, parse_only = false;
	string filename;
	bool use_stdin;

	while((opt = getopt_long(argc, argv, optstr, longopts, NULL)) != -1) {
		switch(opt) {
		case '?':
			printUsageAndDie(argv[0]);
			break;
		case 't':
			tokens_only = true;
			break;
		case 'p':
			parse_only = true;
			break;
		case 'h':
			printf(helpstr, argv[0]);
			exit(EXIT_SUCCESS);
			break;
		default:
			break;
		}
	}

	if(string(argv[optind]) == string("-")) use_stdin = true;
	else filename = argv[optind];

	std::istream *in;
	if(use_stdin) in = &std::cin;
	else in = new std::ifstream(filename);
	Reader inputreader(in);
	
	// create lexical analyzer
	IBTLLexer lexer(inputreader);
	TokenStream tokstream(&lexer);
	if(tokens_only) {
		printTokens(tokstream);
		exit(EXIT_SUCCESS);
	}
	else if(parse_only) {
		printParse(tokstream);
		exit(EXIT_SUCCESS);
	}

}

