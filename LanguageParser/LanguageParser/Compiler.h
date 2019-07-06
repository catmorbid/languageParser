#pragma once
#include <sstream>
#include <string>
#include <vector>

#include "Expression.h"
#include "Scope.h"

using namespace std;

class Compiler {

    vector<Scope*> m_scopes;
    vector<string> m_errorStack;
    Scope m_rootScope = Scope(ROOT_SCOPE);
    Scope* m_currentScope;
    stringstream ss = stringstream();

    bool Evaluate(Expression* exp);

    bool HandleEndScopeCommand(Expression* exp);

    bool HandleAccessCommand(Expression* exp);

    bool HandleDeclareCommand(Expression* exp);

    bool HandleScopeCommand(Expression* exp);

    Scope* GetScope(Expression* exp)
    {
        return new Scope(exp->GetScope());
    }

public:

    Compiler();

    bool Compile(vector<Expression*> expressions);

    void printErrorStack();

};