
#include <lexer/lexer.h>
#include <parser/newparser.h>
#include <symtable.h>
#include <getopt.h>
#include <iostream>
#include <fstream>

using namespace std;

extern char *optarg;
extern int optind, opterr, optopt;

const char *optstr = "tsp";
const struct option longopts[] = {
	{"help", 0, NULL, 'h'}
};
	
const char *helpstr = "\
IBTL Compiler \n\
Usage: 	%s -[t|p] file \n\
Options: \n\
	-t	tokenize only \n\
	-p	tokenize & parse \n\
";

void printUsageAndDie(const char *prog)
{
	printf(helpstr, prog);
	exit(EXIT_FAILURE);
}

void printTokens(IBTLLexer &lex)
{
	Token cur;
	while(true) {
		cur = lex.getToken();
		if(cur.name == TK_EOF) break;
		else
			cout << cur << endl;
	}
}


void printParse(IBTLLexer &lexer)
{
	IBTLParser parser(lexer);
	ProgramNode *p = parser.parse();
	cout << *p << endl;
	delete p;
}




int main(int argc, char **argv)
{
	int opt;
	bool tokens_only = false, parse_only = false, symbols_only = false;
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
		case 's':
			symbols_only = true;
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

	// main file processing loop
	int idx = optind;
	while(idx < argc) {
		filename = argv[idx];
		ifstream in(filename);
		if(!in) {
			cout << "error: could not open file " << filename << endl;
			exit(EXIT_FAILURE);
		}
		cout << "Processing " << filename << endl;
		Reader inputReader(&in);
		SymbolTable symTable;
		IBTLLexer lexer(inputReader, symTable);
		if(tokens_only) printTokens(lexer);
		else if(parse_only) printParse(lexer);
		else printParse(lexer); // parse by default
		cout << endl;
		idx++;
	}

	exit(EXIT_SUCCESS);
}

