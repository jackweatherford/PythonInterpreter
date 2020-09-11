#include <iostream>

#include "Token.hpp"
#include "Tokenizer.hpp"
#include "Parser.hpp"


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


int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " nameOfAnInputFile\n";
        exit(1);
    }

    std::ifstream inputStream;

    inputStream.open(argv[1], std::ios::in);
    if (!inputStream.is_open()) {
        std::cout << "Unable top open " << argv[1] << ". Terminating...";
        perror("Error when attempting to open the input file.");
        exit(2);
    }

    Tokenizer tokenizer1(inputStream);
    identifyTokens(tokenizer1);
    // Reset inputStream
    inputStream.clear();
    inputStream.seekg(0);

    Tokenizer tokenizer(inputStream);
    Functions *funcs = new Functions();
    Parser parser(tokenizer, funcs);
    Statements *statements = parser.statements();
    Token lastToken = tokenizer.getToken();
    if (!lastToken.eof()) {
        std::cout << "Unexpected token in input." << std::endl;
        lastToken.print();
        exit(1);
    }

    SymTab symTab;

    statements->print();
    statements->evaluate(symTab);
    std::cout << std::endl << "Symbol table contains the following variables.\n";
    symTab.print();

    return 0;
}
