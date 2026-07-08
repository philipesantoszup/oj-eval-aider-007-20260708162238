/*
 * File: program.cpp
 * -----------------
 * This file implements the Program class for storing a BASIC
 * program.
 */

#include "program.hpp"
#include "Utils/error.hpp"

Program::Program() = default;

Program::~Program() {
    clear();
}

void Program::clear() {
    for (auto const& [line, pair] : lines) {
        delete pair.second;
    }
    lines.clear();
}

void Program::addSourceLine(int lineNumber, const std::string& line) {
    if (lines.count(lineNumber)) {
        delete lines[lineNumber].second;
    }
    lines[lineNumber] = {line, nullptr};
}

void Program::removeSourceLine(int lineNumber) {
    if (lines.count(lineNumber)) {
        delete lines[lineNumber].second;
        lines.erase(lineNumber);
    }
}

std::string Program::getSourceLine(int lineNumber) {
    if (lines.count(lineNumber)) {
        return lines[lineNumber].first;
    }
    return "";
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    if (!lines.count(lineNumber)) {
        error("setParsedStatement: Line number " + std::to_string(lineNumber) + " does not exist");
    }
    if (lines[lineNumber].second != nullptr) {
        delete lines[lineNumber].second;
    }
    lines[lineNumber].second = stmt;
}

Statement *Program::getParsedStatement(int lineNumber) {
    if (lines.count(lineNumber)) {
        return lines[lineNumber].second;
    }
    return nullptr;
}

int Program::getFirstLineNumber() {
    if (lines.empty()) return -1;
    return lines.begin()->first;
}

int Program::getNextLineNumber(int lineNumber) {
    auto it = lines.upper_bound(lineNumber);
    if (it == lines.end()) return -1;
    return it->first;
}
