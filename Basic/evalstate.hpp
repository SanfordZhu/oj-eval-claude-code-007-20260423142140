/*
 * File: evalstate.h
 * -----------------
 * This interface exports a class called EvalState, which keeps track
 * of additional information required by the evaluator, most notably
 * the values of variables.
 */

#ifndef _evalstate_h
#define _evalstate_h

#include <string>
#include <map>

class EvalState {
public:
    EvalState();
    ~EvalState();

    void setValue(std::string var, int value);
    int getValue(std::string var);
    bool isDefined(std::string var);
    void Clear();

    // Program counter helpers for control flow
    void setPC(int pc);
    bool hasPC() const;
    int getPC() const;
    void clearPC();

private:
    std::map<std::string, int> symbolTable;
    bool pcSet = false;
    int pcValue = -1;
};

#endif
