#pragma once
#include <vector>
#include <string>

using namespace std;

class Scope {
    vector<string> m_variables;
    string m_name;

public:
    Scope(string name);
    void AddVariable(string var);

    bool Is(string scope);

    bool IsDeclared(string variable);
};