#ifndef EXPRINTER_TOKENIZER_HPP
#define EXPRINTER_TOKENIZER_HPP


#include <fstream>
#include <vector>
#include "Token.hpp"

class Tokenizer {

public:
    Tokenizer(std::ifstream &inStream);
    Token getToken();
    void ungetToken();
    void printProcessedTokens();

private:
    Token lastToken;
    bool ungottenToken;
    std::ifstream &inStream;
    std::vector<Token> _tokens;
    std::vector<int> indentStack;
    bool parsingANewLine;

private:
    std::string readName();
    int readInteger();
    double readDouble(int left);
    std::string readString(char end);
};

#endif //EXPRINTER_TOKENIZER_HPP
