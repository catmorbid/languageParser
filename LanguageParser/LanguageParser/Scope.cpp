#include <vector>
#include <string>

using namespace std;

Scope::Scope(string name) {
    m_name = name;
}
void Scope::AddVariable(string var)
{
    m_variables.push_back(var);
}

bool Scope::Is(string scope)
{
    return scope == m_name;
}

bool Scope::IsDeclared(string variable)
{
    for (string var : m_variables)
    {
        if (var == variable)
            return true;
    }
    return false;
}