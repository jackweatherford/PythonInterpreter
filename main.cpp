#include <iostream>
#include <cstring>

#include "Token.hpp"
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "Globals.hpp"
bool verbose;

// Prints each token and the number of lines in the input file.
void identifyTokens(Tokenizer tokenizer) {
    Token tok = tokenizer.getToken();
    int numLines = 1;
    while (!tok.eof()) {
        if (tok.eol()) {
            std::cout << std::endl;
            numLines++;
        }
        else
            tok.print();
        tok = tokenizer.getToken();
    }
    std::cout << "\nThe input has " << numLines << " lines.\n\n";
}


int main(int argc, const char *argv[]) {
	
    if (argc == 1 || argc > 3 || (argc == 2 && strcmp(argv[1], "-v") == 0)) {
        std::cout << "Usage: ./python.exe [-v] <path>\n";
		std::cout << "-v is optional, include it to enable verbose mode, mainly for debugging\n";
        exit(1);
	} else if (argc == 3) {
		if (strcmp(argv[1], "-v") == 0) {
			verbose = true;
			argv[1] = argv[2];
		} else if (strcmp(argv[2], "-v") == 0) {
			verbose = true;
		}
		else {
			std::cout << "Usage: ./python.exe [-v] <path>\n";
			std::cout << "-v is optional, include it to enable verbose mode, mainly for debugging\n";
			exit(1);
		}
	}
	
    std::ifstream inputStream;

    inputStream.open(argv[1], std::ios::in);
    if (!inputStream.is_open()) {
        std::cout << "Unable to open " << argv[1] << ". Terminating...\n";
        perror("Error when attempting to open the input file");
        exit(2);
    }

	if (verbose) {
		Tokenizer tokenizer1(inputStream);
		identifyTokens(tokenizer1);
		// Reset inputStream
		inputStream.clear();
		inputStream.seekg(0);
	}

    Tokenizer tokenizer(inputStream);
    Functions *funcs = new Functions();
    Parser parser(tokenizer, funcs);
    Statements *statements = parser.statements();
    Token lastToken = tokenizer.getToken();
    if (!lastToken.eof()) {
        std::cout << "Unexpected token in input:" << std::endl;
        lastToken.print();
        exit(1);
    }

    SymTab symTab;
	if (verbose) { statements->print(); }
    statements->evaluate(symTab);
	if (verbose == true) {
		std::cout << std::endl << "The symbol table contains the following variables:\n";
		symTab.print();
	}

    return 0;
}
