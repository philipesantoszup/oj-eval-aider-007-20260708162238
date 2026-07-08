/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type and its subclasses.
 */

#ifndef _statement_h
#define _statement_h

#include <string>
#include <sstream>
#include "evalstate.hpp"
#include "exp.hpp"
#include "Utils/tokenScanner.hpp"
#include "program.hpp"
#include "parser.hpp"
#include "Utils/error.hpp"
#include "Utils/strlib.hpp"

class Program;

class Statement {
public:
    Statement();
    virtual ~Statement();
    // Returns: -1 for END, -2 for next line, or a positive integer for GOTO
    virtual int execute(EvalState &state, Program &program) = 0;
};

class RemStatement : public Statement {
public:
    RemStatement(TokenScanner &scanner);
    virtual int execute(EvalState &state, Program &program) override;
};

class LetStatement : public Statement {
public:
    LetStatement(TokenScanner &scanner);
    virtual int execute(EvalState &state, Program &program) override;
    ~LetStatement();
private:
    std::string varName;
    Expression *exp;
};

class PrintStatement : public Statement {
public:
    PrintStatement(TokenScanner &scanner);
    virtual int execute(EvalState &state, Program &program) override;
    ~PrintStatement();
private:
    Expression *exp;
};

class InputStatement : public Statement {
public:
    InputStatement(TokenScanner &scanner);
    virtual int execute(EvalState &state, Program &program) override;
private:
    std::string varName;
};

class EndStatement : public Statement {
public:
    EndStatement(TokenScanner &scanner);
    virtual int execute(EvalState &state, Program &program) override;
};

class GotoStatement : public Statement {
public:
    GotoStatement(TokenScanner &scanner);
    virtual int execute(EvalState &state, Program &program) override;
private:
    int line;
};

class IfStatement : public Statement {
public:
    IfStatement(TokenScanner &scanner);
    virtual int execute(EvalState &state, Program &program) override;
    ~IfStatement();
private:
    Expression *lhs;
    std::string op;
    Expression *rhs;
    int targetLine;
};

#endif
