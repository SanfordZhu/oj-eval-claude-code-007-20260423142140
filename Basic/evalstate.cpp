#include "evalstate.hpp"

EvalState::EvalState() {}
EvalState::~EvalState() {}

void EvalState::setValue(std::string var, int value) {
    symbolTable[var] = value;
}

int EvalState::getValue(std::string var) {
    return symbolTable[var];
}

bool EvalState::isDefined(std::string var) {
    return symbolTable.find(var) != symbolTable.end();
}

void EvalState::Clear() {
    symbolTable.clear();
    pcSet = false;
    pcValue = -1;
}

void EvalState::setPC(int pc) { pcSet = true; pcValue = pc; }
bool EvalState::hasPC() const { return pcSet; }
int EvalState::getPC() const { return pcValue; }
void EvalState::clearPC() { pcSet = false; pcValue = -1; }
