
#include <lexer/lexer.h>
#include <parser/newparser.h>
#include <symtable.h>
#include <getopt.h>
#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;

extern char *optarg;
extern int optind, opterr, optopt;

const char *optstr = "tspo:";
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


ProgramNode *parse(IBTLLexer &lexer, bool printTree, const string &filename)
{
	IBTLParser parser(lexer);
	try {
		ProgramNode *p = parser.parse();
        if(printTree) {
		    cout << "Parse tree for " << filename << ":" << endl;
		    cout << *p << endl;
        }
		return p;
	}
	catch(ParseException &ex) {
		cout << "parse error: " << ex.what() << endl;
		exit(EXIT_FAILURE);
	}
	catch(LexException &ex) {
		cout << "lexer error: " << ex.what() << endl;
		exit(EXIT_FAILURE);
	}
}


void printCode(ProgramNode *p, SymbolTable &sym, const string &filename, 
               const string &outputname, std::ostream &file)
{
    try {
	    p->generate(file, sym, 0);
    }
    catch(GenException &ex) {
        cout << std::endl << "code generator error: " << ex.what() << endl;
        remove(outputname.c_str());
        exit(EXIT_FAILURE);
    }
}




int main(int argc, char **argv)
{
	int opt;
	bool tokens_only = false, parse_only = false, symbols_only = false;
	string filename;
	string outputname;

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
        case 'o':
            outputname = std::string(optarg);
            break;
		case 'h':
			printf(helpstr, argv[0]);
			exit(EXIT_SUCCESS);
			break;
		default:
			break;
		}
	}

    if(!outputname.size()) outputname = std::string("a.out");
    std::ofstream outputfile(outputname);
    if(!outputfile) {
        cout << "could not open output file " << outputname << " for writing"
            << endl;
        exit(EXIT_FAILURE);
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
		Reader inputReader(&in);
		SymbolTable symTable;
		IBTLLexer lexer(inputReader, symTable);
        ProgramNode *p;

		if(tokens_only) printTokens(lexer);
		else {
            p = parse(lexer, parse_only, filename);
            if(!parse_only) {
                printCode(p, symTable, filename, outputname, outputfile);
            }
        }
		
		cout << endl;
		idx++;
	}

	exit(EXIT_SUCCESS);
}

