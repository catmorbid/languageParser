#include <sstream>
#include <regex>

#include "Parser.h"
#include "Expression.h"

//public
Parser::Parser() {

}

Parser::Parser(string input) {
    m_input = Convert(input);
    Parse();
}

string Parser::getOutput() {
    string output = "\nOutput:\n";

    int nline = 0;
    for (int i = 0; i < m_expressions.size(); i++)
    {
        output += m_expressions.at(i)->PrintLine();
    }
    output += "\n";
    return output;
}

vector<Expression*> Parser::getExpressions() {
    return m_expressions;
}

//private

string Parser::Convert(string src) {
    int pos = 0;
    string str = src;

    bool found = false;
    while ((pos = str.find("\r\n")) != string::npos)
    {
        str.erase(pos, 1);
        found = true;
    }

    if (!found)
    {
        while ((pos = str.find("\r")) != string::npos)
        {
            str.replace(pos, 1, "\n");
        }
    }

    return str;
}


bool Parser::Parse() {

    vector<string> lines;
    stringstream ss = stringstream(m_input);
    string line;
    int i = 0;

    while (getline(ss, line)) {
        //cout << "DEBUG: parse line: "+to_string(++i)+": "+ line << endl;
        lines.push_back(line);
    }

    for (int i = 0; i < lines.size(); i++) {
        string line = lines.at(i);
        if (line.size() == 0)
            continue;
        Expression* expr = Expression::Parse(i + 1, line);

        if (expr->error)
        {
            cout << "SYNTAX ERROR at line: " + to_string(i + 1);
            return false;
            break;
        }

        while (!expr->IsComplete() && !expr->error)
        {
            int index = ++i;
            expr->Feed(index + 1, lines.at(index));
        }
        m_expressions.push_back(expr);
    }

    return true;

}


vector<string> Parser::FindTokens(const string& input, string expr) {

    vector<string> result;

    try {
        std::smatch m;
        std::regex r(expr);
        std::regex_search(input, m, r);


        /*for (auto v : m)
            result.push_back(v);*/

    }
    catch (std::regex_error e) {
        cout << e.what();
    }

    return result;
}