#include <vector>
#include <iostream>

#include "Token.hpp"
#include "Parser.hpp"
#include "Statements.hpp"
#include "Range.hpp"

// Parser functions
void Parser::die(std::string where, std::string message, Token &token) {
    std::cout << where << " " << message << std::endl;
    token.print();
    std::cout << std::endl;
    std::cout << "\nThe following is a list of tokens that have been identified up to this point.\n";
    tokenizer.printProcessedTokens();
    exit(1);
}

Statements *Parser::statements() {
    // This function is called when we KNOW that we are about to parse
    // a series of statements.

    // This function parses the grammar rules:

    // file_input: {NEWLINE | stmt}* ENDMARKER
    // stmt: simple_stmt | compound_stmt
    // simple_stmt: (print_stmt | assign_stmt | array_ops | call_stmt | return_stmt) NEWLINE
    // compound_stmt: if_stmt | for_stmt | func_def

    Statements *stmts = new Statements();
    Token tok = tokenizer.getToken();
    while (tok.eol())
        tok = tokenizer.getToken();
    while (tok.isName() || tok.isCmnt()) {
        if (tok.isKeyword()) {
            if (tok.isKeywordPrint()) {
                PrintStatement *printStmt = printStatement();
                stmts->addStatement(printStmt);
            } else if (tok.isKeywordFor()) {
                ForStatement *forStmt = forStatement();
                stmts->addStatement(forStmt);
                tok = tokenizer.getToken();
                if (tok.isName())
                    continue;
                else
                    tokenizer.ungetToken();
            } else if (tok.isKeywordIf()) {
                IfStatement *ifStmt = ifStatement();
                stmts->addStatement(ifStmt);
            } else if (tok.isKeywordReturn()) {
                ReturnStatement *returnStmt = returnStatement();
                stmts->addStatement(returnStmt);
            } else if (tok.isKeywordDef()) {
                funcs->addFunction(funcDef());
            } else {
                die("Parser::statements", "Invalid keyword", tok);
            }
        } else if (tok.isName()) {
            Token next = tokenizer.getToken();
            tokenizer.ungetToken();
            if ( next.isAssignmentOperator() || next.isOpenBracket() ) {
                AssignmentStatement *assignStmt = assignStatement(tok);
                stmts->addStatement(assignStmt);
            } else if ( next.isOpenParen() ) {
                tokenizer.getToken();
                CallStatement *callStmt = callStatement(tok);
                stmts->addStatement(callStmt);
            } else if ( next.isPeriod() ) {
                tokenizer.getToken();
                ArrayOpStatement *arrayOpStmt = arrayOp(tok);
                stmts->addStatement(arrayOpStmt);
            }
        } else if (tok.isCmnt()) {
            tok = tokenizer.getToken();
            if (!tok.eol() && !tok.eof() && !tok.dedent())
                die("Parser::statements", "Expected an eol token, instead got", tok);
        }
        if (!tok.eof()) {
            tok = tokenizer.getToken();
            while (tok.eol())
                tok = tokenizer.getToken();
        }
    }
    while (tok.eol())
        tok = tokenizer.getToken();
    tokenizer.ungetToken();
    return stmts;
}

AssignmentStatement *Parser::assignStatement(Token varName) {
    // This function parses the grammar rules:

    // assign_stmt: (ID | subscription) '=' (test | array_init)
    // ID: [_a-zA-Z][_a-zA-Z0-9]*

    if (!varName.isName())
        die("Parser::assignStatement", "Expected a name token, instead got", varName);

    Token tok = tokenizer.getToken();
    if (!tok.isAssignmentOperator() && !tok.isOpenBracket())
        die("Parser::assignStatement", "Expected an equal sign or an open bracket, instead got", tok);
    ExprNode *leftHandSideExpr;
    if (tok.isOpenBracket()) {
        leftHandSideExpr = subscription(varName);
        tok = tokenizer.getToken();
        if (!tok.isAssignmentOperator())
            die("Parser::assignStatement", "Expected an equal sign, instead got", tok);
    }
    else {
        leftHandSideExpr = new Variable(varName);
    }

    ExprNode *rightHandSideExpr = nullptr;
    std::vector<ExprNode*> rightHandSideArr;
    tok = tokenizer.getToken();
    if (tok.isOpenBracket()) {
        rightHandSideArr = array_init();
    } else {
        tokenizer.ungetToken();
        rightHandSideExpr = test();
    }

    Token endLine = tokenizer.getToken();
    if (!endLine.eol() && !endLine.eof() && !endLine.isCmnt() && !endLine.dedent())
        die("Parser::assignStatement", "Expected an end of line token, instead got", endLine);
    if (endLine.eof())
        tokenizer.ungetToken();

    if (rightHandSideExpr == nullptr)
        return new AssignmentStatement(leftHandSideExpr, rightHandSideArr);
    else
        return new AssignmentStatement(leftHandSideExpr, rightHandSideExpr);
}

PrintStatement *Parser::printStatement() {
    // This function parses the grammar rule:

    // print_stmt: 'print' [ testlist ]

    Token t = tokenizer.getToken();
    if (t.eol() || t.eof() || t.isCmnt()) {
        std::vector<ExprNode*> empty;
        if (t.isCmnt())
            tokenizer.ungetToken();
        return new PrintStatement(empty);
    } else {
        tokenizer.ungetToken();
    }

    std::vector<ExprNode*> list = testlist();

    Token endLine = tokenizer.getToken();
    if (!endLine.eol() && !endLine.eof() && !endLine.isCmnt() && !endLine.dedent())
        die("Parser::printStatement", "Expected an end of line token, instead got", endLine);
    if (endLine.eof() || endLine.isCmnt() || endLine.dedent())
        tokenizer.ungetToken();

    return new PrintStatement(list);
}

ForStatement *Parser::forStatement() {
    // This function parses the grammar rule:

    // for_stmt: 'for' ID 'in' 'range' '(' testlist ')'  ':' suite

    Token loopVar = tokenizer.getToken();
    if (!loopVar.isName())
        die("Parser::forStatement", "Expected a name token, instead got", loopVar);

    Token t = tokenizer.getToken();
    if (!t.isKeywordIn())
        die("Parser::forStatement", "Expected the 'in' keyword, instead got", t);

    t = tokenizer.getToken();
    if (!t.isKeywordRange())
        die("Parser::forStatement", "Expected the 'range' keyword, instead got", t);

    t = tokenizer.getToken();
    if (!t.isOpenParen())
        die("Parser::forStatement", "Expected an open parenthesis, instead got", t);

    std::vector<ExprNode*> args = testlist();
    if (args.size() > 3) {
        Token empty;
        die("Parser::forStatement", "Expected a list of at most 3 integers, instead got more than 3 integers", empty);
    }

    t = tokenizer.getToken();
    if (!t.isCloseParen())
        die("Parser::forStatement", "Expected a close parenthesis, instead got", t);

    t = tokenizer.getToken();
    if (!t.isColon())
        die("Parser::forStatement", "Expected a colon, instead got", t);

    t = tokenizer.getToken();
    if (t.isCmnt())
        t = tokenizer.getToken();
    if (t.eol())
        tokenizer.ungetToken();

    Statements *body = suite();

    return new ForStatement(loopVar, args, body);
}

IfStatement *Parser::ifStatement() {
    // This function parses the grammar rule:

    // if_stmt: ’if’ test ’:’ suite {’elif’ test ’:’ suite}* [’else’ ’:’ suite]

    std::vector<ExprNode*> conditions;
    conditions.push_back(test());

    Token t = tokenizer.getToken();
    if (!t.isColon())
        die("Parser::ifStatement", "Expected a colon, instead got", t);

    std::vector<Statements*> bodies;
    bodies.push_back(suite());

    t = tokenizer.getToken();
    while (t.isKeywordElif()) {
        conditions.push_back(test());
        t = tokenizer.getToken();
        if (!t.isColon())
            die("Parser::ifStatement", "Expected a colon, instead got", t);
        bodies.push_back(suite());
        t = tokenizer.getToken();
    }

    if (t.isKeywordElse()) {
        t = tokenizer.getToken();
        if (!t.isColon())
            die("Parser::ifStatement", "Expected a colon, instead got", t);
        bodies.push_back(suite());
    }
    else
        tokenizer.ungetToken();

    return new IfStatement(conditions, bodies);
}

Function *Parser::funcDef() {
    // This function parses the grammar rule:

    // func_def: 'def' ID '(' [parameter_list] ')' ':' suite

    Token name = tokenizer.getToken();
    if (!name.isName())
        die("Parser::funcDef", "Expected a name token, instead got", name);
    Token tok = tokenizer.getToken();
    if (!tok.isOpenParen())
        die("Parser::funcDef", "Expected an open parenthesis token, instead got", tok);

    std::vector<std::string> args;
    tok = tokenizer.getToken();
    if (!tok.isCloseParen()) {
        tokenizer.ungetToken();
        args = parameter_list();
        tok = tokenizer.getToken();
        if (!tok.isCloseParen())
            die("Parser::funcDef", "Expected a close parenthesis token, instead got", tok);
    }

    tok = tokenizer.getToken();
    if (!tok.isColon())
        die("Parser::funcDef", "Expected a colon token, instead got", tok);

    Statements* body = suite();

    return new Function(name.getName(), args, body);
}

std::vector<std::string> Parser::parameter_list() {
    // This function parses the grammar rule:

    // parameter_list: ID {',' ID}*

    std::vector<std::string> list;
    Token tok = tokenizer.getToken();
    if (!tok.isName())
        die("Parser::parameter_list", "Expected a name token, instead got", tok);
    list.push_back(tok.getName());
    tok = tokenizer.getToken();
    while (tok.isComma()) {
        tok = tokenizer.getToken();
        if (!tok.isName())
            die("Parser::parameter_list", "Expected a name token, instead got", tok);
        list.push_back(tok.getName());
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return list;
}

CallStatement *Parser::callStatement(Token varName) {
    // This function parses the grammar rule:

    // call_stmt: call

    Function* func = funcs->findFunction(varName.getName());
    if (!func)
        die("Parser::callStatement", "No function exists with the name:", varName);
    Statements* body = func->body();
    std::vector<std::string> params = func->args();

    Token tok = tokenizer.getToken();
    std::vector<ExprNode*> args;
    if (!tok.isCloseParen()) {
        tokenizer.ungetToken();
        args = testlist();
        tok = tokenizer.getToken();
        if (!tok.isCloseParen())
            die("Parser::callStatement", "Expected a close parenthesis token, instead got", tok);
    }
    return new CallStatement(varName, params, args, body);
}

ExprNode *Parser::call(Token varName) {
    // This function parses the grammar rule:

    // call: ID '(' {testlist} ')'

    Function* func = funcs->findFunction(varName.getName());
    if (!func)
        die("Parser::call", "No function exists with the name:", varName);
    Statements* body = func->body();
    std::vector<std::string> params = func->args();

    Token tok = tokenizer.getToken();
    std::vector<ExprNode*> args;
    if (!tok.isCloseParen()) {
        tokenizer.ungetToken();
        args = testlist();
        tok = tokenizer.getToken();
        if (!tok.isCloseParen())
            die("Parser::call", "Expected a close parenthesis token, instead got", tok);
    }
    return new Call(varName, funcs, args);
}

ReturnStatement *Parser::returnStatement() {
    // This function parses the grammar rule:

    // return_stmt: 'return' [test]

    Token tok = tokenizer.getToken();
    if (tok.eof() || tok.eol() || tok.isCmnt()) {
        ExprNode *none = nullptr;
        return new ReturnStatement(none);
    } else {
        tokenizer.ungetToken();
        return new ReturnStatement(test());
    }
}

ArrayOpStatement *Parser::arrayOp(Token varName) {
    // This function parses the grammar rule:

    // array_ops: ID '.' ( 'append' | 'pop' ) '(' test ')'

    Token tok = tokenizer.getToken();
    if (tok.isKeywordAppend() || tok.isKeywordPush()) {
        tok = tokenizer.getToken();
        if (!tok.isOpenParen())
            die("Parser::arrayOp", "Expected an open parenthesis token, instead got", tok);
        ExprNode* arg = test();
        tok = tokenizer.getToken();
        if (!tok.isCloseParen())
            die("Parser::arrayOp", "Expected a close parenthesis token, instead got", tok);
        return new ArrayOpStatement(varName, arg);
    } else if (tok.isKeywordPop()) {
        tok = tokenizer.getToken();
        if (!tok.isOpenParen())
            die("Parser::arrayOp", "Expected an open parenthesis token, instead got", tok);
        ExprNode* arg = nullptr;
        tok = tokenizer.getToken();
        if (!tok.isCloseParen())
            die("Parser::arrayOp", "Expected a close parenthesis token, instead got", tok);
        return new ArrayOpStatement(varName, arg);
    } else {
        die("Parser::arrayOp", "Expected an 'append', 'push', or 'pop' keyword, instead got", tok);
    }
    return nullptr;
}

Subscription *Parser::subscription(Token varName) {
    // This function parses the grammar rule:

    // subscription: ID '[' test ']'

    ExprNode *subscript = test();

    Token tok = tokenizer.getToken();
    if (!tok.isCloseBracket())
        die("Parser::subscription", "Expected a close bracket token, instead got", tok);

    return new Subscription(varName, subscript);
}

std::vector<ExprNode*> Parser::array_init() {
    // This function parses the grammar rule:

    // array_init: '[' [ testlist ] ']'

    Token t = tokenizer.getToken();
    if (t.isCloseBracket()) {
        std::vector<ExprNode*> empty;
        return empty;
    } else {
        tokenizer.ungetToken();
    }

    std::vector<ExprNode*> list = testlist();

    t = tokenizer.getToken();
    if (!t.isCloseBracket())
        die("Parser::array_init", "Expected a close bracket token, instead got", t);

    return list;
}

Statements *Parser::suite() {
    // This function parses the grammar rule:

    // suite: NEWLINE INDENT stmt+ DEDENT

    Token t = tokenizer.getToken();
    if (t.isCmnt())
        t = tokenizer.getToken();
    if (!t.eol())
        die("Parser::suite", "Expected an end of line token, instead got", t);

    t = tokenizer.getToken();
    while (t.eol())
        t = tokenizer.getToken();
    if (!t.indent())
        die("Parser::suite", "Expected an indent token, instead got", t);

    Statements *stmts = statements();

    t = tokenizer.getToken();
    if (!t.dedent() && !t.eof())
        die("Parser::suite", "Expected a dedent token, instead got", t);

    return stmts;
}

std::vector<ExprNode*> Parser::testlist() {
    // This function parses the grammar rule:

    // testlist: test {',' test}*

    std::vector<ExprNode*> list;
    list.push_back(test());
    Token tok = tokenizer.getToken();
    while (tok.isComma()) {
        list.push_back(test());
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return list;
}

ExprNode* Parser::array_len() {
    // This function parses the grammar rule:

    // array_len: 'len' '(' ID ')'

    Token tok = tokenizer.getToken();
    if (!tok.isOpenParen()) {
        die("Parser::array_len", "Expected an open parenthesis token, instead got", tok);
    }
    tok = tokenizer.getToken();
    Len* len = new Len(tok);
    tok = tokenizer.getToken();
    if (!tok.isCloseParen()) {
        die("Parser::array_len", "Expected a close parenthesis token, instead got", tok);
    }
    return len;
}


ExprNode *Parser::test() {
    // This function parses the grammar rule:

    // test: or_test

    return or_test();
}

ExprNode *Parser::or_test() {
    // This function parses the grammar rule:

    // or_test: and_test {’or’ and_test}*

    ExprNode *left = and_test();
    Token tok = tokenizer.getToken();
    while (tok.isOrOperator()) {
        BoolExprNode *p = new BoolExprNode(tok);
        p->left() = left;
        p->right() = and_test();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::and_test() {
    // This function parses the grammar rule:

    // and_test: not_test {’and’ not_test}*

    ExprNode *left = not_test();
    Token tok = tokenizer.getToken();
    while (tok.isAndOperator()) {
        BoolExprNode *p = new BoolExprNode(tok);
        p->left() = left;
        p->right() = not_test();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::not_test() {
    // This function parses the grammar rule:

    // and_test: 'not' not_test | comparison

    int notCount = 0;
    Token tok = tokenizer.getToken();
    while(tok.isNotOperator()) {
        notCount++;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    if (notCount % 2 == 1) {
        Token n;
        n.op("not");
        NotExprNode *p = new NotExprNode(n);
        p->right() = comparison();
        return p;
    }
    else
        return comparison();
}

ExprNode *Parser::comparison() {
    // This function parses the grammar rules:

    // comparison: arith_expr {comp_op arith_expr}*
    // comp_op: '<'|'>'|'=='|'>='|'<='|'<>'|'!='

    ExprNode *left = arithExpr();
    Token tok = tokenizer.getToken();
    while (tok.isRelationalOperator()) {
        RelExprNode *p = new RelExprNode(tok);
        p->left() = left;
        p->right() = arithExpr();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::arithExpr() {
    // This function parses the grammar rule:

    // arith_expr: term {('+'|'-') term}*

    // However, the implementation makes the operator left associative.

    ExprNode *left = term();
    Token tok = tokenizer.getToken();
    while (tok.isAdditionOperator() || tok.isSubtractionOperator()) {
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = term();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}


ExprNode *Parser::term() {
    // This function parses the grammar rule:

    // term: factor {('*'|'/'|'%'|'//') factor}*

    // However, the implementation makes the operator left associate.

    ExprNode *left = factor();
    Token tok = tokenizer.getToken();

    while (tok.isMultiplicationOperator() || tok.isDivisionOperator() || tok.isModuloOperator() || tok.isIntDivisionOperator()) {
        InfixExprNode *p = new InfixExprNode(tok);
        p->left() = left;
        p->right() = factor();
        left = p;
        tok = tokenizer.getToken();
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::factor() {
    // This function parses the grammar rule:

    // factor: {'-'} factor | atom | call | subscription | array_len

    Token tok = tokenizer.getToken();

    int numHyphens = 0;

    while (tok.isSubtractionOperator()) {
        numHyphens++;
        tok = tokenizer.getToken();
    }
    if (tok.isKeywordLen()) {
        if (numHyphens % 2 == 1) {
            Token mult = Token();
            mult.symbol('*');
            InfixExprNode *negVar = new InfixExprNode(mult);
            Token num = Token();
            num.setWholeNumber(-1);
            negVar->left() = new WholeNumber(num);
            negVar->right() = array_len();
            return negVar;
        }
        return array_len();
    }
    Token prev = tok;
    tok = tokenizer.getToken();
    if (tok.isOpenBracket()) {
        if (numHyphens % 2 == 1) {
            Token mult = Token();
            mult.symbol('*');
            InfixExprNode *negVar = new InfixExprNode(mult);
            Token num = Token();
            num.setWholeNumber(-1);
            negVar->left() = new WholeNumber(num);
            negVar->right() = subscription(prev);
            return negVar;
        }
        return subscription(prev);
    }
    if (tok.isOpenParen()) {
        if (numHyphens % 2 == 1) {
            Token mult = Token();
            mult.symbol('*');
            InfixExprNode *negVar = new InfixExprNode(mult);
            Token num = Token();
            num.setWholeNumber(-1);
            negVar->left() = new WholeNumber(num);
            negVar->right() = call(prev);
            return negVar;
        }
        return call(prev);
    }
    tokenizer.ungetToken();
    return atom(prev, numHyphens);
}


ExprNode *Parser::atom(Token prev, int numHyphens) {
    // This function parses the grammar rules:

    // atom: ID | NUMBER | STRING+ | '(' test ')'
    // ID: [_a-zA-Z][_a-zA-Z0-9]*
    // NUMBER: [0-9]+
    // STRING: [_a-zA-Z0-9]*

    Token tok = prev;

    if ( tok.isWholeNumber() ) {
        if (numHyphens % 2 == 1)
            tok.setWholeNumber(-(tok.getWholeNumber()));
        return new WholeNumber(tok);
    } else if( tok.isName() && !tok.isKeyword() ) {
        if (numHyphens % 2 == 1) {
            Token mult = Token();
            mult.symbol('*');
            InfixExprNode *negVar = new InfixExprNode(mult);
            Token num = Token();
            num.setWholeNumber(-1);
            negVar->left() = new WholeNumber(num);
            negVar->right() = new Variable(tok);
            return negVar;
        }
        return new Variable(tok);
    } else if( tok.isDouble() ) {
        if (numHyphens % 2 == 1)
            tok.setDouble(-(tok.getDouble()));
        return new Double(tok);
    } else if( tok.isString() ) {
        if (numHyphens > 0)
            die("Parser::atom", "Bad operand type for unary -: 'str':", tok);
        return new String(tok);
    } else if ( tok.isOpenParen() ) {
        if (numHyphens % 2 == 1) {
            Token mult = Token();
            mult.symbol('*');
            InfixExprNode *negExpr = new InfixExprNode(mult);
            Token num = Token();
            num.setWholeNumber(-1);
            negExpr->left() = new WholeNumber(num);
            negExpr->right() = test();
            tok = tokenizer.getToken();
            if ( !tok.isCloseParen() )
                die ("Parser::atom", "Expected a close parenthesis, instead got", tok);
            return negExpr;
        }
        ExprNode *expr = test();

        tok = tokenizer.getToken();
        if ( !tok.isCloseParen() )
            die ("Parser::atom", "Expected a close parenthesis, instead got", tok);
        return expr;
    }

    die("Parser::atom", "Expected a whole number, name, double, string, or parenthesis, instead got", tok);

    return nullptr; // Will never reach here
}
