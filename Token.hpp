#ifndef EXPRINTER_TOKEN_HPP
#define EXPRINTER_TOKEN_HPP

#include <string>

class Token {

public:
    Token();

    bool &eof()  { return _eof; }
    bool &eol()  { return _eol; }

    bool eof() const { return _eof; }
    bool eol() const  { return _eol; }

    bool isOpenParen() const  { return _symbol == '('; }
    bool isCloseParen() const { return _symbol == ')'; }

    bool isOpenBrace() const  { return _symbol == '{'; }
    bool isCloseBrace() const { return _symbol == '}'; }

    bool isOpenBracket() const { return _symbol == '['; }
    bool isCloseBracket() const { return _symbol == ']'; }

    void symbol(char c) { _symbol = c; }
    char symbol() { return _symbol; }

    void op(std::string s) { _op = s; }
    std::string op() { return _op; }

    bool isSemiColon() const              { return _symbol == ';'; }
    bool isAssignmentOperator() const     { return _symbol == '='; }
    bool isMultiplicationOperator() const { return _symbol == '*'; }
    bool isAdditionOperator() const       { return _symbol == '+'; }
    bool isSubtractionOperator() const    { return _symbol == '-'; }
    bool isModuloOperator() const         { return _symbol == '%'; }
    bool isDivisionOperator() const       { return _symbol == '/'; }
    bool isIntDivisionOperator() const    { return _op == "//"; }
    bool isArithmeticOperator() const {
        return isMultiplicationOperator() ||
               isAdditionOperator() ||
               isSubtractionOperator() ||
               isModuloOperator() ||
               isDivisionOperator();
    }
    bool isGreaterThanOperator() const          { return _symbol == '>'; }
    bool isGreaterThanOrEqualToOperator() const { return _op == ">="; }
    bool isLessThanOperator() const             { return _symbol == '<'; }
    bool isLessThanOrEqualToOperator() const    { return _op == "<="; }
    bool isEqualityOperator() const             { return _op == "=="; }
    bool isInequalityOperator() const           { return _op == "!="; }
    bool isAltInequalityOperator() const        { return _op == "<>";}
    bool isRelationalOperator() const {
        return isGreaterThanOperator() ||
               isGreaterThanOrEqualToOperator() ||
               isLessThanOperator() ||
               isLessThanOrEqualToOperator() ||
               isEqualityOperator() ||
               isInequalityOperator() ||
               isAltInequalityOperator();
    }

    bool isOrOperator() const             { return _op == "or"; }
    bool isAndOperator() const            { return _op == "and"; }
    bool isNotOperator() const            { return _op == "not"; }
    bool isBooleanOperator() const {
        return isOrOperator() ||
               isAndOperator() ||
               isNotOperator();
    }

    bool isComma() const                  { return _symbol == ','; }

    bool isPeriod() const                 { return _symbol == '.'; }

    bool isColon() const                  { return _symbol == ':'; }

    bool isName() const                   { return _name.length() > 0; }
    std::string getName() const                  { return _name; }
    void setName(std::string n) { _name = n; }

    bool isKeywordPrint() const  { return _name == "print"; }
    bool isKeywordFor() const    { return _name == "for"; }
    bool isKeywordIn() const     { return _name == "in"; }
    bool isKeywordRange() const  { return _name == "range"; }
    bool isKeywordIf() const     { return _name == "if"; }
    bool isKeywordElse() const   { return _name == "else"; }
    bool isKeywordElif() const   { return _name == "elif"; }
    bool isKeywordReturn() const { return _name == "return"; }
    bool isKeywordDef() const    { return _name == "def"; }
    bool isKeywordAppend() const { return _name == "append"; }
    bool isKeywordPush() const   { return _name == "push"; }
    bool isKeywordPop() const    { return _name == "pop"; }
    bool isKeywordLen() const    { return _name == "len"; }
    bool isKeyword() const {
        return isKeywordPrint() ||
               isKeywordFor() ||
               isKeywordIn() ||
               isKeywordRange() ||
               isKeywordIf() ||
               isKeywordElse() ||
               isKeywordElif() ||
               isKeywordReturn() ||
               isKeywordDef() ||
               isKeywordAppend() ||
               isKeywordPush() ||
               isKeywordPop() ||
               isKeywordLen();
    }

    bool &isWholeNumber() { return _isWholeNumber; }
    bool isWholeNumber() const { return _isWholeNumber; }
    int getWholeNumber() const { return _wholeNumber; }
    void setWholeNumber(int n) {
        _wholeNumber = n;
        isWholeNumber() = true;
    }

    bool &isDouble() { return _isDouble; }
    bool isDouble() const { return _isDouble; }
    double getDouble() const { return _double; }
    void setDouble(double f) {
        _double = f;
        isDouble() = true;
    }

    bool &isString() { return _isString; }
    bool isString() const { return _isString; }
    std::string getString() const { return _str; }
    void setString(std::string s) {
        _str = s;
        isString() = true;
    }

    bool &isCmnt() { return _isCmnt; }
    bool isCmnt() const { return _isCmnt; }
    std::string getCmnt() const { return _cmnt; }
    void setCmnt(std::string s) {
        _cmnt = s;
        isCmnt() = true;
    }

    bool &indent()  { return _indent; }
    bool indent() const { return _indent; }

    bool &dedent()  { return _dedent; }
    bool dedent() const { return _dedent; }

    void print() const;

private:
    std::string _name;
    bool _eof, _eol;
    bool _isWholeNumber;
    bool _isDouble;
    bool _isString;
    bool _isCmnt;
    char _symbol;
    std::string _op;
    int _wholeNumber;
    double _double;
    std::string _str;
    std::string _cmnt;
    bool _indent;
    bool _dedent;
};

#endif //EXPRINTER_TOKEN_HPP
