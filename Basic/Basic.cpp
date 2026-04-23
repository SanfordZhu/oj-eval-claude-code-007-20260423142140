/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            if (!std::getline(std::cin, input)) break;
            if (input.empty()) continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    std::string first = scanner.nextToken();
    if (first.empty()) return;
    TokenType tt = scanner.getTokenType(first);

    if (tt == NUMBER) {
        // program line
        int lineNumber = stringToInteger(first);
        std::string rest = line.substr(line.find(first) + first.size());
        if (rest.size() == 0 || (rest.size() == 1 && rest[0] == ' ')) {
            program.removeSourceLine(lineNumber);
            return;
        }
        program.addSourceLine(lineNumber, line);
        // parse statement for this line
        TokenScanner s;
        s.ignoreWhitespace();
        s.scanNumbers();
        s.setInput(trim(rest));
        std::string cmd = s.nextToken();
        Statement *stmt = nullptr;
        if (cmd == "REM") stmt = new RemStatement(s);
        else if (cmd == "LET") stmt = new LetStatement(s);
        else if (cmd == "PRINT") stmt = new PrintStatement(s);
        else if (cmd == "INPUT") stmt = new InputStatement(s);
        else if (cmd == "END") stmt = new EndStatement(s);
        else if (cmd == "GOTO") stmt = new GotoStatement(s);
        else if (cmd == "IF") stmt = new IfStatement(s);
        else error("SYNTAX ERROR");
        program.setParsedStatement(lineNumber, stmt);
        return;
    }

    // Immediate commands
    if (first == "LIST") {
        int ln = program.getFirstLineNumber();
        while (ln != -1) {
            std::cout << program.getSourceLine(ln) << std::endl;
            ln = program.getNextLineNumber(ln);
        }
        return;
    } else if (first == "CLEAR") {
        program.clear();
        state.Clear();
        return;
    } else if (first == "QUIT") {
        exit(0);
    } else if (first == "HELP") {
        return;
    } else if (first == "RUN") {
        int pc = program.getFirstLineNumber();
        while (pc != -1) {
            Statement *stmt = program.getParsedStatement(pc);
            if (!stmt) error("SYNTAX ERROR");
            try {
                stmt->execute(state, program);
            } catch (ErrorException &ex) {
                if (ex.getMessage() == std::string("END")) return;
                throw;
            }
            if (state.hasPC()) {
                pc = state.getPC();
                if (pc != -1 && program.getSourceLine(pc).empty()) error("LINE NUMBER ERROR");
                state.clearPC();
                if (pc != -1 && program.getParsedStatement(pc) == nullptr) error("LINE NUMBER ERROR");
            } else {
                pc = program.getNextLineNumber(pc);
            }
        }
        return;
    }

    // Direct executable statements
    Statement *stmt = nullptr;
    if (first == "REM") stmt = new RemStatement(scanner);
    else if (first == "LET") stmt = new LetStatement(scanner);
    else if (first == "PRINT") stmt = new PrintStatement(scanner);
    else if (first == "INPUT") stmt = new InputStatement(scanner);
    else if (first == "END") stmt = new EndStatement(scanner);
    else if (first == "GOTO") stmt = new GotoStatement(scanner);
    else if (first == "IF") stmt = new IfStatement(scanner);
    else error("SYNTAX ERROR");

    if (stmt) {
        try {
            stmt->execute(state, program);
        } catch (ErrorException &ex) {
            if (ex.getMessage() == std::string("END")) return;
            delete stmt;
            throw;
        }
        delete stmt;
    }
}

