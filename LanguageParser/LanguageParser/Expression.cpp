#include <sstream>

#include "Expression.h";


//private

string Expression::parseVariable(string var) {
    char last = var.at(var.length() - 1);
    if (last == CH_END_EXPRESSION) {
        //valid variable name, remove the last char.
        var.pop_back();
    }

    //determines scope. Nested scopes are not allowed!
    stringstream ss = stringstream(var);
    vector<string> result;
    string s;

    vector<string> scope = FindScope(var);
    m_scope = ROOT_SCOPE;
    if (scope.size() > 1)
    {
        m_scope = scope.at(scope.size() - 2);
        var = scope.at(scope.size() - 1);
    }

    return var;
}

vector<string> Expression::FindScope(string var)
{
    vector<string> result;
    string delim = "::";
    std::size_t current, previous = 0;
    current = var.find(delim);
    while (current != std::string::npos) {
        result.push_back(var.substr(previous, current - previous));
        previous = current + delim.size();
        current = var.find(delim, previous);
    }
    result.push_back(var.substr(previous, current - previous));

    return result;
}

//public

Expression::Expression(int nline) {
    m_command = UNKNOWN;
    m_line = nline;
}

Expression::Expression(int nline, TokenType t) {
    m_command = t;
    m_line = nline;
}

Expression::Expression(int nline, TokenType t, string variable, bool complete = true)
{
    m_command = t;
    m_variable = parseVariable(variable);
    m_complete = complete;
    m_line = nline;
}

int Expression::GetNumLine() { return m_line; }
TokenType GetCommand() { return m_command; }

string Expression::GetVariable() { return m_variable; }

string Expression::GetScope() { return m_scope; }

bool Expression::IsCommand(TokenType t) {
    return m_command == t;
}

bool Expression::IsComplete() {
    return m_complete;
}

void Expression::Feed(int ln, string line)
{
    Expression* e = Parse(ln, line);
    if (e->IsCommand(ENDSCOPE))
    {
        m_complete = true;
    }
    m_childExpressions.push_back(e);
}

string Expression::printError() {
    if (error)
        return " -> " + errorMsg;
    return " -> Syntax OK";
}

string Expression::PrintLine() {
    stringstream ss;
    ss << GetNumLine() << ": " << TOKENS[m_command] << " " + m_scope << " " << m_variable << printError() << endl;
    for (Expression* exp : m_childExpressions)
    {
        ss << exp->PrintLine();
    }
    return ss.str();
}

vector<Expression*> Expression::Children() {
    return m_childExpressions;
}


static Expression* Expression::Parse(int ln, string line) {
    istringstream iss(line);
    vector<string> words(istream_iterator<string>{iss},
        istream_iterator<string>());

    // can only be endscope
    if (words.size() == 1) {
        if (words.at(0) == TOKENS[ENDSCOPE])
            return new Expression(ln, ENDSCOPE);
    }
    //check for ACCESS DECLARE
    else if (words.size() == 2) {
        if (words.at(0) == TOKENS[ACCESS])
            return new Expression(ln, ACCESS, words.at(1));
        else if (words.at(0) == TOKENS[DECLARE])
            return new Expression(ln, DECLARE, words.at(1));
    }
    else if (words.size() == 3)
    {
        if (words.at(0) == TOKENS[SCOPE] && words.at(2) == TOKENS[STARTSCOPE])
            return new Expression(ln, SCOPE, words.at(1), false);
    }
    //handle error
    return Expression::Error(ln, "Invalid word count: " + words.size());
}

static Expression* Expression::Error(int ln, string msg)
{
    Expression* e = new Expression(ln);
    e->error = true;
    e->errorMsg = msg;
    return e;
}