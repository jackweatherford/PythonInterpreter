#include <iostream>
#include "Token.hpp"

Token::Token(): _name{""}, _eof{false}, _eol{false}, _symbol{'\0'}, _op{""}, _isWholeNumber{false}, _isDouble{false},
                _isString{false}, _isCmnt{false}, _str{""}, _cmnt{""}, _indent{false}, _dedent{false} {}

void Token::print() const {
    if( eol() ) std::cout << "EOL\n" ;
    else if( eof() )  std::cout << "EOF" ;
    else if( isOpenParen() )  std::cout << "(" ;
    else if( isCloseParen() )  std::cout << ")" ;
    else if( isOpenBrace() )  std::cout << "{" ;
    else if( isCloseBrace() )  std::cout << "}" ;
    else if( isOpenBracket() )  std::cout << "[" ;
    else if( isCloseBracket() )  std::cout << "]" ;
    else if( isAssignmentOperator() )  std::cout << " = " ;
    else if( isSemiColon() )  std::cout << "; " ;
    else if( isMultiplicationOperator() )  std::cout << " * " ;
    else if( isAdditionOperator() )  std::cout << " + " ;
    else if( isSubtractionOperator() )  std::cout << " - " ;
    else if( isModuloOperator() )  std::cout << " % " ;
    else if( isDivisionOperator() )  std::cout << " / " ;
    else if( isIntDivisionOperator() )  std::cout << " // ";
    else if( isGreaterThanOperator() )  std::cout << " > ";
    else if( isGreaterThanOrEqualToOperator() )  std::cout << " >= ";
    else if( isLessThanOperator() )  std::cout << " < ";
    else if( isLessThanOrEqualToOperator() )  std::cout << " <= ";
    else if( isEqualityOperator() )  std::cout << " == ";
    else if( isInequalityOperator() )  std::cout << " != ";
    else if( isAltInequalityOperator() )  std::cout << " <> ";
    else if( isOrOperator() )  std::cout << " or ";
    else if( isAndOperator() )  std::cout << " and ";
    else if( isNotOperator() )  std::cout << " not ";
    else if( isComma() ) std::cout << ", ";
    else if( isPeriod() ) std::cout << ".";
    else if( isColon() ) std::cout << ": ";
    else if( isKeywordIn() ) std::cout << " " << getName() << " ";
    else if( isKeywordRange() || isKeywordElse() ||
             isKeywordPush() || isKeywordAppend() || isKeywordPop() ) std::cout << getName();
    else if( isKeyword() )  std::cout << getName() << " ";
    else if( isName() )  std::cout << getName();
    else if( isWholeNumber() ) std::cout << getWholeNumber();
    else if( isDouble() ) std::cout << getDouble();
    else if( isString() ) std::cout << '"' << getString() << '"';
    else if( isCmnt() ) std::cout << getCmnt();
    else if( indent() ) std::cout << "INDENT ";
    else if( dedent() ) std::cout << "DEDENT ";
    else std::cout << "Uninitialized token.\n";
}
