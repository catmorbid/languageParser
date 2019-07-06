#pragma once

#include <vector>
#include <string>
#include <sstream>

#include "Constants.h"

using namespace std;



class Expression {
    TokenType m_command;
    string m_variable;
    string m_scope;
    int m_line;
    ///determines whether the expression requires to be fed more lines (has sub-expressions). When complete, none more is required.
    bool m_complete = false;
    vector<Expression*> m_childExpressions;

    string parseVariable(string var);

    vector<string> FindScope(string var);

public:

    bool error = false;
    string errorMsg = "";


    Expression(int nline);

    Expression(int nline, TokenType t);

    Expression(int nline, TokenType t, string variable, bool complete = true);

    int GetNumLine();
    TokenType GetCommand();

    string GetVariable();

    string GetScope();

    bool IsCommand(TokenType t);

    bool IsComplete();

    void Feed(int ln, string line);

    string printError();

    string PrintLine();

    vector<Expression*> Children();


    static Expression* Parse(int ln, string line);

    static Expression* Error(int ln, string msg);
};