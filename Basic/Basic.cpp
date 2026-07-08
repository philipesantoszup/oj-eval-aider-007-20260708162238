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
        string remaining = line.substr(firstToken.length());
        
        // If the line only contains the number, it's a deletion
        TokenScanner remScanner;
        remScanner.ignoreWhitespace();
        remScanner.setInput(remaining);
        if (!remScanner.hasMoreTokens()) {
            program.removeSourceLine(lineNum);
        } else {
            program.addSourceLine(lineNum, line);
            // Parse the statement
            string cmd = remScanner.nextToken();
            string upperCmd = toUpperCase(cmd);
            Statement *stmt = nullptr;
            if (upperCmd == "LET") stmt = new LetStatement(remScanner);
            else if (upperCmd == "PRINT") stmt = new PrintStatement(remScanner);
            else if (upperCmd == "INPUT") stmt = new InputStatement(remScanner);
            else if (upperCmd == "REM") stmt = new RemStatement(remScanner);
            else if (upperCmd == "GOTO") stmt = new GotoStatement(remScanner);
            else if (upperCmd == "IF") stmt = new IfStatement(remScanner);
            else if (upperCmd == "END") stmt = new EndStatement(remScanner);
            else error("Unknown statement type: " + upperCmd);
            
            program.setParsedStatement(lineNum, stmt);
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
                if (!stmt) error("Line " + to_string(pc) + " has no parsed statement");
                int next = stmt->execute(state, program);
                if (next == -1) break;
                if (next == -2) pc = program.getNextLineNumber(pc);
                else pc = next;
                if (pc != -1 && !program.getParsedStatement(pc)) {
                    // If the line exists but isn't parsed, we should probably error or skip
                    // Based on spec, we assume all lines in RUN are valid.
                }
            }
        } else if (upperCmd == "LIST") {
            // This requires a way to iterate through the program. 
            // I'll add a helper or just use the map.
            // Since I can't easily add methods to Program without updating .hpp, 
            // I'll assume I can use the existing ones or I'll update the .hpp.
            // Actually, I'll just implement a simple loop using getFirst and getNext.
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
