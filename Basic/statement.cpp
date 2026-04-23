#include "statement.hpp"
#include "Utils/strlib.hpp"
#include <iostream>

Statement::Statement() = default;
Statement::~Statement() = default;

RemStatement::RemStatement(TokenScanner &scanner) {
    // consume rest tokens as comment
    while (scanner.hasMoreTokens()) scanner.nextToken();
}

void RemStatement::execute(EvalState &state, Program &program) { (void)state; (void)program; }

LetStatement::LetStatement(TokenScanner &scanner) {
    // Expect: IDENT = EXP
    std::string ident = scanner.nextToken();
    if (scanner.getTokenType(ident) != WORD) error("SYNTAX ERROR");
    std::string eq = scanner.nextToken();
    if (eq != "=") error("SYNTAX ERROR");
    // Build assignment expression: ident = <exp>
    exp = new CompoundExp("=", new IdentifierExp(ident), readE(scanner));
}

LetStatement::~LetStatement() { delete exp; }

void LetStatement::execute(EvalState &state, Program &program) { (void)program; exp->eval(state); }

PrintStatement::PrintStatement(TokenScanner &scanner) { exp = parseExp(scanner); }

PrintStatement::~PrintStatement() { delete exp; }

void PrintStatement::execute(EvalState &state, Program &program) {
    (void)program;
    std::cout << exp->eval(state) << std::endl;
}

InputStatement::InputStatement(TokenScanner &scanner) {
    var = scanner.nextToken();
    if (scanner.getTokenType(var) != WORD) error("SYNTAX ERROR");
    if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
}

void InputStatement::execute(EvalState &state, Program &program) {
    (void)program;
    std::string line;
    if (!std::getline(std::cin, line)) error("INVALID NUMBER");
    TokenScanner s;
    s.ignoreWhitespace();
    s.scanNumbers();
    s.setInput(line);
    std::string token = s.nextToken();
    if (token.empty() || s.getTokenType(token) != NUMBER || s.hasMoreTokens()) error("INVALID NUMBER");
    int val = stringToInteger(token);
    state.setValue(var, val);
}

EndStatement::EndStatement(TokenScanner &scanner) {
    if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
}

void EndStatement::execute(EvalState &state, Program &program) {
    (void)state; (void)program;
    throw ErrorException("END");
}

GotoStatement::GotoStatement(TokenScanner &scanner) {
    std::string t = scanner.nextToken();
    if (scanner.getTokenType(t) != NUMBER || scanner.hasMoreTokens()) error("SYNTAX ERROR");
    target = stringToInteger(t);
}

void GotoStatement::execute(EvalState &state, Program &program) {
    (void)program;
    int cur = target;
    if (cur <= 0) error("LINE NUMBER ERROR");
    state.setPC(cur);
}

IfStatement::IfStatement(TokenScanner &scanner) {
    lhs = readE(scanner);
    op = scanner.nextToken();
    if (op != "=" && op != ">" && op != "<") error("SYNTAX ERROR");
    rhs = readE(scanner);
    std::string thenTok = scanner.nextToken();
    if (thenTok != "THEN") error("SYNTAX ERROR");
    std::string t = scanner.nextToken();
    if (scanner.getTokenType(t) != NUMBER || scanner.hasMoreTokens()) error("SYNTAX ERROR");
    target = stringToInteger(t);
}

IfStatement::~IfStatement() { delete lhs; delete rhs; }

void IfStatement::execute(EvalState &state, Program &program) {
    (void)program;
    int lv = lhs->eval(state);
    int rv = rhs->eval(state);
    bool cond = false;
    if (op == "=") cond = (lv == rv);
    else if (op == ">") cond = (lv > rv);
    else if (op == "<") cond = (lv < rv);
    if (cond) state.setPC(target);
}
