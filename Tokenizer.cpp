#include <iostream>
#include <string>
#include "Tokenizer.hpp"

std::string Tokenizer::readName() {
    // This function is called when it is known that
    // the first character in input is an alphabetic character.
    // The function reads and returns all characters of the name.

    std::string name;
    char c;
    while( inStream.get(c) && isalnum(c) ) {
        name += c;
    }
    if(inStream.good())  // In the loop, we have read one char too many.
        inStream.putback(c);
    return name;
}

int Tokenizer::readInteger() {
    // This function is called when it is known that
    // the first character in input is a digit.
    // The function reads and returns all remaining digits.

    int intValue = 0;
    char c;
    while( inStream.get(c) && isdigit(c) && c != '.') {
        intValue = intValue * 10 + c - '0';
    }
    if(inStream.good() || c == '.')  // In the loop, we have read one char too many.
        inStream.putback(c);
    return intValue;
}

double Tokenizer::readDouble(int left) {
    // This function is called when it is known that
    // there is a double in input (prev char was a .)
    // The function reads and returns a double containing left.(remaining digits)

    std::string s = std::to_string(left) + '.';
    char c;
    while( inStream.get(c) && isdigit(c)) {
        s += c;
    }
    if(inStream.good())  // In the loop, we have read one char too many.
        inStream.putback(c);
    return std::stod(s);
}

std::string Tokenizer::readString(char end) {
    // This function is called when it is known that
    // there is a string in input (prev char was " or ') (stored in end)
    // The function reads and returns all characters of the string.

    std::string s;
    char c;
    while( inStream.get(c) && c != end ) {
        if (c == '\n' || inStream.eof()) {
            std::cout << std::endl << "Tokenizer::readString EOL while scanning string literal" << std::endl;
            exit(1);
        }
        s += c;
    }
    return s;
}

Tokenizer::Tokenizer(std::ifstream &stream): ungottenToken{false}, inStream{stream}, lastToken{}, parsingANewLine{true} {
    indentStack.push_back(0);
}

Token Tokenizer::getToken() {

    if (ungottenToken) {
        ungottenToken = false;
        return lastToken;
    }

    char c;
    int numSpaces = 0;
    while (inStream.get(c) && isspace(c) && c != '\n')
        numSpaces++;

    if (inStream.bad()) {
        std::cout << "Error while reading the input stream in Tokenizer.\n";
        exit(1);
    }

    Token token;

    if (inStream.eof() || inStream.fail()) {
        if (indentStack.back() != 0) {
            token.dedent() = true;
            indentStack.pop_back();
            _tokens.push_back(token);
            return lastToken = token;
        }
        token.eof() = true;
        _tokens.push_back(token);
        return lastToken = token;
    }

    if (parsingANewLine) {
        /* while (c == '\n') {
            numSpaces = 0;
            while (inStream.get(c) && isspace(c))
                numSpaces++;
        }*/
        if (c == '\n') {
            token.eol() = true;
            parsingANewLine = true;
            _tokens.push_back(token);
            return lastToken = token;
        }
        if (numSpaces > indentStack.back()) {
            inStream.putback(c);
            token.indent() = true;
            indentStack.push_back(numSpaces);
            parsingANewLine = false;
            _tokens.push_back(token);
            return lastToken = token;
        } else if (numSpaces < indentStack.back()) {
            bool valid = false;
            for (int i = indentStack.size(); i >= 0; i--) {
                if (numSpaces == indentStack[i]) {
                    valid = true;
                    break;
                }
            }
            if (!valid) {
                std::cout << std::endl << "Tokenizer::getToken invalid indentation level" << std::endl;
                exit(1);
            }
            inStream.putback(c);
            token.dedent() = true;
            indentStack.pop_back();
            if (indentStack.back() == numSpaces)
                parsingANewLine = false;
            _tokens.push_back(token);
            return lastToken = token;
        }
    }
    parsingANewLine = false;

    if (c == '\n') {  // will not ever be the case unless new-line characters are not suppressed.
        token.eol() = true;
        parsingANewLine = true;
    } else if (isdigit(c)) { // an integer?
        // put the digit back into the input stream so
        // we read the entire number in a function
        inStream.putback(c);
        int left = readInteger();
        inStream.get(c);
        if (c == '.') {
            token.setDouble(readDouble(left));
        } else {
            inStream.putback(c);
            token.setWholeNumber(left);
        }

    } else if (c == '=') {
        inStream.get(c);
        if (c == '=')
            token.op("==");
        else {
            inStream.putback(c);
            token.symbol('=');
        }
    } else if (c == '+' || c == '-' || c == '*' || c == '%')
        token.symbol(c);
    else if (c == '/') {
        inStream.get(c);
        if (c == '/')
            token.op("//");
        else {
            inStream.putback(c);
            token.symbol('/');
        }
    } else if (c == '>' || c == '<' || c == '!') {
        char c1 = c;
        inStream.get(c);
        if (c1 == '<' && c == '>')
            token.op("<>");
        else if (c == '=')
            token.op(std::string(1, c1) + '=');
        else {
            inStream.putback(c);
            token.symbol(c1);
        }
    } else if (c == ';')
        token.symbol(c);
    else if (c == '(' || c == ')')
        token.symbol(c);
    else if (c == '{' || c == '}')
        token.symbol(c);
    else if (c == '[' || c == ']')
        token.symbol(c);
    else if (c == '"')
        token.setString(readString(c));
    else if (c == '\'')
        token.setString(readString(c));
    else if (c == ',')
        token.symbol(c);
    else if (c == '.')
        token.symbol(c);
    else if (c == ':')
        token.symbol(c);
    else if (c == '#') {
        std::string cmnt = "#";
        while (inStream.get(c) && c != '\n')
            cmnt += c;
        token.setCmnt(cmnt);
        inStream.putback(c);
    } else if (isalpha(c)) {  // an identifier?
        // put c back into the stream so we can read the entire name in a function.
        inStream.putback(c);
        std::string name = readName();
        if (name == "or")
            token.op("or");
        else if (name == "and")
            token.op("and");
        else if (name == "not")
            token.op("not");
        else
            token.setName(name);
    } else {
        std::cout << "Unknown character in input. ->" << c << "<-" << std::endl;
        exit(1);
    }
    _tokens.push_back(token);
    return lastToken = token;
}

void Tokenizer::ungetToken() {
    ungottenToken = true;
}

void Tokenizer::printProcessedTokens() {
    for(auto iter = _tokens.begin(); iter != _tokens.end(); ++iter) {
        iter->print();
        std::cout << std::endl;
    }
}
