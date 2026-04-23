#include "program.hpp"
#include "Utils/error.hpp"

Program::Program() = default;

Program::~Program() { clear(); }

void Program::clear() {
    for (auto &p : parsed) {
        delete p.second;
    }
    parsed.clear();
    sourceLines.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    if (parsed.count(lineNumber)) {
        delete parsed[lineNumber];
        parsed.erase(lineNumber);
    }
    sourceLines[lineNumber] = line;
}

void Program::removeSourceLine(int lineNumber) {
    if (parsed.count(lineNumber)) {
        delete parsed[lineNumber];
        parsed.erase(lineNumber);
    }
    sourceLines.erase(lineNumber);
}

std::string Program::getSourceLine(int lineNumber) {
    auto it = sourceLines.find(lineNumber);
    if (it == sourceLines.end()) return "";
    return it->second;
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    if (!sourceLines.count(lineNumber)) error("LINE NUMBER ERROR");
    if (parsed.count(lineNumber)) delete parsed[lineNumber];
    parsed[lineNumber] = stmt;
}

Statement *Program::getParsedStatement(int lineNumber) {
    auto it = parsed.find(lineNumber);
    if (it == parsed.end()) return nullptr;
    return it->second;
}

int Program::getFirstLineNumber() {
    if (sourceLines.empty()) return -1;
    return sourceLines.begin()->first;
}

int Program::getNextLineNumber(int lineNumber) {
    auto it = sourceLines.upper_bound(lineNumber);
    if (it == sourceLines.end()) return -1;
    return it->first;
}
