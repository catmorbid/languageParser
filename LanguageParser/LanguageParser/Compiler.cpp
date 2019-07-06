#include "Compiler.h"

using namespace std;

bool Compiler::Evaluate(Expression* exp)
{
    ss.clear();
    // handle pre-compilation errors
    if (exp->error) {
        m_errorStack.push_back(exp->errorMsg);
        return false;
    }
    string scop;
    bool fail = false;
    //handle commands
    switch (exp->GetCommand())
    {
    case ACCESS:
        return HandleAccessCommand(exp);
    case DECLARE:
        return HandleDeclareCommand(exp);
    case SCOPE:
        return HandleScopeCommand(exp);
    case ENDSCOPE:
        return HandleEndScopeCommand(exp);


    }
    ss << "Unhandled COMMAND: " << exp->GetCommand() << " at line: " << exp->GetNumLine() << endl;
    m_errorStack.push_back(ss.str());
    return false;
}


//handle commands

bool Compiler::HandleEndScopeCommand(Expression* exp)
{
    m_currentScope = &m_rootScope;
    return true;
}

bool Compiler::HandleAccessCommand(Expression* exp)
{
    string scop = exp->GetScope();
    if (scop != ROOT_SCOPE && !m_currentScope->Is(scop))
    {
        for (Scope* s : m_scopes)
        {
            if (s->Is(scop) && s->IsDeclared(exp->GetVariable()))
                return true;
        }
        ss << "Illegal scope: " << scop << " at " << exp->GetNumLine() << endl;
        m_errorStack.push_back(ss.str());
        return false;
    }
    else if (m_currentScope->IsDeclared(exp->GetVariable()))
    {
        return true;
    }

    ss << "Variable " << exp->GetVariable() << " not declared at line: " << exp->GetNumLine() << endl;
    m_errorStack.push_back(ss.str());
    return false;
}

bool Compiler::HandleDeclareCommand(Expression* exp) {
    m_currentScope->AddVariable(exp->GetVariable());
    return true;
}

bool Compiler::HandleScopeCommand(Expression* exp)
{
    Scope* newScope = new Scope(exp->GetVariable());
    m_currentScope = newScope;
    m_scopes.push_back(m_currentScope);
    for (Expression* e : exp->Children()) {
        if (!Evaluate(e))
            return false;
    }
    return true;
}

//public

Compiler::Compiler() {
    m_currentScope = &m_rootScope;
}

bool Compiler::Compile(vector<Expression*> expressions)
{
    for (Expression* exp : expressions)
    {
        if (!Evaluate(exp)) {
            m_errorStack.push_back("Expression Failed: " + exp->PrintLine());
            return false;
        }
    }
    return true;
}

void Compiler::printErrorStack() {
    for (string e : m_errorStack)
    {
        cout << e << endl;
    }
}