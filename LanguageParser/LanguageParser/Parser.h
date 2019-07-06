#pragma once

#include <string>
#include <vector>

#include "Expression.h"

using namespace std;

class Parser {

public:
    Parser();

    Parser(string input);

    string getOutput();

    vector<Expression*> getExpressions();

private:

    string m_input;
    vector<Expression*> m_expressions;

    //converts line endings..
    string Convert(string src);

    bool Parse();

    vector<string> FindTokens(const string& input, string expr);

};