/*
 * File: Basic.cpp
 * ---------------
 * This file is the main entry point for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include <algorithm>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "statement.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"

using namespace std;

void processLine(string line, Program &program, EvalState &state);

int main() {
    EvalState state;
    Program program;
    while (true) {
        try {
            string input;
            if (!getline(cin, input)) break;
            if (input.empty()) continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            cout << ex.getMessage() << endl;
        }
    }
    return 0;
}

void processLine(string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    if (!scanner.hasMoreTokens()) return;

    string firstToken = scanner.nextToken();
    if (isdigit(firstToken[0])) {
        // Program Mode: Numbered Line
        int lineNum = stringToInteger(firstToken);
        
        // To check if it's a deletion, we see if there are more tokens
        // We use a fresh scanner for the remaining part of the line
        string remaining = line.substr(firstToken.length());
        TokenScanner remScanner;
        remScanner.ignoreWhitespace();
        remScanner.scanNumbers();
        remScanner.setInput(remaining);
        
        if (!remScanner.hasMoreTokens()) {
            program.removeSourceLine(lineNum);
        } else {
            program.addSourceLine(lineNum, line);
            
            // Parse the statement
            string cmd = remScanner.nextToken();
            string upperCmd = toUpperCase(cmd);
            Statement *stmt = nullptr;
            try {
                if (upperCmd == "LET") stmt = new LetStatement(remScanner);
                else if (upperCmd == "PRINT") stmt = new PrintStatement(remScanner);
                else if (upperCmd == "INPUT") stmt = new InputStatement(remScanner);
                else if (upperCmd == "REM") stmt = new RemStatement(remScanner);
                else if (upperCmd == "GOTO") stmt = new GotoStatement(remScanner);
                else if (upperCmd == "IF") stmt = new IfStatement(remScanner);
                else if (upperCmd == "END") stmt = new EndStatement(remScanner);
                else error("Unknown statement type: " + upperCmd);
                
                program.setParsedStatement(lineNum, stmt);
            } catch (...) {
                delete stmt;
                throw;
            }
        }
    } else {
        // Immediate Mode: Command
        string upperCmd = toUpperCase(firstToken);
        if (upperCmd == "PRINT") {
            TokenScanner s; s.ignoreWhitespace(); s.scanNumbers(); s.setInput(line.substr(firstToken.length()));
            Expression *e = parseExp(s);
            cout << e->eval(state) << endl;
            delete e;
        } else if (upperCmd == "LET") {
            TokenScanner s; s.ignoreWhitespace(); s.scanNumbers(); s.setInput(line.substr(firstToken.length()));
            string var = s.nextToken();
            s.verifyToken("=");
            Expression *e = parseExp(s);
            state.setValue(var, e->eval(state));
            delete e;
        } else if (upperCmd == "RUN") {
            int pc = program.getFirstLineNumber();
            while (pc != -1) {
                Statement *stmt = program.getParsedStatement(pc);
                if (!stmt) {
                    pc = program.getNextLineNumber(pc);
                    continue;
                }
                int next = stmt->execute(state, program);
                if (next == -1) break;
                if (next == -2) pc = program.getNextLineNumber(pc);
                else pc = next;
            }
        } else if (upperCmd == "LIST") {
            int pc = program.getFirstLineNumber();
            while (pc != -1) {
                cout << program.getSourceLine(pc) << endl;
                pc = program.getNextLineNumber(pc);
            }
        } else if (upperCmd == "CLEAR") {
            program.clear();
        } else if (upperCmd == "QUIT") {
            exit(0);
        } else if (upperCmd == "REM") {
            // Ignore
        } else {
            error("Unknown command: " + upperCmd);
        }
    }
}
