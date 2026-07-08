/*
 * File: statement.cpp
 * -------------------
 * Implementation of the Statement hierarchy.
 */

#include "statement.hpp"
#include <iostream>

Statement::Statement() = default;
Statement::~Statement() = default;

// REM
RemStatement::RemStatement(TokenScanner &scanner) {
    // Consume the rest of the line
    while (scanner.hasMoreTokens()) {
        scanner.nextToken();
    }
}
int RemStatement::execute(EvalState &state, Program &program) {
    return -2;
}

// LET
LetStatement::LetStatement(TokenScanner &scanner) {
    // The "LET" keyword has already been consumed by Basic.cpp
    // The tokens remaining are: <var> = <exp>
    varName = scanner.nextToken();
    if (varName == "LET") error("SYNTAX ERROR"); // Keyword as variable
    scanner.verifyToken("=");
    exp = parseExp(scanner);
}
int LetStatement::execute(EvalState &state, Program &program) {
    state.setValue(varName, exp->eval(state));
    return -2;
}
LetStatement::~LetStatement() {
    delete exp;
}

// PRINT
PrintStatement::PrintStatement(TokenScanner &scanner) {
    exp = parseExp(scanner);
}
int PrintStatement::execute(EvalState &state, Program &program) {
    std::cout << exp->eval(state) << std::endl;
    return -2;
}
PrintStatement::~PrintStatement() {
    delete exp;
}

// INPUT
InputStatement::InputStatement(TokenScanner &scanner) {
    varName = scanner.nextToken();
}
int InputStatement::execute(EvalState &state, Program &program) {
    int val;
    if (!(std::cin >> val)) {
        // Handle input failure if necessary
    }
    state.setValue(varName, val);
    return -2;
}

// END
EndStatement::EndStatement(TokenScanner &scanner) {
    if (scanner.hasMoreTokens()) error("END statement must not have extra tokens");
}
int EndStatement::execute(EvalState &state, Program &program) {
    return -1;
}

// GOTO
GotoStatement::GotoStatement(TokenScanner &scanner) {
    std::string token = scanner.nextToken();
    line = stringToInteger(token);
}
int GotoStatement::execute(EvalState &state, Program &program) {
    return line;
}

// IF
IfStatement::IfStatement(TokenScanner &scanner) {
    // IF <exp1> <op> <exp2> THEN <line>
    lhs = readE(scanner);
    op = scanner.nextToken();
    rhs = readE(scanner);
    scanner.verifyToken("THEN");
    std::string target = scanner.nextToken();
    targetLine = stringToInteger(target);
}
int IfStatement::execute(EvalState &state, Program &program) {
    int v1 = lhs->eval(state);
    int v2 = rhs->eval(state);
    bool condition = false;
    if (op == ">") condition = v1 > v2;
    else if (op == "<") condition = v1 < v2;
    else if (op == ">=") condition = v1 >= v2;
    else if (op == "<=") condition = v1 <= v2;
    else if (op == "==") condition = v1 == v2;
    else if (op == "!=") condition = v1 != v2;
    
    if (condition) return targetLine;
    return -2;
}
IfStatement::~IfStatement() {
    delete lhs;
    delete rhs;
}
